#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindRegionTypeDef::MindRegionTypeDef() {
	originalTypeDef = NULL;
}

MindRegionTypeDef::~MindRegionTypeDef() {
	connectorSet.destroy();
}

void MindRegionTypeDef::createFromXml( Xml xml ) {
	if( !xml.exists() )
		return;

	// attributes
	name = xml.getAttribute( "name" );
	Object::setInstance( name );

	implementation = xml.getAttribute( "implementation" );

	// childs
	createConnectorSetFromXml( xml );
}

void MindRegionTypeDef::resolveReferences( MindMap *map ) {
	if( implementation.equals( "mock" ) )
		originalTypeDef = map -> getRegionTypeDefByName( name );
}

void MindRegionTypeDef::createConnectorSetFromXml( Xml xml ) {
	if( !xml.exists() )
		return;

	for( Xml xmlChild = xml.getFirstChild( "connector" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "connector" ) ) {
		// construct MindArea from attributes
		MindRegionConnectorDef *connector = new MindRegionConnectorDef;
		connector -> createFromXml( xmlChild );

		String id = connector -> getId();
		ASSERTMSG( !id.isEmpty() , "region type connector is not well-defined: " + xmlChild.serialize() );

		// add to set and map
		connectorSet.add( connector );
		connectorMap.add( id , connector );
	}
}

MindRegionConnectorDef *MindRegionTypeDef::getConnector( String p_id ) {
	MindRegionConnectorDef *connector = connectorMap.get( p_id );
	ASSERTMSG( connector != NULL , "unknown connector id=" + p_id );
	return( connector );
}
