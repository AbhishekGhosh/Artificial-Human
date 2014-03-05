#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

WikiRegionPage::WikiRegionPage( WikiMaker *p_wm , String p_wikiDir , MindRegion *p_region ) 
:	info( p_wm -> hmindxml.getElementInfo( p_region -> getRegionId() ) ) {
	attachLogger();
	wm = p_wm;
	wikiDir = p_wikiDir;
	region = p_region;
}

WikiRegionPage::~WikiRegionPage() {
}

void WikiRegionPage::execute() {
	StringList sections;
	sections.add( "= Components =" );
	sections.add( "= Connectivity =" );
	sections.add( "= Thirdparty Circuits =" );
	sections.add( "= References =" );
	String wikiPage = wm -> getRegionPage( info.id );
	wm -> ensureFileExists( wikiDir , wikiPage , sections );

	createHeading();
	createChildTableSection();
	createConnectivitySection();
	createThirdpartyAndReferencesSection();
}

void WikiRegionPage::createHeading() {
	StringList lines;
	lines.add( "#summary " + info.name );
	String areaWikiPage = wm -> getAreaPage( region -> getArea() -> getAreaId() );
	lines.add( "@@[Home] -> [BiologicalLifeResearch] -> [" + areaWikiPage + "] -> [" + wm -> getRegionPage( info.id ) + "]" );
	lines.add( "----" );
	lines.add( "" );
	lines.add( "This page covers biological details of component " + info.name + "." );
	lines.add( "Region is part of aHuman target integrated biological model." );
	lines.add( "" );

	// add path
	StringList listItems;
	Xml node = wm -> hmindxml.getNodeXml( info.id );
	for( Xml nodePath = node; nodePath.exists(); nodePath = nodePath.getParentNode() ) {
		if( nodePath.getName().equals( "division" ) ) {
			String name = nodePath.getAttribute( "name" );
			listItems.add( name );
			break;
		}
		else {
			String id = nodePath.getAttribute( "id" , "" );
			String name = nodePath.getAttribute( "name" , "" );
			String value = name;
			if( !id.isEmpty() ) {
				if( value.isEmpty() )
					value = id;
				else
					value += " (" + id + ")";
			}
			listItems.add( value );
		}
	}
	String s;
	for( int k = listItems.count() - 1; k >= 0; k-- ) {
		s += listItems.get( k );
		if( k > 0 )
			s += " -> ";
	}
	lines.add( "  * *Top-down path to region*: " + s + " (see [OverallMindMaps Mind Maps])" );

	// mind area
	MindArea *area = region -> getArea();
	lines.add( "  * *Brain area*: [BrainArea" + area -> getAreaId() + " " + area -> getMindAreaDef() -> getAreaName() + "]" );

	// function
	if( !info.brodmannid.isEmpty() )
		lines.add( "  * *Brodmann ID*: " + info.brodmannid );
	lines.add( "  * *Role*: " + region -> getRegionInfo() -> getRoleName() );
	lines.add( "  * *Function*: " + info.function );
	if( !info.comment.isEmpty() )
		lines.add( "  * *Notes to structure*: " + info.comment );
	if( !info.notes.isEmpty() )
		lines.add( "  * *Notes to function*: " + info.notes );

	String wikiPage = wm -> getRegionPage( info.id );
	wm -> updateFileHeading( wikiDir , wikiPage , lines );
}

void WikiRegionPage::createChildTableSection() {
	Xml node = wm -> hmindxml.getNodeXml( info.id );
	if( !node.exists() ) {
		logger.logInfo( "unable to find region=" + info.id );
		return;
	}

	// components section
	StringList lines;

	// lines hierarchy
	lines.add( "" );
	if( node.getChildNode( "element" ).exists() ) {
		lines.add( "*Component items*:" );
		createChildTableSection_addChilds( node , "  * " , lines );
	}
	else
		lines.add( "  * no child items defined" );

	String wikiPage = wm -> getRegionPage( info.id );
	String section = "Components";
	wm -> updateFileSection( wikiDir , wikiPage , section , lines );
}

