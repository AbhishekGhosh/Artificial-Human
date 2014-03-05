#include <ah_platform.h>
#include <ah_media_impl.h>

// #############################################################################
// #############################################################################

Service *MediaService::newService() {
	return( new MediaService() );
}

void MediaService::configureService( Xml p_config ) {
	config = p_config;
}

void MediaService::createService() {
	Xml configListeners = config.getChildNode( "listeners" );
	createListeners( configListeners );

	Xml configActiveSockets = config.getChildNode( "directchannels" );
	createActiveSockets( configActiveSockets );
}

void MediaService::initService() {
	SocketProtocol::initSocketLib();
}

void MediaService::runService() {
	startListeners();
	startActiveSockets();
}

void MediaService::stopService() {
	stopListeners();
	stopActiveSockets();
}

void MediaService::exitService() {
	exitListeners();
	exitActiveSockets();
}

void MediaService::destroyService() {
	listeners.destroy();
	activeSockets.destroy();

	SocketProtocol::exitSocketLib();
}

/*#########################################################################*/
/*#########################################################################*/

SocketListener *MediaService::runListenerFactory( String name , Xml config ) {
	ASSERT( listeners.get( name ) == NULL );

	SocketListener *listener = new SocketServer( name );
	listener -> configure( config );

	listeners.add( name , listener );

	return( listener );
}

void MediaService::createListeners( Xml config ) {
	if( !config.exists() )
		return;

	// scan configuration
	for( Xml item = config.getFirstChild( "listener" ); item.exists(); item = item.getNextChild( "listener" ) ) {
		String name = item.getAttribute( "name" );
		if( !item.getBooleanAttribute( "run" ) ) {
			logger.logInfo( "createListeners: listener is not configured to run - name=" + name );
			continue;
		}

		// create and configure
		SocketListener *listener = runListenerFactory( name , item );
		logger.logInfo( "createListeners: listener created - name=" + name );
	}
}

void MediaService::startListeners() {
	// for all listeners
	for( int k = 0; k < listeners.count(); k++ ) {
		SocketListener *listener = listeners.getClassByIndex( k );

		// start
		String name = listener -> getName();
		ASSERTMSG( listener -> startListener() , 
			"startListeners: cannot start listener=" + name );
		logger.logInfo( "startListeners: listener started at " + listener -> getAddress() + " - name=" + name );
	}
}

ActiveSocket *MediaService::runActiveSocketFactory( String p_name , Xml config ) {
	ASSERT( activeSockets.get( p_name ) == NULL );

	ActiveSocket *as = new ActiveSocket( p_name );
	as -> configure( config );

	activeSockets.add( p_name , as );

	return( as );
}

void MediaService::createActiveSockets( Xml config ) {
	if( !config.exists() )
		return;

	// scan configuration
	for( Xml item = config.getFirstChild( "channel" ); item.exists(); item = item.getNextChild( "channel" ) ) {
		String name = item.getAttribute( "name" );
		if( !item.getBooleanAttribute( "run" ) ) {
			logger.logInfo( "createActiveSockets: direct channel is not configured to run - name=" + name );
			continue;
		}

		// create and configure
		ActiveSocket *ac = runActiveSocketFactory( name , item );
		logger.logInfo( "createActiveSockets: direct channel created - name=" + name );
	}
}

void MediaService::startActiveSockets() {
	// for all active sockets
	for( int k = 0; k < activeSockets.count(); k++ ) {
		ActiveSocket *ac = activeSockets.getClassByIndex( k );

		// start
		String name = ac -> getName();
		ASSERTMSG( ac -> startActiveSocket() , 
			"startActiveSockets: cannot start channel=" + name );
		logger.logInfo( "startActiveSockets: direct channel started name=" + name );
	}
}

void MediaService::stopActiveSockets() {
	for( int k = 0; k < activeSockets.count(); k++ ) {
		ActiveSocket *ac = activeSockets.getClassByIndex( k );
		ac -> stopActiveSocket();
	}
}

void MediaService::stopListeners() {
	for( int k = 0; k < listeners.count(); k++ ) {
		SocketListener *listener = listeners.getClassByIndex( k );
		listener -> stopListener();
	}
}

void MediaService::exitActiveSockets() {
	for( int k = 0; k < activeSockets.count(); k++ ) {
		ActiveSocket *ac = activeSockets.getClassByIndex( k );
		ac -> exitActiveSocket();
	}
}

void MediaService::exitListeners() {
	for( int k = 0; k < listeners.count(); k++ ) {
		SocketListener *listener = listeners.getClassByIndex( k );
		listener -> exitListener();
	}
}

ActiveSocket *MediaService::getActiveSocket( String name ) {
	ActiveSocket *socket = activeSockets.get( name );
	ASSERTMSG( socket != NULL , "Unable to find channel by name=" + name );
	return( socket );
}

void MediaService::sendTextToDirectChannel( String name , String url , String text ) {
	ActiveSocket *s = getActiveSocket( name );
	s -> sendText( url , text );
}

String MediaService::receiveTextFromDirectChannel( String name , String url , bool wait ) {
	ActiveSocket *s = getActiveSocket( name );
	return( s -> receiveText( url , wait ) );
}

String MediaService::receiveFixedSizeTextFromDirectChannel( String name , String url , int size , bool wait ) {
	ActiveSocket *s = getActiveSocket( name );
	return( s -> receiveFixedSizeText( url , size , wait ) );
}
