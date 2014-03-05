#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

WikiAreaPages::WikiAreaPages( WikiMaker *p_wm ) {
	attachLogger();
	wm = p_wm;
}

WikiAreaPages::~WikiAreaPages() {
}

void WikiAreaPages::execute() {
	bool createAreaPages = wm -> wiki.getBooleanProperty( "createAreaPages" , true );
	if( createAreaPages == false ) {
		logger.logInfo( "skip creating area pages" );
		return;
	}

	// get wiki file
	String wikiDir = wm -> wiki.getProperty( "wikiPath" );
	String selectedArea = wm -> wiki.getProperty( "selectedArea" , "" );

	MindService *ms = MindService::getService();

	// by area
	bool createRegionPages = wm -> wiki.getBooleanProperty( "createRegionPages" , true );
	MapStringToClass<MindArea>& areaMap = ms -> getMindAreas();
	for( int k = 0; k < areaMap.count(); k++ ) {
		MindArea *area = areaMap.getClassByIndex( k );

		if( !selectedArea.isEmpty() ) {
			if( !selectedArea.equals( area -> getAreaId() ) )
				continue;
		}

		createAreaPages_createRegionTableSection( wikiDir , area );
		createAreaPages_createConnectivityTableSection( wikiDir , area );
		createAreaPages_createCircuitsAndReferencesTableSection( wikiDir , area );

		if( createRegionPages )
			createAreaPages_createRegionPages( wikiDir , area );
	}
}

void WikiAreaPages::createAreaPages_createRegionTableSection( String wikiDir , MindArea *area ) {
	StringList lines;

	// heading
	String wikiPage = wm -> getAreaPage( area -> getAreaId() );
	String s = createAreaPages_getRegionTableRow( NULL );
	lines.add( s );

	// regions
	MapStringToClass<MindRegion>& regionMap = area -> getRegions();
	for( int k = 0; k < regionMap.count(); k++ ) {
		MindRegion *region = regionMap.getClassByIndex( k );
		s = createAreaPages_getRegionTableRow( region );
		lines.add( s );
	}

	String sectionName = "Components";
	wm -> updateFileSection( wikiDir , wikiPage , sectionName , lines );
}

String WikiAreaPages::createAreaPages_getRegionTableRow( MindRegion *region ) {
	String value;

	// heading
	if( region == NULL ) {
		value = "|| *Component ID* || *Component Name* || *Type* || *Function* || *Comments* ||";
		return( value );
	}

	// region row
	String regionId = region -> getRegionId();
	const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( regionId );
	
	value = "|| " + wm -> getRegionReference( regionId ) + " || " + 
		createAreaPages_getTableCellAttribute( info , "name" , info.name , true , 0 ) + " || " + 
		createAreaPages_getTableCellAttribute( info , "type" , info.batype , true , 0 ) + " || " + 
		createAreaPages_getTableCellAttribute( info , "function" , info.function , true , 80 ) + " || " + 
		createAreaPages_getTableCellAttribute( info , "notes" , info.notes , false , 50 ) + " ||";

	return( value );
}

String WikiAreaPages::createAreaPages_getTableCellAttribute( const XmlHMindElementInfo& info , String attribute , String value , bool required , int columnWidth ) {
	ASSERTMSG( ( required == false ) || ( value.isEmpty() == false ) , "attribute=" + attribute + " is empty for region=" + info.id );

	if( columnWidth == 0 )
		return( value );

	if( value.length() <= columnWidth )
		return( value );

	// format to column width
	StringList list;
	value.split( list , " " );
	value.clear();

	String valueline;
	for( int k = 0; k < list.count(); k++ ) {
		String item = list.get( k );
		if( valueline.length() + item.length() <= columnWidth || valueline.isEmpty() ) {
			if( !valueline.isEmpty() )
				valueline += " ";
			valueline += item;
			continue;
		}

		if( !value.isEmpty() )
			value += "<BR>";
		value += valueline;
		valueline = item;
	}

	if( !value.isEmpty() )
		value += "<BR>";
	value += valueline;

	return( value );
}

