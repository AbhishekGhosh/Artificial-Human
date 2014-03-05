#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

TargetCircuitConnectionDef::TargetCircuitConnectionDef( TargetCircuitDef *circuitInfo ) : MindCircuitConnectionDef( circuitInfo ) {
	regionConnection = NULL;
	connectionTypeDef = NULL;
}

TargetCircuitConnectionDef::~TargetCircuitConnectionDef() {
	if( connectionTypeDef != NULL )
		delete connectionTypeDef;
}

void TargetCircuitConnectionDef::defineCircuitConnectorDef( TargetRegionConnectorDef *p_regionConnection , Xml xml ) {
	regionConnection = p_regionConnection;
	TargetCircuitDef *circuitInfoTarget = ( TargetCircuitDef * )MindCircuitConnectionDef::circuitDef;

	// check direction
	if( regionConnection -> getType().equals( "target" ) ) {
		MindCircuitConnectionDef::srcRegion = regionConnection -> getRegion();
		MindCircuitConnectionDef::dstRegion = circuitInfoTarget -> getActuatorId();
	}
	else {
		MindCircuitConnectionDef::srcRegion = circuitInfoTarget -> getActuatorId();
		MindCircuitConnectionDef::dstRegion = regionConnection -> getRegion();
	}

	// create unique connection type
	connectionTypeDef = new TargetConnectionTypeDef();
	connectionTypeDef -> defineConnectionTypeDef( circuitInfoTarget , regionConnection , xml );

	MindCircuitConnectionDef::typeName = connectionTypeDef -> getName();
	Object::setInstance( MindCircuitConnectionDef::typeName );
	MindCircuitConnectionDef::type = connectionTypeDef;
	MindCircuitConnectionDef::primary = xml.getBooleanAttribute( "primary" , false );
}
