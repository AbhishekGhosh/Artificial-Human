#include "stdafx.h"
#include "xmlhuman.h"

/*#########################################################################*/
/*#########################################################################*/

XmlNerves::XmlNerves() {
}

XmlNerves::~XmlNerves() {
	nodes.destroy();
	nerves.destroy();
}

void XmlNerves::load() {
	if( divisions.count() > 0 )
		return;

	// read circuits
	EnvService *es = EnvService::getService();
	Xml xml = es -> loadXml( "hpns.xml" );
	ASSERTMSG( xml.exists() , "unable to read file hpns.xml" );
	Xml xmlNerves = xml.getChildNamedNode( "category" , "nerves" );
	ASSERTMSG( xmlNerves.exists() , "unable to read nerve divisions" );

	// scan
	for( Xml xmlDivision = xmlNerves.getFirstChild( "division" ); xmlDivision.exists(); xmlDivision = xmlDivision.getNextChild( "division" ) ) {
		String name = xmlDivision.getAttribute( "name" );
		String file = xmlDivision.getAttribute( "xmlfile" );
		String page = xmlDivision.getAttribute( "page" );

		Xml xmlFile = es -> loadXml( file );
		Xml xmlDivisionFile = xmlFile.getChildNamedNode( "division" , name );
		ASSERTMSG( xmlDivisionFile.exists() , "unable to read division=" + name + " from file=" + file );
		bool tree = xmlDivisionFile.getBooleanAttribute( "tree" , true );

		XmlNerveInfo *division = new XmlNerveInfo;
		division -> div = NULL;
		division -> name = name;
		division -> xml = xmlDivisionFile;
		division -> origin = page;
		division -> tree = tree;
		divisions.add( division );

		addChilds( division , division );
	}
}

void XmlNerves::addChilds( XmlNerveInfo *division , XmlNerveInfo *parent ) {
	for( Xml item = parent -> xml.getFirstChild( "element" ); item.exists(); item = item.getNextChild( "element" ) ) {
		String id = item.getAttribute( "name" );
		ASSERTMSG( nodes.get( id ) == NULL , "Duplicate nerve division=" + division -> name + ", name=" + id );

		// add item
		nodes.addnew( id , new Xml( item ) );
		XmlNerveInfo *nerveInfo = createNerveInfo( id , item , division );
		nerves.add( id , nerveInfo );
		parent -> childs.add( id , nerveInfo );

		addChilds( division , nerveInfo );
	}
}

void XmlNerves::getNerveList( StringList& list ) {
	// scan
	for( int k = 0; k < nodes.count(); k++ ) {
		String id = nodes.getKeyByIndex( k );
		list.add( id );
	}
}

Xml XmlNerves::getNerveXml( String id ) {
	Xml *nodeXml = nodes.get( id );
	ASSERTMSG( nodeXml != NULL , "unable to find circuit with ID=" + id );

	return( *nodeXml );
}

XmlNerveInfo& XmlNerves::getNerveInfo( String nerve ) {
	XmlNerveInfo *pinfo = nerves.get( nerve );
	ASSERTMSG( pinfo != NULL , "unable to find nerve=" + nerve );
	return( *pinfo );
}

bool XmlNerves::checkNerve( String nerve ) {
	XmlNerveInfo *pinfo = nerves.get( nerve );
	if( pinfo == NULL )
		return( false );
	return( true );
}

XmlNerveInfo *XmlNerves::createNerveInfo( String nerve , Xml xmlitem , XmlNerveInfo *div ) {
	XmlNerveInfo *pinfo = new XmlNerveInfo;

	XmlNerveInfo& info = *pinfo;
	info.xml = xmlitem;
	info.div = div;

	info.name = xmlitem.getAttribute( "name" );
	info.type = xmlitem.getAttribute( "type" , "" );
	info.synonyms = xmlitem.getAttribute( "synonyms" , "" );
	info.origin = xmlitem.getAttribute( "origin" , "" );
	info.branches = xmlitem.getAttribute( "branches" , "" );
	info.distribution = xmlitem.getAttribute( "distribution" , "" );
	info.imgsrc = xmlitem.getAttribute( "imgsrc" , "" );
	info.imgheight = xmlitem.getAttribute( "imgheight" , "" );
	info.modality = xmlitem.getAttribute( "modality" , "" );
	info.modality.split( info.mods , "," );
	info.mods.trim();
	info.action = xmlitem.getAttribute( "action" , "" );
	
	ASSERTMSG( !info.modality.isEmpty() , "createNerveInfo: nerve=" + info.name + ", modality is not defined" );

	// data integrity
	for( int k = 0; k < info.mods.count(); k++ ) {
		String nm = info.mods.get( k );

		ASSERTMSG( nm.equals( "general sensory" ) || nm.equals( "cranial motor" ) || nm.equals( "visceral motor" ) || 
			nm.equals( "sympathetic motor" ) || nm.equals( "parasympathetic motor" ) || 
			nm.equals( "autonomic sensory" ) || nm.equals( "special sensory" ) || nm.equals( "flexor motor" ) || nm.equals( "extensor motor" ) ,
			"createNerveInfo: nerve=" + info.name + ", unknown modality=" + nm );
	}

	for( Xml item = xmlitem.getFirstChild( "fibers" ); item.exists(); item = item.getNextChild( "fibers" ) ) {
		XmlNerveFiberInfo *nf = new XmlNerveFiberInfo;
		info.fibers.add( nf );

		nf -> type = item.getAttribute( "type" );
		nf -> src = item.getAttribute( "src" );
		nf -> dst = item.getAttribute( "dst" );

		// data integrity
		String nt = nf -> type;
		ASSERTMSG( nt.equals( "GSE" ) || nt.equals( "GSA" ) || nt.equals( "GVE" ) || nt.equals( "GVA" ) || nt.equals( "SVA" ) || nt.equals( "SVE" ) || nt.equals( "SSA" ) ,
			"createNerveInfo: nerve=" + info.name + ", unknown fiber type=" + nt );

		for( Xml mid = item.getFirstChild( "mid" ); mid.exists(); mid = mid.getNextChild( "mid" ) ) {
			String id = mid.getAttribute( "id" );
			nf -> mids.add( id );
		}
	}

	// root nerves
	for( Xml item = xmlitem.getFirstChild( "nerve" ); item.exists(); item = item.getNextChild( "nerve" ) ) {
		String nerve = item.getAttribute( "name" );
		info.rootNerves.add( nerve );
	}

	return( pinfo );
}

XmlNerveInfo& XmlNerves::getNerveDivision( String nerve ) {
	XmlNerveInfo& info = getNerveInfo( nerve );
	return( ( XmlNerveInfo& )*info.div );
}
