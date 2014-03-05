#include <ah_platform.h>
#include <ah_objects_impl.h>

#include <ctype.h>

// #############################################################################
// #############################################################################

ObjectField::ObjectField()
{
	init();
}

void ObjectField::init()
{
	id = 0;
	type = 0;
	soField = NULL;
	isOwnData = false;
	value.value = NULL;
	sizeOfList = 0;
	soFieldSrc = NULL;
}

ObjectField::ObjectField( int p_id , const char *p_name , char p_type , bool isList )
{
	init();

	ASSERT( type != TP_OBJECT );

	id = p_id;
	name = p_name;
	type = p_type;

	if( isList ) {
		switch( p_type ) {
			case TP_CHAR :		throw RuntimeError( "ObjectField::ObjectField: wrong type" );
			case TP_INT :		type = TP_INTLIST; break;
			case TP_FLOAT :		type = TP_FLOATLIST; break;
			case TP_STRING :	type = TP_STRINGLIST; break;
		}
	}
}

ObjectField::ObjectField( int p_id , const char *p_name , SerializeObject *p_so , bool isList )
{
	init();

	id = p_id;
	name = p_name;
	type = ( isList )? TP_OBJECTLIST : TP_OBJECT;
	soField = p_so;

	// create object container (always owned by field)
	if( type == TP_OBJECTLIST )
		value.objectListValue = new FlatList<Object *>;
}

ObjectField::~ObjectField()
{
	// free own data
	clearData();

	if( type == TP_OBJECTLIST )
		if( value.objectListValue != NULL )
			delete value.objectListValue;
}

void ObjectField::clearData( bool pFree )
{
	if( isOwnData ) {
		// free
		isOwnData = false;
		if( pFree ) {
			switch( type ) {
				case TP_STRING :
				case TP_STRINGLIST :
				case TP_INTLIST :
				case TP_FLOATLIST :
					if( value.value != NULL )
						free( ( void * )value.value );
					break;
				case TP_OBJECT :
					if( value.value != NULL )
						delete value.objectValue;
					break;
				case TP_OBJECTLIST :
					if( value.value != NULL )
						value.objectListValue -> destroy();
					break;
			}
		}
	}

	// object container always owned by field
	if( type != TP_OBJECTLIST )
		value.value = NULL;

	// set zero data
	sizeOfList = 0;
}

SerializeObject *ObjectField::getFieldSerializeObject()
{
	if( type != TP_OBJECT && type != TP_OBJECTLIST )
		throw RuntimeError( "ObjectField::getFieldSerializeObject: field is not object" );

	return( soField );
}

bool ObjectField::isChar()
{
	return( type == TP_CHAR );
}

bool ObjectField::isInt()
{
	return( type == TP_INT || type == TP_INTLIST );
}

bool ObjectField::isFloat()
{
	return( type == TP_FLOAT || type == TP_FLOATLIST );
}

bool ObjectField::isString()
{
	return( type == TP_STRING || type == TP_STRINGLIST );
}

bool ObjectField::isObject()
{
	return( type == TP_OBJECT || type == TP_OBJECTLIST );
}

bool ObjectField::isList()
{
	return( type == TP_INTLIST || type == TP_FLOATLIST || 
		type == TP_STRINGLIST || type == TP_OBJECTLIST );
}

char ObjectField::getChar()
{
	if( type != TP_CHAR )
		throw RuntimeError( "ObjectField::getChar: field is not char" );

	return( value.charValue );
}

int	ObjectField::getInt()
{
	if( type != TP_INT )
		throw RuntimeError( "ObjectField::getInt: field is not int" );

	return( value.intValue );
}

float ObjectField::getFloat()
{
	if( type != TP_FLOAT )
		throw RuntimeError( "ObjectField::getFloat: field is not float" );

	return( value.floatValue );
}

const char *ObjectField::getString( bool clearOwn )
{
	if( type != TP_STRING )
		throw RuntimeError( "ObjectField::getString: field is not String" );

	if( clearOwn )
		isOwnData = false;
	return( value.stringValue );
}

Object *ObjectField::getObject( bool clearOwn )
{
	if( type != TP_OBJECT )
		throw RuntimeError( "ObjectField::getObject: field is not object" );

	if( clearOwn )
		isOwnData = false;
	return( value.objectValue );
}

int	*ObjectField::getIntList( int *p_size , bool clearOwn )
{
	if( type != TP_INTLIST )
		throw RuntimeError( "ObjectField::getIntList: field is not int list" );

	if( clearOwn )
		isOwnData = false;
	*p_size = sizeOfList;
	return( value.intListValue );
}

float *ObjectField::getFloatList( int *p_size , bool clearOwn )
{
	if( type != TP_FLOATLIST )
		throw RuntimeError( "ObjectField::getFloatList: field is not float list" );

	if( clearOwn )
		isOwnData = false;
	*p_size = sizeOfList;
	return( value.floatListValue );
}

const char **ObjectField::getStringList( int *p_size , bool clearOwn )
{
	if( type != TP_STRINGLIST )
		throw RuntimeError( "ObjectField::getStringList: field is not String list" );

	if( clearOwn )
		isOwnData = false;
	*p_size = sizeOfList;
	return( value.stringListValue );
}

