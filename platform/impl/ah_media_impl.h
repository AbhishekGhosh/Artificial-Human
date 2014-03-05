#ifndef	INCLUDE_AH_MEDIA_IMPL_H
#define INCLUDE_AH_MEDIA_IMPL_H

/*#########################################################################*/
/*#########################################################################*/

#include <winsock2.h>

#include <ah_platform.h>

#define _closesocket	closesocket
#define _ioctlsocket	ioctlsocket

#define _fd_init( _fd_set ) \
	(_fd_set) -> fd_count = 0

#define _fd_sethnd( _fd_set , _fd_hnd ) \
	(_fd_set) -> fd_array[ (_fd_set) -> fd_count++ ] = _fd_hnd

#define _fd_checkr( _res , _fd_set , _fd_time , _fd_hnd ) \
	(_fd_set) -> fd_count = 1; \
	(_fd_set) -> fd_array[ 0 ] = _fd_hnd; \
	_res = select( 0 , _fd_set , NULL , NULL , _fd_time )
	
#define _fd_checkw( _res , _fd_set , _fd_time , _fd_hnd ) \
	(_fd_set) -> fd_count = 1; \
	(_fd_set) -> fd_array[ 0 ] = _fd_hnd; \
	_res = select( 0 , NULL , _fd_set , NULL , _fd_time )

#define _fd_checke( _res , _fd_set , _fd_time , _fd_hnd ) \
	(_fd_set) -> fd_count = 1; \
	(_fd_set) -> fd_array[ 0 ] = _fd_hnd; \
	_res = select( 0 , NULL , NULL , _fd_set , _fd_time )

class SocketListener;
class SocketServer;
class ListenerSocketConnection;
class ActiveSocket;
class ActiveSocketConnection;
class SocketUrl;

/*#########################################################################*/
/*#########################################################################*/

class SocketProtocol {
// protocol configuration
public:
	typedef enum {
		FLOW_PROTOCOL_UNKNOWN = 0 , 
		FLOW_PROTOCOL_TEXT_MESSAGES = 1 ,
		FLOW_PROTOCOL_XML_MESSAGES = 2 ,
		FLOW_PROTOCOL_HTTP_MESSAGES = 3 ,
		FLOW_PROTOCOL_TEXT_STREAM = 4 ,
		FLOW_PROTOCOL_BINARY_STREAM = 5
	} FLOW_PROTOCOL;

	String protocolNameIn;
	String protocolNameOut;
	FLOW_PROTOCOL pin;
	FLOW_PROTOCOL pout;
	String delimiterIn;
	String delimiterOut;
	bool showMessagesIn;
	bool showMessagesOut;
	bool showPacketsIn;
	bool showPacketsOut;

	int maxPacketSize;
	static const int MAX_PACKET_SIZE_DEFAULT = 1000000;
	int maxReadSize;
	static const int MAX_READ_SIZE_DEFAULT = 4096;
	int waitTimeSec;
	static const int WAIT_TIME_SEC_DEFAULT = 5;

private:
	String inPending;
	bool shutdownInProgress;
	Logger& logger;

	// message data
	String startLine;
	MapStringToString headerLines;
	String chunks;
	char state;
	char substate;
	int chunkSize;

public:
	SocketProtocol( Logger& logger );
	void create( Xml config );
	void copy( SocketProtocol& src );
	void init();

public:
	static void initSocketLib();
	static void exitSocketLib();
	static bool waitSocketDataInfinite( SOCKET socket , bool& p_error );
	static bool waitSocketDataTimeoutSec( SOCKET socket , int p_sec , bool& p_error );
	static SOCKET open( SocketUrl& url , struct sockaddr_in *addr );
	static SOCKET open( String host , unsigned short port , struct sockaddr_in *addr );
	static void close( SOCKET socket );

public:
	bool readMessage( SOCKET socketHandle , String& msg , bool wait , bool& connectionClosed );
	bool readFixedSizeMessage( SOCKET socketHandle , int size , String& msg , bool wait , bool& connectionClosed );

	bool readXmlMessage( SOCKET socketHandle , Xml& xml , String& msg , bool wait , bool& connectionClosed );
	void writeMessage( SOCKET socketHandle , const String& msg , bool& connectionClosed );
	bool waitSocketData( SOCKET socket , bool p_wait );

// internals
private:
	void createFlow( Xml config , String& protocolName , FLOW_PROTOCOL& proto , String& delimiter , String prototype , bool& showPackets , bool& showMessages );

