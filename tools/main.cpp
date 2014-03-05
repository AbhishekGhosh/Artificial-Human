#include <ah_tools.h>

/*#########################################################################*/
/*#########################################################################*/

int main(int argc, char **argv) {

	// platform based
	ServiceManager sm;
	sm.setRootLogLevel( Logger::LogLevelDebug );

	// switch params
	if( argc < 2 ) {
		printf( "arguments: <tool> [params]\n" );
		return( -1 );
	}

	String toolName = argv[1];
	ToolBase *tool = ToolBase::getTool( toolName );
	if( tool == NULL ) {
		printf( "Unknown tool: " + toolName );
		return -1;
	}

	int result = 0;
	try {
		result = tool -> execute( argc - 2 , argv + 2 );
	}
	catch( RuntimeException& e ) {
		Logger logger;
		logger.attachRoot();
		logger.printStack( e );
		result = -1;
	}
	catch( ... ) {
		printf( "Unknown exception cought" );
		result = -1;
	}

	delete tool;
	return( result );
}

