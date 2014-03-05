#include <ah_platform.h>
#include <ah_media_impl.h>

/*#########################################################################*/
/*#########################################################################*/

SocketProtocol::SocketProtocol( Logger& p_logger )
:	logger( p_logger ) {
	pin = FLOW_PROTOCOL_UNKNOWN;
	pout = FLOW_PROTOCOL_UNKNOWN;

	maxPacketSize = MAX_PACKET_SIZE_DEFAULT;
	maxReadSize = MAX_READ_SIZE_DEFAULT;
	waitTimeSec = WAIT_TIME_SEC_DEFAULT;

	shutdownInProgress = false;

	showMessagesIn = false;
	showMessagesOut = false;
	showPacketsIn = false;
	showPacketsOut = false;

	init();
}

void SocketProtocol::init() {
	state = '0';
	substate = '0';
	startLine.clear();
	headerLines.destroy();
	chunks.clear();
	inPending.clear();
}

void SocketProtocol::copy( SocketProtocol& src ) {
	pin = src.pin;;
	pout = src.pout;
	delimiterIn = src.delimiterIn;
	delimiterOut = src.delimiterOut;

	maxPacketSize = src.maxPacketSize;
	maxReadSize = src.maxReadSize;
	waitTimeSec = src.waitTimeSec;

	shutdownInProgress = false;

	showMessagesIn = src.showMessagesIn;
	showMessagesOut = src.showMessagesOut;
	showPacketsIn = src.showPacketsIn;
	showPacketsOut = src.showPacketsOut;

	init();
}

void SocketProtocol::initSocketLib() {
	WSADATA l_wsa;
	memset( &l_wsa, 0 , sizeof( WSADATA ) );
	WSAStartup( MAKEWORD( 2 , 2 ) , &l_wsa );
}

void SocketProtocol::exitSocketLib() {
	/* cleanup sockets */
	WSACleanup();
}

void SocketProtocol::create( Xml config ) {
	String protocol = config.getProperty( "protocol" );
	ASSERTMSG( !protocol.isEmpty() , "protocol is not defined" );

	maxPacketSize = config.getIntProperty( "protocol.max-packet-size" , MAX_PACKET_SIZE_DEFAULT );
	maxReadSize = config.getIntProperty( "protocol.max-read-size" , MAX_READ_SIZE_DEFAULT );
	waitTimeSec = config.getIntProperty( "protocol.wait-time-sec" , WAIT_TIME_SEC_DEFAULT );

	if( protocol.equals( "asymmetric" ) ) {
		createFlow( config , protocolNameIn , pin , delimiterIn , "protocol.inbound" , showPacketsIn , showMessagesIn );
		createFlow( config , protocolNameOut , pout , delimiterOut , "protocol.outbound" , showPacketsOut , showMessagesOut );
	}
	else {
		createFlow( config , protocolNameIn , pin , delimiterIn , "protocol" , showPacketsIn , showMessagesIn );
		protocolNameOut = protocolNameIn;
		pout = pin;
		delimiterOut = delimiterIn;
		showPacketsOut = showPacketsIn; 
		showMessagesOut = showMessagesIn;
	}
}

void SocketProtocol::createFlow( Xml config , String& protocolName , FLOW_PROTOCOL& proto , String& delimiter , String prototype , bool& showPackets , bool& showMessages ) {
	protocolName = config.getProperty( prototype );
	showMessages = config.getBooleanProperty( prototype + ".showmessages" );
	showPackets = config.getBooleanProperty( prototype + ".showpackets" );

	if( protocolName.equals( "xml-messages" ) ) {
		proto = FLOW_PROTOCOL_XML_MESSAGES;

		String s = config.getProperty( prototype + ".delimiter" , "" );
		delimiter = String::parseStringLiteral( s );
	}
	else
	if( protocolName.equals( "http" ) ) {
		proto = FLOW_PROTOCOL_HTTP_MESSAGES;
	}
	else
	if( protocolName.equals( "text-messages" ) ) {
		proto = FLOW_PROTOCOL_TEXT_MESSAGES;

		String s = config.getProperty( prototype + ".delimiter" , "" );
		delimiter = String::parseStringLiteral( s );
	}
	else
	if( protocolName.equals( "text-stream" ) ) {
		proto = FLOW_PROTOCOL_TEXT_STREAM;
		delimiter.clear();
	}
	else
	if( protocolName.equals( "binary-stream" ) ) {
		proto = FLOW_PROTOCOL_BINARY_STREAM;
		delimiter.clear();
	}
	else {
		ASSERTFAILED( "Unknown protocol=" + protocolName );
	}
}