void WikiAreaPages::createAreaPages_addExternalConnections( MindArea *area , StringList& lines , MapStringToClass<MindRegionLink>& connections , bool p_inputs ) {
	MindService *ms = MindService::getService();

	if( connections.count() == 0 ) {
		lines.add( "  * no connections" );
		return;
	}

	createAreaPages_getExternalConnectionTableLine( area , NULL , lines , p_inputs , NULL );

	// split connections by area types
	MapStringToClass<MapStringToClass<MindRegionLink>> groups;
	for( int k = 0; k < connections.count(); k++ ) {
		MindRegionLink *c = connections.getClassByIndex( k );
		MindRegion *region = ( p_inputs )? c -> getSrcRegion() : c -> getDstRegion();
		String areaType = region -> getArea() -> getMindAreaDef() -> getAreaType();

		MapStringToClass<MindRegionLink> *z = groups.get( areaType );
		if( z == NULL ) {
			z = new MapStringToClass<MindRegionLink>;
			groups.add( areaType , z );
		}

		String key = connections.getKeyByIndex( k );
		z -> add( key , c );
	}

	for( int k = 0; k < groups.count(); k++ ) {
		String areaType = groups.getKeyByIndex( k );
		MapStringToClass<MindRegionLink> *z = groups.getClassByIndex( k );
		createAreaPages_getExternalConnectionTableLine( area , NULL , lines , p_inputs , areaType );

		for( int m = 0; m < z -> count(); m++ ) {
			MindRegionLink *c = z -> getClassByIndex( m );
			createAreaPages_getExternalConnectionTableLine( area , c , lines , p_inputs , NULL );
		}
	}
}

void WikiAreaPages::createAreaPages_createConnectivityTableSection( String wikiDir , MindArea *area ) {
	StringList lines;
	MapStringToClass<MindRegionLink> connections;

	if( !area -> isTargetArea() ) {
		// internal connections
		createAreaPages_getInternalConnections( area , connections );
		createAreaPages_getInternalConnectionTableLine( area , NULL , lines );
		for( int k = 0; k < connections.count(); k++ ) {
			MindRegionLink *c = connections.getClassByIndex( k );
			createAreaPages_getInternalConnectionTableLine( area , c , lines );
		}
	}

	// external connections - in
	lines.add( "" );
	lines.add( "*External Inbound Region Connections:*" );
	MapStringToClass<MindRegionLink> connectionsInputs;
	createAreaPages_getExternalConnections( area , connectionsInputs , true );
	createAreaPages_addExternalConnections( area , lines , connectionsInputs , true );

	// external connections - out
	lines.add( "" );
	lines.add( "*External Outbound Region Connections:*" );
	MapStringToClass<MindRegionLink> connectionsOutputs;
	createAreaPages_getExternalConnections( area , connectionsOutputs , false );
	createAreaPages_addExternalConnections( area , lines , connectionsOutputs , false );

	String sectionName = "Connectivity";
	String wikiPage = wm -> getAreaPage( area -> getAreaId() );
	wm -> updateFileSection( wikiDir , wikiPage , sectionName , lines );

	if( !area -> isTargetArea() ) {
		// create dot file
		createDotFile( area , connections , connectionsInputs , connectionsOutputs );
	}
}

void WikiAreaPages::createAreaPages_getInternalConnections( MindArea *area , MapStringToClass<MindRegionLink>& connections ) {
	ClassList<MindRegionLink>& links = area -> getInternalRegionLinks();

	for( int k = 0; k < links.count(); k++ ) {
		MindRegionLink *c = links.get( k );

		// ignore secondary
		if( !c -> isPrimary() )
			continue;

		String key = c -> getSrcRegion() -> getRegionId() + "#" + c -> getDstRegion() -> getRegionId();
		if( connections.get( key ) == NULL )
			connections.add( key , c );
	}
}

