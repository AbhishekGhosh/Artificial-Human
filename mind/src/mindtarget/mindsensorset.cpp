#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindSensorSet::MindSensorSet( SensorArea *p_area ) {
	attachLogger();
	area = p_area;
}

int MindSensorSet::getCount() {
	return( list.count() );
}

MindSensor *MindSensorSet::getSetItem( int k ) {
	return( list.get( k ) );
}

void MindSensorSet::addSetItem( MindSensor *sensor ) {
	list.add( sensor );
	map.add( sensor -> getClass() , sensor );
}

MindSensor *MindSensorSet::getSensor( String name ) {
	return( map.get( name ) );
}

void MindSensorSet::createSensorSet() {
	MindRegionSet *set = area -> getRegionSet();
	for( int k = 0; k < list.count(); k++ ) {
		MindSensor *sensor = list.get( k );

		String id = String( area -> getClass() ) + "." + sensor -> getClass();
		// sensor -> create( id );
		set -> addSetItem( sensor );
	}
}

void MindSensorSet::startSensorSet() {
	for( int k = 0; k < list.count(); k++ ) {
		MindSensor *sensor = list.get( k );
		sensor -> startSensor();
		logger.logInfo( String( "startSensorSet: sensor started - name=" ) + sensor -> getClass() );
	}
}

void MindSensorSet::stopSensorSet() {
	for( int k = 0; k < list.count(); k++ ) {
		MindSensor *sensor = list.get( k );
		sensor -> stopSensor();
		logger.logInfo( String( "stopSensorSet: sensor stopped - name=" ) + sensor -> getClass() );
	}
}

void MindSensorSet::pollSensorSet( int timeNowMs , int *minPollNextMs ) {
	int minMs = 0;
	for( int k = 0; k < list.count(); k++ ) {
		MindSensor *sensor = list.get( k );

		// get poll status and time to next poll
		bool poll = sensor -> getPollState();
		if( poll ) {
			int ms = sensor -> getPollIntervalMs( timeNowMs );

			// run all in poll state
			if( ms <= 0 )
				sensor -> pollSensor();

			if( minMs == 0 || ( ms > 0 && ms < minMs ) )
				minMs = ms;
		}
	}

	*minPollNextMs = minMs;
}