SOCKET SocketProtocol::open( SocketUrl& url , struct sockaddr_in *addr ) {
	return( open( url.getHost() , url.getPort() , addr ) );
}

SOCKET SocketProtocol::open( String host , unsigned short port , struct sockaddr_in *addr ) {
	memset( addr , 0 , sizeof( struct sockaddr_in ) );

	addr -> sin_family = AF_INET;
	addr -> sin_port = htons( port );

	char c = *( const char * )host;
	if( isdigit( c ) ) {
		// get IP address
		unsigned long l_addr = inet_addr( host );
		addr -> sin_addr.S_un.S_addr = l_addr;
	}
	else {
		// get IP by hostname
		struct hostent *he = gethostbyname( host );
		ASSERTMSG( he != NULL , "Cannot resolve host address" );

		addr -> sin_addr.S_un.S_addr = *( unsigned long * )he -> h_addr_list[ 0 ];
	}
			
	SOCKET socketHandle = socket( AF_INET , SOCK_STREAM , 0 );
	int res = connect( socketHandle , ( struct sockaddr * )addr , sizeof( sockaddr_in ) );
	int errorCode = WSAGetLastError();
	ASSERTMSG( res == 0 , String( "Error code found: " ) + errorCode );

	unsigned long flag = 1;
	ioctlsocket( socketHandle , FIONBIO , &flag );

	return( socketHandle );
}

void SocketProtocol::close( SOCKET socket ) {
	shutdown( socket , SD_BOTH );
	_closesocket( socket );
}

bool SocketProtocol::waitSocketDataInfinite( SOCKET socket , bool& p_error ) {
	return( waitSocketDataTimeoutSec( socket , -1 , p_error ) );
}

bool SocketProtocol::waitSocketDataTimeoutSec( SOCKET socket , int p_sec , bool& p_error ) {
	struct fd_set l_set;
	struct timeval l_t;

	_fd_init( &l_set );
	_fd_sethnd( &l_set , socket );
	memset( &l_t , 0 , sizeof( struct timeval ) );

	struct timeval *l_pt = NULL;
	if( p_sec >= 0 ) {
		l_pt = &l_t;
		l_t.tv_sec = p_sec;
	}

	p_error = true;
	int l_res = select( 0 , &l_set , NULL , &l_set , l_pt );

	p_error = false;
	if( l_res <= 0 ) {
		if( l_res < 0 )
			p_error = true;
		return( false );
	}

	// check wakeup reason
	int l_check = 0;
	l_t.tv_sec = 0;

	// check exception
	_fd_checke( l_check , &l_set , &l_t , socket );
	if( l_check ) {
		p_error = true;
		return( false );
	}

	// check read status - should be
	p_error = false;
	_fd_checkr( l_check , &l_set , &l_t , socket );
	if( !l_check )
		return( false );

	return( true );
}

bool SocketProtocol::waitSocketData( SOCKET socket , bool p_wait ) {
	int socket_waitTimeSec = ( p_wait )? waitTimeSec : 0;

	bool error = false;
	if( waitSocketDataTimeoutSec( socket , socket_waitTimeSec , error ) )
		return( true );

	if( !shutdownInProgress )
		ASSERTFAILED( "SocketProtocol::waitSocketData: error while reading socket" );

	return( false );
}

bool SocketProtocol::readMessage( SOCKET socketHandle , String& msg , bool wait , bool& connectionClosed ) {
	Timer timer;
	timer.createRunWindowSec( waitTimeSec );

	return( readMessageInternal( socketHandle , msg , 0 , wait , timer , connectionClosed ) );
}

