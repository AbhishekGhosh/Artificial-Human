#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

WikiMaker::WikiMaker( Xml p_wiki ) {
	attachLogger();
	wiki = p_wiki;

}

WikiMaker::~WikiMaker() {
}

void WikiMaker::createPages() {
	logger.logInfo( "create mind model wiki pages..." );

	MindModel::load();

	// create pages
	createMainPage();
	createCircuitPages();
	updateHierarchyPage();
	createAreaPages();
	createNerveMainPage();
	createNerveSpecPages();
	createMuscleMainPage();
	createMuscleSpecPages();
	createSpinalCordPage();
}

void WikiMaker::createSpinalCordPage() {
	WikiSpinalCordPage maker( this );
	maker.execute();
}

void WikiMaker::createMainPage() {
	WikiMainPage maker( this );
	maker.execute();
}

void WikiMaker::createCircuitPages() {
	WikiCircuitPages maker( this );
	maker.execute();
}

void WikiMaker::updateHierarchyPage() {
	WikiHierarchyPage maker( this );
	maker.execute();
}

void WikiMaker::createAreaPages() {
	WikiAreaPages maker( this );
	maker.execute();
}

void WikiMaker::createNerveMainPage() {
	WikiNerveMainPage maker( this );
	maker.execute();
}

void WikiMaker::createMuscleMainPage() {
	WikiMuscleMainPage maker( this );
	maker.execute();
}

void WikiMaker::createNerveSpecPages() {
	WikiNerveSpecPages maker( this );
	maker.execute();
}

void WikiMaker::createMuscleSpecPages() {
	WikiMuscleSpecPages maker( this );
	maker.execute();
}

/*#########################################################################*/
/*#########################################################################*/
// common

void WikiMaker::updateFileSection( String wikiDir , String wikiPage , String section , StringList& lines ) {
	String fileName = wikiDir + "/" + wikiPage + ".wiki";

	// scan file until section
	FILE *fr = fopen( fileName , "rt" );
	ASSERTMSG( fr != NULL , "unable to read from file " + fileName );

	String fileNameNew = fileName + ".new";
	remove( fileNameNew );
	FILE *fw = fopen( fileNameNew , "wt" );
	ASSERTMSG( fw != NULL , "unable to write to file " + fileNameNew );
	String s;

	int maxSize = 10000;
	s.resize( maxSize );

	// find start of section
	bool found = false;
	while( !feof( fr ) ) {
		fgets( s.getBuffer() , maxSize , fr );
		fputs( s , fw );

		s.trim();
		s.trim( '=' );
		s.trim();
		if( s.equals( section ) ) {
			found = true;
			break;
		}
	}

	if( !found ) {
		fclose( fr );
		fclose( fw );
		ASSERTFAILED( "unable to find section=" + section + " in page=" + wikiPage );
	}

	// write section lines
	fputs( "(generated)\n" , fw );
	fputs( "\n" , fw );
	for( int k = 0; k < lines.count(); k++ )
		fputs( lines.get( k ) + "\n" , fw );

	fputs( "\n" , fw );

	// skip current section content
	while( !feof( fr ) ) {
		fgets( s.getBuffer() , maxSize , fr );
		if( s.startsFrom( "= " ) || s.startsFrom( "== " ) || s.startsFrom( "=== " ) ) {
			fputs( s , fw );
			break;
		}
	}

	// write remaining file content
	while( !feof( fr ) ) {
		fgets( s.getBuffer() , maxSize , fr );
		fputs( s , fw );
	}

	// close files
	fclose( fr );
	fclose( fw );

	// rename file
	remove( fileName );
	rename( fileNameNew , fileName );

	logger.logInfo( "page " + wikiPage + " - replaced section " + section + " total nlines=" + lines.count() );
}

void WikiMaker::updateFileHeading( String wikiDir , String wikiPage , StringList& lines ) {
	String fileName = wikiDir + "/" + wikiPage + ".wiki";

	// scan file until section
	FILE *fr = fopen( fileName , "rt" );
	ASSERTMSG( fr != NULL , "unable to read from file " + fileName );

	String fileNameNew = fileName + ".new";
	remove( fileNameNew );
	FILE *fw = fopen( fileNameNew , "wt" );
	ASSERTMSG( fw != NULL , "unable to write to file " + fileNameNew );
	String s;

	int maxSize = 10000;
	s.resize( maxSize );

	// write heading lines
	for( int k = 0; k < lines.count(); k++ )
		fputs( lines.get( k ) + "\n" , fw );

	fputs( "(generated)\n" , fw );

	// skip current heading content
	while( !feof( fr ) ) {
		fgets( s.getBuffer() , maxSize , fr );
		if( s.startsFrom( "= " ) || s.startsFrom( "== " ) || s.startsFrom( "=== " ) ) {
			fputs( s , fw );
			break;
		}
	}

	// write remaining file content
	while( !feof( fr ) ) {
		fgets( s.getBuffer() , maxSize , fr );
		fputs( s , fw );
	}

	// close files
	fclose( fr );
	fclose( fw );

	// rename file
	remove( fileName );
	rename( fileNameNew , fileName );

	logger.logInfo( "page " + wikiPage + " - replaced heading, total nlines=" + lines.count() );
}

