#include <ah_platform.h>
#include <ah_objecttypes.h>

// #############################################################################
// #############################################################################

Service *ObjectService::newService() {
	return( new ObjectService() );
}

ObjectService::ObjectService() {
	mapObjectTypeIdToSerializeObject = NULL;
}

void ObjectService::createService() {
	mapObjectTypeIdToSerializeObject = rfc_map_strcreate();

	// register serializable classes
	SerializeObject::createSerializeObject();
	Scale::createSerializeObject();
}

void ObjectService::destroyService() {
	// free serialize objects
	if( mapObjectTypeIdToSerializeObject != NULL ) {
		destroySerializeObjectInstances();
		rfc_map_strdrop( mapObjectTypeIdToSerializeObject );
	}
}

// serialized objects
void ObjectService::registerSerializeObject( SerializeObject *so )
{
	addSerializeObjectInstance( so , so -> getObjectClass() );
}

SerializeObject *ObjectService::getSerializeObject( const char *className )
{
	// try to find existent
	SerializeObject *so = getSerializeObjectInstance( className );
	if( so == NULL )
		throw RuntimeError( String( "AIEngineImpl::getSerializeObject: object " ) + 
			className + " not found" );

	return( so );
}

void ObjectService::addSerializeObjectInstance( SerializeObject *so , const char *p_inst )
{
	// old cannot exist
	SerializeObject *soOld = getSerializeObjectInstance( p_inst );
	if( soOld != NULL )
		throw RuntimeError( String( "AIDBImpl::addSerializeObjectInstance: old object exists: " ) + p_inst );

	rfc_map_stradd( mapObjectTypeIdToSerializeObject , p_inst , so );
	so -> ref();
}

void ObjectService::replaceSerializeObjectInstance( SerializeObject *so , const char *p_inst )
{
	SerializeObject *soOld = getSerializeObjectInstance( p_inst );

	// check the same
	if( soOld == so )
		return;

	// replace
	rfc_map_stradd( mapObjectTypeIdToSerializeObject , p_inst , so );
	so -> ref();

	// delete old if any
	if( soOld == NULL )
		return;

	// deref old
	soOld -> deref();
}

void ObjectService::replaceSerializeObjectInstanceSrc( SerializeObject *so )
{
	String className = so -> getObjectClass();
	className += "#src";
	replaceSerializeObjectInstance( so , className );
}

SerializeObject *ObjectService::getSerializeObjectInstance( const char *p_inst )
{
	// try to find existent
	SerializeObject *so;
	if( rfc_map_strcheck( mapObjectTypeIdToSerializeObject , p_inst , ( void ** )&so ) >= 0 )
		return( so );

	return( NULL );
}

void ObjectService::destroySerializeObjectInstances()
{
	for( int k = 0; k < rfc_map_strcount( mapObjectTypeIdToSerializeObject ); k++ ) {
		SerializeObject *so = ( SerializeObject * )rfc_map_strget( mapObjectTypeIdToSerializeObject , k );
		so -> deref();
	}
	rfc_map_strclear( mapObjectTypeIdToSerializeObject );
}

