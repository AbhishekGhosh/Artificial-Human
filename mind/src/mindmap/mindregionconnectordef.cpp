#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindRegionConnectorDef::MindRegionConnectorDef() {
}

void MindRegionConnectorDef::createFromXml( Xml xml ) {
	// attributes
	id = xml.getAttribute( "id" );
	Object::setInstance( id );

	type = xml.getAttribute( "type" );
}

bool MindRegionConnectorDef::isTarget() {
	return( type.equals( "target" ) );
}

