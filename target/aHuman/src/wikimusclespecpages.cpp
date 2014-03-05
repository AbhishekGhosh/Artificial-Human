#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

WikiMuscleSpecPages::WikiMuscleSpecPages( WikiMaker *p_wm ) {
	attachLogger();
	wm = p_wm;
}

WikiMuscleSpecPages::~WikiMuscleSpecPages() {
}

void WikiMuscleSpecPages::execute() {
	bool createMainPage = wm -> wiki.getBooleanProperty( "createMusclePages" , true );
	if( createMainPage == false ) {
		logger.logInfo( "skip creating muscle spec pages" );
		return;
	}

	// get nerves from pns
	MapStringToClass<XmlMuscleDivision>& divs = wm -> musclesxml.getDivisions();

	// collect model circuit section lines
	for( int k = 0; k < divs.count(); k++ )
		addMuscleDivision( divs.getClassRefByIndex( k ) );
}

void WikiMuscleSpecPages::addMuscleDivision( XmlMuscleDivision& div ) {
	StringList lines;
	String divPage = div.page;
	String divSection = div.name;

	for( int k = 0; k < div.childs.count(); k++ )
		addMuscleList( 0 , div.childs.getClassRefByIndex( k ) , lines );

	String wikiDir = wm -> wiki.getProperty( "wikiPath" );
	wm -> updateFileSection( wikiDir , divPage , divSection , lines );
}

String WikiMuscleSpecPages::getNerveList( XmlMuscleInfo& muscle ) {
	String s;
	for( int k = 0; k < muscle.nerves.count(); k++ ) {
		String nerve = muscle.nerves.getKeyByIndex( k );
		String page = wm -> getNerveWikiPage( nerve );
		String pagelink = "[" + page + " " + nerve + "]";

		if( k > 0 )
			s += "; ";
		s += pagelink;
	}

	return( s );
}

void WikiMuscleSpecPages::addMuscleList( int level , XmlMuscleInfo& muscle , StringList& lines ) {
	// item string
	String name;
	if( muscle.link.isEmpty() )
		name = muscle.name;
	else
		name = "[" + muscle.link + " " + muscle.name + "]";
	String s = String( " " ).replicate( level + 2 ) + "* *" + name + "*";

	if( !muscle.type.isEmpty() )
		s += "; TYPE={" + muscle.type + "}";
	if( muscle.nerves.count() > 0 )
		s += "; NERVES={" + getNerveList( muscle ) + "}";
	if( !muscle.action.isEmpty() )
		s += "; ACTION={" + muscle.action + "}";
	lines.add( s );

	// image
	if( !muscle.imgsrc.isEmpty() ) {
		s = String( " " ).replicate( level + 3 ) + "* <img src=\"" + muscle.imgsrc + "\"";
		if( !muscle.imgheight.isEmpty() )
			s += " height=" + muscle.imgheight;
		s += ">";
		lines.add( s );
	}

	// childs
	for( int k = 0; k < muscle.childs.count(); k++ )
		addMuscleList( level + 1 , muscle.childs.getClassRefByIndex( k ) , lines );
}
