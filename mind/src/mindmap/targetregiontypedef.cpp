#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

TargetRegionTypeDef::TargetRegionTypeDef() {
	circuitInfo = NULL;
}

TargetRegionTypeDef::~TargetRegionTypeDef() {
	if( circuitInfo != NULL )
		delete circuitInfo;
}

void TargetRegionTypeDef::defineSensorRegionType( Xml xml ) {
	defineRegionType( true , xml );
}

void TargetRegionTypeDef::defineEffectorRegionType( Xml xml ) {
	defineRegionType( false , xml );
}

void TargetRegionTypeDef::defineRegionType( bool sensor , Xml xml ) {
	MindRegionTypeDef::name = xml.getAttribute( "id" );
	Object::setInstance( MindRegionTypeDef::name );

	MindRegionTypeDef::implementation = "normal";

	// childs & actuator-aligned circuit and unique connection types
	circuitInfo = new TargetCircuitDef();
	circuitInfo -> defineCircuit( sensor , xml );
	defineConnectorSet( xml );
}

void TargetRegionTypeDef::defineConnectorSet( Xml xml ) {
	for( Xml xmlChild = xml.getFirstChild( "connector" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "connector" ) ) {
		// construct from attributes
		TargetRegionConnectorDef *connector = new TargetRegionConnectorDef();
		connector -> defineRegionConnectorDef( xmlChild , circuitInfo );

		String id = connector -> getId();
		ASSERTMSG( !id.isEmpty() , "region type connector is not well-defined: " + xmlChild.serialize() );

		// add to set and map
		MindRegionTypeDef::connectorSet.add( connector );
		MindRegionTypeDef::connectorMap.add( id , connector );
	}
}