void WikiRegionPage::createChildTableSection_addChilds( Xml node , String prefix , StringList& lines ) {
	String value;
	StringList refList;
	for( Xml nodeChild = node.getFirstChild( "element" ); nodeChild.exists(); nodeChild = nodeChild.getNextChild( "element" ) ) {
		String id = nodeChild.getAttribute( "id" , "" );
		String nameItem = createChildTableSection_getChildNameItem( nodeChild );
		if( !id.isEmpty() ) {
			String name = nodeChild.getAttribute( "name" , "" );
			if( name.isEmpty() )
				value = createChildTableSection_getChildIdItem( nodeChild );
			else
				value = nameItem + " (" + createChildTableSection_getChildDetails( nodeChild ) + ")";
		}
		else
			value = nameItem;

		String function = nodeChild.getAttribute( "function" , "" );
		if( !function.isEmpty() )
			value += ": " + function;
		lines.add( prefix + value );

		createChildTableSection_addChilds( nodeChild , " " + prefix , lines );
	}
}

String WikiRegionPage::createChildTableSection_getChildNameItem( Xml node ) {
	String name = node.getAttribute( "name" , "" );
	String id = node.getAttribute( "id" , "" );
	if( id.isEmpty() )
		return( "*" + name + "*" );

	const XmlHMindElementInfo& child = wm -> hmindxml.getElementInfo( id );
	if( !child.isConnector() )
		return( "*" + name + "*" );

	XmlMuscleInfo *muscle = wm -> musclesxml.findByConnector( name );
	if( muscle == NULL )
		return( name );

	if( muscle -> link.isEmpty() )
		return( name );

	return( "[" + muscle -> link + " " + name + "]" );
}

String WikiRegionPage::createChildTableSection_getChildIdItem( Xml node ) {
	String name = node.getAttribute( "name" , "" );
	String id = node.getAttribute( "id" , "" );
	return( wm -> getMuscleReference( name , id ) );
}

String WikiRegionPage::createChildTableSection_getChildDetails( Xml node ) {
	String id = node.getAttribute( "id" , "" );
	const XmlHMindElementInfo& child = wm -> hmindxml.getElementInfo( id );
	if( !child.isConnector() )
		return( "*" + id + "*" );

	// find connector
	NeuroLinkSource *ns = region -> getNeuroLinkSource( id );
	NeuroLinkTarget *nt = NULL;
	if( ns == NULL ) {
		nt = region -> getNeuroLinkTarget( id );
		ASSERTMSG( nt != NULL , "createChildTableSection_getChildDetails: unexpected not found entity=" + id );

		ClassList<NeuroLink>& links = nt -> getLinks();
		ASSERTMSG( links.count() > 0 , "createChildTableSection_getChildDetails: unexpected missing links to connector=" + id );
		ASSERTMSG( links.count() == 1 , "createChildTableSection_getChildDetails: unexpected multiple links to connector=" + id );
		String regionId = links.getRef( 0 ).getSource() -> getRegion() -> getRegionId();

		return( wm -> getRegionReference( regionId ) + " -> " + createChildTableSection_getChildIdItem( node ) );
	}

	ClassList<NeuroLink>& links = ns -> getLinks();
	ASSERTMSG( links.count() > 0 , "createChildTableSection_getChildDetails: unexpected missing links to connector=" + id );
	ASSERTMSG( links.count() == 1 , "createChildTableSection_getChildDetails: unexpected multiple links to connector=" + id );
	String regionId = links.getRef( 0 ).getTarget() -> getRegion() -> getRegionId();

	return( createChildTableSection_getChildIdItem( node ) + " -> " + wm -> getRegionReference( regionId ) );
}

