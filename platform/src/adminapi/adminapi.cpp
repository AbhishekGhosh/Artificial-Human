#include <ah_platform.h>
#include <ah_adminapi.h>
#include <ah_adminapi_impl.h>

/*#########################################################################*/
/*#########################################################################*/

AdminApi::AdminApi() {
	socket = new AdminApiSocket;
	initapi();
}

AdminApi::~AdminApi() {
	delete socket;
	exitapi();
}

void AdminApi::connect( String url ) {
	socket -> connect( url );
}

Xml AdminApi::execute( Xml request ) {
	Xml response = socket -> execute( request );
	return( response );
}

void AdminApi::send( Xml request ) {
	socket -> send( request );
}

void AdminApi::disconnect() {
	socket -> disconnect();
}

bool AdminApi::isConnected() {
	return( socket -> isConnected() );
}

Xml AdminApi::createXmlRequest( const char *contenttype ) {
	Xml request = Xml::create( contenttype );
	return( request );
}

Xml AdminApi::createXmlRequest( const char *data , const char *contenttype ) {
	Xml request = Xml::read( data , contenttype );
	return( request );
}

Xml AdminApi::loadXmlRequest( String fileName , const char *contenttype ) {
	Xml request = Xml::load( fileName );
	ASSERTMSG( request.exists() , "Unable to load file path=" + fileName );
	ASSERTMSG( request.getName().equals( contenttype ) , "Loaded file is not request" );
	return( request );
}

void AdminApi::setFunctionName( Xml request , String name ) {
	ASSERTMSG( request.exists() , "Request needs to be created" );
	request.setAttribute( "name" , name );
}

void AdminApi::setParam( Xml request , String name , String value ) {
	ASSERTMSG( request.exists() , "Request needs to be created" );
	Xml p = request.getFirstChild( "parameters" );
	if( !p.exists() )
		p = request.addElement( "parameters" );

	p.setProperty( name , value );			
}

void AdminApi::setBooleanParam( Xml request , String name , bool value ) {
	ASSERTMSG( request.exists() , "Request needs to be created" );
	Xml p = request.getFirstChild( "parameters" );
	if( !p.exists() )
		p = request.addElement( "parameters" );

	p.setBooleanProperty( name , value );			
}

void AdminApi::setIntParam( Xml request , String name , int value ) {
	ASSERTMSG( request.exists() , "Request needs to be created" );
	Xml p = request.getFirstChild( "parameters" );
	if( !p.exists() )
		p = request.addElement( "parameters" );

	p.setIntProperty( name , value );			
}

void AdminApi::setFloatParam( Xml request , String name , float value ) {
	ASSERTMSG( request.exists() , "Request needs to be created" );
	Xml p = request.getFirstChild( "parameters" );
	if( !p.exists() )
		p = request.addElement( "parameters" );

	p.setFloatProperty( name , value );			
}

void AdminApi::initapi() {
	socket -> initThread();
}

void AdminApi::exitapi() {
	socket -> exitThread();
}
