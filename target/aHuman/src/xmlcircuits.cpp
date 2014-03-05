#include "stdafx.h"
#include "xmlhuman.h"

/*#########################################################################*/
/*#########################################################################*/

XmlCircuits::XmlCircuits() {
}

XmlCircuits::~XmlCircuits() {
	nodes.destroy();
	circuits.destroy();
}

void XmlCircuits::load() {
	// read circuits
	EnvService *es = EnvService::getService();
	xml = es -> loadXml( "circuits.xml" );
	ASSERTMSG( xml.exists() , "unable to read file circuits.xml" );

	// scan
	for( Xml xmlCategory = xml.getFirstChild( "category" ); xmlCategory.exists(); xmlCategory = xmlCategory.getNextChild( "category" ) ) {
		for( Xml xmlChild = xmlCategory.getFirstChild( "circuit" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "circuit" ) ) {
			String id = xmlChild.getAttribute( "id" , "" );
			bool ignore = xmlChild.getBooleanAttribute( "ignore" , false );

			if( ignore == true )
				continue;
			
			ASSERTMSG( !id.isEmpty() , "circuit is not ignored but has no ID attribute" );

			// get direct of from separate file
			Xml xmlsrc = xmlChild;
			String file = xmlChild.getAttribute( "file" , "" );
			if( !file.isEmpty() )
				xmlsrc = getCircuitFromFile( id , file );

			Xml *xmlitem = new Xml( xmlsrc );
			nodes.add( id , xmlitem );
		}
	}
}

Xml XmlCircuits::getCircuitFromFile( String id , String file ) {
	// load	file
	EnvService *es = EnvService::getService();
	Xml xmlfile = es -> loadXml( file );
	ASSERTMSG( xmlfile.exists() , "unable to open circuit file " + file );

	// find circuit in file
	for( Xml xmlChild = xmlfile.getFirstChild( "circuit" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "circuit" ) ) {
		String itemid = xmlChild.getAttribute( "id" , "" );
		if( itemid.equals( id ) )
			return( xmlChild );
	}

	return( Xml() );
}

void XmlCircuits::getCircuitList( StringList& circuits ) {
	// scan
	for( int k = 0; k < nodes.count(); k++ ) {
		String id = nodes.getKeyByIndex( k );
		circuits.add( id );
	}
}

Xml XmlCircuits::getCircuitXml( String id ) {
	Xml *nodeXml = nodes.get( id );
	ASSERTMSG( nodeXml != NULL , "unable to find circuit with ID=" + id );

	return( *nodeXml );
}

XmlCircuitInfo& XmlCircuits::getCircuitInfo( String circuit ) {
	XmlCircuitInfo *pinfo = circuits.get( circuit );
	if( pinfo != NULL )
		return( *pinfo );

	pinfo = new XmlCircuitInfo;
	circuits.add( circuit , pinfo );

	XmlCircuitInfo& info = *pinfo;
	Xml xmlitem = getCircuitXml( circuit );
	info.xml = xmlitem;

	info.id = xmlitem.getAttribute( "id" , "" );
	info.ignore = xmlitem.getBooleanAttribute( "ignore" , false );
	info.name = xmlitem.getAttribute( "name" , "" );
	info.image = xmlitem.getAttribute( "image" , "" );
	info.reference = xmlitem.getAttribute( "reference" , "" );
	info.imageHeight = xmlitem.getIntAttribute( "imageHeight" , 0 );
	info.imageWidth = xmlitem.getIntAttribute( "imageWidth" , 0 );

	info.componentMapping.destroy();
	String mapping = xmlitem.getAttribute( "components" , "" );
	StringList pairs;
	mapping.split( pairs , " " );
	for( int k = 0; k < pairs.count(); k++ ) {
		String pair = pairs.get( k );
		pair.trim();

		StringList parts;
		pair.split( parts , "=" );
		ASSERTMSG( parts.count() == 2 , "invalid format pair=" + pair );
		
		info.componentMapping.add( parts.get( 0 ) , parts.get( 1 ) );
	}

	return( info );
}

