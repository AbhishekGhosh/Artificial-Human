#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindMap::MindMap() {
	sensorAreaInfo = NULL;
}

MindMap::~MindMap() {
	if( sensorAreaInfo != NULL )
		delete sensorAreaInfo;

	regionTypeSet.destroy();
	mindAreaSet.destroy();
	connectionTypeSet.destroy();
}

void MindMap::createFromXml( Xml xml ) {
	// load mind definition
	createRegionTypeDefSet( xml.getFirstChild( "region-type-set" ) );
	createAreaDefSet( xml.getFirstChild( "component-set" ) );
	createConnectionTypeDefSet( xml.getFirstChild( "connection-type-set" ) );
	createCircuitDefSet( xml.getFirstChild( "circuit-set" ) );

	// resolve definition references
	for( int k = 0; k < regionTypeSet.count(); k++ )
		regionTypeSet.get( k ) -> resolveReferences( this );
	for( int k = 0; k < mindAreaSet.count(); k++ )
		mindAreaSet.get( k ) -> resolveReferences( this );
	for( int k = 0; k < connectionTypeSet.count(); k++ )
		connectionTypeSet.get( k ) -> resolveReferences( this );
	for( int k = 0; k < mindCircuitSet.count(); k++ )
		mindCircuitSet.get( k ) -> resolveReferences( this );
}

void MindMap::createRegionTypeDefSet( Xml xml ) {
	if( !xml.exists() )
		return;

	for( Xml xmlChild = xml.getFirstChild( "region-type" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "region-type" ) ) {
		// construct MindRegionType from attributes
		MindRegionTypeDef *info = new MindRegionTypeDef;
		regionTypeSet.add( info );

		info -> createFromXml( xmlChild );

		// get region type name
		String name = info -> getName();
		ASSERTMSG( !name.isEmpty() , "region type is not defined: " + xmlChild.serialize() );
		ASSERTMSG( regionTypeMap.get( name ) == NULL , name + ": region type duplicate found for name=" + name );

		// add
		regionTypeMap.add( name , info );
	}
}

void MindMap::createAreaDefSet( Xml xml ) {
	if( !xml.exists() )
		return;

	for( Xml xmlChild = xml.getFirstChild( "area" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "area" ) ) {
		// construct MindArea from attributes
		MindAreaDef *info = new MindAreaDef;
		mindAreaSet.add( info );

		info -> createFromXml( xmlChild );

		// get areaId
		String id = info -> getAreaId();
		ASSERTMSG( !id.isEmpty() , "area is not defined: " + xmlChild.serialize() );
		ASSERTMSG( mindAreaMap.get( id ) == NULL , id + ": area duplicate found for id=" + id );

		// add
		mindAreaMap.add( id , info );

		// add regions to map
		createRegionMap( info );
	}
}

void MindMap::createConnectionTypeDefSet( Xml xml ) {
	if( !xml.exists() )
		return;

	for( Xml xmlChild = xml.getFirstChild( "connection-type" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "connection-type" ) ) {
		// construct MindArea from attributes
		MindConnectionTypeDef *linkType = new MindConnectionTypeDef;
		linkType -> createFromXml( xmlChild );
		connectionTypeSet.add( linkType );
		connectionTypeMap.add( linkType -> getName() , linkType );
	}
}

void MindMap::createCircuitDefSet( Xml xml ) {
	if( !xml.exists() )
		return;

	// check using another file
	EnvService *es = EnvService::getService();
	String xmlFile = xml.getAttribute( "xmlfile" , "" );
	if( !xmlFile.isEmpty() )
		xml = es -> loadXml( xmlFile );

	for( Xml xmlChild = xml.getFirstChild( "circuit" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "circuit" ) ) {
		// construct MindArea from attributes
		MindCircuitDef *circuit = new MindCircuitDef;
		circuit -> createFromXml( xmlChild );
		mindCircuitSet.add( circuit );
		mindCircuitMap.add( circuit -> getName() , circuit );
	}
}