void WikiAreaPages::createAreaPages_getExternalConnections( MindArea *area , MapStringToClass<MindRegionLink>& connections , bool isin ) {
	ClassList<MindAreaLink>& areaLinks = ( isin )? area -> getMasterAreaLinks() : area -> getSlaveAreaLinks();

	for( int k1 = 0; k1 < areaLinks.count(); k1++ ) {
		MindAreaLink *areaLink = areaLinks.get( k1 );
		ClassList<MindRegionLink>& regionLinks = areaLink -> getRegionLinks();

		for( int k2 = 0; k2 < regionLinks.count(); k2++ ) {
			MindRegionLink *c = regionLinks.get( k2 );

			// ignore secondary
			if( !c -> isPrimary() )
				continue;

			String key;
			if( isin == false ) {
				key = areaLink -> getSlaveArea() -> getAreaId() + "#" + c -> getSrcRegion() -> getRegionId() + "#" + c -> getDstRegion() -> getRegionId() + "#1";
				if( connections.get( key ) == NULL )
					connections.add( key , c );
			}
			else {
				key = areaLink -> getMasterArea() -> getAreaId() + "#" + c -> getDstRegion() -> getRegionId() + "#" + c -> getSrcRegion() -> getRegionId() + "#2";
				if( connections.get( key ) == NULL )
					connections.add( key , c );
			}
		}
	}
}

void WikiAreaPages::createAreaPages_getInternalConnectionTableLine( MindArea *area , MindRegionLink *link , StringList& lines ) {
	String line;
	String value1;
	if( link == NULL ) {
		// add heading
		line = "*Internal Region Connections:*";
		lines.add( line );
		lines.add( "" );
		String dotImageWikiPath = wm -> wiki.getProperty( "imageWikiPath" );
		line = dotImageWikiPath + "/" + area -> getAreaId() + ".dot.jpg";
		lines.add( line );
		lines.add( "" );
		line = "|| *Source Region* || *Target Region* || *Type* || *Reference* ||";
		lines.add( line );
		wm -> clearRepeats1( value1 );
		return;
	}

	// table row
	String reference = wm -> findReference( link );
	value1 = wm -> getRegionReference( link -> getSrcRegion() -> getRegionId() );
	wm -> clearRepeats1( value1 );
	line = "|| " + value1 + " || " + wm -> getRegionReference( link -> getDstRegion() -> getRegionId() ) + " || " + 
		link -> getConnectionType() -> getName() + " || " + reference + " ||" ;
	lines.add( line );
}

void WikiAreaPages::createAreaPages_getExternalConnectionTableLine( MindArea *area , MindRegionLink *link , StringList& lines , bool isin , const char *p_areatype ) {
	String line;
	String value1;
	String value2;
	if( link == NULL ) {
		if( p_areatype == NULL ) {
			// add heading
			lines.add( "" );

			if( isin )
				line = "|| *Source Area* || *Local Region* || *Source Region* || *Source Name* || *Type* || *Reference* ||";
			else
				line = "|| *Target Area* || *Local Region* || *Target Region* || *Target Name* || *Type* || *Reference* ||";
		}
		else {
			line = "|| <font color=\"red\">" + String( p_areatype ) + "</font> || || || || || ||";
		}

		lines.add( line );
		wm -> clearRepeats2( value1 , value2 );
		return;
	}

	// table row
	MindService *ms = MindService::getService();
	String srcRegionId = link -> getSrcRegion() -> getRegionId();
	String dstRegionId = link -> getDstRegion() -> getRegionId();

	String reference = wm -> findReference( link );
	if( isin ) {
		const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( srcRegionId );
		value1 = wm -> getAreaReference( link -> getSrcRegion() -> getArea() -> getAreaId() );
		value2 = wm -> getRegionReference( dstRegionId );
		wm -> clearRepeats2( value1 , value2 );
		line = "|| " + value1 + " || " + value2 + " || " + 
			wm -> getRegionReference( srcRegionId ) + " || " + info.name + " || " + link -> getConnectionType() -> getName() + " || " + reference + " ||";
	}
	else {
		const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( dstRegionId );
		value1 = wm -> getAreaReference( link -> getDstRegion() -> getArea() -> getAreaId() );
		value2 = wm -> getRegionReference( srcRegionId );
		wm -> clearRepeats2( value1 , value2 );
		line = "|| " + value1 + " || " + value2 + " || " + 
			wm -> getRegionReference( dstRegionId ) + " || " + info.name + " || " + link -> getConnectionType() -> getName() + " || " + reference + " ||";
	}
	lines.add( line );
}