void XmlCircuits::getCircuitLinks( String circuit , FlatList<Xml>& links ) {
	Xml xmlitem = getCircuitXml( circuit );
	for( Xml xmlChild = xmlitem.getFirstChild( "link" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "link" ) )
		links.add( xmlChild );
}

void XmlCircuits::getCircuitLinkInfo( Xml link , XmlCircuitLinkInfo& info ) {
	info.compSrc = link.getAttribute( "src" );
	info.compDst = link.getAttribute( "dst" );
	info.function = link.getAttribute( "function" , "" );
}

String XmlCircuits::mapComponent( XmlCircuitInfo& circuit , String circuitComponent ) {
	String value = circuit.componentMapping.get( circuitComponent );
	ASSERTMSG( !value.isEmpty() , "unknown circuit link component=" + circuitComponent );
	return( value );
}

bool XmlCircuits::checkRegionUsedByCircuit( String region , String circuit ) {
	// region is covered by circuit if region is exactly mentioned in circuit
	XmlCircuitInfo& info = getCircuitInfo( circuit );

	// check amond mapping
	for( int k = 0; k < info.componentMapping.count(); k++ )
		if( region.equals( info.componentMapping.getClassByIndex( k ) ) )
			return( true );

	return( false );
}

void XmlCircuits::getCircuitComponents( XmlCircuitInfo& info , StringList& components ) {
	for( int k = 0; k < info.componentMapping.count(); k++ ) {
		String component = info.componentMapping.getClassByIndex( k );
		if( components.find( component ) < 0 )
			components.add( component );
	}
}

bool XmlCircuits::findReferenceLink( XmlHMind& hmindxml , String srcRegion , String dstRegion , XmlCircuitFind& find ) {
	// check by map
	String key = srcRegion + "." + dstRegion;
	XmlCircuitFind *xfind = referenceMap.get( key );
	if( xfind != NULL ) {
		find = *xfind;
		return( true );
	}

	xfind = new XmlCircuitFind;

	// scan thirdparty circuits - check specific coverage
	StringList circuits;
	getCircuitList( circuits );
	bool found = false;
	bool indirect = false;
	for( int k = 0; k < circuits.count(); k++ ) {
		XmlCircuitInfo& info = getCircuitInfo( circuits.get( k ) );
		if( findReferenceCircuitLink( hmindxml , srcRegion , dstRegion , info , xfind , true ) ) {
			xfind -> circuit = &info;
			found = true;
			break;
		}
	}

	// scan thirdparty circuits - check high-level coverage
	if( found == false ) {
		for( int k = 0; k < circuits.count(); k++ ) {
			XmlCircuitInfo& info = getCircuitInfo( circuits.get( k ) );
			if( findReferenceCircuitLink( hmindxml , srcRegion , dstRegion , info , xfind , false ) ) {
				xfind -> circuit = &info;
				found = true;
				break;
			}
		}
	}

	// add to map and return
	referenceMap.add( key , xfind );
	find = *xfind;
	return( found );
}

bool XmlCircuits::findReferenceCircuitLink( XmlHMind& hmindxml , String checkSrcRegion , String checkDstRegion , XmlCircuitInfo& info , XmlCircuitFind *find , bool directOnly ) {
	FlatList<Xml> links;
	XmlCircuitLinkInfo linkinfo;
	getCircuitLinks( info.id , links );

	// find covering link
	for( int k = 0; k < links.count(); k++ ) {
		getCircuitLinkInfo( links.get( k ) , linkinfo );
		String srcComponent = mapComponent( info , linkinfo.compSrc );
		String dstComponent = mapComponent( info , linkinfo.compDst );

		if( directOnly ) {
			String srcRegion = hmindxml.getMappedRegion( srcComponent );
			String dstRegion = hmindxml.getMappedRegion( dstComponent );
			if( srcRegion.equals( checkSrcRegion ) && dstRegion.equals( checkDstRegion ) ) {
				find -> link = linkinfo;
				find -> isAbstractLink = false;
				return( true );
			}
		}
		else {
			if( hmindxml.checkAbstractLinkCoveredByRegionLink( srcComponent , dstComponent , checkSrcRegion , checkDstRegion ) ) {
				find -> link = linkinfo;
				find -> isAbstractLink = true;
				return( true );
			}
		}
	}

	return( false );
}

