#include <ah_platform.h>
#include <ah_testing_impl.h>

/*#########################################################################*/
/*#########################################################################*/

class DirectChannelsTest : public TestUnit , public MessageSubscriber {
// data
private:
	RFC_HND msgEvent;
	String pageResults;

	MessageSubscription *sub;
	MessagePublisher *pub;

// construction
public:
	DirectChannelsTest() : TestUnit( "DirectChannelsTest" ) {
		msgEvent = rfc_hnd_evcreate();

		pub = NULL;
		sub = NULL;
	};
	~DirectChannelsTest() {
		rfc_hnd_evdestroy( msgEvent );
	};

	virtual void onCreate() {
		ADD_METHOD( DirectChannelsTest::testRequestPage );
	}
	virtual void onInit() {
		MessagingService *ms = MessagingService::getService();
		sub = ms -> subscribe( NULL , "http.response" , "http.test" , this );
		pub = ms -> createPublisher( NULL , "http.request" , "http.test" , "text" );
	}
	virtual void onExit() {
		rfc_hnd_evsignal( msgEvent );
	}
	
// tests
public:

	void testRequestPage( XmlCall& call ) {
		String endpoint = call.getParam( "endpoint" );
		String page = call.getParam( "page" , "" );
		
		rfc_hnd_evreset( msgEvent );
		pageResults.clear();

		// send page request
		Message *msg = pub -> createTextMessage( endpoint + "/" + page );
		msg -> setEndPoint( endpoint );
		String msgId = pub -> publish( call.getSession() , msg );

		// wait for response
		rfc_hnd_waitevent( msgEvent , 5000 );
		
		// publish if received
		ASSERTMSG( !pageResults.isEmpty() , "No response from endpoint=" + endpoint + ", page=" + page );
		Xml xml = call.createResponse();
		xml.addTextElement( "page" , page );
		xml.addTextElement( "body" , pageResults );
	}

	virtual void onTextMessage( TextMessage *msg ) {
		pageResults = msg -> getText();
		rfc_hnd_evsignal( msgEvent );
	}
};

/*#########################################################################*/
/*#########################################################################*/

TestUnit *TestUnit::createDirectChannelsTest() {
	return( new DirectChannelsTest() );
}

