#include <ah_platform.h>
#include <ah_media_impl.h>

/*#########################################################################*/
/*#########################################################################*/

ActiveSocketConnection::ActiveSocketConnection( String activeSocketName , String p_urlkey , SocketProtocol& p_protocol , MessagePublisher *p_pub )
:	protocol( p_protocol ) {
	urlKey = p_urlkey;
	pub = p_pub;

	socketHandle = INVALID_SOCKET;
	continueRead = false;
	connected = false;
	socketThread = ( RFC_HND )NULL;
	threadStarted = false;
	shutdownInProgress = false;
	reconnectionTimeoutSec = 0;

	lock = rfc_hnd_semcreate();
	setInstance( "ac." + activeSocketName );
}

ActiveSocketConnection::~ActiveSocketConnection() {
	rfc_hnd_semdestroy( lock );
}

String ActiveSocketConnection::getUrlKey() {
	return( urlKey );
}

void ActiveSocketConnection::setUrlKey( String p_urlkey ) {
	rfc_hnd_semlock( lock );

	try {
		if( connected ) {
			disconnectSocket();
			waitThreadFinished();
		}

		urlKey = p_urlkey;
		setInstance( urlKey );
		rfc_hnd_semunlock( lock );
	}
	catch( RuntimeException& e ) {
		rfc_hnd_semunlock( lock );
		throw e;
	}
}

void ActiveSocketConnection::setReconnectionTimeoutSec( int p_reconnectionTimeoutSec ) {
	reconnectionTimeoutSec = p_reconnectionTimeoutSec;
}

bool ActiveSocketConnection::connectSocket() {
	rfc_hnd_semlock( lock );
	bool res = false;

	try {
		res = connectSocketProtected();
		rfc_hnd_semunlock( lock );
	}
	catch( ... ) {
		rfc_hnd_semunlock( lock );
		logger.printStack();
		logger.logDebug( "connectSocket: unexpected exception caught for urlkey=" + urlKey );
	}

	return( res );
}

String ActiveSocketConnection::getAddress() {
	SocketUrl url( urlKey );
	String host = url.getHost();
	int port = url.getPort();
	return( host + ":" + port );
}

void ActiveSocketConnection::closeSocket() {
	// signal to thread
	shutdownInProgress = true;
	disconnectSocket();
}

void ActiveSocketConnection::exitSocket() {
	waitThreadFinished();
}

void ActiveSocketConnection::sendText( SocketUrl& url , String text ) {
	// connect to external address
	if( !connected )
		ASSERTMSG( connectSocket() , "receiveText: unable to connect urlkey=" + urlKey );

	bool connectionClosed;
	protocol.writeMessage( socketHandle , text , connectionClosed );
	if( connectionClosed )
		handleBrokenConnection();
	else
		logger.logInfo( "sendText: sent message to urlkey=" + urlKey + ", text=" + text );
}

String ActiveSocketConnection::receiveText( SocketUrl& url , bool wait ) {
	// connect to external address
	if( !connected )
		ASSERTMSG( connectSocket() , "receiveText: unable to connect urlkey=" + urlKey );

	bool connectionClosed;
	String final;
	bool res = protocol.readMessage( socketHandle , final , wait , connectionClosed );
	if( connectionClosed )
		handleBrokenConnection();
	else
	if( res )
		logger.logInfo( "receiveText: received message from urlkey=" + urlKey +", text=" + final );

	if( !res )
		return( "" );

	return( final );
}

String ActiveSocketConnection::receiveFixedText( SocketUrl& url , int size , bool wait ) {
	// connect to external address
	if( !connected )
		ASSERTMSG( connectSocket() , "receiveFixedText: unable to connect urlkey=" + urlKey );

	bool connectionClosed;
	String final;
	bool res = protocol.readFixedSizeMessage( socketHandle , size , final , wait , connectionClosed );
	if( connectionClosed )
		handleBrokenConnection();
	else
	if( res )
		logger.logInfo( "receiveFixedText: received message from urlkey=" + urlKey +", text=" + final );

	if( !res )
		return( "" );

	return( final );
}

bool ActiveSocketConnection::waitReadSocket( SocketUrl& url , bool wait ) {
	// connect to external address
	if( !connected )
		ASSERTMSG( connectSocket() , "waitReadSocket: unable to connect urlkey=" + urlKey );

	return( protocol.waitSocketData( socketHandle , wait ) );
}

/*#########################################################################*/
/*#########################################################################*/

void ActiveSocketConnection::waitThreadFinished() {
	if( socketThread != NULL ) {
		ThreadService *ts = ThreadService::getService();
		ts -> waitThreadExited( socketThread );

		socketThread = NULL;
	}
}

bool ActiveSocketConnection::connectSocketProtected() {
	if( connected )
		return( true );

	try {
		SocketUrl su( urlKey );
		socketHandle = protocol.open( su , &addr );

		// start reading thread
		if( pub != NULL && threadStarted == false ) {
			ThreadService *ts = ThreadService::getService();
			socketThread = ts -> runThread( urlKey , this , ( ObjectThreadFunction )&ActiveSocketConnection::readSocketThread , NULL );
			threadStarted = true;
		}

		logger.logInfo( "connectSocketProtected: successfully connected to urlkey=" + urlKey );
		connected = true;
	}
	catch( RuntimeException& e ) {
		logger.printStack( e );
		ASSERTFAILED( "connectSocketProtected: runtime exception, urlkey=" +  urlKey );
	}

	return( true );
}

void ActiveSocketConnection::disconnectSocket() {
	if( connected ) {
		connected = false;

		_closesocket( socketHandle );
		socketHandle = INVALID_SOCKET;

		logger.logInfo( "disconnectSocket: disconnected urlkey=" + urlKey );
	}
}

void ActiveSocketConnection::handleBrokenConnection() {
	if( !shutdownInProgress )
		disconnectSocket();
}

void ActiveSocketConnection::readSocketThread( void *p_arg ) {
	try {
		readSocketThreadProtected();
	}
	catch( RuntimeException& e ) {
		logger.logInfo( "readSocketThread: unexpected exception for urlkey=" + urlKey );
		logger.printStack( e );
	}
}

void ActiveSocketConnection::readSocketThreadProtected() {
	while( !shutdownInProgress ) {
		if( !connected ) {
			// try to connect
			if( !connectSocket() ) {
				ThreadService *ts = ThreadService::getService();
				ts -> threadSleepMs( reconnectionTimeoutSec * 1000 );
				continue;
			}
		}

		// try reading
		bool connectionClosed;
		String final;
		bool res = protocol.readMessage( socketHandle , final , true , connectionClosed );
		if( connectionClosed ) {
			handleBrokenConnection();
			continue;
		}

		if( !res )
			continue;

		// write message to channel
		String msgid = pub -> publish( NULL , final );
		logger.logInfo( "readSocketThreadProtected: forwarded message from urlkey=" + urlKey + ", msgid=" + msgid + " to channel=" + pub -> getChannelName() );
	}

	threadStarted = false;
}

