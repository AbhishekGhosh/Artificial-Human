#include <ah_platform.h>
#include <ah_messaging_impl.h>

/*#########################################################################*/
/*#########################################################################*/

// input/output messages
// class AIIODuplexChannel
MessageChannel::MessageChannel( String p_msgid , String p_name , bool p_sync , bool p_ignore ) {
	name = p_name;

	opened = false;
	sync = p_sync;
	ignore = p_ignore;

	running = false;
	stopping = false;
	queueMessageId = 0;

	messages = NULL;
	channelLock = rfc_lock_create();

	Object::setInstance( "mc." + name );
}

MessageChannel::~MessageChannel() {
	close();
	rfc_lock_destroy( channelLock );

	if( messages != NULL )
		delete messages;
}

/*#########################################################################*/
/*#########################################################################*/

String MessageChannel::getName() {
	return( name );
}

void MessageChannel::threadChannelFunction( void *p_arg ) {
	processMessages();
}

void MessageChannel::open() {
	attachLogger();

	lock();
	if( opened ) {
		unlock();
		return;
	}

	messages = new MessageQueue( Object::getInstance() );
	opened = true;
}

void MessageChannel::start() {
	lock();

	// create channel thread
	if( !sync ) {
		ThreadService *ts = ThreadService::getService();
		channelThread = ts -> runThread( name ,  this , ( ObjectThreadFunction )&MessageChannel::threadChannelFunction , NULL );
		running = true;
	}

	unlock();
}

void MessageChannel::stop() {
	lock();

	if( !running ) {
		unlock();
		return;
	}

	// mark stopping and wakeup thread
	stopping = true;
	messages -> wakeup();

	unlock();
}

void MessageChannel::close() {
	lock();

	if( !opened ) {
		unlock();
		return;
	}

	// close queue and cleanup
	opened = false;
	if( messages != NULL )
		messages -> makeEmptyAndWakeup();

	if( running ) {
		// stop thread
		ThreadService *ts = ThreadService::getService();
		ts -> waitThreadExited( channelThread );
		channelThread = NULL;
		running = false;
	}

	// clear subscribers and publishers
	disconnectSubscriptions();
	disconnectPublishers();

	// stopped
	stopping = false;
	unlock();
}

String MessageChannel::publish( MessageSession *p_session , MessagePublisher *pub , const char *msg ) {
	TextMessage *l_msg = new TextMessage();
	l_msg -> setMessageType( pub -> msgtype );
	l_msg -> setText( msg );
	return( publish( p_session , pub , l_msg ) );
}

String MessageChannel::publish( MessageSession *p_session , MessagePublisher *pub , Message *msg ) {
	ASSERTMSG( opened , "cannot publish to i/o channel name=" + name + " - not open" );

	String id = getNewMessageId();
	msg -> setChannelMessageId( id ); 
	msg -> setSourceId( pub -> name );
	msg -> setSession( p_session );

	if( sync ) {
		subscribeEvent( msg );
		delete msg;
	}
	else
		messages -> addMessage( msg );

	return( id );
}

String MessageChannel::publish( MessageSession *p_session , Message *msg ) {
	ASSERTMSG( opened , "cannot publish to i/o channel name=" + name + " - not open" );

	String id = getNewMessageId();
	msg -> setChannelMessageId( id ); 
	msg -> setSession( p_session );

	if( sync ) {
		subscribeEvent( msg );
		delete msg;
	}
	else
		messages -> addMessage( msg );

	return( id );
}

void MessageChannel::addSubscription( String key , MessageSubscription *sub ) {
	lock();
	ASSERT( subs.get( key ) == NULL );
	subs.add( key , sub );
	unlock();
}

void MessageChannel::deleteSubscription( String key ) {
	lock();
	MessageSubscription *sub = subs.get( key );
	if( sub != NULL )
		subs.remove( key );
	unlock();
}

void MessageChannel::addPublisher( String key , MessagePublisher *pub ) {
	lock();
	MessagePublisher *tmp = pubs.get( key );
	if( tmp != NULL ) {
		unlock();
		ASSERT( false );
	}

	pubs.add( key , pub );
	unlock();
}

void MessageChannel::deletePublisher( String key ) {
	lock();
	MessagePublisher *pub = pubs.get( key );
	if( pub != NULL )
		pubs.remove( key );
	unlock();
}

// executing in separate thread
void MessageChannel::processMessages() {
	logger.logInfo( String( "processMessages: channel is opened" ) );
	while( true ) {
		// get next message
		Message *message = messages -> getNextMessage();
		if( message == NULL ) {
			// exit if channel should be stopped
			if( stopping )
				break;

			// ignore wakeup
			continue;
		}

		// log
		logger.logInfo( String( "processMessages: message extracted, msgid=" ) + message -> getChannelMessageId() );

		// pass message
		subscribeEvent( message );
		delete message;
	}

	opened = false;
	logger.logInfo( "processMessages: closed channel" );
}

/*#########################################################################*/
/*#########################################################################*/

void MessageChannel::subscribeEvent( Message *p_msg ) {
	if( !ignore ) {
		lock();
		MessageSession *l_session = p_msg -> getSession();
		for( int k = 0; k < subs.count(); k++ ) {
			MessageSubscription *sub = subs.getClassByIndex( k );
			MessageSession *l_sub_session = sub -> session;
			if( l_sub_session == NULL || l_session == l_sub_session )
				sub -> processMessage( p_msg );
		}
		unlock();
	}

	// after all executions
	p_msg -> postExecute();
}

void MessageChannel::lock() {
	rfc_lock_shared( channelLock );
}

void MessageChannel::unlock() {
	rfc_lock_release( channelLock );
}

String MessageChannel::getNewMessageId() {
	// assign message number
	String msgId = Object::getInstance();
	msgId += "-";
	char l_buf[ 10 ];

	int id = rfc_thr_increment( &queueMessageId );
	sprintf( l_buf , "%d" , id );
	msgId += l_buf;
	
	return( msgId );
}

void MessageChannel::disconnectSubscriptions() {
	for( int k = 0; k < subs.count(); k++ ) {
		MessageSubscription *sub = subs.getClassByIndex( k );
		sub -> disconnected();
	}
	subs.clear();
}

void MessageChannel::disconnectPublishers() {
	for( int k = 0; k < pubs.count(); k++ ) {
		MessagePublisher *pub = pubs.getClassByIndex( k );
		pub -> disconnected();
	}
	pubs.clear();
}

