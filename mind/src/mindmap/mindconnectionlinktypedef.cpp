#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindConnectionLinkTypeDef::MindConnectionLinkTypeDef( MindConnectionTypeDef *p_connectionDef ) {
	connectionDef = p_connectionDef;
	back = true;
}

MindConnectionLinkTypeDef::~MindConnectionLinkTypeDef() {
}

void MindConnectionLinkTypeDef::createFromXml( Xml xml ) {
	// attributes
	name = xml.getAttribute( "name" );
	Object::setInstance( name );

	implementation = xml.getAttribute( "implementation" );
	type = xml.getAttribute( "nttype" );
	neurotransmitter = xml.getAttribute( "nt" );
	srcConnector = xml.getAttribute( "src" );
	dstConnector = xml.getAttribute( "dst" );
	back = xml.getBooleanAttribute( "back" , false );
}