void WikiRegionPage::createConnectivitySection() {
	StringList lines;
	MapStringToClass<MindRegionLink> connections;
	MapStringToClass<MindRegion> regions;
	MapStringToClass<MindRegionLink> connectionsTotal;

	MindService *ms = MindService::getService();

	// image
	lines.add( "" );
	String dotImageWikiPath = wm -> wiki.getProperty( "imageWikiPath" );
	String line = dotImageWikiPath + "/" + info.id + ".dot.jpg";
	lines.add( line );

	// external connections - in
	lines.add( "" );
	connections.clear();
	createConnectivitySection_getExternalConnections( connections , true );
	connectionsTotal.add( connections );
	lines.add( "*Inbound Region Connections:*" );
	if( connections.count() == 0 ) {
		lines.add( "  * no connections" );
	}
	else {
		createConnectivitySection_getExternalConnectionTableLine( NULL , lines , true );
		for( int k = 0; k < connections.count(); k++ ) {
			MindRegionLink *c = connections.getClassByIndex( k );
			createConnectivitySection_getExternalConnectionTableLine( c , lines , true );
			regions.addnew( c -> getSrcRegion() -> getRegionId() , c -> getSrcRegion() );
		}
	}

	// external connections - out
	lines.add( "" );
	connections.clear();
	createConnectivitySection_getExternalConnections( connections , false );
	connectionsTotal.add( connections );
	lines.add( "*Outbound Region Connections:*" );
	if( connections.count() == 0 ) {
		lines.add( "  * no connections" );
	}
	else {
		createConnectivitySection_getExternalConnectionTableLine( NULL , lines , false );
		for( int k = 0; k < connections.count(); k++ ) {
			MindRegionLink *c = connections.getClassByIndex( k );
			createConnectivitySection_getExternalConnectionTableLine( c , lines , false );
			regions.addnew( c -> getDstRegion() -> getRegionId() , c -> getDstRegion() );
		}
	}

	String wikiPage = wm -> getRegionPage( info.id );
	String section = "Connectivity";
	wm -> updateFileSection( wikiDir , wikiPage , section , lines );

	// create dot file
	createDotFile( regions , connectionsTotal );
}

void WikiRegionPage::createConnectivitySection_getExternalConnections( MapStringToClass<MindRegionLink>& connections , bool isin ) {
	ClassList<MindRegionLink>& links = ( isin )? region -> getMasterRegionLinks() : region -> getSlaveRegionLinks();

	String key;
	for( int k = 0; k < links.count(); k++ ) {
		MindRegionLink *c = links.get( k );
		if( isin == false ) {
			key = c -> getDstRegion() -> getArea() -> getAreaId() + "#" + c -> getSrcRegion() -> getRegionId() + "#" + c -> getDstRegion() -> getRegionId() + "#1";
			if( connections.get( key ) == NULL )
				connections.add( key , c );
		}
		else {
			key = c -> getSrcRegion() -> getArea() -> getAreaId() + "#" + c -> getDstRegion() -> getRegionId() + "#" + c -> getSrcRegion() -> getRegionId() + "#2";
			if( connections.get( key ) == NULL )
				connections.add( key , c );
		}
	}
}

void WikiRegionPage::createConnectivitySection_getExternalConnectionTableLine( MindRegionLink *link , StringList& lines , bool isin ) {
	String line;
	String value1;
	if( link == NULL ) {
		// add heading
		lines.add( "" );

		if( isin )
			line = "|| *Source Area* || *Source Region* || *Source Name* || *Type* || *Reference* ||";
		else
			line = "|| *Target Area* || *Target Region* || *Target Name* || *Type* || *Reference* ||";
		lines.add( line );
		wm -> clearRepeats1( value1 );
		return;
	}

	// table row
	MindService *ms = MindService::getService();
	String srcRegionId = link -> getSrcRegion() -> getRegionId();
	String dstRegionId = link -> getDstRegion() -> getRegionId();

	String reference = wm -> findReference( link );
	if( isin == true ) {
		const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( srcRegionId );
		value1 = wm -> getAreaReference( link -> getSrcRegion() -> getArea() -> getAreaId() );
		wm -> clearRepeats1( value1 );
		line = "|| " + value1 + " || " + 
			wm -> getRegionReference( srcRegionId ) + " || " + info.name + " || " + link -> getConnectionType() -> getName() + " || " + reference + " ||";
	}
	else {
		const XmlHMindElementInfo& info = wm -> hmindxml.getElementInfo( dstRegionId );
		value1 = wm -> getAreaReference( link -> getDstRegion() -> getArea() -> getAreaId() );
		wm -> clearRepeats1( value1 );
		line = "|| " + value1 + " || " + 
			wm -> getRegionReference( dstRegionId ) + " || " + info.name + " || " + link -> getConnectionType() -> getName() + " || " + reference + " ||";
	}
	lines.add( line );
}

