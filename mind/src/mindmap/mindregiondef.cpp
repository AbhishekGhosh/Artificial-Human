#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindRegionDef::MindRegionDef( MindAreaDef *p_area ) {
	attachLogger();
	area = p_area;
	role = MIND_REGION_ROLE_NONE;
}

MindRegionDef::~MindRegionDef() {
};

void MindRegionDef::createFromXml( Xml xml ) {
	// attributes are properties
	id = xml.getAttribute( "id" );
	Object::setInstance( id );

	typeName = xml.getAttribute( "type" );
	size = xml.getIntAttribute( "size" , 0 );

	String roleName = xml.getAttribute( "role" );

	if( roleName.equals( "processor" ) )
		role = MIND_REGION_ROLE_PROCESSOR;
	else if( roleName.equals( "relay" ) )
		role = MIND_REGION_ROLE_RELAY;
	else if( roleName.equals( "sensory" ) )
		role = MIND_REGION_ROLE_SENSORY;
	else if( roleName.equals( "sympathetic" ) )
		role = MIND_REGION_ROLE_SYMPATHETIC;
	else if( roleName.equals( "parasympathetic" ) )
		role = MIND_REGION_ROLE_PARASYMPATHETIC;
	else if( roleName.equals( "flexor" ) )
		role = MIND_REGION_ROLE_FLEXOR;
	else if( roleName.equals( "extensor" ) )
		role = MIND_REGION_ROLE_EXTENSOR;
	else
		ASSERTFAILED( "Invalid role of region id=" + id + ", role=" + roleName );
}

void MindRegionDef::resolveReferences( MindMap *map ) {
	type = map -> getRegionTypeDefByName( typeName );
}

String MindRegionDef::getRoleName() {
	if( role == MIND_REGION_ROLE_PROCESSOR )
		return( "processor" );
	else if( role == MIND_REGION_ROLE_RELAY )
		return( "relay" );
	else if( role == MIND_REGION_ROLE_SENSORY )
		return( "sensory" );
	else if( role == MIND_REGION_ROLE_SYMPATHETIC )
		return( "sympathetic" );
	else if( role == MIND_REGION_ROLE_PARASYMPATHETIC )
		return( "parasympathetic" );
	else if( role == MIND_REGION_ROLE_FLEXOR )
		return( "flexor" );
	else if( role == MIND_REGION_ROLE_EXTENSOR )
		return( "extensor" );

	return( "unknown" );
}

