#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

TargetRegionConnectorDef::TargetRegionConnectorDef() {
}

TargetRegionConnectorDef::~TargetRegionConnectorDef() {
}

void TargetRegionConnectorDef::defineRegionConnectorDef( Xml xml , TargetCircuitDef *circuitInfo ) {
	MindRegionConnectorDef::id = xml.getAttribute( "entity" );
	Object::setInstance( MindRegionConnectorDef::id );
	MindRegionConnectorDef::type = xml.getAttribute( "type" );

	region = xml.getAttribute( "region" );
	connector = xml.getAttribute( "connector" );

	circuitInfo -> defineCircuitConnection( this , xml );
}

