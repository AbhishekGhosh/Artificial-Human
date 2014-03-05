#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindCircuitDef::MindCircuitDef() {
	attachLogger();
	enabled = false;
}

MindCircuitDef::~MindCircuitDef() {
	connections.destroy();
};

void MindCircuitDef::createFromXml( Xml xml ) {
	// attributes are properties
	id = xml.getAttribute( "id" );
	name = xml.getAttribute( "name" );
	Object::setInstance( name );

	enabled = xml.getBooleanAttribute( "enabled" , true );
	if( !enabled )
		return;

	// read connections
	for( Xml xmlChild = xml.getFirstChild( "connection" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "connection" ) ) {
		// construct MindArea from attributes
		MindCircuitConnectionDef *connection = new MindCircuitConnectionDef( this );
		connection -> createFromXml( xmlChild );
		connections.add( connection );
	}
}

void MindCircuitDef::resolveReferences( MindMap *map ) {
	for( int k = 0; k < connections.count(); k++ )
		connections.get( k ) -> resolveReferences( map );
}
