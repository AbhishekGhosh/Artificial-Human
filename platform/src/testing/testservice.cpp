#include <ah_platform.h>
#include <ah_testing_impl.h>

/*#########################################################################*/
/*#########################################################################*/

Service *TestService::newService() {
	return( new TestService() );
}

TestService::TestService() {
	executor = NULL;
}

void TestService::configureService( Xml config ) {
	Xml xml = config.getFirstChild( "channels" );
	channelIn = xml.getProperty( "request" );
	channelOut = xml.getProperty( "response" );
}

void TestService::createService() {
	executor = new TestExecutor();
}

void TestService::initService() {
	executor -> connectChannels( channelIn , channelOut );
	executor -> addTestUnit( TestUnit::createEngineThreadsTest() );
	executor -> addTestUnit( TestUnit::createDirectChannelsTest() );
	executor -> addTestUnit( TestUnit::createThreadPoolTest() );
}

void TestService::runService() {
	executor -> start();
}

void TestService::stopService() {
	executor -> stop();
}

void TestService::exitService() {
	executor -> exit();
}

void TestService::destroyService() {
	if( executor != NULL ) {
		delete executor;
		executor = NULL;
	}
}
