#include <ah_platform.h>
#include <ah_testing_impl.h>

/*#########################################################################*/
/*#########################################################################*/

class EngineThreadsTest : public TestUnit {
// construction
public:
	EngineThreadsTest() : TestUnit( "EngineThreadsTest" ) {
	};
	~EngineThreadsTest() {
	};

	virtual void onCreate() {
		ADD_METHOD( EngineThreadsTest::testThreadDump );
		ADD_METHOD( EngineThreadsTest::testShowThread );
	}
	
// tests
public:

	void testThreadDump( XmlCall& call ) {
		bool showStack = call.getBooleanParam( "showStack" , false );
		ThreadService *ts = ThreadService::getService();
		ts -> threadDumpAll( showStack );
	}

	void testShowThread( XmlCall& call ) {
		String thread = call.getParam( "threadName" );
		bool showStack = call.getBooleanParam( "showStack" );
		ThreadService *ts = ThreadService::getService();
		ts -> threadDumpByName( thread , showStack );
	}
};

/*#########################################################################*/
/*#########################################################################*/

TestUnit *TestUnit::createEngineThreadsTest() {
	return( new EngineThreadsTest );
}