	bool readMessageInternal( SOCKET socketHandle , String& msg , int fixedSize , bool wait , Timer& timer , bool& connectionClosed );
	bool readXmlMessageInternal( SOCKET socketHandle , Xml& xml , String& msg , bool wait , Timer& timer , bool& connectionClosed );
	bool readTextMessageInternal( SOCKET socketHandle , String& msg , int fixedSize , bool wait , Timer& timer , bool& connectionClosed );
	bool readTextStreamInternal( SOCKET socketHandle , String& msg , int fixedSize , bool wait , Timer& timer , bool& connectionClosed );
	bool readHttpMessageInternal( SOCKET socketHandle , String& msg , bool wait , Timer& timer , bool& connectionClosed );
	bool readHttpMessageStartLine( SOCKET socketHandle , bool wait , Timer& timer , bool& connectionClosed );
	bool readHttpMessageHeader( SOCKET socketHandle , bool wait , Timer& timer , bool& connectionClosed );
	bool readHttpMessageBody( SOCKET socketHandle , String& msg , bool wait , Timer& timer , bool& connectionClosed );
	bool readHttpMessageBodyChunk( SOCKET socketHandle , String& msg , int size , bool wait , Timer& timer , bool& connectionClosed );
	void writeHttpMessageInternal( SOCKET socketHandle , const String& msg , bool& connectionClosed );
	void writeSocketInternal( SOCKET socketHandle , const char *s , bool& connectionClosed );
	bool readSocketInternal( SOCKET socketHandle , bool wait , Timer& timer , bool& connectionClosed );
};

/*#########################################################################*/
/*#########################################################################*/

class SocketListener : public Object {
private:
	SocketProtocol protocol;
	int lastConnectionId;
	Message::MsgType msgType;
	String contentType;
	String name;
	MapStringToClass<ListenerSocketConnection> connections;

public:
	// interface
	virtual void configure( Xml config ) = 0;
	virtual bool startListener() = 0;
	virtual void stopListener() = 0;
	virtual void exitListener() = 0;
	virtual String getAddress() = 0;

public:
	SocketListener( String name );
	virtual ~SocketListener();

	virtual const char *getClass() { return( "SocketListener" ); };

public:
	void setMsgType( Message::MsgType p_msgType ) { msgType = p_msgType; };
	Message::MsgType getMsgType() { return( msgType ); };
	void setContentType( String p_contentType ) { contentType = p_contentType; };
	String getContentType() { return( contentType ); };

	String getName() { return( name ); };
	SocketProtocol& getProtocol() { return( protocol ); };

	void addListenerConnection( ListenerSocketConnection *connection );
	void removeListenerConnection( ListenerSocketConnection *connection );
	void stopListenerConnections();
	void exitListenerConnections();
};

/*#########################################################################*/
/*#########################################################################*/

class ActiveSocket : public Object , public MessageSubscriber {
private:
	typedef enum { 
		CONNECTION_SIGNLE = 1 ,
		CONNECTION_PERMANENT = 2 ,
		CONNECTION_MULTIPLE = 3
	} ConnectionType;

	typedef enum { 
		SESSION_PERMANENT = 1 ,
		SESSION_REQUESTRESPONSE = 2 ,
		SESSION_REQUEST = 3
	} SessionType;

	SocketProtocol protocol;
	RFC_HND lock;

	bool shutdownInProgress;

	// configuration
	int reconnectionTimeout;
	String name;
	String host;
	int port;
	ConnectionType connectionType;
	SessionType sessionType;

	bool redirectInbound;
	bool redirectOutbound;
	String inboundChannelName;
	String outboundChannelName;

	// messaging
	MessageSubscription *sub;
	MessagePublisher *pub;

	// multiple connections - map "<protocol>:<port>//<host>" to connection created
	MapStringToClass<ActiveSocketConnection> connections;
	ActiveSocketConnection *connection;

public:
	// interface
	void sendText( String url , String text );
	String receiveText( String url , bool wait );
	String receiveFixedSizeText( String url , int size , bool wait );
	bool waitReadSocket( String url , bool wait );

	void configure( Xml config );
	bool startActiveSocket();
	void stopActiveSocket();
	void exitActiveSocket();

protected:
	virtual void onTextMessage( TextMessage *msg );

public:
	ActiveSocket( String name );
	virtual ~ActiveSocket();
	virtual const char *getClass() { return( "ActiveSocket" ); };

public:
	String getName();

private:
	ActiveSocketConnection *getConnection( bool sendWay , String url , SocketUrl& su );
	ActiveSocketConnection *openConnection();
};

/*#########################################################################*/
/*#########################################################################*/

class ActiveSocketConnection : public Object {
private:
	String urlKey;
	SocketProtocol& protocol;
	MessagePublisher *pub;

