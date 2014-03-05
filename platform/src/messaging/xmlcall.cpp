#include <ah_platform.h>
#include <ah_messaging_impl.h>

/*#########################################################################*/
/*#########################################################################*/

XmlCall::XmlCall( MessageChannel *p_channelIn , MessageChannel *p_channelOut , const char *txt )
:	XmlMessage( Message::MsgType_XmlCall ) {
	TextMessage::setText( txt );
	responseSent = false;
	channelIn = p_channelIn;
	channelOut = p_channelOut;
}

XmlCall::~XmlCall() {
	if( xmlResponse.exists() && responseSent == false )
		xmlResponse.destroy();
}

void XmlCall::postExecute() {
	if( responseSent == false )
		sendUnknownResponse();
}

void XmlCall::setXmlFromMessage() {
	XmlMessage::setXmlFromMessage( "xmlcall" );
	
	Xml xml = XmlMessage::getXml();
	requestId = xml.getAttribute( "requestId" , "" );
	name = xml.getAttribute( "name" , "" );

	className = xml.getAttribute( "class" );
	functionName = xml.getAttribute( "method" );
	params = xml.getChildNode( "parameters" );
}

// request
String XmlCall::getClassName() {
	return( className );
}

String XmlCall::getFunctionName() {
	return( functionName );
}

String XmlCall::getParam( String paramName ) {
	return( params.getProperty( paramName ) );
}

String XmlCall::getParam( String paramName , String defaultValue ) {
	if( !params.exists() )
		return( defaultValue );
	return( params.getProperty( paramName , defaultValue ) );
}

int XmlCall::getIntParam( String paramName ) {
	return( params.getIntProperty( paramName ) );
}

int XmlCall::getIntParam( String paramName , int defaultValue ) {
	if( !params.exists() )
		return( defaultValue );
	return( params.getIntProperty( paramName , defaultValue ) );
}

bool XmlCall::getBooleanParam( String paramName ) {
	return( params.getBooleanProperty( paramName ) );
}

bool XmlCall::getBooleanParam( String paramName , bool defaultValue ) {
	if( !params.exists() )
		return( defaultValue );
	return( params.getBooleanProperty( paramName , defaultValue ) );
}

float XmlCall::getFloatParam( String paramName ) {
	return( params.getFloatProperty( paramName ) );
}

float XmlCall::getFloatParam( String paramName , float defaultValue ) {
	if( !params.exists() )
		return( defaultValue );
	return( params.getFloatProperty( paramName , defaultValue ) );
}

// response 
Xml XmlCall::createResponse() {
	ASSERT( !xmlResponse.exists() );
	
	xmlResponse = Xml::create( "xmlcallresult" );
	if( !requestId.isEmpty() )
		xmlResponse.setAttribute( "requestId" , requestId );
	if( !name.isEmpty() )
		xmlResponse.setAttribute( "name" , name );

	return( xmlResponse );
}

Xml XmlCall::getResponse()
{
	return( xmlResponse );
}

String XmlCall::sendResponse( MessagePublisher *pub ) {
	ASSERT( xmlResponse.exists() );
	XmlMessage *msg = new XmlMessage( xmlResponse );
	responseSent = true;
	return( pub -> publish( getSession() , msg ) );
}

String XmlCall::sendResponseException( MessagePublisher *pub , RuntimeException& e ) {
	String error = e.printStackToString();
	Xml res = createResponse();
	res.setAttribute( "status" , "Exception" );
	res.addTextElement( "exception" , error );
	String msg = sendResponse( pub );

	return( msg );
}

String XmlCall::sendResponseUnknownException( MessagePublisher *pub ) {
	Xml res = createResponse();
	res.setAttribute( "status" , "Exception" );
	res.addTextElement( "exception" , "Unknown Error" );
	String msg = sendResponse( pub );

	return( msg );
}

String XmlCall::sendUnknownResponse() {
	// send default response
	createStatusResponse( "UNKNOWN" );

	XmlMessage *msg = new XmlMessage( xmlResponse );
	String msgId = channelOut -> publish( getSession() , msg );
	responseSent = true;

	return( msgId );
}

Xml XmlCall::createStatusResponse( String status ) {
	if( !xmlResponse.exists() )
		xmlResponse = createResponse();

	xmlResponse.setAttribute( "status" , status );
	return( xmlResponse );
}

String XmlCall::sendStatusResponse( String status ) {
	// send default response
	createStatusResponse( status );

	XmlMessage *msg = new XmlMessage( xmlResponse );
	String msgId = channelOut -> publish( getSession() , msg );
	responseSent = true;

	return( msgId );
}