bool SocketProtocol::readXmlMessage( SOCKET socketHandle , Xml& xml , String& msg , bool wait , bool& connectionClosed ) {
	Timer timer;
	timer.createRunWindowSec( waitTimeSec );

	connectionClosed = false;
	if( pin == FLOW_PROTOCOL_HTTP_MESSAGES )
		return( readHttpMessageInternal( socketHandle , msg , wait , timer , connectionClosed ) );

	if( pin == FLOW_PROTOCOL_XML_MESSAGES )
		return( readXmlMessageInternal( socketHandle , xml , msg , wait , timer , connectionClosed ) );

	ASSERTFAILED( "SocketProtocol::readMessage: protocol is not defined to read from socket" );
	return( false );
}

void SocketProtocol::writeMessage( SOCKET socketHandle , const String& msg , bool& connectionClosed ) {
	if( pin == FLOW_PROTOCOL_HTTP_MESSAGES ) {
		// write HTTP request
		writeHttpMessageInternal( socketHandle , msg , connectionClosed );
	}
	else {
		// write message
		writeSocketInternal( socketHandle , msg , connectionClosed );

		// add delimiter if specified
		if( !delimiterOut.isEmpty() )
			writeSocketInternal( socketHandle , delimiterOut , connectionClosed );
	}

	if( showMessagesOut )
		logger.logDebug( String( "writeMessage: message sent, size=" ) + msg.length() + ", text=" + msg );
}

bool SocketProtocol::readFixedSizeMessage( SOCKET socketHandle , int size , String& msg , bool wait , bool& connectionClosed ) {
	Timer timer;
	timer.createRunWindowSec( waitTimeSec );

	return( readMessageInternal( socketHandle , msg , size , wait , timer , connectionClosed ) );
}

/*#########################################################################*/
/*#########################################################################*/

bool SocketProtocol::readXmlMessageInternal( SOCKET socketHandle , Xml& xml , String& msg , bool wait , Timer& timer , bool& connectionClosed ) {
	connectionClosed = false;

	while( true ) {
		// check time passed
		if( wait ) {
			if( !timer.go() )
				return( false );
		}

		// try to extract next doc
		if( inPending.length() > 0 ) {
			int len = 0;
			bool errflag = false;

			// parse string
			xml = Xml::parse( inPending , len , errflag );
			if( errflag ) {
				if( xml.exists() )
					xml.destroy();
				continue;
			}
				
			if( xml.exists() ) {
				msg = inPending.getMid( 0 , len );
				inPending.remove( 0 , len + delimiterIn.length() );
				return( true );
			}
		}

		// wait next chunk from socket
		if( !readSocketInternal( socketHandle , wait , timer , connectionClosed ) )
			return( false );
	}

	return( false );
}

bool SocketProtocol::readSocketInternal( SOCKET socketHandle , bool wait , Timer& timer , bool& connectionClosed ) {
	char buf[ 1024 ];

	while( true ) {
		int socket_waitTimeSec = 0;
		if( wait ) {
			socket_waitTimeSec = timer.timeRemainedSec();
			if( socket_waitTimeSec <= 0 )
				return( false );
		}

		// if wait succeeded
		logger.logDebug( String( "waitSocketDataTimeoutSec: socketHandle=" ) + socketHandle + ", socket_waitTimeSec=" + socket_waitTimeSec );
		if( waitSocketDataTimeoutSec( socketHandle , socket_waitTimeSec , connectionClosed ) )
			break;

		// check whether more wait is required
		if( connectionClosed || wait == false )
			return( false );
	}

	// read into buffer
	int l_recv = recv( socketHandle , buf , sizeof( buf ) - 1 , 0 );
	if( l_recv == 0 || l_recv == SOCKET_ERROR ) {
		connectionClosed = true;
		inPending.clear();
		return( false );
	}

	buf[ l_recv ] = 0;
	inPending += buf;

	if( showPacketsIn )
		logger.logDebug( String( "readSocketInternal: packet received text=" ) + buf );
	return( true );
}

