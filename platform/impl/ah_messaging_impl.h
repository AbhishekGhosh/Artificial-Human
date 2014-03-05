#ifndef	INCLUDE_AH_MESSAGING_IMPL_H
#define INCLUDE_AH_MESSAGING_IMPL_H

/*#########################################################################*/
/*#########################################################################*/

#include <ah_platform.h>

class MessageChannel;
class MessageQueue;

// #############################################################################
// #############################################################################

// input/output messages
class MessageChannel : public Object {
public:
	String getName();
	void open();
	void start();
	void stop();
	void close();
	String publish( MessageSession *session , MessagePublisher *pub , const char *msg );
	String publish( MessageSession *session , MessagePublisher *pub , Message *msg );
	String publish( MessageSession *session , Message *msg );

	bool isOpened() { return( opened ); };

	// subscribers and publishers
	void addSubscription( String key , MessageSubscription *sub );
	void deleteSubscription( String key );
	void addPublisher( String key , MessagePublisher *pub );
	void deletePublisher( String key );

	// read messages and call subscribers
	void threadChannelFunction( void *p_arg );
	void processMessages();

public:
	MessageChannel( String msgid , String name , bool sync , bool ignore );
	~MessageChannel();

	virtual const char *getClass() { return( "MessageChannel" ); };

// internals
private:
	String getNewMessageId();
	void subscribeEvent( Message *p_msg );
	void lock();
	void unlock();
	void disconnectSubscriptions();
	void disconnectPublishers();

private:
	String name;
	bool opened;
	bool sync;
	bool ignore;
	bool running;
	bool stopping;
	int queueMessageId;

	rfc_lock *channelLock;
	RFC_HND channelThread;
	MessageQueue *messages;
	MapStringToClass<MessageSubscription> subs;
	MapStringToClass<MessagePublisher> pubs;
};

// #############################################################################
// #############################################################################

// sync queue
class MessageQueue : public Object {
public:
	MessageQueue( String p_queueId );
	~MessageQueue();

	virtual const char *getClass() { return( "MessageQueue" ); };

public:
	void addMessage( Message *p_str );
	Message *getNextMessage();
	Message *getNextMessageNoLock();
	void wakeup();
	void makeEmptyAndWakeup();
	bool isEmpty();
	const char *getId() { return( queueId ); };

private:
	void clearMessages();

private:
	String queueId;
	rfc_lock *queueLock;
	RFC_HND queueWakeupEvent;
	rfc_list *queueMessages;
};

/*#########################################################################*/
/*#########################################################################*/

#endif	// INCLUDE_AH_MESSAGING_IMPL_H
