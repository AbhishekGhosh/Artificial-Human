#include "stdafx.h"

/*#########################################################################*/
/*#########################################################################*/

void execute( String etcpath ) {
	// create service manager
	ServiceManager sm;
	sm.setRootLogLevel( Logger::LogLevelDebug );

	// add services
	sm.addService( new ARobotTarget() );
	sm.addService( MindService::newService() );
	sm.addPlatformServices();

	// configure all
	if( !sm.configureDefault( etcpath ) )
		return;

	// execute services
	sm.execute();
}

int main(int argc, char** argv) {
	if( argc < 2 ) {
		printf( "Arguments: [config directory path]\n" );
		return( -1 );
	}

	execute( argv[1] );
}