void SocketProtocol::writeSocketInternal( SOCKET socketHandle , const char *s , bool& connectionClosed ) {
	if( s == NULL )
		return;

	int lenRemained = strlen( s );
	int lenSent = 0;
	connectionClosed = false;
	while( lenRemained > 0 ) {
		int len = send( socketHandle , s + lenSent , lenRemained , 0 );
		if( len == SOCKET_ERROR || len == 0 ) {
			connectionClosed = true;
			return;
		};

		if( showPacketsOut )
			logger.logDebug( String( "writeSocketInternal: packet sent, size=" ) + len + ", text=" + s );

		lenSent += len;
		lenRemained -= len;
	}
}

bool SocketProtocol::readMessageInternal( SOCKET socketHandle , String& msg , int fixedSize , bool wait , Timer& timer , bool& connectionClosed ) {
	connectionClosed = false;
	Xml xml;
	switch( pin ) {
		default :
		case FLOW_PROTOCOL_UNKNOWN :
			ASSERTFAILED( "readMessageInternal: protocol is not defined to read from socket" );
			break;

		case FLOW_PROTOCOL_HTTP_MESSAGES :
			if( !readHttpMessageInternal( socketHandle , msg , wait , timer , connectionClosed ) )
				return( false );
			break;

		case FLOW_PROTOCOL_XML_MESSAGES :
			if( !readXmlMessageInternal( socketHandle , xml , msg , wait , timer , connectionClosed ) )
				return( false );
			xml.destroy();
			break;

		case FLOW_PROTOCOL_TEXT_MESSAGES :
			if( !readTextMessageInternal( socketHandle , msg , fixedSize , wait , timer , connectionClosed ) )
				return( false );
			break;

		case FLOW_PROTOCOL_TEXT_STREAM :
			if( !readTextStreamInternal( socketHandle , msg , fixedSize , wait , timer , connectionClosed ) )
				return( false );
			break;
	}

	// xml is actualy not required
	if( fixedSize > 0 && fixedSize != msg.length() )
		ASSERTFAILED( "SocketProtocol::readMessageInternal - returned xml message has length, different from fixed size=" + fixedSize );

	if( showMessagesIn )
		logger.logDebug( String( "readMessageInternal: message received, size=" ) + msg.length() + ", text=" + msg );
	return( true );
}

bool SocketProtocol::readTextMessageInternal( SOCKET socketHandle , String& msg , int fixedSize , bool wait , Timer& timer , bool& connectionClosed ) {
	char buf[ 1024 ];
	while( true ) {
		// check timer
		if( wait ) {
			if( timer.timeRemainedSec() <= 0 )
				return( false );
		}

		// message with delimiters
		int index = inPending.find( delimiterIn );
		if( index > 0 ) {
			msg = inPending.getMid( 0 , index );
			inPending.remove( 0 , index + delimiterIn.length() );
			if( fixedSize > 0 )
				ASSERTMSG( msg.length() == fixedSize , String( "SocketProtocol::readTextMessageInternal - received message has size=" ) + msg.length() + ", expected size=" + fixedSize );
			return( true );
		}

		// wait next chunk from socket
		while( true ) {
			int socket_waitTimeSec = 0;
			if( wait ) {
				socket_waitTimeSec = timer.timeRemainedSec();
				if( socket_waitTimeSec <= 0 )
					return( false );
			}
			if( waitSocketDataTimeoutSec( socketHandle , socket_waitTimeSec , connectionClosed ) )
				break;

			if( connectionClosed )
				return( false );

			if( !wait )
				return( false );
		}

		// read into buffer
		int l_recv = recv( socketHandle , buf , sizeof( buf ) - 1 , 0 );
		if( l_recv == 0 || l_recv == SOCKET_ERROR ) {
			connectionClosed = true;
			inPending.clear();
			return( false );
		}

		buf[ l_recv ] = 0;
		if( showPacketsIn )
			logger.logDebug( String( "readMessageInternal: packet received, size=" ) + l_recv + ", text=" + buf );

		// treat packet as message
		if( delimiterIn.isEmpty() ) {
			// check fixed size
			if( fixedSize > 0 )
				ASSERTMSG( l_recv == fixedSize , String( "SocketProtocol::readMessageInternal - received message has size=" ) + l_recv + ", expected size=" + fixedSize );

			msg = buf;
			return( true );
		}

		inPending += buf;
	}

	return( false );
}