void MindMap::createRegionMap( MindAreaDef *info ) {
	ClassList<MindRegionDef>& regions = info -> getRegions();
	for( int k = 0; k < regions.count(); k++ ) {
		MindRegionDef *region = regions.get( k );
		String id = region -> getId();

		ASSERTMSG( mindRegionMap.get( id ) == NULL , "duplicate region ID=" + id );
		mindRegionMap.add( id , region );
	}
}

MindConnectionTypeDef *MindMap::getConnectionTypeDefByName( String typeName ) {
	return( connectionTypeMap.get( typeName ) );
}

MindRegionTypeDef *MindMap::getRegionTypeDefByName( String regionTypeName ) {
	MindRegionTypeDef *info = regionTypeMap.get( regionTypeName );
	ASSERTMSG( info != NULL , "Wrong region type name=" + regionTypeName );
	return( info );
}

MindRegionDef *MindMap::getRegionDefById( String regionId ) {
	MindRegionDef *info = mindRegionMap.get( regionId );
	ASSERTMSG( info != NULL , "Wrong region id=" + regionId );
	return( info );
}

MindAreaDef *MindMap::getAreaDefById( String areaId ) {
	MindAreaDef *info = mindAreaMap.get( areaId );
	ASSERTMSG( info != NULL , "Wrong area id=" + areaId );
	return( info );
}

void MindMap::getMapRegions( MapStringToClass<MindRegionDef>& regionMap ) {
	for( int k = 0; k < mindAreaMap.count(); k++ ) {
		MindAreaDef *areaDef = mindAreaMap.getClassByIndex( k );
		ClassList<MindRegionDef>& regionlist = areaDef -> getRegions();
		for( int m = 0; m < regionlist.count(); m++ ) {
			MindRegionDef *regionDef = regionlist.get( m );
			regionMap.add( regionDef -> getId() , regionDef );
		}
	}
}

void MindMap::createTargetMeta_defineCircuit( Xml xml , TargetAreaDef *areaDef , bool sensors ) {
	if( sensors )
		areaDef -> defineSensorArea();
	else
		areaDef -> defineEffectorArea();

	mindAreaMap.add( areaDef -> getAreaId() , areaDef );

	// create sensor meta
	String itemsElement = ( sensors )? "sensors" :  "effectors";
	String itemElement = ( sensors )? "sensor" :  "effector";

	Xml config = xml.getChildNode( itemsElement );
	for( Xml xmlChild = config.getFirstChild( itemElement ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( itemElement ) ) {
		TargetRegionDef *regionInfo = new TargetRegionDef( areaDef );

		if( sensors )
			regionInfo -> defineSensorRegion( xmlChild );
		else
			regionInfo -> defineEffectorRegion( xmlChild );

		// add to maps
		areaDef -> addRegion( regionInfo );
		mindRegionMap.add( regionInfo -> getId() , regionInfo );
		TargetRegionTypeDef *regionTypeInfo = ( TargetRegionTypeDef * )regionInfo -> getType();
		regionTypeMap.add( regionTypeInfo -> getName() , regionTypeInfo );

		TargetCircuitDef *circuitInfo = regionTypeInfo -> getCircuitInfo();
		mindCircuitMap.add( circuitInfo -> getName() , circuitInfo );

		regionInfo -> setCircuitDef( circuitInfo );
	}
}

void MindMap::createTargetMeta( Xml xml ) {
	// sensor area
	sensorAreaInfo = new TargetAreaDef();
	createTargetMeta_defineCircuit( xml , sensorAreaInfo , true );

	// effector area
	effectorAreaInfo = new TargetAreaDef();
	createTargetMeta_defineCircuit( xml , effectorAreaInfo , false );
}

TargetRegionDef *MindMap::getTargetRegionDefById( String regionId ) {
	TargetRegionDef *info = ( TargetRegionDef * )mindRegionMap.get( regionId );
	ASSERTMSG( info != NULL , "Wrong target region id=" + regionId );
	return( info );
}