void WikiAreaPages::createAreaPages_createCircuitsAndReferencesTableSection( String wikiDir , MindArea *area ) {
	// skip circuits for target areas - will be in region pages
	if( area -> isTargetArea() )
		return;

	// collect circuits which reference any of area regions
	MindService *ms = MindService::getService();

	StringList circuits;
	wm -> circuitsxml.getCircuitList( circuits );

	MapStringToString circuitKeys;
	for( int k = 0; k < circuits.count(); k++ ) {
		String circuitId = circuits.get( k );
		XmlCircuitInfo& info = wm -> circuitsxml.getCircuitInfo( circuitId );

		String key = createAreaPages_getCircuitKey( area , info );
		if( key.isEmpty() )
			continue;

		circuitKeys.add( key , circuitId );
	}

	// add circuits section - sorted by relevance
	StringList lines;
	for( int k = circuitKeys.count() - 1; k >= 0; k-- ) {
		String circuitId = circuitKeys.getClassByIndex( k );
		XmlCircuitInfo& info = wm -> circuitsxml.getCircuitInfo( circuitId );
		createAreaPages_getCircuitLines( info , lines );
	}

	String sectionName = "Thirdparty Circuits";
	String wikiPage = wm -> getAreaPage( area -> getAreaId() );
	wm -> updateFileSection( wikiDir , wikiPage , sectionName , lines );
	lines.clear();

	// add unique and sorted references - sorted by relevance
	MapStringToClass<MindArea> refs;
	for( int k = circuitKeys.count() - 1; k >= 0; k-- ) {
		String circuitId = circuitKeys.getClassByIndex( k );
		XmlCircuitInfo& info = wm -> circuitsxml.getCircuitInfo( circuitId );

		if( !info.reference.equals( "UNKNOWN" ) )
			if( refs.get( info.reference ) == NULL ) {
				refs.add( info.reference , area );
				lines.add( String( "  * " ) + info.reference );
			}
	}

	sectionName = "References";
	wm -> updateFileSection( wikiDir , wikiPage , sectionName , lines );
}

String WikiAreaPages::createAreaPages_getCircuitKey( MindArea *area , XmlCircuitInfo& info ) {
	// get circuit regions
	StringList comps;
	wm -> circuitsxml.getCircuitComponents( info , comps );

	// check circuit mentions area regions
	StringList compUsed;
	for( int k = 0; k < comps.count(); k++ ) {
		String comp = comps.get( k );
		String region = wm -> hmindxml.getMappedRegion( comp );
		if( !region.isEmpty() )
			if( area -> getMindAreaDef() -> findRegion( region ) != NULL )
				compUsed.add( region );
	}

	if( compUsed.count() == 0 )
		return( "" );

	// make key
	compUsed.sort();
	char l_buf[ 10 ];
	sprintf( l_buf , "%3.3d" , compUsed.count() );

	String key = l_buf;
	for( int k = 0; k < compUsed.count(); k++ )
		key += "." + compUsed.get( k );

	// ensure unique
	key += "." + info.id;
	return( key );
}

void WikiAreaPages::createAreaPages_getCircuitLines( XmlCircuitInfo& info , StringList& lines ) {
	String ref;
	if( info.reference.equals( "UNKNOWN" ) )
		ref = "(unknown reference)";
	else
		ref = "- see [" + info.reference + " Reference]";

	lines.add( "  * [" + info.image + " " + info.name + "] " + ref );
	lines.add( "" );
	String sizeInfo;
	if( info.imageHeight > 0 )
		sizeInfo += String( "height=" ) + info.imageHeight;
	if( info.imageWidth > 0 )
		sizeInfo += String( "width=" ) + info.imageWidth;

	lines.add( "<img src=\"" + info.image + "\" alt=\"unavailable\"" + sizeInfo + ">" );
	lines.add( "" );
}

