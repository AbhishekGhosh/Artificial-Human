#include "stdafx.h"
#include "xmlhuman.h"

/*#########################################################################*/
/*#########################################################################*/

XmlHMind::XmlHMind() {
	layout = new XmlSpinalCordLayout( this );
}

XmlHMind::~XmlHMind() {
	nodeInfo.destroy();
	delete layout;
}

void XmlHMind::load() {
	// read hierarchy
	EnvService *es = EnvService::getService();
	xml = es -> loadXml( "hmind.xml" );
	ASSERTMSG( xml.exists() , "unable to read file hmind.xml" );

	// scan
	for( Xml xmlChild = xml.getFirstChild( "division" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "division" ) ) {
		if( xmlChild.getBooleanAttribute( "ignore" , false ) )
			continue;

		createDivisionElement( xmlChild );
	}
}

void XmlHMind::createDivisionElement( Xml item ) {
	String mapId = item.getAttribute( "name" );
	String xmlFileName = item.getAttribute( "xmlfile" );

	EnvService *es = EnvService::getService();
	Xml xmlFile = es -> loadXml( xmlFileName );
	ASSERTMSG( xmlFile.exists() , "unable to read file " + xmlFileName );
	Xml div = xmlFile.getChildNamedNode( "division" , mapId );
	ASSERTMSG( div.exists() , "unable to find division name=" + mapId );

	XmlHMindElementInfo *info = new XmlHMindElementInfo();
	createElementInfo( mapId , div , *info );
	nodeInfo.add( mapId , info );

	scanChildItems( div );

	if( mapId.equals( "Spinal Cord" ) )
		layout -> load( div );
}

void XmlHMind::scanChildItems( Xml xmlItem ) {
	// childs
	for( Xml xmlChild = xmlItem.getFirstChild( "element" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "element" ) ) {
		if( xmlChild.getBooleanAttribute( "ignore" , false ) )
			continue;

		createRegionElement( xmlChild );
		scanChildItems( xmlChild );
	}
}

Xml XmlHMind::getNodeXml( String node ) {
	XmlHMindElementInfo *info = nodeInfo.get( node );
	ASSERTMSG( info != NULL , "unable to find node with ID=" + node );
	return( info -> xml );
}

bool XmlHMind::isComponent( String node ) {
	XmlHMindElementInfo *info = nodeInfo.get( node );
	ASSERTMSG( info != NULL , "unable to find node with ID=" + node );

	if( info -> id.isEmpty() )
		return( false );
	return( true );
}

void XmlHMind::getDivisions( StringList& divisions ) {
	for( Xml xmlChild = xml.getFirstChild( "division" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "division" ) ) {
		if( xmlChild.getBooleanAttribute( "ignore" , false ) )
			continue;

		String name = xmlChild.getAttribute( "name" );
		divisions.add( name );
	}
}

void XmlHMind::getElements( String parentNode , StringList& elements ) {
	Xml xmlParent = getNodeXml( parentNode );
	for( Xml xmlChild = xmlParent.getFirstChild( "element" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "element" ) ) {
		if( xmlChild.getBooleanAttribute( "ignore" , false ) )
			continue;

		String mapId = getRegionMapId( xmlChild );
		elements.add( mapId );
	}
}

void XmlHMind::getConnectors( String parentNode , StringList& elements ) {
	Xml xmlParent = getNodeXml( parentNode );
	for( Xml xmlChild = xmlParent.getFirstChild( "element" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "element" ) ) {
		if( xmlChild.getBooleanAttribute( "ignore" , false ) )
			continue;

		String id = xmlChild.getAttribute( "id" , "" );
		if( !id.isEmpty() ) {
			String type = xmlChild.getAttribute( "type" , "" );
			if( type.equals( "connector" ) )
				elements.add( id );
			else
				getConnectors( id , elements );
		}
	}
}

void XmlHMind::getIdentifiedElements( String parentNode , StringList& elements ) {
	Xml xmlParent = getNodeXml( parentNode );
	for( Xml xmlChild = xmlParent.getFirstChild( "element" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "element" ) ) {
		if( xmlChild.getBooleanAttribute( "ignore" , false ) )
			continue;

		String id = xmlChild.getAttribute( "id" , "" );
		if( !id.isEmpty() )
			elements.add( id );
	}
}

String XmlHMind::getRegionMapId( Xml item ) {
	String mapId = item.getAttribute( "id" , "" );
	if( mapId.isEmpty() )
		mapId = String( "auto-" ) + ( unsigned )item.getNode();
	return( mapId );
}

