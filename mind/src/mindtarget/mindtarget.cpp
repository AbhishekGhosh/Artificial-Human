#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindTarget::MindTarget() {
	attachLogger();

	sensorArea = NULL;
	sensorTracker = NULL;

	effectorArea = NULL;
}

void MindTarget::configureService( Xml p_config ) {
	config = p_config;

	// call final
	configureTarget( p_config );
}

void MindTarget::createService() {
	sensorArea = new SensorArea( this );
	effectorArea = new EffectorArea( this );

	// call target with sensors and effectors
	sensorArea -> createSensorArea();
	effectorArea -> createEffectorArea();

	// call final
	createTarget( sensorArea , effectorArea );
}

void MindTarget::initService() {
	MindService *ms = MindService::getService();
	MindMap *mm = ms -> getMindMap();
	mm -> createTargetMeta( config );
	ms -> setMindTarget( this );

	sensorArea -> configure( mm -> getSensorAreaDef() );
	effectorArea -> configure( mm -> getEffectorAreaDef() );
	sensorTracker = new MindSensorSetTracker( sensorArea -> getSensors() );

	// init areas
	sensorArea -> initSensorArea();
	effectorArea -> initEffectorArea();

	// call final target
	initTarget();
}

void MindTarget::runService() {
	// start sensors
	sensorArea -> startSensorArea();
	effectorArea -> startEffectorArea();

	// start poller
	sensorTracker -> startTracker();

	// call final target
	runTarget();
}

void MindTarget::stopService() {
	// stop poller
	sensorTracker -> stopTracker();

	// stop sensors
	sensorArea -> stopSensorArea();
	effectorArea -> stopEffectorArea();

	// call final target
	stopTarget();
}

void MindTarget::exitService() {
	// call final target
	exitTarget();
}

void MindTarget::destroyService() {
	// call final target
	destroyTarget();
}

void MindTarget::addSensor( String p_id , MindSensor *sensor ) {
	Xml configSensors = config.getChildNode( "sensors" );
	Xml xconfig = configSensors.findChildByPathAttr( "sensor" , "id" , p_id );

	bool offline = true;
	if( xconfig.exists() && xconfig.getBooleanAttribute( "run" , true ) ) {
		sensor -> configureSensor( xconfig );
		offline = false;
	}

	sensor -> setSensorId( p_id );
	sensorArea -> addSensor( sensor , offline );
}

void MindTarget::addEffector( String p_id , MindEffector *effector ) {
	Xml configEffectors = config.getChildNode( "effectors" );
	Xml xconfig = configEffectors.findChildByPathAttr( "effector" , "id" , p_id );

	ASSERTMSG( xconfig.exists() , "addEffector: unknown target id=" + p_id );

	bool offline = true;
	if( xconfig.getBooleanAttribute( "run" , true ) ) {
		effector -> configureEffector( xconfig );
		offline = false;
	}

	effector -> setEffectorId( p_id );
	effectorArea -> addEffector( effector , offline );
}