bool SocketProtocol::readTextStreamInternal( SOCKET socketHandle , String& msg , int fixedSize , bool wait , Timer& timer , bool& connectionClosed ) {
	int packetSize = 0;
	inPending.clear();

	char buf[ 1024 ];
	while( true ) {
		if( fixedSize > 0 ) {
			// if waiting for fixed size data
			int socket_waitTimeSec = 0;
			if( wait ) {
				socket_waitTimeSec = timer.timeRemainedSec();
				if( socket_waitTimeSec <= 0 )
					return( false );
			}

			if( !waitSocketDataTimeoutSec( socketHandle , socket_waitTimeSec , connectionClosed ) ) {
				if( connectionClosed )
					return( false );

				// in wait mode try again
				if( wait )
					continue;

				// return no message
				return( false );
			}
		}
		else {
			// just any part of text stream - wait can be only for first time
			bool waitCall = ( packetSize == 0 )? wait : false;
			int socket_waitTimeSec = 0;
			if( waitCall ) {
				socket_waitTimeSec = timer.timeRemainedSec();
				if( socket_waitTimeSec <= 0 )
					return( false );
			}

			if( !waitSocketDataTimeoutSec( socketHandle , socket_waitTimeSec , connectionClosed ) ) {
				if( connectionClosed )
					return( false );

				if( packetSize > 0 ) {
					msg = inPending;
					inPending.clear();
					return( true );
				}

				// in wait mode need at least one packet
				if( wait )
					continue;

				// return no message
				return( false );
			}
		}

		int readOneSize = ( fixedSize > 0 )? fixedSize - packetSize : maxPacketSize - packetSize;
		if( readOneSize > maxReadSize )
			readOneSize = maxReadSize;

		// allocate space
		inPending.resize( readOneSize + packetSize );

		// read packet
		char *p = inPending.getBuffer();
		int l_recv = recv( socketHandle , p + packetSize , readOneSize , 0 );
		if( l_recv == SOCKET_ERROR || l_recv == 0 ) {
			connectionClosed = true;
			inPending.clear();
			return( false );
		}

		packetSize += l_recv;
		p[ packetSize ] = 0;
		if( showPacketsIn )
			logger.logDebug( String( "readMessageInternal: packet received, size=" ) + l_recv + ", text=" + buf );

		// check fixed size is received
		if( fixedSize > 0 && packetSize == fixedSize ) {
			msg = inPending;
			inPending.clear();
			return( true );
		}

		// check max size is received
		if( fixedSize <= 0 && packetSize == maxPacketSize ) {
			msg = inPending;
			inPending.clear();
			return( true );
		}
	}
}

void SocketProtocol::writeHttpMessageInternal( SOCKET socketHandle , const String& s , bool& connectionClosed ) {
	// form request based on requested page=s
	String request = "GET " + s + " HTTP/1.1\n\r\n";
	writeSocketInternal( socketHandle , request , connectionClosed );
}

bool SocketProtocol::readHttpMessageInternal( SOCKET socketHandle , String& msg , bool wait , Timer& timer , bool& connectionClosed ) {
	// start-line
	if( state == '0' ) {
		if( !readHttpMessageStartLine( socketHandle , wait , timer , connectionClosed ) )
			return( false );
		state = 'H';
	}

	// *message-header CRLF
	if( state == 'H' ) {
		if( !readHttpMessageHeader( socketHandle , wait , timer , connectionClosed ) )
			return( false );
		state = 'B';
		substate = '0';
	}

	if( state == 'B' ) {
		// [ message-body ]
		if( !readHttpMessageBody( socketHandle , msg , wait , timer , connectionClosed ) )
			return( false );
		state = '0';
		substate = '0';
	}

	return( true );
}

bool SocketProtocol::readHttpMessageStartLine( SOCKET socketHandle , bool wait , Timer& timer , bool& connectionClosed ) {
	headerLines.destroy();
	startLine.clear();

	while( true ) {
		// check line completed
		int index = inPending.find( "\r\n" );
		if( index >= 0 ) {
			startLine = inPending.getMid( 0 , index );
			inPending.remove( 0 , index + 2 );
			return( true );
		}

		if( wait ) {
			if( !timer.go() )
				return( false );
		}

		// read more data
		if( !readSocketInternal( socketHandle , wait , timer , connectionClosed ) )
			return( false );
	}

	return( false );
}

