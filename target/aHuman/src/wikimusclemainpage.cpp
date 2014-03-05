#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

WikiMuscleMainPage::WikiMuscleMainPage( WikiMaker *p_wm ) {
	attachLogger();
	wm = p_wm;
}

WikiMuscleMainPage::~WikiMuscleMainPage() {
}

void WikiMuscleMainPage::execute() {
	bool createMainPage = wm -> wiki.getBooleanProperty( "createMusclePages" , true );
	if( createMainPage == false ) {
		logger.logInfo( "skip creating muscle main page" );
		return;
	}

	String wikiDir = wm -> wiki.getProperty( "wikiPath" );
	String wikiPage = wm -> wiki.getProperty( "wikiPageMuscleMain" );
	String sectionName = wm -> wiki.getProperty( "wikiPageMuscleMainSection" );

	// get nerves from pns
	StringList categories;
	wm -> musclesxml.getCategories( categories );
	MapStringToClass<XmlMuscleDivision>& divs = wm -> musclesxml.getDivisions();

	// collect model circuit section lines
	StringList lines;
	String prefix = "  * ";
	for( int dc = 0; dc < categories.count(); dc++ ) {
		String dcs = categories.get( dc );
		String s = prefix + "*" + dcs + "*";
		lines.add( s );

		for( int k = 0; k < divs.count(); k++ ) {
			XmlMuscleDivision& div = divs.getClassRefByIndex( k );
			if( !div.category.equals( dcs ) )
				continue;

			s = " " + prefix + "[" + div.page + " " + div.name + "]";
			lines.add( s );

			addMuscles( 2 , div , div.childs , lines );
		}
	}

	wm -> updateFileSection( wikiDir , wikiPage , sectionName , lines );
}

void WikiMuscleMainPage::addMuscles( int level , XmlMuscleDivision& div , MapStringToClass<XmlMuscleInfo>& items , StringList& lines ) {
	if( items.count() == 0 )
		return;

	String prefix = "  * ";
	if( level > 0 )
		prefix = String( " " ).replicate( level ) + prefix;

	for( int k = 0; k < items.count(); k++ ) {
		XmlMuscleInfo& mc = items.getClassRefByIndex( k );

		String page = ( mc.link.isEmpty() )? div.page : mc.link;
		String item = "[" + page + " " + mc.name + "]";
		if( !mc.type.isEmpty() )
			 item += " - " + mc.type;

		lines.add( prefix + item );

		addMuscles( level + 1 , div , mc.childs , lines );
	}
}