void WikiRegionPage::createThirdpartyAndReferencesSection() {
	// collect circuits which reference any of area regions
	MindService *ms = MindService::getService();

	StringList circuits;
	wm -> circuitsxml.getCircuitList( circuits );

	MindArea *area = region -> getArea();
	MapStringToString circuitKeys;
	for( int k = 0; k < circuits.count(); k++ ) {
		String circuitId = circuits.get( k );
		XmlCircuitInfo& infoPaired = wm -> circuitsxml.getCircuitInfo( circuitId );

		String key = createThirdpartyAndReferencesSection_getCircuitKey( region , infoPaired );
		if( key.isEmpty() )
			continue;

		circuitKeys.add( key , circuitId );
	}

	// add circuits section - sorted by relevance
	StringList lines;
	if( circuitKeys.count() > 0 ) {
		for( int k = circuitKeys.count() - 1; k >= 0; k-- ) {
			String circuitId = circuitKeys.getClassByIndex( k );
			XmlCircuitInfo& infoPaired = wm -> circuitsxml.getCircuitInfo( circuitId );
			createThirdpartyAndReferencesSection_getCircuitLines( infoPaired , lines );
		}
	}
	else {
		lines.add( "  * no circuits defined" );
	}

	String sectionName = "Thirdparty Circuits";
	String wikiPage = wm -> getRegionPage( info.id );
	wm -> updateFileSection( wikiDir , wikiPage , sectionName , lines );
	lines.clear();

	// add unique and sorted references - sorted by relevance
	if( circuitKeys.count() > 0 ) {
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
	}
	else {
		lines.add( "  * no references defined" );
	}

	sectionName = "References";
	wm -> updateFileSection( wikiDir , wikiPage , sectionName , lines );
}

String WikiRegionPage::createThirdpartyAndReferencesSection_getCircuitKey( MindRegion *region , XmlCircuitInfo& cinfo ) {
	// get circuit regions
	StringList comps;
	wm -> circuitsxml.getCircuitComponents( cinfo , comps );

	// check circuit mentions region
	String regionId = region -> getRegionId();
	for( int k = 0; k < comps.count(); k++ ) {
		String comp = comps.get( k );
		if( comp.equals( regionId ) )
			return( "0." + cinfo.id );

		// check whether circuit component is region part
		String regionComp = wm -> hmindxml.getMappedRegion( comp );
		if( !regionComp.isEmpty() )
			if( regionComp.equals( regionId ) )
				return( "1." + cinfo.id );
	}

	return( "" );
}

void WikiRegionPage::createThirdpartyAndReferencesSection_getCircuitLines( XmlCircuitInfo& cinfo , StringList& lines ) {
	String ref;
	if( cinfo.reference.equals( "UNKNOWN" ) )
		ref = "(unknown reference)";
	else
		ref = "- see [" + cinfo.reference + " Reference]";

	lines.add( "  * [" + cinfo.image + " " + cinfo.name + "] " + ref );
	lines.add( "" );
	String sizeInfo;
	if( cinfo.imageHeight > 0 )
		sizeInfo += String( "height=" ) + cinfo.imageHeight;
	if( cinfo.imageWidth > 0 )
		sizeInfo += String( "width=" ) + cinfo.imageWidth;

	lines.add( "<img src=\"" + cinfo.image + "\" alt=\"unavailable\"" + sizeInfo + ">" );
	lines.add( "" );
}

