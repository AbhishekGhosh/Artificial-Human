#include <ah_platform.h>
#include <ah_threads_impl.h>

#include <windows.h>
#include <signal.h>

unsigned long tlsIndex = 0;

/*#########################################################################*/
/*#########################################################################*/

/* if termination signal catched */
static void on_exit( int p_sig ) {
	ThreadService *ts = ThreadService::getService();
	if( ts != NULL )
		ts -> stopServicesBySignal( p_sig );
}

static unsigned __stdcall threadMainFunction( void *p_arg ) {
	ThreadData *td = ( ThreadData * )p_arg;
	ThreadService *ts = ThreadService::getService();

	try {
		unsigned value = ts -> threadFunction( td );
		return( value );
	}
	catch( ... ) {
		fprintf( stderr , "threadMainFunction: uncaught exception while running thread\n" );
	}
	return( -1 );
}

static void UnhandledExceptionTranslator( unsigned int exceptionCode , struct _EXCEPTION_POINTERS *exceptionInfo ) {
	throw RuntimeException( exceptionCode , 3 , exceptionInfo -> ExceptionRecord -> ExceptionAddress );
}

/*#########################################################################*/
/*#########################################################################*/

Service *ThreadService::newService() {
	return( new ThreadService() );
}

void ThreadService::createService() {
	workerStatus = 0;
	lockExit = rfc_hnd_semcreate();
	eventExit = rfc_hnd_evcreate();
	countExit = 0;

	tlsIndex = TlsAlloc();

	Timer::startAdjustment();
	rfc_thr_sleep( 1 );
	Timer::stopAdjustment();

	// initialize event
	rfc_hnd_evreset( eventExit );

	// cover main thread
	addMainThread();
}

void ThreadService::stopService() {
	// ensure all thread pools are stopped
	for( int k = 0; k < threadpools.count(); k++ ) {
		ThreadPool *tp = threadpools.getClassByIndex( k );
		tp -> ensureStopped();
	}
}

void ThreadService::exitService() {
	// stop all thread pools
	logger.logInfo( String( "exitService: stop all thread pools..." ) );
	for( int k = 0; k < threadpools.count(); k++ ) {
		String name = threadpools.getKeyByIndex( k );
		stopThreadPool( name );
	}
}

void ThreadService::destroyService() {
	rfc_hnd_semdestroy( lockExit );
	rfc_hnd_evdestroy( eventExit );

	// drop all
	threadpools.destroy();
}

void ThreadService::addMainThread() {
	// register main thread
	workerCreated();

	ThreadData *td = new ThreadData;
	td -> name = "main";
	td -> threadExtId.s_ih = ::OpenThread( THREAD_ALL_ACCESS , FALSE , ::GetCurrentThreadId() );
	td -> threadExtId.s_ip = NULL;
	workerStarted( td );

	// reset signal handlers
	setSignalHandlers();
}

int ThreadService::getThreadId() {
	ThreadData *threadData = getThreadData();
	if( threadData == NULL )
		return( 0 );
	return( threadData -> threadId );
}

RFC_HND ThreadService::getThreadHandle() {
	ThreadData *threadData = getThreadData();
	if( threadData == NULL )
		return( ( RFC_HND )NULL );
	return( threadData -> threadExtId.s_ih );
}

unsigned ThreadService::threadFunction( ThreadData *td ) {
	int status = 0;
	Object *o = td -> object;
	Logger& tlogger = o -> getLogger();

	String name = td -> name;
	try {
		workerStarted( td );

		void ( Object::*of )( void *p_arg ) = td -> objectFunction;
		void *oa = td -> objectFunctionArg;
		( o ->* of )( oa );
	}
	catch ( RuntimeException& e ) {
		tlogger.printStack( e );
		status = -12;
	}
	catch ( ... ) {
		logger.printStack();
		tlogger.logError( "threadFunction: thread name=" + name + " - unknown exception" );
		tlogger.printStack();
		status = -13;
	}

	workerExited( status );
	return( status );
}

RFC_HND ThreadService::runThread( String p_name , Object *object , void (Object::*p_function)( void *p_arg ) , void *p_arg ) {
	ServiceManager& sm = ServiceManager::getInstance();
	ASSERTMSG( sm.canStartThread() , String( "Cannot start any thread in current state" ) );
	workerCreated();

	ThreadData *td = new ThreadData;
	td -> object = object;
	td -> objectFunction = p_function;
	td -> objectFunctionArg = p_arg;
	td -> name = p_name;

	if( rfc_thr_process( &td -> threadExtId , ( void * )td , threadMainFunction ) ) {
		logger.logError( "runThread: cannot start thread name=" + td -> name );
		workerExited( td , -10 );
		return( NULL );
	}

	return( td -> threadExtId.s_ih );
}

void ThreadService::workerCreated() {
	rfc_hnd_semlock( lockExit );
	countExit++;
	rfc_hnd_semunlock( lockExit );
}

