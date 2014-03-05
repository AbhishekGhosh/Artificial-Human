#include <ah_platform.h>
#include <ah_media_impl.h>

/*#########################################################################*/
/*#########################################################################*/

void ActiveSocket::onTextMessage( TextMessage *msg ) {
	String url = msg -> getEndPoint();
	String text = msg -> getText();

	SocketUrl su;
	ActiveSocketConnection *ac = getConnection( true , url , su );
	ac -> sendText( su , text );
}

ActiveSocket::ActiveSocket( String p_name )
:	protocol( logger ) {
	name = p_name;

	setInstance( name );
	attachLogger();

	redirectInbound = false;
	redirectOutbound = false;

	sub = NULL;
	pub = NULL;
	connection = NULL;
	shutdownInProgress = false;

	lock = rfc_hnd_semcreate();
}

ActiveSocket::~ActiveSocket() {
	rfc_hnd_semdestroy( lock );
}

void ActiveSocket::configure( Xml config ) {
	inboundChannelName = config.getProperty( "redirect-in-topic" , "" );
	outboundChannelName = config.getProperty( "redirect-out-topic" , "" );
	redirectInbound = ( inboundChannelName.isEmpty() )? false : true;
	redirectOutbound = ( outboundChannelName.isEmpty() )? false : true;

	host = config.getProperty( "host" , "" );
	String portString = config.getProperty( "port" , "" );
	if( portString.isEmpty() )
		port = 0;
	else
		port = portString.toInteger();

	connectionType = ( ConnectionType )config.getEnumProperty( "connection" , String( "single=" ) + CONNECTION_SIGNLE + 
		", permanent=" + CONNECTION_PERMANENT +
		", multiple=" + CONNECTION_MULTIPLE );
	sessionType = ( SessionType )config.getEnumProperty( "session" , String( "permanent=" ) + SESSION_PERMANENT + 
		", request-response=" + SESSION_REQUESTRESPONSE +
		", request=" + SESSION_REQUEST );
	reconnectionTimeout = config.getIntProperty( "reconnectionTimeoutSec" , 10 );

	// start protocol
	protocol.create( config );

	// start logging
	Object::setInstance( name );
	attachLogger();
}

bool ActiveSocket::startActiveSocket() {
	// init messaging
	MessagingService *io = MessagingService::getService();
	if( redirectOutbound )
		sub = io -> subscribe( NULL , outboundChannelName , "as." + name , this );
	if( redirectInbound )
		pub = io -> createPublisher( NULL , inboundChannelName , "as." + name , "text" );

	// create connection when starting
	if( connectionType == CONNECTION_PERMANENT ) {
		SocketUrl su;
		getConnection( false , NULL , su );
	}

	return( true );
}

void ActiveSocket::stopActiveSocket() {
	rfc_hnd_semlock( lock );

	shutdownInProgress = true;

	try {
		if( connection != NULL )
			connection -> closeSocket();
		else {
			for( int k = 0; k < connections.count(); k++ ) {
				ActiveSocketConnection *c = connections.getClassByIndex( k );
				connection -> closeSocket();
			}
			connections.destroy();
		}
		rfc_hnd_semunlock( lock );
	}
	catch( RuntimeException& e ) {
		rfc_hnd_semunlock( lock );
		throw e;
	}
}

void ActiveSocket::exitActiveSocket() {
	rfc_hnd_semlock( lock );

	try {
		if( connection != NULL ) {
			connection -> exitSocket();
		
			delete connection;
			connection = NULL;
		}
		else {
			for( int k = 0; k < connections.count(); k++ ) {
				ActiveSocketConnection *c = connections.getClassByIndex( k );
				connection -> exitSocket();
			}

			connections.destroy();
		}

		shutdownInProgress = false;
		rfc_hnd_semunlock( lock );
	}
	catch( RuntimeException& e ) {
		rfc_hnd_semunlock( lock );
		throw e;
	}
}

String ActiveSocket::getName() {
	return( name );
}

void ActiveSocket::sendText( String url , String text ) {
	SocketUrl su;
	ActiveSocketConnection *ac = getConnection( true , url , su );
	ac -> sendText( su , text );
}

String ActiveSocket::receiveText( String url , bool wait ) {
	SocketUrl su;
	ActiveSocketConnection *ac = getConnection( false , url , su );
	String text = ac -> receiveText( su , wait );
	return( text );
}

String ActiveSocket::receiveFixedSizeText( String url , int size , bool wait ) {
	SocketUrl su;
	ActiveSocketConnection *ac = getConnection( false , url , su );
	String text = ac -> receiveText( su , wait );
	return( text );
}

bool ActiveSocket::waitReadSocket( String url , bool wait ) {
	SocketUrl su;
	ActiveSocketConnection *ac = getConnection( false , url , su );
	return( ac -> waitReadSocket( su , wait ) );
}

ActiveSocketConnection *ActiveSocket::getConnection( bool sendWay , String url , SocketUrl& su ) {
	ActiveSocketConnection *ac = NULL;

	// extract url key
	su.setUrl( url );

	// check protocol is the same
	String protocolNameUrl = su.getProtocol();
	String protocolNameConfigured = ( sendWay )? protocol.protocolNameOut : protocol.protocolNameIn;
	if( protocolNameUrl.isEmpty() )
		su.setProtocol( protocolNameConfigured );
	else
		ASSERTMSG( protocolNameConfigured.equals( protocolNameUrl ) , "Url protocol=" + protocolNameUrl + " differs from confgured protocol=" + protocolNameConfigured );

	// host
	String hostUrl = su.getHost();
	if( hostUrl.isEmpty() )
		su.setHost( host );

	// port
	int portUrl = su.getPort();
	if( portUrl == 0 )
		su.setPort( port );

	// get url key and check constraints
	String urlKey = su.getValidUrlWithoutPage();
	if( connectionType == CONNECTION_PERMANENT && connection != NULL ) {
		String urlKeyCurrent = connection -> getUrlKey();
		ASSERTMSG( url.equals( urlKeyCurrent ) , "attempt to change urlkey for permanent connection current=" + urlKeyCurrent + ", new=" + urlKey );
	}

	rfc_hnd_semlock( lock );

	try {
		// get/create connection
		if( connectionType == CONNECTION_MULTIPLE ) {
			ac = connections.get( urlKey );
			if( ac == NULL )
				ac = new ActiveSocketConnection( name , urlKey , protocol , pub );
		}
		else {
			if( connection == NULL )
				connection = new ActiveSocketConnection( name , urlKey , protocol , pub );
			else
			if( !urlKey.equals( connection -> getUrlKey() ) )
				connection -> setUrlKey( urlKey );
			ac = connection;
		}

		// connect to external address
		ASSERTMSG( ac -> connectSocket() , "sendText - unable to connect ActiveSocket=" + name );
		rfc_hnd_semunlock( lock );
	}
	catch( RuntimeException& e ) {
		rfc_hnd_semunlock( lock );
		throw e;
	}

	return( ac );
}
