#include <ah_platform.h>
#include <ah_media_impl.h>

/*#########################################################################*/
/*#########################################################################*/

ListenerSocketConnection::ListenerSocketConnection( SocketServer *p_server , SOCKET p_clientSocket , struct sockaddr_in *p_clientAddress , Message::MsgType p_msgType )
:	protocol( logger ) {
	listener = NULL;
	session = NULL; 

	server = p_server;
	pub = NULL;
	sub = NULL;

	socket = p_clientSocket;
	memcpy( &addr , p_clientAddress , sizeof( struct sockaddr_in ) );
	protocol.copy( server -> getProtocol() );

	thread = NULL;
	threadStarted = false;
	connected = false;
	logout = false;
	continueRead = false;
	msgType = p_msgType;

	attachLogger();
}

ListenerSocketConnection::~ListenerSocketConnection() {
	// delete session
	if( session != NULL ) {
		MessagingService *ms = MessagingService::getService();
		ms -> closeSession( session );
	}
}

void ListenerSocketConnection::setName( String name ) {
	setInstance( name );
	attachLogger();
}

void ListenerSocketConnection::threadClientFunction( void *p_arg ) {
	// startup sockets
	SocketProtocol::initSocketLib();

	// read messages
	readMessages();

	SocketListener *listener = getListener();
	listener -> removeListenerConnection( this );

	// cleanup sockets
	SocketProtocol::exitSocketLib();
}

bool ListenerSocketConnection::startConnection() {
	// complete connection
	logger.logInfo( String( "startConnection: client name=" ) + getName() + 
		" connected on listener=" + server -> getName() + 
		" from network address=" + getClientSocketName() );

	// connect to topics
	MessagingService *ms = MessagingService::getService();
	session = ms -> createSession();
	if( server -> getWayIn() ) {
		String topicIn = server -> getTopicIn();
		pub = ms -> createPublisher( session , topicIn , getName() , server -> getContentType() );
	}

	if( server -> getWayOut() ) {
		String topicOut = server -> getTopicOut();
		sub = ms -> subscribe( session , topicOut , getName() , this );
	}

	// start reading thread
	if( server -> getWayIn() || server -> getAuth() ) {
		ThreadService *ts = ThreadService::getService();
		thread = ts -> runThread( getInstance() , this , ( ObjectThreadFunction )&ListenerSocketConnection::threadClientFunction , NULL );
		threadStarted = true;
	}

	// log success
	connected = true;

	return( true );
}

void ListenerSocketConnection::readMessages() {
	String key = getInstance();

	try {
		continueRead = true;
		while( continueRead )
			performRead();
	}
	catch ( RuntimeException& e ) {
		logger.logError( "readMessages: connection=" + key + " - exception caught:" );
		logger.printStack( e );
	}
	catch ( ... ) {
		logger.printStack();
		logger.logError( "readMessages: connection=" + key + " - unknown exception in read" );
	}

	try {
		stopConnection();
	}
	catch ( ... ) {
		logger.printStack();
		logger.logError( "readMessages: connection=" + key + " - unknown exception in stop" );
	}
}

void ListenerSocketConnection::performRead() {
	// wait for input
	if( server -> getWayIn() == false && server -> getAuth() == false )
		return;

	bool connectionClosed;
	String msg;
	if( protocol.readMessage( socket , msg , true , connectionClosed ) ) {
		processMessage( msg );
		return;
	}

	if( connectionClosed ) {
		continueRead = false;
		logout = true;
		logger.logDebug( "performRead: empty message, disconnecting" );
	}
}

void ListenerSocketConnection::stopConnection() {
	// do not read more
	continueRead = false;

	// stop receiving messages
	MessagingService *ms = MessagingService::getService();
	if( pub != NULL ) {
		ms -> destroyPublisher( pub );
		pub = NULL;
	}

	if( sub != NULL ) {
		ms -> unsubscribe( sub );
		sub = NULL;
	}

	// close socket part
	if( socket != INVALID_SOCKET ) {
		shutdown( socket , SD_BOTH );

		logout = true;
		_closesocket( socket );
		socket = INVALID_SOCKET;
	}

	if( connected ) {
		logger.logInfo( "stopConnection: connection=" + getName() + ": disconnected" );
		connected = false;
	}
}

void ListenerSocketConnection::exitConnection() {
}

void ListenerSocketConnection::processMessage( const char *p_msg ) {
	if( server -> getAuth() && connected == false ) {
		// the only message acceptable is connect
		tryLogin( p_msg );
		return;
	}

	// pass to channel
	logger.logDebug( "processMessage: connection=" + getName() + " - socket received message (" + p_msg + ")" );
							    
	if( msgType == Message::MsgType_Text ) {
		pub -> publish( session , p_msg );
	}
	else
	if( msgType == Message::MsgType_Xml ) {
		XmlMessage *l_msg = new XmlMessage( p_msg );
		l_msg -> setXmlFromMessage( pub -> getMsgType() );
		pub -> publish( session , l_msg );
	}
	else
	if( msgType == Message::MsgType_XmlCall ) {
		XmlCall *call = new XmlCall( pub -> getChannel() , sub -> getChannel() , p_msg );
		call -> setXmlFromMessage();
		pub -> publish( session , call );
	}
}

void ListenerSocketConnection::tryLogin( const char *p_msg ) {
}

void ListenerSocketConnection::writeMessage( TextMessage *p_msg ) {
	const String& s = p_msg -> getText();
	sendString( ( const char * )s , s.length() );
}

void ListenerSocketConnection::sendString( const char *p_msg , int p_len ) {
	ASSERTMSG( p_len > 0 , "SocketConnection::sendString - empty message" );

	// send message
	logger.logDebug( "sendString: conection=" + getName() + " - send message to socket - (" + String( p_msg , p_len ) + ")" );

	bool connectionClosed;
	protocol.writeMessage( socket , p_msg , connectionClosed );

	if( connectionClosed ) {
		continueRead = false;
		logout = true;
		logger.logDebug( "sendString: cannot send" );
	}
}

String ListenerSocketConnection::getClientSocketName() {
	String msg = SocketServer::getAddress( &addr );
	return( msg );
}

void ListenerSocketConnection::onTextMessage( TextMessage *msg ) {
	writeMessage( msg );
}

void ListenerSocketConnection::onXmlMessage( XmlMessage *msg ) {
	msg -> setMessageFromXml();
	writeMessage( msg );
}

void ListenerSocketConnection::onXmlCall( XmlCall *msg ) {
	ASSERTMSG( false , "not implemented yet" );
}
