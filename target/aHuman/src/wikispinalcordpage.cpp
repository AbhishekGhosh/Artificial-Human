#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

WikiSpinalCordPage::WikiSpinalCordPage( WikiMaker *p_wm ) {
	attachLogger();
	wm = p_wm;
}

WikiSpinalCordPage::~WikiSpinalCordPage() {
}

void WikiSpinalCordPage::execute() {
	bool createMainPage = wm -> wiki.getBooleanProperty( "createMainPages" , false );
	if( createMainPage == false ) {
		logger.logInfo( "skip creating spinal cord page" );
		return;
	}

	String wikiDir = wm -> wiki.getProperty( "wikiPath" );
	String wikiPage = wm -> wiki.getProperty( "wikiPageSpinalCord" );
	String sectionName = wm -> wiki.getProperty( "wikiSpinalCordLayoutSection" );

	// collect section lines
	StringList lines;
	XmlSpinalCordLayout *layout = wm -> hmindxml.getLayout();
	String s = wm -> getImageWikiLink( layout -> getImageSrc() , "" );
	lines.add( s );
	lines.add( "" );

	const char **levels = layout -> getLevels();
	const char **laminas = layout -> getLaminas();
	s = "|| *Level/Lamina* ||";
	for( int m = 0; laminas[ m ] != NULL; m++ )
		s += " *" + String( laminas[ m ] ) + "* ||";
	lines.add( s );

	for( int k = 0; levels[ k ] != NULL; k++ ) {
		String line = "|| " + String( levels[ k ] ) + " ||";
		for( int m = 0; laminas[ m ] != NULL; m++ ) {
			StringList& items = layout -> getCellItems( levels[ k ] , laminas[ m ] );
			if( items.count() > 0 )
				line += " " + items.combine( "," ) + " ||";
			else
				line += " ||";
		}
		lines.add( line );
	}

	wm -> updateFileSection( wikiDir , wikiPage , sectionName , lines );
}
