#include <ah_platform.h>

// #############################################################################
// #############################################################################

const char *Object::getClass() { 
	throw RuntimeError( "Object::getClass: virtual function undefined for class" );
}

void Object::serialize( SerializeObject& so ) {
	throw RuntimeError( "Object::serialize: virtual function undefined for class" );
}

void Object::deserialize( Object *parent , SerializeObject& so ) {
	throw RuntimeError( "Object::deserialize: virtual function undefined for class" );
}

String Object::getPK() { 
	throw RuntimeError( "Object::getPK: virtual function undefined for class" );
}

const char *Object::getInstance() {
	return( instance );
}

void Object::setLoggerName( String name ) {
	loggerName = name;
	logger.setLoggerName( loggerName );
}

const char *Object::getLoggerName() {
	if( loggerName.isEmpty() )
		return( getClass() );
	return( loggerName );
}

void Object::setInstance( const char *p_instance ) {
	instance = p_instance;
	attachLogger();
}

void Object::attachLogger() {
	logger.attachObjectInstance( getClass() , instance , loggerName );
}

Object *Object::createObject( const char *className ) {
	SerializeObject *so = ObjectService::getService() -> getSerializeObject( className );
	return( so -> createObject() );
}

SerializeObject *Object::getSerializeObject() {
	return( ObjectService::getService() -> getSerializeObject( getClass() ) );
}

void Object::serialize( Object *o , SerializeObject& so ) {
	so.clearData();
	so.setEffectiveObjectClass( o -> getClass() );

	o -> serialize( so );
}

void Object::deserialize( Object *parent , Object *o , SerializeObject& so ) {
	if( strcmp( so.getEffectiveObjectClass() , o -> getClass() ) )
		throw RuntimeError( "Object::deserialize: different object and so classes" );

	o -> deserialize( parent , so );
}

void Object::log( const char *prompt , Logger::LogLevel p_logLevel ) {
	if( logger.disabled( p_logLevel ) )
		return;

	String p = prompt;
	
	SerializeObject *so = getSerializeObject();
	Object::serialize( this , *so );

	String data = so -> getDataStringNameToValue();
	p += ": " + data;

	logger.log( p , p_logLevel );
}
