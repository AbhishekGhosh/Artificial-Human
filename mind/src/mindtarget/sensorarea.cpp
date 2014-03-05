#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

SensorArea::SensorArea( MindTarget *p_target ) {
	attachLogger();

	MindArea::targetArea = true;
	target = p_target;
	sensors = NULL;
	sensorsOffline = NULL;
}

SensorArea::~SensorArea() {
}

void SensorArea::createSensorArea() {
	sensors = new MindSensorSet( this );
	sensorsOffline = new MindSensorSet( this );
}

void SensorArea::startSensorArea() {
	// start sensors
	sensors -> startSensorSet();
}

void SensorArea::stopSensorArea() {
	// stop sensors
	sensors -> stopSensorSet();
}

void SensorArea::initSensorArea() {
	MindService *ms = MindService::getService();

	// add regions
	for( int k = 0; k < sensors -> getCount(); k++ ) {
		MindSensor *sensor = sensors -> getSetItem( k );
		createSensor( sensor );
	}

	ms -> addArea( this );

	// add links
	for( int k = 0; k < sensors -> getCount(); k++ ) {
		MindSensor *sensor = sensors -> getSetItem( k );
		createSensorLinks( sensor );
	}
}

void SensorArea::addSensor( MindSensor *sensor , bool offline ) {
	sensor -> setRegionId( sensor -> getSensorId() );

	if( !offline ) {
		sensors -> addSetItem( sensor );
		logger.logInfo( "addSensor: sensor added - id=" + sensor -> getSensorId() );
	}
	else {
		sensorsOffline -> addSetItem( sensor );
		logger.logInfo( "addSensor: sensor is not configured to run - id=" + sensor -> getSensorId() );
	}
}

MindSensor *SensorArea::getSensor( String id ) {
	return( sensors -> getSensor( id ) );
}

void SensorArea::createSensor( MindSensor *sensor ) {
	logger.logInfo( "createSensor: create sensor id=" + sensor -> getSensorId() + " ..." );

	MindService *ms = MindService::getService();
	MindMap *mm = ms -> getMindMap();

	TargetRegionDef *regioninfo = mm -> getTargetRegionDefById( sensor -> getSensorId() );

	MindRegionCreateInfo regionInfo;
	MindRegionTypeDef *regiontype = regioninfo -> getType();
	regionInfo.setId( sensor -> getSensorId() );
	regionInfo.setInfo( regioninfo );
	regionInfo.setType( regiontype );

	sensor -> setSensorInfo( regioninfo );
	sensor -> setRegionInfo( regioninfo );
	sensor -> setRegionType( regiontype );

	// create sensor data
	MindRegion *region = ( MindRegion * )sensor;
	region -> createRegion( &regionInfo );
	MindArea::addRegion( region );
}

void SensorArea::createSensorLinks( MindSensor *sensor ) {
	MindService *ms = MindService::getService();
	MindMap *mm = ms -> getMindMap();

	TargetRegionDef *regioninfo = sensor -> getSensorInfo();
	TargetCircuitDef *circuitinfo = regioninfo -> getCircuitDef();
	ms -> createCircuitLinks( circuitinfo );
}

