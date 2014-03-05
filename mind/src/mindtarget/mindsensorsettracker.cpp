#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindSensorSetTracker::MindSensorSetTracker( MindSensorSet *p_set ) {
	set = p_set;

	// init variables
	threadSensesTracker = ( RFC_HND )NULL;
	runSensesTracker = true;

	attachLogger();
}

void MindSensorSetTracker::startTracker() {
	// start tracking thread
	ThreadService *ts = ThreadService::getService();
	threadSensesTracker = ts -> runThread( "SensesTracker" , this , ( ObjectThreadFunction )&MindSensorSetTracker::threadSensesTrackerMain , NULL );
}

void MindSensorSetTracker::stopTracker() {
	runSensesTracker = false;
	if( threadSensesTracker != NULL ) {
		ThreadService *ts = ThreadService::getService();
		ts -> waitThreadExited( threadSensesTracker );
		threadSensesTracker = ( RFC_HND )NULL;
	}
}

void MindSensorSetTracker::threadSensesTrackerMain( void *p_arg ) {
	// run until stop signal
	int sleepRemained = 0;
	while( runSensesTracker )
		pollIteration( sleepRemained );
}

void MindSensorSetTracker::pollIteration( int& sleepRemained ) {
	Timer t;
	int timeNowMs = t.timeCreatedMs();

	// iterate sensors and find sleep time required
	int minMs = 0;
	set -> pollSensorSet( timeNowMs , &minMs );

	// check need sleep
	if( minMs == 0 )
		rfc_thr_sleep( 1 );
	else {
		// buffered sleep time
		sleepRemained += minMs;

		// do not sleep less than 50 ms
		if( sleepRemained >= 50 ) {
			ThreadService *ts = ThreadService::getService();
			ts -> threadSleepMs( sleepRemained );
		}
	}
}