void ThreadService::workerStarted( ThreadData *threadData ) {
	rfc_hnd_semlock( lockExit );

	try {
		String name = threadData -> name;

		// thread-allocated data
		TlsSetValue( tlsIndex , threadData );
		threadData -> threadId = ::GetCurrentThreadId();

		// init logging
		ThreadHelper *to = new ThreadHelper;
		to -> addThreadObject();
		manageThreadCallStack();

		logger.logInfo( "workerStarted: thread started name=" + name + ", threadId=0x" + String::toHex( ( int )threadData -> threadId ) );

		// add worker to managed list
		if( threads.get( name ) != NULL )
			ASSERTFAILED( "Thread already started with name=" + name );

		threads.add( name , threadData );
	}
	catch( RuntimeException& e ) {
		rfc_hnd_semunlock( lockExit );
		throw e;
	}

	rfc_hnd_semunlock( lockExit );
}

void ThreadService::workerExited( ThreadData *threadData , int status ) {
	ASSERT( threadData != NULL );

	rfc_hnd_semlock( lockExit );

	// drop from thread list
	String name = threadData -> name;
	int threadId = ( int )threadData -> threadId;
	threads.remove( name );

	threadData -> map.destroy();

	// decrease counter
	countExit--;

	// if no more workers (except main) - notify
	if( countExit == 1 )
		rfc_hnd_evsignal( eventExit );

	logger.logInfo( "workerExited: thread stopped name=" + name + ", threadId=0x" + String::toHex( ( int )threadData -> threadId ) );
	delete threadData;

	rfc_hnd_semunlock( lockExit );
}

void ThreadService::workerExited( int status ) {
	// get thread-allocated data - clean it in thread memory
	ThreadData *threadData = getThreadData();

	// run common
	workerExited( threadData , status );

	// to be no more used
	TlsSetValue( tlsIndex , NULL );
}

bool ThreadService::waitThreadExited( RFC_HND threadId ) {
	if( threadId == NULL )
		return( false );

	RFC_THREAD th;
	th.s_ih = threadId;
	th.s_ip = NULL;
	if( rfc_thr_waitexit( &th ) < 0 )
		return( false );

	return( true );
}

String ThreadService::getActiveThreads() {
	String rs = "";
	rfc_hnd_semlock( lockExit );
	for( int k = 0; k < threads.count(); k++ ) {
		ThreadData *td = threads.getClassByIndex( k );
		if( td -> name.equals( "main" ) )
			continue;

		if( !rs.isEmpty() )
			rs += ", ";
		rs += td -> name;
	}
	rfc_hnd_semunlock( lockExit );
	return( rs );
}

void ThreadService::addThreadObject( const char *key , ThreadObject *to ) {
	ASSERT( key != NULL );
	ASSERT( to != NULL );
	ThreadData *threadData = getThreadData();
	ASSERT( threadData != NULL );
	ASSERT( threadData -> map.get( key ) == NULL );

	threadData -> map.add( key , to );
}

ThreadData *ThreadService::getThreadData() {
	return( ( ThreadData * )TlsGetValue( tlsIndex ) );
}

ThreadObject *ThreadService::getThreadObject( const char *key ) {
	ThreadData *threadData = getThreadData();
	if( threadData == NULL )
		return( NULL );
	return( threadData -> map.get( key ) );
}

void ThreadService::manageThreadCallStack() {
	ThreadHelper *to = ThreadHelper::getThreadObject();
	to -> oldAIUnhandledExceptionTranslator = ( void (*)() )::_set_se_translator( UnhandledExceptionTranslator );
}

void ThreadService::threadDumpAll( bool showStackTrace ) {
	rfc_hnd_semlock( lockExit );
	logger.logInfo( String( "threadDumpAll: THREAD DUMP (" ) + "THREAD COUNT=" + threads.count() + "):" , Logger::LogStart );
	logger.logInfo( "threadDumpAll: ------------" , Logger::LogLine );

	for( int k = 0; k < threads.count(); k++ ) {
		ThreadData *td = threads.getClassByIndex( k );

		String threadInfo = String( "threadDumpAll: THREAD DUMP: thread index=" ) + k + ", name=" + td -> name + ", threadId=0x" + String::toHex( ( int )td -> threadId );
		if( showStackTrace )
			 threadInfo += ":";
		logger.logInfo( threadInfo , Logger::LogLine );

		if( showStackTrace )
			printThreadStackTrace( td );
	}

	logger.logInfo( "threadDumpAll: ------------" , Logger::LogStop );
	rfc_hnd_semunlock( lockExit );
}

void ThreadService::threadDumpByName( String name , bool showStackTrace ) {
	rfc_hnd_semlock( lockExit );
	ThreadData *td = threads.get( name );
	if( td != NULL ) {
		String threadInfo = String( "threadDumpByName: THREAD DUMP: thread name=" ) + td -> name + ", threadId=0x" + String::toHex( ( int )td -> threadId );
		if( showStackTrace )
			 threadInfo += ":";
		logger.logInfo( threadInfo );

		if( showStackTrace )
			printThreadStackTrace( td );
	}
	rfc_hnd_semunlock( lockExit );

	ASSERTMSG( td != NULL , "Unknown thread name=" + name );
}

