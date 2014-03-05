#include <ah_platform.h>
#include <ah_messaging_impl.h>

/*#########################################################################
 * This class is the implementation of the queue data structure so that all
 * the messages from various sources can be added and processed later.
/*#########################################################################*/

/*
 * Constructer to create the instance of queue so that we can 
 * create a queue to append messages to.
 * @param queue ID
 */
MessageQueue::MessageQueue( String p_queueId ) {
	queueId = p_queueId;
	queueLock = rfc_lock_create();
	queueWakeupEvent = rfc_hnd_evcreate();

	/* Create a list to add messages of type pointer */
	queueMessages = rfc_lst_create( RFC_EXT_TYPEPTR );

	Object::setInstance( queueId );
}

/*
 * Destructor that clears all the messages from queue.
 */
MessageQueue::~MessageQueue() {
	// delete all pending messages
	rfc_lock_exclusive( queueLock );
	clearMessages();
	rfc_lst_destroy( queueMessages );
	rfc_hnd_evdestroy( queueWakeupEvent );
	rfc_lock_destroy( queueLock );
}

/*
 * This function adds the message passed, to the queue.
 * @param Message Pointer
 */
void MessageQueue::addMessage( Message *p_str ) {
	rfc_lock_exclusive( queueLock );

	// log
	logger.logInfo( String( "addMessage: id=" ) + p_str -> getChannelMessageId() );

	// add to list
	RFC_TYPE val;
	val.u_p = p_str;
	rfc_lst_add( queueMessages , &val );

	// signal for waiter and release lock
	rfc_hnd_evsignal( queueWakeupEvent );
	logger.logDebug( "addMessage: signal on" );
	rfc_lock_release( queueLock );
}	

/*
 * Gets the message from top of the queue.
 */
Message *MessageQueue::getNextMessage() {
	rfc_lock_exclusive( queueLock );
	
	// check if no message
	if( rfc_lst_count( queueMessages ) == 0 ) {
		// wait for signal
		logger.logDebug( "getNextMessage: found zero records " );
		rfc_lock_release( queueLock );
		logger.logDebug( "getNextMessage: start wait event..." );
		rfc_hnd_waitevent( queueWakeupEvent , -1 );
		logger.logDebug( "getNextMessage: event found" );

		// lock again and reset event
		rfc_lock_exclusive( queueLock );

		// check if no messages
		if( rfc_lst_count( queueMessages ) == 0 ) {
			// return empty message
			logger.logInfo( "getNextMessage: wakeup without messages" );
			rfc_lock_release( queueLock );
			return( NULL );
		}
	}

	// get first item
	RFC_TYPE *val = rfc_lst_get( queueMessages , 0 );
	/* Get u_p type as the list that we created was of type pointer */
	Message *x = ( Message * )val -> u_p;

	// remove from queue
	rfc_lst_remove( queueMessages , 0 );
	logger.logInfo( String( "getNextMessage: extracted message from queue: " ) + x -> getChannelMessageId() );

	// signal off if no records remained
	if( rfc_lst_count( queueMessages ) == 0 ) {
		logger.logDebug( "getNextMessage: set signal off" );
		rfc_hnd_evreset( queueWakeupEvent );
	}

	// unlock and return
	rfc_lock_release( queueLock );
	return( x );
}

Message *MessageQueue::getNextMessageNoLock() {
	rfc_lock_shared( queueLock );
	
	// check if no message
	if( rfc_lst_count( queueMessages ) == 0 ) {
		rfc_lock_release( queueLock );
		return( NULL );
	}

	// make exclusive lock
	rfc_lock_escalate( queueLock );

	// get first item
	RFC_TYPE *val = rfc_lst_get( queueMessages , 0 );
	Message *x = ( Message * )val -> u_p;

	// remove from queue
	rfc_lst_remove( queueMessages , 0 );

	// unlock and return
	rfc_lock_release( queueLock );
	return( x );
}

/*
 * This API will send the wake up event
 */
void MessageQueue::wakeup() {
	rfc_lock_exclusive( queueLock );
	rfc_hnd_evsignal( queueWakeupEvent );
	rfc_lock_release( queueLock );
}

/*
 * This API will clear all the messages and will send the wake up event
 */
void MessageQueue::makeEmptyAndWakeup() {
	rfc_lock_exclusive( queueLock );
	// remove all messages
	clearMessages();
	rfc_lock_release( queueLock );

	// signal for waiter and release lock
	wakeup();
}

/*
 * Check to ses if the list has some messages or not.
 */
bool MessageQueue::isEmpty() {
	rfc_lock_shared( queueLock );
	// signal for waiter and release lock
	bool retval = ( rfc_lst_count( queueMessages ) > 0 )? true : false;
	rfc_lock_release( queueLock );

	return( retval );
}

/*
 * Delete all the messages from queue.
 */
void MessageQueue::clearMessages() {
	for( int k = rfc_lst_count( queueMessages ) - 1; k >= 0; k-- ) {
		Message *x = ( Message * )rfc_lst_get( queueMessages , k );
		delete x;
	}
	rfc_lst_clear( queueMessages );
}