void WikiMaker::createFileContent( String fileName , StringList& lines ) {
	FILE *f = fopen( fileName , "wt" );
	ASSERTMSG( f != NULL , "Unable to open file=" + fileName );
	for( int k = 0; k < lines.count(); k++ ) {
		String s = lines.get( k ) + "\n";
		fputs( s , f );
	}
	fclose( f );
}

String WikiMaker::setSpecialCharacters( String data ) {
	String s = data.replace( "\\t" , "\t" );
	s = s.replace( "\\n" , "\n" );
	return( s );
}

String WikiMaker::findReference( MindRegionLink *link ) {
	return( findReference( link -> getSrcRegion() -> getRegionId() , link -> getDstRegion() -> getRegionId() ) );
}

String WikiMaker::findReference( MindCircuitConnectionDef *link ) {
	return( findReference( link -> getSrcRegion() , link -> getDstRegion() ) );
}

String WikiMaker::findReference( String srcRegion , String dstRegion ) {
	// check by map
	String key = srcRegion + "." + dstRegion;
	String reference = referenceMap.get( key );
	if( !reference.isEmpty() )
		return( reference );

	XmlCircuitFind find;
	if( circuitsxml.findReferenceLink( hmindxml , srcRegion , dstRegion , find ) ) {
		String circuitLink = find.link.compSrc + " -> " + find.link.compDst;
		if( find.isAbstractLink )
			circuitLink += ", abstract";

		reference = find.circuit -> reference;
		if( reference.startsFrom( "article:" ) )
			reference = "http://ahuman.googlecode.com/svn/research/articles/" + reference.getMid( 8 );
		else
		if( reference.equals( "UNKNOWN" ) ) {
			if( find.circuit -> image.isEmpty() )
				reference = "not linked reference, circuit=" + find.circuit -> name + " (" + circuitLink + ")";
			else
				reference = find.circuit -> image;
		}

		reference = "[" + reference + " " + find.circuit -> name + " (" + circuitLink + ")]";
	}
	else
		reference = "(unknown reference)";

	// add to map and return
	referenceMap.add( key , reference );
	return( reference );
}

String WikiMaker::getAreaPage( String area ) {
	return( "BrainArea" + area );
}

String WikiMaker::getRegionPage( String region ) {
	return( "BrainRegion" + region.replace( "." , "_" ) );
}

void WikiMaker::ensureFileExists( String wikiDir , String wikiPage , StringList& sections ) {
	String fileName = wikiDir + "/" + wikiPage + ".wiki";

	// scan file until section
	FILE *fr = fopen( fileName , "rt" );
	if( fr != NULL ) {
		fclose( fr );
		return;
	}

	// create sections
	FILE *fw = fopen( fileName , "wt" );
	for( int k = 0; k < sections.count(); k++ ) {
		fputs( sections.get( k ) + "\n" , fw );
		fputs( "\n" , fw );
	}
	fclose( fw );
}

String WikiMaker::getRegionReference( String region ) {
	return( "[" + getRegionPage( region ) + " " + region + "]" );
}

String WikiMaker::getMuscleReference( String name , String text ) {
	XmlMuscleInfo *muscle = musclesxml.findByConnector( name );
	if( muscle == NULL )
		return( text );

	String link = "[" + muscle -> division -> page + " " + text + "]";
	return( link );
}

String WikiMaker::getAreaReference( String area ) {
	return( "[" + getAreaPage( area ) + " " + area + "]" );
}

String WikiMaker::getDotColor( String dotdef ) {
	int index = dotdef.find( "color=" );
	if( index < 0 )
		return( "" );

	int stop = dotdef.find( index , "." );

	String color;
	if( stop < 0 )
		color = dotdef.getMid( index + 6 );
	else
		color = dotdef.getMid( index + 6 , stop - index - 6 );

	color.trim();
	return( color );
}

void WikiMaker::clearRepeats1( String& value1 ) {
	if( saveRepeats1.equals( value1 ) )
		value1.clear();
	else
		saveRepeats1 = value1;
}

void WikiMaker::clearRepeats2( String& value1 , String& value2 ) {
	if( saveRepeats1.equals( value1 ) ) {
		value1.clear();
		if( saveRepeats2.equals( value2 ) )
			value2.clear();
		else
			saveRepeats2 = value2;
	}
	else {
		saveRepeats1 = value1;
		saveRepeats2 = value2;
	}
}

String WikiMaker::getNerveWikiPage( String nerve ) {
	// load nerves
	nervesxml.load();
	XmlNerveInfo& div = nervesxml.getNerveDivision( nerve );
	return( div.origin );
}

String WikiMaker::getImageWikiLink( String imgsrc , String height ) {
	String s = "<img src=\"" + imgsrc + "\"";
	if( !height.isEmpty() )
		s += " height=" + height;
	s += ">";
	return( s );
}

