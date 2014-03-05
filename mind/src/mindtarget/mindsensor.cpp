#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindSensor::MindSensor( SensorArea *p_area )
:	MindRegion( p_area )
{
	msgId = 0;
	pollState = false;
	pollNextMs = 0;
	pollIntervalMs = 0;
}

void MindSensor::configure( Xml p_config ) {
	config = p_config;
	configureSensor( config );
}

void MindSensor::setPollState( bool state ) {
	pollState = state;
}

void MindSensor::createRegion( MindRegionCreateInfo *info ) {
	// call sensor creation
	createSensor( info );
	logger.logDebug( String( "createRegion: created sensor region" ) );
}

void MindSensor::exitRegion() {
}

void MindSensor::destroyRegion() {
}

void MindSensor::processSensorData( NeuroLinkSource *src , String id ) {
	NeuroSignal *memorySensorySignal = getSensorySignal( src );
	memorySensorySignal -> setId( id );

	// log sensor message
	logger.logInfo( "processSensorData: send signal id=" + id );
	LOGDEBUG( "processSensorData: data signal id=" + id + ", data=" + memorySensorySignal -> getNumberDataString() );

	src -> sendMessage( memorySensorySignal );
}

bool MindSensor::getPollState() {
	return( pollState );
}

int MindSensor::getPollIntervalMs( int timeNowMs ) {
	return( pollNextMs - timeNowMs );
}

NeuroSignal *MindSensor::getSensorySignal( NeuroLinkSource *src ) {
	++msgId;
	NeuroSignal *memorySensorySignal = NULL;
	memorySensorySignal -> setTs( Timer::getCurrentTimeMillis() );

	return( memorySensorySignal );
}
