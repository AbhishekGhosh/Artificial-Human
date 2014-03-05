#ifndef	INCLUDE_AH_MEDIA_H
#define INCLUDE_AH_MEDIA_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_services.h"
#include "ah_messaging.h"

class MediaService;

/*#########################################################################*/
/*#########################################################################*/

class SocketListener;
class ActiveSocket;

// handles different media AI will use to communicate
class MediaService : public Service {
public:
	void sendTextToDirectChannel( String name , String url , String text );
	String receiveTextFromDirectChannel( String name , String url , bool wait );
	String receiveFixedSizeTextFromDirectChannel( String name , String url , int size , bool wait );

	virtual const char *getServiceName() { return( "MediaService" ); };
	virtual void configureService( Xml config );
	virtual void createService();
	virtual void initService();
	virtual void runService();
	virtual void stopService();
	virtual void exitService();
	virtual void destroyService();

// engine helpers
protected:
	MediaService() {};
public:
	static Service *newService();
	static MediaService *getService() { return( ( MediaService * )ServiceManager::getInstance().getService( "MediaService" ) ); };

// internals
public:
	SocketListener *getListener( String name );

private:
	// implement set of listeners
	void createListeners( Xml config );
	void startListeners();
	void stopListeners();
	void exitListeners();
	SocketListener *runListenerFactory( String name , Xml config );

	// implement active connections
	void createActiveSockets( Xml config );
	void startActiveSockets();
	void stopActiveSockets();
	void exitActiveSockets();
	ActiveSocket *runActiveSocketFactory( String name , Xml config );
	ActiveSocket *getActiveSocket( String name );

private:
	Xml config;
	MapStringToClass<SocketListener> listeners;
	MapStringToClass<ActiveSocket> activeSockets;
};

/*#########################################################################*/
/*#########################################################################*/

#endif	// INCLUDE_AH_MEDIA_H
