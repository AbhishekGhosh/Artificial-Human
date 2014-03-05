#include <ah_platform.h>
#include <ah_scheduler_impl.h>

// #############################################################################
// #############################################################################

SchedulerTask::SchedulerTask( MessagePublisher *p_notifyPublisher ) {
	attachLogger();

	notifyPublisher = p_notifyPublisher;
	intervalSec = 0;
	taskThread = ( RFC_HND )NULL;
	stopEvent = rfc_hnd_evcreate();
	stopSignal = false;
}

SchedulerTask::~SchedulerTask() {
	ASSERTMSG( taskThread == ( RFC_HND )NULL , "thread is running for task name=" + name );
	rfc_hnd_evdestroy( stopEvent );
}

void SchedulerTask::setName( String p_name ) {
	name = p_name;
}

void SchedulerTask::setIntervalSec( int nSec ) {
	intervalSec = nSec;
}

void SchedulerTask::start() {
	ASSERTMSG( taskThread == ( RFC_HND )NULL , "thread is running for task name=" + name );
	ThreadService *ts = ThreadService::getService();
	stopSignal = false;
	rfc_hnd_evreset( stopEvent );
	taskThread = ts -> runThread( "scheduler." + name , this , ( ObjectThreadFunction )&SchedulerTask::runSchedulerTask , NULL );
}

void SchedulerTask::stop() {
	if( taskThread == ( RFC_HND )NULL )
		return;

	stopSignal = true;
	rfc_hnd_evsignal( stopEvent );
	ThreadService *ts = ThreadService::getService();
	ts -> waitThreadExited( taskThread );
	taskThread = ( RFC_HND )NULL;
}

void SchedulerTask::runSchedulerTask( void *p_arg ) {
	logger.logInfo( "runSchedulerTask: started task name=" + name );

	RFC_INT64 startTime = Timer::getCurrentTimeMillis();
	RFC_INT64 lastTime = startTime;
	RFC_INT64 interval = intervalSec * 1000;

	while( !stopSignal ) {
		// keep interval
		RFC_INT64 nextTime = lastTime + interval;
		RFC_INT64 currentTime = Timer::getCurrentTimeMillis();

		// sleep to keep interval
		if( nextTime > currentTime )
			rfc_hnd_waitevent( stopEvent , ( int )( nextTime - currentTime ) );

		// check sleep ended by stop signal
		if( stopSignal )
			break;

		// execute task		
		logger.logInfo( "runSchedulerTask: execute task name=" + name );
		TextMessage *msg = notifyPublisher -> createTextMessage();
		msg -> setProperty( "TASK" , name );
		notifyPublisher -> publish( NULL , msg );

		lastTime = nextTime;
	}

	logger.logInfo( "runSchedulerTask: stopped task name=" + name );
}