void WikiAreaPages::createAreaPages_createRegionPages( String wikiDir , MindArea *area ) {
	MapStringToClass<MindRegion>& regions = area -> getRegions();
	for( int k = 0; k < regions.count(); k++ ) {
		WikiRegionPage maker( wm , wikiDir , regions.getClassByIndex( k ) );
		maker.execute();
	}
}

void WikiAreaPages::createDotFile( MindArea *area , MapStringToClass<MindRegionLink>& internals , MapStringToClass<MindRegionLink>& inputs , MapStringToClass<MindRegionLink>& outputs ) {
	// create dot file
	String dotDir = wm -> wiki.getProperty( "dotPath" );
	String fileName = dotDir + "/" + area -> getAreaId() + ".dot";
	StringList text;

	// header
	text.add( "digraph \"" + area -> getAreaId() + "\" {" );
	text.add( "\tconcentrate=true;" );
	text.add( "\tcompound=true;" );
	String defaultDotSetup = wm -> wiki.getProperty( "defaultDotSetup" );
	text.add( wm -> setSpecialCharacters( defaultDotSetup ) );
	text.add( "" );

	// list nodes
	text.add( "\tsubgraph cluster_" + area -> getAreaId() + " {" );
	text.add( "\tlabel=<<b>" + area -> getMindAreaDef() -> getAreaName() + "</b>>;" );
	MapStringToClass<MindRegion>& regions = area -> getRegions();
	for( int k = 0; k < regions.count(); k++ ) {
		MindRegion *region = regions.getClassByIndex( k );

		String dotdef = wm -> hmindxml.getDotDef( region -> getRegionId() );
		String nodeline = "\t\"" + region -> getRegionId() + "\"";
		if( !dotdef.isEmpty() ) {
			String s = wm -> setSpecialCharacters( dotdef );
			s += ", label=<" + createDotFile_getRegionLabel( region , inputs , outputs ) + ">";
			nodeline += " [" + s + "]";
		}
		nodeline += ";";

		text.add( nodeline );
	}
	text.add( "\t}" );

	// list connections
	text.add( "" );
	for( int k = 0; k < internals.count(); k++ ) {
		MindRegionLink *c = internals.getClassByIndex( k );
		String linkline = "\t\"" + c -> getSrcRegion() -> getRegionId() + "\" -> \"" + c -> getDstRegion() -> getRegionId() + "\";";
		text.add( linkline );
	}

	// add subgraph
	String linkItem;
	createDotFile_subgraph( area , true , inputs , text , linkItem );
	createDotFile_subgraph( area , false , outputs , text , linkItem );

	// footer
	text.add( "}" );

	// out to file
	wm -> createFileContent( fileName , text );
}

String WikiAreaPages::createDotFile_getRegionLabel( MindRegion *region , MapStringToClass<MindRegionLink>& inputs , MapStringToClass<MindRegionLink>& outputs ) {
	MindService *ms = MindService::getService();
	String label = "<b>" + region -> getRegionId() + "</b>";

	MapStringToClass<MindRegionLink> inputAreas;
	for( int k = 0; k < inputs.count(); k++ ) {
		MindRegionLink *c = inputs.getClassByIndex( k );
		if( region != c -> getDstRegion() )
			continue;

		MindRegion *regionExt = c -> getSrcRegion();
		inputAreas.addnew( regionExt -> getArea() -> getAreaId() , c );
	}

	String in;
	for( int k = 0; k < inputAreas.count(); k++ ) {
		String area = inputAreas.getKeyByIndex( k );
		if( !in.isEmpty() )
			in += ",";
		in += area;
	}

	MapStringToClass<MindRegionLink> outputAreas;
	for( int k = 0; k < outputs.count(); k++ ) {
		MindRegionLink *c = outputs.getClassByIndex( k );
		if( region != c -> getSrcRegion() )
			continue;

		MindRegion *regionExt = c -> getDstRegion();
		outputAreas.addnew( regionExt -> getArea() -> getAreaId() , c );
	}

	String out;
	for( int k = 0; k < outputAreas.count(); k++ ) {
		String area = outputAreas.getKeyByIndex( k );
		if( !out.isEmpty() )
			out += ",";
		out += area;
	}

	if( !in.isEmpty() )
		label += "<br/>IN: " + in;
	if( !out.isEmpty() )
		label += "<br/>OUT: " + out;

	return( label );
}

