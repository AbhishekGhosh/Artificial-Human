#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

EffectorArea::EffectorArea( MindTarget *p_target ) {
	attachLogger();

	MindArea::targetArea = true;
	target = p_target;
	effectors = NULL;
	effectorsOffline = NULL;
}

EffectorArea::~EffectorArea() {
}

// own functions
void EffectorArea::createEffectorArea() {
	effectors = new MindEffectorSet( this );
	effectorsOffline = new MindEffectorSet( this );
}

void EffectorArea::startEffectorArea() {
	// start sensors
	effectors -> startEffectorSet();
}

void EffectorArea::stopEffectorArea() {
	// stop sensors
	effectors -> stopEffectorSet();
}

void EffectorArea::initEffectorArea() {
	MindService *ms = MindService::getService();

	// add regions
	for( int k = 0; k < effectors -> getCount(); k++ ) {
		MindEffector *effector = effectors -> getSetItem( k );
		createEffector( effector );
	}

	ms -> addArea( this );

	// add links
	for( int k = 0; k < effectors -> getCount(); k++ ) {
		MindEffector *effector = effectors -> getSetItem( k );
		createEffectorLinks( effector );
	}
}

void EffectorArea::addEffector( MindEffector *effector , bool offline ) {
	effector -> setRegionId( effector -> getEffectorId() );

	if( !offline ) {
		effectors -> addSetItem( effector );
		logger.logInfo( "addEffector: effector added - id=" + effector -> getEffectorId() );
	}
	else {
		effectorsOffline -> addSetItem( effector );
		logger.logInfo( "addEffector: effector is not configured to run - id=" + effector -> getEffectorId() );
	}
}

MindEffector *EffectorArea::getEffector( String id ) {
	return( effectors -> getEffector( id ) );
}

void EffectorArea::createEffector( MindEffector *effector ) {
	logger.logInfo( "createSensor: create effector id=" + effector -> getEffectorId() + " ..." );

	MindService *ms = MindService::getService();
	MindMap *mm = ms -> getMindMap();

	TargetRegionDef *regioninfo = mm -> getTargetRegionDefById( effector -> getEffectorId() );

	MindRegionCreateInfo regionInfo;
	MindRegionTypeDef *regiontype = regioninfo -> getType();
	regionInfo.setId( effector -> getEffectorId() );
	regionInfo.setInfo( regioninfo );
	regionInfo.setType( regiontype );

	effector -> setEffectorInfo( regioninfo );
	effector -> setRegionInfo( regioninfo );
	effector -> setRegionType( regiontype );

	// create effector data
	MindRegion *region = ( MindRegion * )effector;
	region -> createRegion( &regionInfo );
	MindArea::addRegion( region );
}

void EffectorArea::createEffectorLinks( MindEffector *effector ) {
	MindService *ms = MindService::getService();
	MindMap *mm = ms -> getMindMap();

	TargetRegionDef *regioninfo = effector -> getEffectorInfo();
	TargetCircuitDef *circuitinfo = regioninfo -> getCircuitDef();
	ms -> createCircuitLinks( circuitinfo );
}