	SOCKET socketHandle;
	struct sockaddr_in addr;
	bool continueRead;
	bool connected;
	RFC_HND socketThread;
	bool threadStarted;
	RFC_HND lock;
	bool shutdownInProgress;
	int reconnectionTimeoutSec;

public:
	String getUrlKey();
	void setUrlKey( String p_urlkey );
	void setReconnectionTimeoutSec( int reconnectionTimeoutSec );

	bool connectSocket();
	void closeSocket();
	void exitSocket();

	String getAddress();
	void sendText( SocketUrl& su , String text );
	String receiveText( SocketUrl& su , bool wait );
	String receiveFixedText( SocketUrl& su , int size , bool wait);
	bool waitReadSocket( SocketUrl& su , bool wait );

public:
	ActiveSocketConnection( String activeSocketName , String p_urlkey , SocketProtocol& protocol , MessagePublisher *pub );
	~ActiveSocketConnection();

protected:
	virtual const char *getClass() { return( "ActiveSocketConnection" ); };

private:
	bool connectSocketProtected();
	void disconnectSocket();
	void handleBrokenConnection();
	void readSocketThread( void *p_arg );
	void readSocketThreadProtected();
	void waitThreadFinished();
};

/*#########################################################################*/
/*#########################################################################*/

class ListenerSocketConnection : public Object , public MessageSubscriber {
private:
	SocketProtocol protocol;

	SocketServer *server;
	MessagePublisher *pub;
	MessageSubscription *sub;
	Message::MsgType msgType;

	SOCKET socket;
	struct sockaddr_in addr;
	String message;

	RFC_HND thread;
	bool threadStarted;
	bool continueRead;
	bool connected;
	bool logout;

	// connection
	SocketListener *listener;
	MessageSession *session;

public:
	ListenerSocketConnection( SocketServer *server , SOCKET clientSocket , struct sockaddr_in *clientAddress , Message::MsgType msgType );
	~ListenerSocketConnection();

	virtual const char *getClass() { return( "ListenerSocketConnection" ); };

public:
	bool startConnection();
	void stopConnection();
	void exitConnection();

	String getClientSocketName();

	void readMessages();
	void sendString( const char *p_msg , int len );
	void writeMessage( TextMessage *p_msg );

	// subscriber
	virtual void onTextMessage( TextMessage *msg );
	virtual void onXmlMessage( XmlMessage *msg );
	virtual void onXmlCall( XmlCall *msg );

	// connection
	void setListener( SocketListener *p_listener ) { listener = p_listener; };
	SocketListener *getListener() { return( listener ); };

	void setName( String name );
	String getName() { return( getInstance() ); };

	void setSession( MessageSession *p_session ) { session = p_session; };
	MessageSession *getSession() { return( session ); };

public:
	void threadClientFunction( void *p_arg );

private:
	void tryLogin( const char *p_msg );
	void performRead();
	void processMessage( const char *p_msg );
};

/*#########################################################################*/
/*#########################################################################*/

class SocketServer : public SocketListener
{
public:
	SocketServer( String name );
	~SocketServer();

public:
	virtual const char *getClass() { return( "SocketServer" ); };
	virtual void configure( Xml config );
	virtual bool startListener();
	virtual void stopListener();
	virtual void exitListener();
	virtual String getAddress();

public:
	String getTopicIn() { return( topicIn ); };
	String getTopicOut() { return( topicOut ); };
	String getContentType() { return( contentType ); };

	bool getAuth() { return( auth ); };
	bool getWayIn() { return( wayIn ); };
	bool getWayOut() { return( wayOut ); };

	void acceptConnectionLoop();
	bool openListeningPort();
	void closeListeningPort();
	void exitListeningPort();

	void performConnect();
	bool startConnection( SOCKET clientSocket , struct sockaddr_in *clientAddress );
	void threadConnectFunction( void *p_arg );

	static String getAddress( struct sockaddr_in *clientAddress );

private:
	String name;

	bool continueConnecting;
	bool shutdownInProgress;
	int port;

	bool auth;
	bool wayIn;
	bool wayOut;
	String topicIn;
	String topicOut;
	String contentType;

	RFC_HND listenThread;
	SOCKET listenSocket;
	struct sockaddr_in listen_inet;
};

/*#########################################################################*/
/*#########################################################################*/

class SocketUrl : public Object {
private:
	String protocol;
	String host;
	int port;
	String page;

public:
	String getValidUrl();
	String getValidUrlWithoutPage();
	String getProtocol();
	String getHost();
	int getPort();
	String getPage();

	void setUrl( String url );
	void setProtocol( String protocol );
	void setHost( String host );
	void setPort( int port );
	void setPage( String page );

public:
	SocketUrl();
	SocketUrl( String url );
	virtual const char *getClass() { return( "SocketUrl" ); };
};

/*#########################################################################*/
/*#########################################################################*/

#endif	// INCLUDE_AIMEDIA_IMPL_H