void WikiAreaPages::createDotFile_subgraph( MindArea *area , bool p_inputs , MapStringToClass<MindRegionLink>& connections , StringList& text , String& linkItem ) {
	MindService *ms = MindService::getService();
	String postfix = ( p_inputs )? "IN" : "OUT";
	String postfixString = ( p_inputs )? "Inputs" : "Outputs";

	// inter-area links
	text.add( "" );
	if( p_inputs ) {
		text.add( "\tsubgraph cluster_ExtIn {" );
		text.add( "\tlabel=<<b>Inputs</b>>;" );
		text.add( "\t/* external inbound links */" );
	}
	else {
		text.add( "\tsubgraph cluster_ExtOut {" );
		text.add( "\tlabel=<<b>Outputs</b>>;" );
		text.add( "\t/* external outbound links */" );
	}

	MapStringToClass<MindRegion> areaTypes;
	MapStringToString areas;
	MapStringToClass<MapStringToPtr> areaTypeRegionMap;
	for( int k = 0; k < connections.count(); k++ ) {
		MindRegionLink *c = connections.getClassByIndex( k );
		MindRegion *region = ( p_inputs )? c -> getSrcRegion() : c -> getDstRegion();
		MindRegion *regionLocal = ( p_inputs )? c -> getDstRegion() : c -> getSrcRegion();
		String areaId = region -> getArea() -> getAreaId();
		String areaType = region -> getArea() -> getMindAreaDef() -> getAreaType();

		String key = areaType + ":" + regionLocal -> getRegionId();
		MapStringToPtr *lineAreas = areaTypeRegionMap.get( key );
		if( lineAreas == NULL ) {
			lineAreas = new MapStringToPtr;
			areaTypeRegionMap.add( key , lineAreas );
		}

		if( lineAreas -> get( areaId ) == NULL )
			lineAreas -> add( areaId , c );

		areas.addnew( areaId , areaType );
		areaTypes.addnew( areaType , regionLocal );
	}

	// add named lines
	String s;
	String localArea = area -> getAreaId();
	for( int k = 1; k < areaTypes.count(); k++ ) {
		String areaType = areaTypes.getKeyByIndex( k );

		if( k == 1 )
			s = String( "\"" ) + areaTypes.getKeyByIndex( 0 ) + "." + postfix + "\"";
		s += String( " -> " ) + "\"" + areaType + "." + postfix + "\"";
	}

	if( !s.isEmpty() ) {
		s += " [ style=invis ];";
		text.add( "\t" + s );
	}

	// header
	text.add( "" );

	for( int k = 0; k < areaTypes.count(); k++ ) {
		String areaType = areaTypes.getKeyByIndex( k );
		text.add( "\tsubgraph cluster_" + areaType + "_" + postfix + " {" );
		text.add( "\tlabel=\"" + areaType + " " + postfixString + "\";" );
		text.add( "\t\"" + areaType + "." + postfix + "\" [ shape=box, style=dotted , label = <" );
		text.add( "\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\"><TR>" );

		for( int a = 0; a < areas.count(); a++ ) {
			if( !areaType.equals( areas.getClassByIndex( a ) ) )
				continue;

			String area = areas.getKeyByIndex( a );
			text.add( "\t\t<TD PORT=\"" + area + "\" BGCOLOR=\"springgreen\">" + area + "</TD>" );
		}

		text.add( "\t\t</TR></TABLE>> ];" );
		text.add( "\t}" );
	}

	text.add( "\t}" );
	areaTypeRegionMap.destroy();

	// input-output link
	if( areaTypes.count() > 0 ) {
		if( p_inputs == true )
			linkItem = areaTypes.getKeyByIndex( areaTypes.count() - 1 ) + String( "." ) + postfix;
		else 
		if( !linkItem.isEmpty() ) {
			String linkTo = areaTypes.getKeyByIndex( 0 ) + String( "." ) + postfix;
			text.add( "\t\"" + linkItem + "\" -> \"" + linkTo + "\" [ style=invis ];" );
		}
	}
}