FlatList<Object *> *ObjectField::getObjectList()
{
	if( type != TP_OBJECTLIST )
		throw RuntimeError( "ObjectField::getObjectList: field is not object list" );

	ASSERT( value.objectListValue != NULL );
	return( value.objectListValue );
}

void ObjectField::setChar( char v )
{
	if( type != TP_CHAR )
		throw RuntimeError( "ObjectField::setChar: field is not char" );

	value.charValue = v;
}

void ObjectField::setInt( int v )
{
	if( type != TP_INT )
		throw RuntimeError( "ObjectField::setInt: field is not int" );

	value.intValue = v;
}

void ObjectField::setFloat( float v )
{
	if( type != TP_FLOAT )
		throw RuntimeError( "ObjectField::setFloat: field is not float" );

	value.floatValue = v;
}

void ObjectField::setString( const char *v , bool pOwn )
{
	if( type != TP_STRING )
		throw RuntimeError( "ObjectField::setString: field is not String" );

	clearData();

	isOwnData = pOwn;
	value.stringValue = v;
}

void ObjectField::setObject( Object *v , bool pOwn )
{
	if( type != TP_OBJECT )
		throw RuntimeError( "ObjectField::setObject: field is not object" );

	clearData();

	isOwnData = pOwn;
	value.objectValue = v;
}

void ObjectField::setIntList( int *v , int p_size , bool pOwn )
{
	if( type != TP_INTLIST )
		throw RuntimeError( "ObjectField::setIntList: field is not int list" );

	clearData();

	isOwnData = pOwn;
	value.intListValue = v;
	sizeOfList = p_size;
}

void ObjectField::setFloatList( float *v , int p_size , bool pOwn )
{
	if( type != TP_FLOATLIST )
		throw RuntimeError( "ObjectField::setFloatList: field is not float list" );

	clearData();

	isOwnData = pOwn;
	value.floatListValue = v;
	sizeOfList = p_size;
}

void ObjectField::setStringList( const char **v , int p_size , bool pOwn )
{
	if( type != TP_STRINGLIST )
		throw RuntimeError( "ObjectField::setStringList: field is not String list" );

	clearData();

	isOwnData = pOwn;
	value.stringListValue = v;
	sizeOfList = p_size;
}

void ObjectField::setObjectList( ClassList<Object> *v , bool pOwn )
{
	if( type != TP_OBJECTLIST )
		throw RuntimeError( "ObjectField::setObjectList: field is not object list" );

	clearData();

	isOwnData = pOwn;
	if( v == NULL )
		return;

	value.objectListValue -> create( v -> count() );
	Object **va = value.objectListValue -> getAll();
	for( int k = 0; k < v -> count(); k++ )
		*va++ = v -> get( k );
}

char ObjectField::getBaseType()
{
	switch( type ) {
		case TP_CHAR :
			return( TP_CHAR );
		case TP_INT :
		case TP_INTLIST :
			return( TP_INT );
		case TP_FLOAT :
		case TP_FLOATLIST :
			return( TP_FLOAT );
		case TP_STRING :
		case TP_STRINGLIST :
			return( TP_STRING );
		case TP_OBJECT :
		case TP_OBJECTLIST :
			return( TP_OBJECT );
	}

	throw RuntimeError( "ObjectField::getBaseType: unknown datatype" );
}

int ObjectField::getId()
{
	return( id );
}

const char *ObjectField::getName()
{
	return( name );
}

// Object interface
const char *ObjectField::getClass()
{
	return( "ObjectField" );
}

void ObjectField::createSerializeObject()
{
	SerializeObject *so = new SerializeObject( "ObjectField" );
	so -> setFactoryMethod( ObjectField::onCreate );

	so -> addFieldInt( "id" , 1 );
	so -> addFieldString( "name" , 2 );
	so -> addFieldChar( "type" , 3 );
	so -> addFieldObject( "object" , SerializeObject::getSerializeObject() , 4 );
}

SerializeObject *ObjectField::getSerializeObject()
{
	return( ObjectService::getService() -> getSerializeObject( "ObjectField" ) );
}

void ObjectField::serialize( SerializeObject& so )
{
	so.setPropInt( id , "id" );
	so.setPropString( name , "name" , false );
	so.setPropChar( type , "type" );

	if( soField != NULL )
		so.setPropObject( soField , "object" , false );
}

void ObjectField::deserialize( Object *parent , SerializeObject& so )
{
	ASSERT( type == 0 );

	id = so.getPropInt( "id" );
	name = so.getPropString( "name" , false );
	type = so.getPropChar( "type" );

	soFieldSrc = ( SerializeObject * )so.getPropObject( "object" , true );
	if( soFieldSrc != NULL )
		ObjectService::getService() -> replaceSerializeObjectInstanceSrc( soFieldSrc );

	// create object container (always owned by field)
	if( type == TP_OBJECTLIST )
		value.objectListValue = new FlatList<Object *>;
}

String ObjectField::getPK()
{
	String key = getFieldSerializeObject() -> getObjectClass();
	key += ".";
	key += name;
	return( key );
}

void ObjectField::attach( SerializeObject *p_so )
{
	soField = p_so;
}

