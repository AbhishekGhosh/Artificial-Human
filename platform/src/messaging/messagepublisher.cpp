#include <ah_platform.h>
#include <ah_messaging_impl.h>

// This class is responsible to publish the message for processing

/*#########################################################################*/
/*#########################################################################*/

/*#########################################################################*/

/*
 * Constructor to initialize the instance with the required parameters.
 * @param p_session is the pointer to session.
 * @param p_channel is the channel instance
 * @param p_name is the publisher name
 * @param p_msgtype is the message type
 */
MessagePublisher::MessagePublisher( MessageSession *p_session , MessageChannel *p_channel , String p_name , String p_msgtype ) {
	session = p_session;
	channel = p_channel; 
	name = p_name; 
	msgtype = p_msgtype; 
}

/*
 * Default destructor of the class.
 */
MessagePublisher::~MessagePublisher() {
}

/*
 * Returns the channel.
 */
MessageChannel *MessagePublisher::getChannel() {
	return( channel );
}

/*
 * Return the message type.
 */
const String& MessagePublisher::getMsgType() {
	return( msgtype );
}

/*
 * Publish the message to the channel.
 * @param p_session is the session.
 * @param msg is the message
 */
String MessagePublisher::publish( MessageSession *p_session , const char *msg ) {
	ASSERT( channel != NULL );
	return( channel -> publish( p_session , this , msg ) ); 
}

/*
 * Publish the message to the channel.
 * @param p_session is the session.
 * @param msg is the message
 */

String MessagePublisher::publish( MessageSession *p_session , Message *msg ) {
	ASSERT( channel != NULL );
	return( channel -> publish( p_session , this , msg ) ); 
}

/*
 * Disconnects the channel.
 */
void MessagePublisher::disconnected() {
	channel = NULL;
}

TextMessage *MessagePublisher::createTextMessage() {
	TextMessage *l_msg = new TextMessage();
	l_msg -> setMessageType( msgtype );
	return( l_msg );
}

TextMessage *MessagePublisher::createTextMessage( const char *msg ) {
	TextMessage *l_msg = new TextMessage();
	l_msg -> setMessageType( msgtype );
	l_msg -> setText( msg );
	return( l_msg );
}

String MessagePublisher::getChannelName() {
	return( channel -> getName() );
}