void WikiRegionPage::createDotFile( MapStringToClass<MindRegion>& regions , MapStringToClass<MindRegionLink>& connectionsTotal ) {
	String dotDir = wm -> wiki.getProperty( "dotPath" );
	String fileName = dotDir + "/" + info.id + ".dot";
	StringList text;

	// header
	text.add( "digraph \"" + info.id.replace( "." , "_" ) + "\" {" );
	String defaultDotSetup = wm -> wiki.getProperty( "defaultDotSetup" );
	text.add( wm -> setSpecialCharacters( defaultDotSetup ) );
	text.add( "\trankdir=LR;" );
	text.add( "" );

	// main item
	String defaultDotRegionSetup = wm -> wiki.getProperty( "defaultDotRegionSetup" );
	text.add( "\t\"" + info.id + "\" [" + wm -> setSpecialCharacters( defaultDotRegionSetup ) + "];" );
	text.add( "" );

	// areas
	MapStringToClass<MindArea> areas;
	for( int k = 0; k < regions.count(); k++ ) {
		MindRegion *region = regions.getClassByIndex( k );
		MindArea *area = region -> getArea();
		areas.addnew( area -> getAreaId() , area );
	}

	// list nodes by area
	for( int k = 0; k < areas.count(); k++ ) {
		MindArea *area = areas.getClassByIndex( k );

		// area
		text.add( "\tsubgraph cluster_" + area -> getAreaId() + " {" );
		text.add( "\tlabel=\"" + area -> getAreaId() + "\";" );
		text.add( "\t\"" + area -> getAreaId() + "\" [ shape=box, style=filled , label = <" );
		text.add( "\t\t<TABLE BORDER=\"0\" CELLBORDER=\"1\" CELLSPACING=\"0\" CELLPADDING=\"4\">" );

		// regions
		XmlHMindElementInfo infoPair;
		for( int k = 0; k < regions.count(); k++ ) {
			MindRegion *region = regions.getClassByIndex( k );
			if( region -> getArea() != area )
				continue;

			// ignore self
			String regionPaired = region -> getRegionId();
			if( info.id.equals( regionPaired ) )
				continue;

			String dotdef = wm -> hmindxml.getDotDef( regionPaired );
			String color;
			if( !dotdef.isEmpty() ) {
				String s = wm -> setSpecialCharacters( dotdef );
				color = wm -> getDotColor( dotdef );
				if( !color.isEmpty() )
					color = " BGCOLOR=\"" + color + "\"";
			}
			String nodeline = "\t\t<TR><TD PORT=\"" + regionPaired + "\"" + color + ">" + regionPaired + "</TD></TR>";
			text.add( nodeline );
		}

		text.add( "\t\t</TABLE>> ];" );
		text.add( "\t}" );
		text.add( "" );
	}

	// list connections
	for( int k = 0; k < connectionsTotal.count(); k++ ) {
		MindRegionLink *c = connectionsTotal.getClassByIndex( k );

		String src = c -> getSrcRegion() -> getRegionId();
		String dst = c -> getDstRegion() -> getRegionId();
		if( info.id.equals( src ) ) {
			MindRegion *region = regions.get( dst );
			src = "\"" + src + "\"";
			dst = "\"" + region -> getArea() -> getAreaId() + "\":" + "\"" + dst + "\"";
		}
		else {
			MindRegion *region = regions.get( src );
			src = "\"" + region -> getArea() -> getAreaId() + "\":" + "\"" + src + "\"";
			dst = "\"" + dst + "\"";
		}

		String linkline = "\t" + src + " -> " + dst + ";";
		text.add( linkline );
	}

	// footer
	text.add( "}" );

	// out to file
	wm -> createFileContent( fileName , text );
}

