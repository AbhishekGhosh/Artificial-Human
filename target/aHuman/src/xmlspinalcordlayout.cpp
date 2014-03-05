#include "stdafx.h"
#include "xmlhuman.h"

/*#########################################################################*/
/*#########################################################################*/

static const char *levels[32] = {
	"C1" , "C2" , "C3" , "C4" , "C5" , "C6" , "C7" , "C8" ,
	"T1" , "T2" , "T3" , "T4" , "T5" , "T6" , "T7" , "T8" , "T9" , "T10" , "T11" , "T12" ,
	"L1" , "L2" , "L3" , "L4" , "L5" , 
	"S1" , "S2" , "S3" , "S4" , "S5" , 
	"CX" ,
	NULL
};

static const char *laminas[12] = {
	"LO" ,
	"LI" , "LII" , "LIII" , "LIV" , "LV" , "LVI" , "LVII" , "LVIII" , "LIX" , "LX" ,
	NULL
};

XmlSpinalCordLayout::XmlSpinalCordLayout( XmlHMind *p_hmind ) {
	hmind = p_hmind;

	for( int k = 0; levels[ k ] != NULL; k++ ) {
		MapStringToClass<StringList> *levelData = new MapStringToClass<StringList>;
		data.add( levels[ k ] , levelData );

		for( int m = 0; laminas[ m ] != NULL; m++ )
			levelData -> add( laminas[ m ] , new StringList );
	}
}

XmlSpinalCordLayout::~XmlSpinalCordLayout() {
	for( int k = 0; k < data.count(); k++ ) {
		MapStringToClass<StringList>& item = data.getClassRefByIndex( k );
		item.destroy();
	}
	data.destroy();
}

void XmlSpinalCordLayout::load( Xml xmlDiv ) {
	Xml layout = xmlDiv.getFirstChild( "layout" );
	ASSERTMSG( layout.exists() , "layout is not found" );

	imgSrc = layout.getAttribute( "imgsrc" , "" );

	for( Xml xmlChild = layout.getFirstChild( "level" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "level" ) ) {
		String id = xmlChild.getAttribute( "id" );
		MapStringToClass<StringList>& levelData = data.getRef( id );

		loadLevel( xmlChild , levelData );
	}
}

void XmlSpinalCordLayout::loadLevel( Xml xmlLevel , MapStringToClass<StringList>& levelData ) {
	for( Xml xmlChild = xmlLevel.getFirstChild( "lamina" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "lamina" ) ) {
		String id = xmlChild.getAttribute( "id" );
		String elements = xmlChild.getAttribute( "elements" );

		StringList& items = levelData.getRef( id );
		StringList parts;
		elements.split( parts , "," );
		for( int k = 0; k < parts.count(); k++ ) {
			String element = parts.get( k );
			ASSERTMSG( hmind -> getIndexedElement( element ) != NULL , "unknown element index=" + element );

			if( items.find( element ) < 0 )
				items.add( element );
		}
	}
}

const char **XmlSpinalCordLayout::getLevels() {
	return( levels );
}

const char **XmlSpinalCordLayout::getLaminas() {
	return( laminas );
}

StringList& XmlSpinalCordLayout::getCellItems( String level , String lamina ) {
	MapStringToClass<StringList>& levelData = data.getRef( level );
	return( levelData.getRef( lamina ) );
}
