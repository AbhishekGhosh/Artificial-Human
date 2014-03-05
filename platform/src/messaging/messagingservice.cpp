#include <ah_platform.h>
#include <ah_messaging_impl.h>

// #############################################################################
// #############################################################################

MessagingService::MessagingService() {
	dataLock = NULL;
	lastSessionId = 0;
}

Service *MessagingService::newService() {
	return( new MessagingService() );
}

void MessagingService::configureService( Xml p_config ) {
	config = p_config;
}

void MessagingService::createService() {
	dataLock = rfc_lock_create();
	lastSessionId = 0;

	// create all channels
	Xml topics = config.getChildNode( "topics" );
	for( Xml topic = topics.getFirstChild( "topic" ); topic.exists(); topic = topic.getNextChild( "topic" ) )
		createChannel( topic );
}

void MessagingService::initService() {
	openAllChannels();
}

void MessagingService::runService() {
	startAllChannels();
}

void MessagingService::stopService() {
	stopAllChannels();
}

void MessagingService::exitService() {
	closeAllChannels();
}

void MessagingService::destroyService() {
	if( dataLock != NULL ) {
		rfc_lock_destroy( dataLock );
		dataLock = NULL;
	}

	mapChannels.destroy();
	sessions.destroy();
}

/*#########################################################################*/
/*#########################################################################*/

MessageSession *MessagingService::createSession() {
	lock();
	MessageSession *session = new MessageSession( ++lastSessionId );
	sessions.add( session -> getSessionId() , session );
	unlock();

	return( session );
}

void MessagingService::closeSession( MessageSession *session ) {
	lock();
	int id = session -> getSessionId();
	sessions.removeByKey( id );
	unlock();

	delete session;
}

bool MessagingService::checkChannelAvailable( String name ) {
	lock();
	MessageChannel *channel = mapChannels.get( name );
	unlock();

	if( channel != NULL && channel -> isOpened() )
		return( true );

	return( false );
}

MessagePublisher *MessagingService::createPublisher( MessageSession *session , String channel , String pubName , String msgtype ) {
	MessageChannel *ch = getChannel( channel );
	MessagePublisher *pub = new MessagePublisher( session , ch , pubName , msgtype );

	ch -> addPublisher( pubName , pub );
	logger.logInfo( "createPublisher: i/o publisher name=" + pubName + " started on " + channel + " channel" );

	return( pub );
}

MessageSubscription *MessagingService::subscribe( MessageSession *session , String channel , String subName , MessageSubscriber *subHandler ) {
	MessageChannel *ch = getChannel( channel );
	MessageSubscription *sub = new MessageSubscription( session , ch , subName , subHandler );

	ch -> addSubscription( subName , sub );
	logger.logInfo( "subscribe: i/o subscriber name=" + subName + " started on " + channel + " channel" );
	
	return( sub );
}

MessageSubscription *MessagingService::subscribeSelector( MessageSession *session , String channel , String selector , String subName , MessageSubscriber *subHandler ) {
	MessageChannel *ch = getChannel( channel );
	MessageSubscription *sub = new MessageSubscription( session , ch , subName , subHandler );
	sub -> setSelector( selector );

	ch -> addSubscription( subName , sub );
	logger.logInfo( "subscribeSelector: i/o subscriber name=" + subName + " started on " + channel + " channel" );
	
	return( sub );
}

bool MessagingService::destroyPublisher( MessagePublisher *publisher ) {
	MessagePublisher *pub = ( MessagePublisher * )publisher;
	MessageChannel *ch = pub -> channel;

	String name = pub -> name;
	if( ch != NULL ) {
		ch -> deletePublisher( name );
		logger.logInfo( "destroyPublisher: i/o publisher name=" + name + " stopped on " + ch -> getName() + " channel" );
	}

	delete pub;
	return( true );
}

bool MessagingService::unsubscribe( MessageSubscription *subscription ) {
	MessageSubscription *sub = subscription;
	MessageChannel *ch = sub -> channel;

	if( ch != NULL ) {
		ch -> deleteSubscription( sub -> name );
		logger.logInfo( "unsubscribe: i/o subscriber name=" + sub -> name + " unsubscribed from " + ch -> getName() + " channel" );
	}
	delete sub;
	return( true );
}

/*#########################################################################*/
/*#########################################################################*/

MessageChannel *MessagingService::createChannel( Xml config ) {
	String name = config.getAttribute( "name" );
	String msgid = config.getProperty( "msgid" );
	bool auth = config.getBooleanProperty( "auth" );
	bool sync = config.getBooleanProperty( "sync" );
	bool ignore = config.getBooleanProperty( "ignore" , false );

	MessageChannel *channel = new MessageChannel( msgid , name , sync , ignore );
	
	ASSERT( auth == false );

	mapChannels.add( name , channel );
	return( channel );
}

MessageChannel *MessagingService::getChannel( String name ) {
	lock();
	MessageChannel *channel = mapChannels.get( name );
	unlock();
	ASSERTMSG( channel != NULL , String( "i/o channel does not exist: name=" ) + name );

	return( channel );
}

void MessagingService::lock() {
	rfc_lock_shared( dataLock );
}

void MessagingService::unlock() {
	rfc_lock_release( dataLock );
}

void MessagingService::openAllChannels() {
	lock();

	// open all channels
	for( int k = 0; k < mapChannels.count(); k++ ) {
		MessageChannel *channel = mapChannels.getClassByIndex( k );

		// open given channel
		channel -> open();
	}

	unlock();
}

void MessagingService::startAllChannels() {
	lock();

	// close all channels
	for( int k = 0; k < mapChannels.count(); k++ ) {
		MessageChannel *channel = mapChannels.getClassByIndex( k );

		// start given channel
		channel -> start();
	}

	unlock();
}

void MessagingService::stopAllChannels() {
	lock();

	// close all channels
	for( int k = 0; k < mapChannels.count(); k++ ) {
		MessageChannel *channel = mapChannels.getClassByIndex( k );

		// close given channel
		channel -> stop();
	}

	unlock();
}

void MessagingService::closeAllChannels() {
	lock();

	// close all channels
	for( int k = 0; k < mapChannels.count(); k++ ) {
		MessageChannel *channel = mapChannels.getClassByIndex( k );

		// close given channel
		channel -> close();
	}

	unlock();
}
