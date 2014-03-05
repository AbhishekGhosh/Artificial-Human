#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

TargetCircuitDef::TargetCircuitDef() {
}

TargetCircuitDef::~TargetCircuitDef() {
}

void TargetCircuitDef::defineCircuit( bool p_isSensorOption , Xml xmlRegion ) {
	// circuit for the only sensor/effector
	isSensorOption = p_isSensorOption;
	actuatorId = xmlRegion.getAttribute( "id" );

	// circuit data
	MindCircuitDef::id = actuatorId + ".CIRCUIT";
	MindCircuitDef::name = actuatorId  + " Circuit";
	Object::setInstance( MindCircuitDef::id );
	MindCircuitDef::enabled = true;
}

void TargetCircuitDef::defineCircuitConnection( TargetRegionConnectorDef *regionConnection , Xml xml ) {
	TargetCircuitConnectionDef *connectionInfo = new TargetCircuitConnectionDef( this );
	connectionInfo -> defineCircuitConnectorDef( regionConnection , xml );
	MindCircuitDef::connections.add( connectionInfo );
}

