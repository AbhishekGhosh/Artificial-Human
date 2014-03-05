#include <ah_platform.h>
#include <ah_adminapi_impl.h>

/*#########################################################################*/
/*#########################################################################*/

AdminApiSocket::AdminApiSocket()
:	protocol( logger ) {
	lastRequestNumber = 0;
	connected = false;
	waitTimeSec = 5;
	socket = INVALID_SOCKET;

	logger.attachRoot();
}

AdminApiSocket::~AdminApiSocket() {
	if( connected )
		disconnect();
}

void AdminApiSocket::initThread() {
	protocol.initSocketLib();
}

void AdminApiSocket::exitThread() {
	protocol.exitSocketLib();
}

String AdminApiSocket::setXmlRequestId( Xml req ) {
	String requestId = String( "req-" ) + (++lastRequestNumber);
	req.setAttribute( "requestId" , requestId );
	return( requestId );
}

Xml AdminApiSocket::execute( Xml req ) {
	ASSERTMSG( connected , "Not connected" );

	// get request ID
	String requestId = req.getAttribute( "requestId" , "" );
	if( requestId.isEmpty() )
		requestId = setXmlRequestId( req );
	String request = req.serialize();

	bool disconnected = false;
	protocol.writeMessage( socket , request , disconnected );
	if( disconnected )
		connected = false;
	ASSERTMSG( connected , "Connection is closed from server side while making request" );

	String response;
	if( !protocol.readMessage( socket , response , true , disconnected ) ) {
		if( disconnected )
			connected = false;
		ASSERTMSG( connected , "Connection is closed from server side while reading response" );
		ASSERTFAILED( "Timeout while waiting response for request=" + requestId );
		return( Xml() );
	}

	Xml xml = Xml::read( response , "xmlcallresult" );
	return( xml );
}

void AdminApiSocket::send( Xml req ) {
	ASSERTMSG( connected , "Not connected" );

	// get request ID
	String requestId = req.getAttribute( "requestId" , "" );
	if( requestId.isEmpty() )
		requestId = setXmlRequestId( req );
	String request = req.serialize();

	bool disconnected = false;
	protocol.writeMessage( socket , request , disconnected );
	if( disconnected )
		connected = false;
	ASSERTMSG( connected , "Connection is closed from server side while making request" );
}

void AdminApiSocket::connect( String url ) {
	lastRequestNumber = 0;

	const char *p = strrchr( url , ':' );
	ASSERT( p != NULL );
	
	int port = atoi( p + 1 );
	String host = url.getMid( 0 , p - ( const char * )url );

	socket = protocol.open( host , ( unsigned short )port , &addr );
	connected = true;

	// configure protocol
	protocol.pout = protocol.pin = SocketProtocol::FLOW_PROTOCOL_XML_MESSAGES;
	protocol.delimiterOut = protocol.delimiterIn = "\x01";
	protocol.waitTimeSec = waitTimeSec;
}

void AdminApiSocket::disconnect() {
	connected = false;
	protocol.close( socket );
}

bool AdminApiSocket::isConnected() {
	return( connected );
}
