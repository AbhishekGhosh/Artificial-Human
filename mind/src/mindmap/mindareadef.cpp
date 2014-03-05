#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindAreaDef::MindAreaDef() {
	attachLogger();
	enabled = false;
}

MindAreaDef::~MindAreaDef() {
};

void MindAreaDef::createFromXml( Xml xml ) {
	// attributes are properties
	areaId = xml.getAttribute( "id" );
	areaType = xml.getAttribute( "type" );
	areaName = xml.getAttribute( "name" );
	areaFunction = xml.getAttribute( "function" );
	Object::setInstance( areaId );

	enabled = xml.getBooleanAttribute( "enabled" , true );
	if( !enabled )
		return;

	// read regions
	for( Xml xmlChild = xml.getFirstChild( "region" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "region" ) ) {
		// construct MindArea from attributes
		MindRegionDef *region = new MindRegionDef( this );
		region -> createFromXml( xmlChild );
		regions.add( region );
		regionMap.add( region -> getId() , region );
	}
}

void MindAreaDef::resolveReferences( MindMap *map ) {
	for( int k = 0; k < regions.count(); k++ )
		regions.get( k ) -> resolveReferences( map );
}

MindRegionDef *MindAreaDef::findRegion( String region ) {
	return( regionMap.get( region ) );
}

