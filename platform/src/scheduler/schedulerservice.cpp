#include <ah_platform.h>
#include <ah_scheduler_impl.h>

// #############################################################################
// #############################################################################

SchedulerService::SchedulerService() {
}

Service *SchedulerService::newService() {
	return( new SchedulerService() );
}

void SchedulerService::configureService( Xml p_config ) {
	config = p_config;
	notifyChannel = config.getProperty( "notifyChannel" );
}

void SchedulerService::createService() {
}

void SchedulerService::initService() {
	// create channel publisher
	MessagingService *ms = MessagingService::getService();
	notifyPublisher = ms -> createPublisher( NULL , notifyChannel , "SchedulerService" , "notify" );
}

void SchedulerService::runService() {
}

void SchedulerService::stopService() {
	for( int k = 0; k < taskMap.count(); k++ ) {
		SchedulerTask *task = taskMap.getClassByIndex( k );
		task -> stop();
	}
}

void SchedulerService::exitService() {
}

void SchedulerService::destroyService() {
	taskMap.destroy();
}

/*#########################################################################*/
/*#########################################################################*/

String SchedulerService::getNotifyChannel() {
	return( notifyChannel );
}

MessageSubscription *SchedulerService::createTimerTask( String taskName , int intervalSec , MessageSubscriber *sub ) {
	createTimerTask( taskName , intervalSec );
	MessageSubscription *taskSub = subscribeTimerTask( taskName , sub );
	return( taskSub );
}

MessageSubscription *SchedulerService::subscribeTimerTask( String taskName , MessageSubscriber *sub ) {
	// subscribe
	MessagingService *ms = MessagingService::getService();
	MessageSubscription *taskSub = ms -> subscribeSelector( NULL , notifyChannel , "TASK=" + taskName , taskName , sub );
	return( taskSub );
}

void SchedulerService::createTimerTask( String taskName , int intervalSec ) {
	ASSERTMSG( taskMap.get( taskName ) == NULL , "duplicate schedule task name=" + taskName );

	SchedulerTask *task = new SchedulerTask( notifyPublisher );
	task -> setName( taskName );
	task -> setIntervalSec( intervalSec );
	task -> start();
	taskMap.add( taskName , task );
}

void SchedulerService::dropTimerTask( String taskName ) {
	SchedulerTask *task = taskMap.get( taskName );
	task -> stop();
	taskMap.remove( taskName );
	delete task;
}