String XmlHMind::createRegionElement( Xml item ) {
	String mapId = getRegionMapId( item );
	XmlHMindElementInfo *info = new XmlHMindElementInfo();
	createElementInfo( mapId , item , *info );
	nodeInfo.add( mapId , info );

	// add connectors to the map
	if( info -> type.equals( "connector" ) )
		connectorInfo.add( info -> name , info );

	return( mapId );
}

void XmlHMind::createElementInfo( String mapId , Xml item , XmlHMindElementInfo& info ) {
	info.mapId = mapId;
	info.xml = item;

	info.id = item.getAttribute( "id" , "" );
	info.index = item.getAttribute( "index" , "" );
	if( !info.index.isEmpty() )
		mapIndex.add( info.index , &info );

	info.ignore = item.getBooleanAttribute( "ignore" , false );
	info.mapped = item.getBooleanAttribute( "mapped" , false );
	info.name = item.getAttribute( "name" , "" );
	ASSERTMSG( !info.name.isEmpty() , "name attribute is not set for item xpath=" + item.getXPath() );

	info.refs = item.getAttribute( "refs" , "" );
	info.comment = item.getAttribute( "comment" , "" );
	info.brodmannid = item.getAttribute( "brodmannid" , "" );
	info.type = item.getAttribute( "type" , "" );
	info.function = item.getAttribute( "function" , "" );
	info.notes = item.getAttribute( "notes" , "" );
	info.dotdef = item.getAttribute( "dotdef" , "" );

	if( info.brodmannid.isEmpty() )
		info.batype = info.type;
	else
		info.batype = info.type + ", BA " + info.brodmannid;
}

const XmlHMindElementInfo& XmlHMind::getElementInfo( String node ) {
	XmlHMindElementInfo *ni = nodeInfo.get( node );
	return( *ni );
}

const XmlHMindElementInfo *XmlHMind::getConnectorInfo( String name ) {
	XmlHMindElementInfo *ni = connectorInfo.get( name );
	return( ni );
}

XmlHMindElementInfo *XmlHMind::getIndexedElement( String index ) {
	return( mapIndex.get( index ) );
}

String XmlHMind::getMappedRegion( String node ) {
	Xml item = getNodeXml( node );
	while( item.exists() ) {
		if( item.getBooleanAttribute( "mapped" , false ) == true )
			return( item.getAttribute( "id" ) );

		// go up
		item = item.getParentNode();
	}

	return( "" );
}

void XmlHMind::getChildRegions( String node , StringList& regions ) {
	Xml item = getNodeXml( node );
	scanChildRegions( item , regions );
}

void XmlHMind::scanChildRegions( Xml item , StringList& regions ) {
	for( Xml xmlChild = item.getFirstChild( "element" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "element" ) ) {
		if( xmlChild.getBooleanAttribute( "ignore" , false ) )
			continue;

		String id = xmlChild.getAttribute( "id" , "" );
		if( id.isEmpty() )
			continue;

		if( xmlChild.getBooleanAttribute( "mapped" , false ) == true )
			regions.add( id );
		else
			scanChildRegions( xmlChild , regions );
	}
}

String XmlHMind::getDotDef( String node ) {
	Xml item = getNodeXml( node );
	while( item.exists() ) {
		String dotdef = item.getAttribute( "dotdef" , "" );
		if( !dotdef.isEmpty() )
			return( dotdef );

		// go up
		item = item.getParentNode();
	}

	return( "" );
}

bool XmlHMind::checkAbstractLinkCoveredByRegionLink( String compSrc , String compDst , String linkRegionSrc , String linkRegionDst ) {
	// find mapped regions
	String regionSrcId = getMappedRegion( compSrc );
	String regionDstId = getMappedRegion( compDst );
	if( regionSrcId.isEmpty() == false && regionDstId.isEmpty() == false )
		return( regionSrcId.equals( linkRegionSrc ) && regionDstId.equals( linkRegionDst ) );

	// collect sets of child regions
	StringList srcRegions;
	if( regionSrcId.isEmpty() )
		getChildRegions( compSrc , srcRegions );
	else
		srcRegions.add( regionSrcId );

	StringList dstRegions;
	if( regionDstId.isEmpty() )
		getChildRegions( compDst , dstRegions );
	else
		dstRegions.add( regionDstId );

	// check mapping
	for( int k1 = 0; k1 < srcRegions.count(); k1++ ) {
		regionSrcId = srcRegions.get( k1 );
		for( int k2 = 0; k2 < dstRegions.count(); k2++ ) {
			regionDstId = dstRegions.get( k2 );
			if( regionSrcId.equals( linkRegionSrc ) && regionDstId.equals( linkRegionDst ) )
				return( true );
		}
	}

	return( false );
}

