#include "stdafx.h"
#include "xmlhuman.h"

/*#########################################################################*/
/*#########################################################################*/

XmlMuscles::XmlMuscles() {
}

XmlMuscles::~XmlMuscles() {
	muscles.destroy();
	divisions.destroy();
	nerveMuscles.destroy();
}

void XmlMuscles::load() {
	if( divisions.count() > 0 )
		return;

	// read circuits
	EnvService *es = EnvService::getService();
	Xml xml = es -> loadXml( "hpns.xml" );
	ASSERTMSG( xml.exists() , "unable to read file hpns.xml" );
	Xml xmlMuscles = xml.getChildNamedNode( "category" , "muscles" );
	ASSERTMSG( xmlMuscles.exists() , "unable to read muscle divisions" );

	// scan
	for( Xml xmlDivision = xmlMuscles.getFirstChild( "division" ); xmlDivision.exists(); xmlDivision = xmlDivision.getNextChild( "division" ) ) {
		String category = xmlDivision.getAttribute( "category" );
		String name = xmlDivision.getAttribute( "name" );
		String file = xmlDivision.getAttribute( "xmlfile" );
		String page = xmlDivision.getAttribute( "page" );

		Xml xmlFile = es -> loadXml( file );
		Xml xmlDivisionFile = xmlFile.getChildNamedNode( "division" , name );
		ASSERTMSG( xmlDivisionFile.exists() , "file does not contain division=" + name );

		String divc = xmlDivisionFile.getAttribute( "category" );
		String divgroup = xmlDivisionFile.getAttribute( "group" , "" );

		XmlMuscleDivision *division = new XmlMuscleDivision;
		division -> name = name;
		division -> category = category;
		division -> xml = xmlDivisionFile;
		division -> page = page;
		division -> divc = divc;
		division -> divgroup = divgroup;
		divisions.add( name , division );

		addChilds( division , xmlDivisionFile , division -> childs );
	}
}

void XmlMuscles::addNerveMuscles( XmlMuscleInfo *muscle ) {
	for( int k = 0; k < muscle -> nerves.count(); k++ ) {
		String nerve = muscle -> nerves.getKeyByIndex( k );
		if( !nerve.isEmpty() ) {
			StringList *muscles = nerveMuscles.get( nerve );
			if( muscles == NULL ) {
				muscles = new StringList;
				nerveMuscles.add( nerve , muscles );
			}

			muscles -> add( muscle -> name );
		}
	}
}

void XmlMuscles::addChilds( XmlMuscleDivision *division , Xml parent , MapStringToClass<XmlMuscleInfo>& list ) {
	for( Xml item = parent.getFirstChild( "element" ); item.exists(); item = item.getNextChild( "element" ) ) {
		String id = item.getAttribute( "name" );
		ASSERTMSG( muscles.get( id ) == NULL , "Duplicate muscle division=" + division -> name + ", name=" + id );

		// add item
		XmlMuscleInfo *muscleInfo = createMuscleInfo( division , id , item );
		muscles.add( id , muscleInfo );
		list.add( id , muscleInfo );

		addNerveMuscles( muscleInfo );

		addChilds( division , item , muscleInfo -> childs );
	}
}

void XmlMuscles::getMusclesByNerve( String nerve , StringList& muscles ) {
	muscles.clear();

	StringList *pmuscles = nerveMuscles.get( nerve );
	if( pmuscles == NULL )
		return;

	muscles.add( pmuscles );
}

void XmlMuscles::getMuscleList( StringList& list ) {
	// scan
	for( int k = 0; k < muscles.count(); k++ ) {
		String id = muscles.getKeyByIndex( k );
		list.add( id );
	}
}

XmlMuscleInfo& XmlMuscles::getMuscleInfo( String muscle ) {
	XmlMuscleInfo *pinfo = muscles.get( muscle );
	ASSERTMSG( pinfo != NULL , "unable to find muscle=" + muscle );
	return( *pinfo );
}

XmlMuscleInfo *XmlMuscles::createMuscleInfo( XmlMuscleDivision *division , String muscle , Xml xmlitem ) {
	XmlMuscleInfo *pinfo = new XmlMuscleInfo;

	XmlMuscleInfo& info = *pinfo;
	info.xml = xmlitem;

	info.division = division;
	info.name = xmlitem.getAttribute( "name" );
	info.type = xmlitem.getAttribute( "type" , "" );
	info.link = xmlitem.getAttribute( "link" , "" );
	info.action = xmlitem.getAttribute( "action" , "" );

	String nerves = xmlitem.getAttribute( "nerves" , "" );
	if( nerves.isEmpty() )
		return( pinfo );

	if( nerves.equals( "INTERNAL" ) ) {
		muscleConnectors.addnew( muscle , pinfo );
	}
	else {
		StringList pairs;
		nerves.split( pairs, ";" );

		String pair;
		String nerve;
		String connectordet;
		for( int k = 0; k < pairs.count(); k++ ) {
			String pair = pairs.get( k );
			pair.trim();

			int pos = pair.find( ":" );
			if( pos < 0 ) {
				nerve = pair;
				connectordet.clear();
			}
			else {
				nerve = pair.getMid( 0 , pos );
				connectordet = pair.getMid( pos + 1 );
			}

			nerve.trim();
			connectordet.trim();
			if( connectordet.isEmpty() ) {
				connectordet = "default";
				muscleConnectors.addnew( muscle , pinfo );
			}
			else {
				String connector = muscle + ":" + connectordet;
				muscleConnectors.addnew( connector , pinfo );
			}

			info.nerves.add( nerve , connectordet );
		}
	}

	// data integrity
	ASSERTMSG( info.type.equals( "flexor" ) || info.type.equals( "extensor" ) || info.type.equals( "cranial" ) || info.type.equals( "gland" ) ,
		"createMuscleInfo: muscle=" + info.name + " - invalid muscle type=" + info.type );

	return( pinfo );
}

void XmlMuscles::getCategories( StringList& categories ) {
	MapStringToString cmap;

	for( int k = 0; k < divisions.count(); k++ ) {
		XmlMuscleDivision& div = divisions.getClassRefByIndex( k );
		if( cmap.get( div.category ) == NULL ) {
			cmap.add( div.category , div.divc );
			categories.add( div.category );
		}
	}

	categories.sort();
}

XmlMuscleInfo *XmlMuscles::findByConnector( String name ) {
	return( muscleConnectors.get( name ) );
}
