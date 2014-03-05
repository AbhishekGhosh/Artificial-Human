#include <ah_platform.h>
#include <ah_testing_impl.h>

/*#########################################################################*/
/*#########################################################################*/

TestExecutor::TestExecutor() {
	callPub = NULL;
	callSub = NULL;
	enabled = false;
	attachLogger();
}

void TestExecutor::connectChannels( String channelIn , String channelOut ) {
	// subscribe
	MessagingService *ms = MessagingService::getService();
	callSub = ms -> subscribe( NULL , channelIn , "testpool" , this );
	callPub = ms -> createPublisher( NULL , channelOut , "testpool" , "test" );
}

void TestExecutor::start() {
	enabled = true;

	// log available commands
	logger.logInfo( "createService: AVAILABLE TEST METHODS:" );
	for( int k = 0; k < units.count(); k++ ) {
		TestUnit *tu = units.getClassByIndex( k );
		
		MapStringToClass<TestMethod>& methods = tu -> methods;
		for( int m = 0; m < methods.count(); m++ ) {
			logger.logInfo( String( "createService: method=" ) + methods.getKeyByIndex( m ) );
		}
	}
}

void TestExecutor::stop() {
	MessagingService *ms = MessagingService::getService();

	// ensure no more requests
	if( callSub != NULL )
		ms -> unsubscribe( callSub );

	enabled = false;
}

void TestExecutor::exit() {
	// exit all test units
	for( int k = 0; k < units.count(); k++ ) {
		TestUnit *tu = units.getClassByIndex( k );
		tu -> onExit();
	}

	units.destroy();
}

void TestExecutor::addTestUnit( TestUnit *p_unit ) {
	units.add( p_unit -> getClass() , p_unit );
	p_unit -> onCreate();
	p_unit -> attachLogger();
}

void TestExecutor::onXmlCall( XmlCall *msg ) {
	XmlCall& call = *msg;
	String cn = call.getClassName();
	String fn = call.getFunctionName();
	String fnfull = cn + "::" + fn;
	logger.logInfo( "onXmlCall: test method name=" + fnfull + " called in session=" + call.getSession() -> getSessionId() );

	// find unit
	TestUnit *unit = units.get( cn );
	ASSERTMSG( unit != NULL , "Unknown test unit=" + cn );
		
	// find method
	TestMethod *method = unit -> getMethod( fnfull );
	ASSERTMSG( method != NULL , "Unknown test unit method=" + fn );

	try {
		unit -> getLogger().attachCustom( fnfull );
		unit -> callMethod( method , call );
		call.sendStatusResponse( "OK" );
	}
	catch ( RuntimeException& e ) {
		logger.printStack( e );
		call.sendResponseException( callPub , e );
	}
	catch( ... ) {
		logger.printStack();
		logger.logError( "onXmlCall: unknown exception:" );
		call.sendResponseUnknownException( callPub );
	}
}