void ThreadService::printThreadStackTrace( ThreadData *td ) {
	// suspend if not the same thread
	DWORD currentThread = ::GetCurrentThreadId();
	bool sameThread = ( currentThread == td -> threadId )? true : false;
	rfc_threadstack *stack;

	if( sameThread )
		stack = rfc_thr_stackget( 0 );
	else {
		// suspend thread
		HANDLE handle = td -> threadExtId.s_ih;
		::SuspendThread( handle );

		// get stack
		stack = rfc_thr_stackgetforthread( ( RFC_HND )handle , 0 );

		// resume thread
		::ResumeThread( handle );
	}

	// print stack
	logger.printStackInplace( stack , 0 );
}

// services
void ThreadService::setSignalHandlers() {
	signal( SIGABRT , on_exit );
	signal( SIGINT , on_exit );
	signal( SIGTERM , on_exit );
	signal( SIGFPE , on_exit );
	signal( SIGILL , on_exit );
	signal( SIGSEGV , on_exit );
}

void ThreadService::stopServicesBySignal( int status ) {
	logger.logInfo( "stopServices: Stop by signal" );

	// reset signal handlers
	setSignalHandlers();

	// process again
	workerCreated();

	ThreadData *td = new ThreadData;
	td -> name = "exit";
	workerStarted( td );

	// stop sleep all threads
	threadSleepInterruptAll();

	ServiceManager::getInstance().stop();
	rfc_hnd_evsignal( eventExit );
	workerExited( 0 );
}

void ThreadService::waitExitSignal() {
	rfc_hnd_semlock( lockExit );
	if( countExit > 1 ) {
		rfc_hnd_semunlock( lockExit );
		rfc_hnd_waitevent( eventExit , -1 );
	}
}

void ThreadService::waitAllThreads() {
	// wait till instance execution ended
	while( true ) {
		int count = countExit - 1;
		if( count <= 0 )
			break;

		String activeThreadList = getActiveThreads();
		logger.logInfo( String( "waitAllThreads: Waiting for stopping " ) + count + " thread(s): " + activeThreadList );
		rfc_thr_sleep( 1 );
	}
}

// thread pool operations
void ThreadService::createThreadPool( String name , Xml configuration , ClassList<ThreadPoolTask>& tasks ) {
	ThreadPool *tp = createThreadPool( name , configuration );

	// create tasks
	tp -> create( tasks );
}

void ThreadService::createThreadPool( String name , Xml configuration , ResourcePool<ThreadPoolTask>& tasks ) {
	ThreadPool *tp = createThreadPool( name , configuration );

	// create tasks
	tp -> create( tasks );
}

ThreadPool *ThreadService::createThreadPool( String name , Xml configuration ) {
	// check duplication
	ASSERTMSG( threadpools.get( name ) == NULL , "Thread pool already exists with name=" + name );

	// construct
	ThreadPool *tp = new ThreadPool( name );
	threadpools.add( name , tp );

	// configure
	tp -> configure( configuration );
	return( tp );
}

void ThreadService::startThreadPool( String name ) {
	// check exists
	ThreadPool *tp = threadpools.get( name );
	ASSERTMSG( tp != NULL , "thread pool does not exist with name=" + name );

	// start
	tp -> start();
}

void ThreadService::stopThreadPool( String name ) {
	// check exists
	ThreadPool *tp = threadpools.get( name );
	ASSERTMSG( tp != NULL , "thread pool does not exist with name=" + name );

	// stop
	tp -> stop();
}

void ThreadService::suspendThreadPool( String name ) {
	// check exists
	ThreadPool *tp = threadpools.get( name );
	ASSERTMSG( tp != NULL , "thread pool does not exist with name=" + name );

	// suspend
	tp -> suspend();
}

void ThreadService::resumeThreadPool( String name ) {
	// check exists
	ThreadPool *tp = threadpools.get( name );
	ASSERTMSG( tp != NULL , "thread pool does not exist with name=" + name );

	// resume
	tp -> resume();
}

void ThreadService::destroyThreadPool( String name ) {
	// ignore if does not exist
	ThreadPool *tp = threadpools.get( name );
	if( tp == NULL )
		return;

	// destroy
	logger.logInfo( "destroy thread pool name=" + name );
	threadpools.remove( name );
	tp -> ensureStopped();
	delete tp;
}

// interruptable sleep
void ThreadService::threadSleepMs( int ms ) {
	ThreadData *threadData = getThreadData();
	rfc_hnd_waitevent( threadData -> sleepEvent , ms );

	// raise interrupt exception if stopped
	if( threadData -> checkStopped() )
		ASSERTFAILED( "thread was stopped, wait interrupted" );
}

void ThreadService::threadSleepInterrupt( ThreadData *td ) {
	rfc_hnd_evsignal( td -> sleepEvent );
}

void ThreadService::threadSleepInterruptAll() {
	rfc_hnd_semlock( lockExit );
	for( int k = 0; k < threads.count(); k++ ) {
		ThreadData *td = threads.getClassByIndex( k );
		td -> stopThread();
	}
	rfc_hnd_semunlock( lockExit );
}

