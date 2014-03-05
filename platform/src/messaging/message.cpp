#include <ah_platform.h>

/*#########################################################################*/
/*#########################################################################*/

Message::Message( Message::MsgType p_baseType )
:	baseType( p_baseType ) {
	session = NULL;
}

Message::~Message() {
}

XmlCall& Message::toXmlCall() {
	return( *( XmlCall * )this );
}

String Message::getProperty( String p_name ) {
	return( properties.get( p_name ) );
}

void Message::setProperty( String p_name , String p_value ) {
	properties.set( p_name , p_value );
}
