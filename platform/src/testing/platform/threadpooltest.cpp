#include <ah_platform.h>
#include <ah_testing_impl.h>

/*#########################################################################*/
/*#########################################################################*/

class ThreadPoolTest_Task : public ThreadPoolTask {
private:
	String name;
	int taskTimeMs;

public:
	ThreadPoolTest_Task( String p_name , int p_taskTimeMs ) : ThreadPoolTask() {
		name = p_name;
		taskTimeMs = p_taskTimeMs;
		setInstance( name );
	};
	virtual const char *getClass() { return( "ThreadPoolTest_Task" ); };

public:
	virtual void execute() {
		logger.logInfo( String( "task started for taskTimeMs=" ) + taskTimeMs );
		ThreadService *ts = ThreadService::getService();
		ts -> threadSleepMs( taskTimeMs );
	};
};

/*#########################################################################*/
/*#########################################################################*/

class ThreadPoolTest : public TestUnit {
// construction
public:
	ThreadPoolTest() : TestUnit( "ThreadPoolTest" ) {
		attachLogger();
	};
	~ThreadPoolTest() {
	};

	virtual void onCreate() {
		ADD_METHOD( ThreadPoolTest::testWorkflow );
		ADD_METHOD( ThreadPoolTest::testResPoolWorkflow );
		ADD_METHOD( ThreadPoolTest::testResPool );
		ADD_METHOD( ThreadPoolTest::testSemaphore );
	}
	
// tests
public:

	void testWorkflow( XmlCall& call ) {
		// parameters
		String threadPoolName = call.getParam( "threadPoolName" );
		int nTasks = call.getIntParam( "nTasks" );
		int taskTimeMs = call.getIntParam( "taskTimeMs" );
		int runTimeSec = call.getIntParam( "runTimeSec" );
		int suspendTimeSec = call.getIntParam( "suspendTimeSec" );
		int resumeTimeSec = call.getIntParam( "resumeTimeSec" );

		// create task list
		ClassList<ThreadPoolTask> tasks;
		for( int k = 0; k < nTasks; k++ ) {
			ThreadPoolTest_Task *task = new ThreadPoolTest_Task( String( "T" ) + k , taskTimeMs );
			tasks.add( task );
		}

		// create and configure thread pool
		logger.logInfo( "Create thread pool..." );
		ThreadService *ts = ThreadService::getService();
		ts -> createThreadPool( threadPoolName , call.getXml().getChildNode( "threadpoolconfiguration" ) , tasks );

		// workflow
		logger.logInfo( "Start thread pool..." );
		ts -> startThreadPool( threadPoolName );
		ts -> threadSleepMs( runTimeSec * 1000 );

		logger.logInfo( "Suspend thread pool..." );
		ts -> suspendThreadPool( threadPoolName );
		ts -> threadSleepMs( suspendTimeSec * 1000 );
		logger.logInfo( "Resume thread pool..." );
		ts -> resumeThreadPool( threadPoolName );
		ts -> threadSleepMs( resumeTimeSec * 1000 );
		logger.logInfo( "Stop thread pool..." );
		ts -> stopThreadPool( threadPoolName );

		// drop tasks
		tasks.destroy();
		logger.logInfo( "Finished." );
	}

	void testResPoolWorkflow( XmlCall& call ) {
		// parameters
		String threadPoolName = call.getParam( "threadPoolName" );
		int taskTimeMs = call.getIntParam( "taskTimeMs" );

		// create and configure thread pool
		logger.logInfo( "create thread pool..." );
		ThreadService *ts = ThreadService::getService();

		ResourcePool<ThreadPoolTest_Task> tasks;
		ts -> createThreadPool( threadPoolName , call.getXml().getChildNode( "threadpoolconfiguration" ) , ( ResourcePool<ThreadPoolTask>& ) tasks );

		// workflow
		ts -> startThreadPool( threadPoolName );

		try {
			for( int z = 0; z < 10; z++ ) {
				ts -> threadSleepMs( 30000 );
				for( int k = 0; k < 1000; k++ ) {
					String name = String( "task-" ) + z + "-" + k;
					logger.logInfo( "put next task name=" + name + "..." );
					tasks.put( new ThreadPoolTest_Task( name , taskTimeMs ) );
				}
			}

			ts -> suspendThreadPool( threadPoolName );

			for( int k = 0; k < 1000; k++ )
				tasks.put( new ThreadPoolTest_Task( String( "suspend#" ) + k , taskTimeMs ) );

			ts -> resumeThreadPool( threadPoolName );
			ts -> threadSleepMs( 30000 );
		}
		catch( RuntimeException& ) {
			logger.logInfo( "test interrupted" );
		}

		tasks.stop();
		ts -> stopThreadPool( threadPoolName );
		logger.logInfo( "test finished" );
	}

	void testResPool_thread( ResourcePool<Object> *tmp ) {
		rfc_thr_sleep( 5 );
		tmp -> stop();
	}

	void testResPool( XmlCall& call ) {
		ResourcePool<Object> tmp;

		ThreadService *ts = ThreadService::getService();
		ts -> runThread( "testResPool" , this , ( ObjectThreadFunction )&ThreadPoolTest::testResPool_thread , &tmp );
		tmp.get();
	}

	void testSemaphore_thread( RFC_SEM *tmp ) {
		RFC_SEM hnd = *tmp;
		rfc_int_semlock( hnd );
	}

	void testSemaphore( XmlCall& call ) {
		RFC_SEM hnd = rfc_int_semcreate();
		rfc_int_semlock( hnd );

		ThreadService *ts = ThreadService::getService();
		ts -> runThread( "testSemaphore" , this , ( ObjectThreadFunction )&ThreadPoolTest::testSemaphore_thread , &hnd );
		rfc_thr_sleep( 5 );
		rfc_int_semstop( hnd );
		rfc_int_semdestroy( hnd );
	}
};

/*#########################################################################*/
/*#########################################################################*/

TestUnit *TestUnit::createThreadPoolTest() {
	return( new ThreadPoolTest );
}

