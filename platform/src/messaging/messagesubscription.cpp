#include <ah_platform.h>
#include <ah_messaging_impl.h>

/*#########################################################################
 * This class is responsible to process the message depending upon the
 * type of message.
/*#########################################################################*/

/*#########################################################################*/
/*#########################################################################*/

/*
 * Constructor to initialize the instance with the required parameters.
 * @param p_session is the pointer to session.
 * @param p_channel is the channel instance
 * @param p_name is the subscription name
 * @param p_sub is the subscriber
 */
MessageSubscription::MessageSubscription( MessageSession *p_session , MessageChannel *p_channel , String p_name , MessageSubscriber *p_sub ) {
	session = p_session;
	channel = p_channel;
	name = p_name;
	sub = p_sub;
}

MessageSubscription::~MessageSubscription() {
}

/*
 * Returns the channel.
 */
MessageChannel *MessageSubscription::getChannel() {
	return( channel );
}

/*
 * Disconnects the channel.
 */
void MessageSubscription::disconnected() {
    /* TODO; need to see if the memory is free after channel pointer is
        no longer required */
	channel = NULL;
}

/*
 * Process the message depending upon the type.
 * @param msg is the message to be processed.
 */
void MessageSubscription::processMessage( Message *msg ) {
	// verify selector
	if( selectorConditions.count() > 0 )
		if( !isMatchSelector( msg ) )
			return;

	Logger logger = channel -> getLogger();
	String channelName = channel -> getName();
	String subName = name;

	try {
		switch( msg -> getBaseType() ) {
			case Message::MsgType_Text :
				sub -> onTextMessage( ( TextMessage * )msg );
				break;
			case Message::MsgType_Xml :
				sub -> onXmlMessage( ( XmlMessage * )msg );
				break;
			case Message::MsgType_XmlCall :
				sub -> onXmlCall( ( XmlCall * )msg );
				break;
			case Message::MsgType_Binary :
				sub -> onBinaryMessage( ( BinaryMessage * )msg );
				break;
			default:
				sub -> onMessage( msg );
				break;
		}
	}
	catch ( RuntimeException& e ) {
		logger.logError( "processMessage: i/o channel id=" + channelName + ", subscription=" + subName + ": exception when processing message (see below)" );
		logger.printStack( e );
	}
	catch ( ... ) {
		logger.logError( "processMessage: i/o channel id=" + channelName + ", subscription=" + subName + ": unknown exception (see below)" );
		logger.printStack();
	}
}

void MessageSubscription::setSelector( String p_selector ) {
	selectorConditions.destroy();

	// format is "key1=value, key2=value, ...", can include system properties - msgtype and endpoint
	StringList parts;
	p_selector.split( parts , "," );

	// set to selector
	for( int k = 0; k < parts.count(); k++ ) {
		StringList items;
		parts.get( k ).split( items , "=" );
		ASSERTMSG( items.count() == 2 , "invalid selector=" + p_selector );

		String key = items.get( 0 );
		String value = items.get( 1 );
		key.trim();
		value.trim();

		selectorConditions.add( key , value );
	}
}

bool MessageSubscription::isMatchSelector( Message *msg ) {
	// check system and message properties
	for( int k = 0; k < selectorConditions.count(); k++ ) {
		const char *key = selectorConditions.getKeyByIndex( k );

		const char *valueSelector = selectorConditions.get( key );
		String valueMsg;
		if( !strcmp( "msgtype" , key ) )
			valueMsg = msg -> getMessageType();
		else
		if( !strcmp( "endcpoint" , key ) )
			valueMsg = msg -> getEndPoint();
		else
			valueMsg = msg -> getProperty( key );

		if( !valueMsg.equals( valueSelector ) )
			return( false );
	}

	return( true );
}

String MessageSubscription::getChannelName() {
	return( channel -> getName() );
}

