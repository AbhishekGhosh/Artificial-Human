#include <ah_platform.h>

XmlMessage::XmlMessage( const char *txt )
:	TextMessage( Message::MsgType_Xml ) {
	TextMessage::setText( txt );
}

XmlMessage::XmlMessage( Xml p_xml )
:	TextMessage( Message::MsgType_Xml ) {
	xml = p_xml;
}

XmlMessage::~XmlMessage() {
	if( xml.exists() )
		xml.destroy();
}

Xml XmlMessage::getXml() {
	ASSERT( xml.exists() );
	return( xml );
}

void XmlMessage::setXmlFromMessage( const char *contentType ) {
	if( !xml.exists() )
		xml = Xml::read( TextMessage::getText() , contentType );

	ASSERT( xml.getName().equals( contentType ) );
	Message::setMessageType( contentType );
}

void XmlMessage::setMessageFromXml() {
	TextMessage::setText( xml.serialize() );
}
