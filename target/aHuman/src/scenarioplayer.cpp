#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

ScenarioPlayer::ScenarioPlayer() {
	attachLogger();
}

ScenarioPlayer::~ScenarioPlayer() {
}

void ScenarioPlayer::play( Xml scenario ) {
	logger.logInfo( "process scenario ..." );
	for( Xml xmlChild = scenario.getFirstChild( "xmlsignal" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "xmlsignal" ) )
		playSignal( xmlChild );
}

void ScenarioPlayer::playSignal( Xml cmd ) {
	// find target connector
	MindService *ms = MindService::getService();

	String name = cmd.getAttribute( "name" );
	String component = cmd.getProperty( "component" );
	String connector = cmd.getProperty( "connector" );

	MindRegion *region = ms -> getMindRegion( component );
	ASSERTMSG( region != NULL , "unknown region=" + component );

	NeuroLinkTarget *target = region -> getNeuroLinkTarget( connector );
	if( connector == NULL )
		return;

	NeuroSignal *signal = new NeuroSignal();
	static int z = 0;
	signal -> setId( String( "TM" ) + (++z) );
	logger.logInfo( "send signal name=" + name + " to component=" + component + ", connector=" + connector + ", signal id=" + signal -> getId() + " ..." );
	NeuroSignalSet *set = target -> execute( NULL , signal );
	if( set == NULL ) {
		logger.logDebug( signal -> getId() + ": there are no derived signals from signal id=" + signal -> getId() );
		return;
	}

	// define IDs
	ClassList<NeuroSignal>& signals = set -> getSignals();
	for( int k = 0; k < signals.count(); k++ ) {
		NeuroSignal *signalExecuted = signals.get( k );
		signalExecuted -> setId( signal -> getId() + ".S" + (k+1) );
	}

	// follow links
	MindArea *area = region -> getArea();
	area -> followLinks( signal -> getId() , region , set );
	set -> destroy();
}