bool SocketProtocol::readHttpMessageHeader( SOCKET socketHandle , bool wait , Timer& timer , bool& connectionClosed ) {
	while( true ) {
		if( wait ) {
			if( !timer.go() )
				return( false );
		}

		// check line completed
		int index = inPending.find( "\r\n" );
		if( index >= 0 ) {
			String line = inPending.getMid( 0 , index );
			inPending.remove( 0 , index + 2 );

			// check end
			if( line.isEmpty() )
				break;

			// add
			int index = line.find( ":" );
			ASSERTMSG( index > 0 , "Unexpected message header format:" + line );

			String key = line.getMid( 0 , index );
			if( line.getChar( index + 1 ) == ' ' )
				index++;

			String value = line.getMid( index + 1 );

			headerLines.add( key , value );
			continue;
		}

		// read more data
		if( !readSocketInternal( socketHandle , wait , timer , connectionClosed ) )
			return( false );
	}

	return( true );
}

bool SocketProtocol::readHttpMessageBody( SOCKET socketHandle , String& msg , bool wait , Timer& timer , bool& connectionClosed ) {
	if( substate == '0' ) {
		// should be set by Content-Length
		String value = headerLines.get( "Content-Length" );
		if( !value.isEmpty() ) {
			int size = atoi( value );
			return( readHttpMessageBodyChunk( socketHandle , msg , size , wait , timer , connectionClosed ) );
		}
		
		// chunked is expected
		String chunked = headerLines.get( "Transfer-Encoding" );
		ASSERTMSG( chunked.equals( "chunked" ) , "SocketProtocol::readHttpMessageBody - unexpectedly message has no Content-Length header property" );

		substate = 'S';
	}

	// read by chunk
	// Chunked-Body   = *chunk
	while( true ) {
		if( wait ) {
			if( !timer.go() )
				return( false );
		}

		// chunk size
		if( substate == 'S' ) {
			// read line
			int index = inPending.find( "\r\n" );
			if( index == 0 ) {
				inPending.remove( 0 , 2 );
				continue;
			}

			if( index > 0 ) {
				// read size
				int size;
				if( sscanf( inPending , "%x" , &size ) != 1 )
					ASSERTFAILED( "SocketProtocol::readHttpMessageBody - unable to read chunk size inPending=" + inPending );
				inPending.remove( 0 , index + 2 );

				chunkSize = size;
				substate = 'C';
				
				// "0" CRLF
				if( chunkSize == 0 )
					substate = 'F';
			}
		}

		// chunk text
		if( substate == 'C' ) {
			String chunk;
			if( !readHttpMessageBodyChunk( socketHandle , chunk , chunkSize , wait , timer , connectionClosed ) )
				return( false );

			// add to chunks
			chunks += chunk;
			substate = 'S';
			continue;
		}

		// footer
		// *entity-header
		if( substate == 'F' ) {
			int index = inPending.find( "\r\n" );
			if( index > 0 ) {
				// skip footer line
				inPending.remove( 0 , index + 2 );
				continue;
			}

			// end of footer
			// CRLF
			if( index == 0 ) {
				inPending.remove( 0 , 2 );
				break;
			}
		}

		// read more data
		if( !readSocketInternal( socketHandle , wait , timer , connectionClosed ) )
			return( false );
	}

	msg = chunks;
	return( true );
}

bool SocketProtocol::readHttpMessageBodyChunk( SOCKET socketHandle , String& msg , int size , bool wait , Timer& timer , bool& connectionClosed ) {
	// read body
	while( true ) {
		int len = inPending.length();
		if( len >= size ) {
			// all data available
			msg = inPending.getMid( 0 , size );
			inPending.remove( 0 , size );
			break;
		}

		if( wait ) {
			if( !timer.go() )
				return( false );
		}

		if( !readSocketInternal( socketHandle , wait , timer , connectionClosed ) )
			return( false );
	}

	return( true );
}

