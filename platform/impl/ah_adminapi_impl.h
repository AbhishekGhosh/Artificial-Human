#ifndef	INCLUDE_AH_ADMINAPI_SOCKET_H
#define INCLUDE_AH_ADMINAPI_SOCKET_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_media_impl.h"

// #############################################################################
// #############################################################################

class AdminApiSocket {
public:
	AdminApiSocket();
	~AdminApiSocket();

	void initThread();
	void exitThread();

	void connect( String url );
	void disconnect();
	bool isConnected();
	
	Xml execute( Xml req );
	void send( Xml req );

private:
	String setXmlRequestId( Xml req );

private:
	Logger logger;
	SOCKET socket;
	struct sockaddr_in addr;
	SocketProtocol protocol;
	int waitTimeSec;

	bool connected;
	int lastRequestNumber;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_ADMINAPI_SOCKET_H
