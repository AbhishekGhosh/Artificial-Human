#include <ah_platform.h>
#include <ah_threads_impl.h>

/*#########################################################################*/
/*#########################################################################*/

ThreadPoolItem::ThreadPoolItem( String p_name , int id ) {
	name = p_name;
	setInstance( name );

	thread = ( RFC_HND )NULL;
	threadPoolId = id;

	runEvent = rfc_hnd_evcreate();
	suspendEvent = rfc_hnd_evcreate();
	suspendSignal = true;
	stopSignal = false;
	rfc_hnd_evreset( runEvent );
	rfc_hnd_evsignal( suspendEvent );

	ticksPerSecond = Timer::timeMsToTicks( 1000 );
	executionTimeWindowTicks = 0;

	clearStatistics();
}

ThreadPoolItem::~ThreadPoolItem() {
	rfc_hnd_evdestroy( runEvent );
	rfc_hnd_evdestroy( suspendEvent );
}

String ThreadPoolItem::getThreadName() {
	return( name );
}

void ThreadPoolItem::clearStatistics() {
	executionCount = 0;
	ticksExecTimeTotal = 0;
	ticksSleepTimeTotal = 0;
	ticksWaitTimeRemained = 0;
}

void ThreadPoolItem::start() {
	// clear previous statistics
	clearStatistics();

	// start thread
	ThreadService *ts = ThreadService::getService();
	thread = ts -> runThread( name , this , ( ObjectThreadFunction )&ThreadPoolItem::run , NULL );
	state.setState( ThreadState::THREAD_STATE_RUNNING );
}

void ThreadPoolItem::suspend() {
	rfc_hnd_evreset( runEvent );
	suspendSignal = true;
	rfc_hnd_waitevent( suspendEvent , -1 );
	state.setState( ThreadState::THREAD_STATE_SUSPENDED );
}

void ThreadPoolItem::resume() {
	suspendSignal = false;
	rfc_hnd_evsignal( runEvent );
}

void ThreadPoolItem::stop() {
	stopSignal = true;
	rfc_hnd_evsignal( runEvent );

	RFC_HND stopThread = thread;
	if( stopThread != ( RFC_HND )NULL ) {
		ThreadService *ts = ThreadService::getService();
		ts -> waitThreadExited( stopThread );
	}
}

void ThreadPoolItem::execute( ThreadPoolTask *task ) {
	// execute and measure execution time (in milliseconds)
	Timer t;
	t.createWithStartingTimestamp();
	task -> execute();
	int executeTimeTicks = t.timePassedTicks();

	if( executionTimeWindowTicks == 0 )
		executionTimeWindowTicks = executeTimeTicks;

	// to calculate current execute/sleep ratio
	ticksExecTimeTotal += executeTimeTicks;

	// calculate sleep time for this opeation (can be negative!)
	int opSleepTimeTicks = executionTimeWindowTicks - executeTimeTicks;

	// add to pending sleep time
	ticksWaitTimeRemained += opSleepTimeTicks;
	executionCount++;

	// if sleep time is less than second - do nothing
	if( ticksWaitTimeRemained < ticksPerSecond )
		return;
	
	// get whole number of seconds to sleep (probably always will be 1) and sleep
	int sleepSeconds = ticksWaitTimeRemained / ticksPerSecond;

	ThreadService *ts = ThreadService::getService();
	ts -> threadSleepMs( sleepSeconds * 1000 );

	int sleepTimeTicks = t.timePassedTicks() - executeTimeTicks;
	ticksSleepTimeTotal += sleepTimeTicks;
	ticksWaitTimeRemained -= sleepTimeTicks;
}

void ThreadPoolItem::setExecutionTimeWindowTicks( int ticks ) {
	executionTimeWindowTicks = ticks;
}

void ThreadPoolItem::run( void *p_arg ) {
	int executionTimeWindowMs = ( int )( ( 1000. * executionTimeWindowTicks ) / ticksPerSecond );
	logger.logInfo( String( "run: thread started - executionTimeWindowMs=" ) + executionTimeWindowMs );

	while( !stopSignal ) {
		// ensure run enabled
		rfc_hnd_waitevent( runEvent , -1 );

		// get and execute task
		ThreadPoolTask *task = getThreadTask();
		if( task != NULL ) {
			execute( task );

			//  finish task execution
			executedThreadTask( task );
		}

		// process suspend request
		if( suspendSignal ) {
			// notify stopped
			rfc_hnd_evsignal( suspendEvent );
			logger.logInfo( String( "run: thread suspended" ) );

			// wait till resumed
			rfc_hnd_waitevent( runEvent , -1 );
			if( !stopSignal )
				logger.logInfo( String( "run: thread resumed" ) );

			// next cycle
			rfc_hnd_evreset( suspendEvent );
		}
	}

	state.setState( ThreadState::THREAD_STATE_STOPPING );
	thread = NULL;
	state.setState( ThreadState::THREAD_STATE_CREATED );
	logger.logInfo( String( "run: thread stopped" ) );
}

