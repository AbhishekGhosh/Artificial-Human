#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

TargetConnectionLinkTypeDef::TargetConnectionLinkTypeDef( TargetConnectionTypeDef *p_connectionDef ) 
:	MindConnectionLinkTypeDef( p_connectionDef ) {
}

TargetConnectionLinkTypeDef::~TargetConnectionLinkTypeDef() {
}

void TargetConnectionLinkTypeDef::defineConnectionLinkTypeDef( TargetCircuitDef *circuitInfoTarget , TargetRegionConnectorDef *regionConnection , TargetConnectionTypeDef *connectionType , Xml xml ) {
	// attributes
	MindConnectionLinkTypeDef::name = connectionType -> getName();
	Object::setInstance( MindConnectionLinkTypeDef::name );

	MindConnectionLinkTypeDef::implementation = xml.getAttribute( "implementation" );
	MindConnectionLinkTypeDef::type = xml.getAttribute( "nttype" );
	MindConnectionLinkTypeDef::neurotransmitter = xml.getAttribute( "nt" );

	if( regionConnection -> isTarget() ) {
		srcConnector = xml.getAttribute( "connector" );
		dstConnector = xml.getAttribute( "entity" );
	}
	else {
		srcConnector = xml.getAttribute( "entity" );
		dstConnector = xml.getAttribute( "connector" );
	}

	back = false;
}

