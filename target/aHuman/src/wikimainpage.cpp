#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

WikiMainPage::WikiMainPage( WikiMaker *p_wm ) {
	attachLogger();
	wm = p_wm;
}

WikiMainPage::~WikiMainPage() {
}

void WikiMainPage::execute() {
	bool createMainPage = wm -> wiki.getBooleanProperty( "createMainPages" , true );
	if( createMainPage == false ) {
		logger.logInfo( "skip creating main page" );
		return;
	}

	String wikiDir = wm -> wiki.getProperty( "wikiPath" );
	String wikiPage = wm -> wiki.getProperty( "wikiPageMain" );

	// get sorted circuit list
	MindService *ms = MindService::getService();
	MindMap *mm  = ms -> getMindMap();
	ClassList<MindCircuitDef>& circuitList = mm -> getMindCircuits();
	MapStringToClass<MindCircuitDef> circuitMap;
	for( int k = 0; k < circuitList.count(); k++ ) {
		MindCircuitDef *cd = circuitList.get( k );
		circuitMap.add( cd -> getName() , cd );
	}

	// collect model circuit section lines
	StringList lines;
	for( int k = 0; k < circuitMap.count(); k++ ) {
		MindCircuitDef *cd = circuitMap.getClassByIndex( k );
		String name = cd -> getName();
		String id = cd -> getId();

		lines.add( "  * [" + id + " " + name + "]" );
	}

	String sectionName = "Circuits";
	wm -> updateFileSection( wikiDir , wikiPage , sectionName , lines );
}

