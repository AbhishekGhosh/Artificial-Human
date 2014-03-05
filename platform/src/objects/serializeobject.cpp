#include <ah_platform.h>
#include <ah_objects_impl.h>

#include <ctype.h>

#define TB_OPEN '{'
#define TB_CLOSE '}'
#define TB_LISTDELIMITER ','
#define TB_LISTDELIMITER_AND_SPACE ", "

/*#########################################################################*/
/*#########################################################################*/

SerializeObject::SerializeObject()
{
	init();
}

void SerializeObject::init()
{
	mapIdToField = rfc_map_ptrcreate();
	mapNameToField = rfc_map_strcreate();

	pfnCreateObject = NULL;
	refCount = 0;
}

SerializeObject::SerializeObject( Object *o )
{
	init();
	objectClass = o -> getClass();
	effectiveObjectClass = objectClass;
}

SerializeObject::SerializeObject( const char *className )
{
	init();
	objectClass = className;
	effectiveObjectClass = objectClass;

	ObjectService::getService() -> registerSerializeObject( this );
}

SerializeObject::~SerializeObject()
{
	rfc_map_strdrop( mapNameToField );
	rfc_map_ptrdrop( mapIdToField );

	// drop fields
	fieldList.destroy();
}

void SerializeObject::ref()
{
	refCount++;
}

void SerializeObject::deref()
{
	if( --refCount )
		return;

	delete this;
}

ObjectField *SerializeObject::addFieldBool( const char *name , int id )
{
	return( addFieldChar( name , id ) );
}

ObjectField *SerializeObject::addFieldChar( const char *name , int id )
{
	return( addField( id , name , TP_CHAR ) );
}

ObjectField *SerializeObject::addFieldInt( const char *name , int id )
{
	return( addField( id , name , TP_INT ) );
}

ObjectField *SerializeObject::addFieldIntList( const char *name , int id )
{
	return( addFieldList( id , name , TP_INT ) );
}

ObjectField *SerializeObject::addFieldFloat( const char *name , int id )
{
	return( addField( id , name , TP_FLOAT ) );
}

ObjectField *SerializeObject::addFieldFloatList( const char *name , int id )
{
	return( addFieldList( id , name , TP_FLOAT ) );
}

ObjectField *SerializeObject::addFieldString( const char *name , int id )
{
	return( addField( id , name , TP_STRING ) );
}

ObjectField *SerializeObject::addFieldStringList( const char *name , int id )
{
	return( addFieldList( id , name , TP_STRING ) );
}

ObjectField *SerializeObject::addField( int id , const char *name , char type )
{
	if( id <= 0 )
		id = fieldList.count() + 1;

	ObjectField *field = new ObjectField( id , name , type , false );
	return( addField( field , id , name ) );
}

ObjectField *SerializeObject::addFieldList( int id , const char *name , char type )
{
	if( id <= 0 )
		id = fieldList.count() + 1;

	ObjectField *field = new ObjectField( id , name , type , true );
	return( addField( field , id , name ) );
}

ObjectField *SerializeObject::addFieldObject( const char *name , SerializeObject *so , int id )
{
	if( id <= 0 )
		id = fieldList.count() + 1;

	ObjectField *field = new ObjectField( id , name , so , false );
	return( addField( field , id , name ) );
}

ObjectField *SerializeObject::addFieldObjectList( const char *name , SerializeObject *so , int id )
{
	if( id <= 0 )
		id = fieldList.count() + 1;

	ObjectField *field = new ObjectField( id , name , so , true );
	return( addField( field , id , name ) );
}

ObjectField *SerializeObject::addField( ObjectField *field , int id , const char *name )
{
	if( getFieldById( id ) != NULL )
		throw RuntimeError( String( "SerializeObject::addField: duplicate ID: " ) + id );
	if( getFieldByName( name ) != NULL )
		throw RuntimeError( String( "SerializeObject::addField: duplicate NAME: " ) + name );

	rfc_map_stradd( mapNameToField , name , ( void * )field );
	rfc_map_ptradd( mapIdToField , ( void * )id , ( unsigned long )field );
	fieldList.add( field );

	return( field );
}

ObjectField *SerializeObject::getFieldById( int id )
{
	ObjectField *field;
	if( rfc_map_ptrcheck( mapIdToField , ( void * )id , ( unsigned long * )&field ) < 0 )
		return( NULL );

	return( field );
}

ObjectField *SerializeObject::getFieldById( int id , SerializeObject *so )
{
	// id is from so object - find via name
	ObjectField *fieldSrc = so -> getFieldById( id );
	if( fieldSrc == NULL )
		throw RuntimeError( String( "SerializeObject::getFieldById: unknown ID: " ) + id );

	// find by name
	return( getFieldByName( fieldSrc -> getName() ) );
}

ObjectField *SerializeObject::getFieldByName( const char *name )
{
	ObjectField *field;
	if( rfc_map_strcheck( mapNameToField , name , ( void ** )&field ) < 0 )
		return( NULL );

	return( field );
}

bool SerializeObject::getPropBool( const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropBool: unknown field: " ) + p_field );

	return( ( field -> getChar() ) == 'Y' );
}

char SerializeObject::getPropChar( const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropChar: unknown field: " ) + p_field );

	return( field -> getChar() );
}

int SerializeObject::getPropInt( const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropInt: unknown field: " ) + p_field );

	return( field -> getInt() );
}

int *SerializeObject::getPropIntList( const char *p_field , int *n , bool clearOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropIntList: unknown field: " ) + p_field );

	return( field -> getIntList( n , clearOwn ) );
}

const char *SerializeObject::getPropString( const char *p_field , bool clearOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropString: unknown field: " ) + p_field );

	return( field -> getString( clearOwn ) );
}

const char **SerializeObject::getPropStringList( const char *p_field , int *n , bool clearOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropStringList: unknown field: " ) + p_field );

	return( field -> getStringList( n , clearOwn ) );
}

float SerializeObject::getPropFloat( const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropFloat: unknown field: " ) + p_field );

	return( field -> getFloat() );
}

float *SerializeObject::getPropFloatList( const char *p_field , int *n , bool clearOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropStringList: unknown field: " ) + p_field );

	return( field -> getFloatList( n , clearOwn ) );
}

void SerializeObject::getPropFloatList( FlatList<float>& p_val , const char *p_field , bool clearOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropStringList: unknown field: " ) + p_field );

	int n;
	float *v = field -> getFloatList( &n , clearOwn );

	// copy
	p_val.set( v , n );

	// free if clear
	if( clearOwn )
		if( v != NULL )
			free( v );
}

const FlatList<Object *> *SerializeObject::getPropObjectList( const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropObjectList: unknown field: " ) + p_field );

	return( field -> getObjectList() );
}

Object *SerializeObject::getPropObject( const char *p_field , bool clearOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropObject: unknown field: " ) + p_field );

	return( field -> getObject( clearOwn ) );
}

void SerializeObject::setPropBool( bool p_val , const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropBool: unknown field: " ) + p_field );

	field -> setChar( ( p_val )? 'Y' : 'N' );
}

void SerializeObject::setPropChar( char p_val , const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropInt: unknown field: " ) + p_field );

	field -> setChar( p_val );
}

void SerializeObject::setPropInt( int p_val , const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropInt: unknown field: " ) + p_field );

	field -> setInt( p_val );
}

void SerializeObject::setPropIntList( int *p_val , int n , const char *p_field , bool setOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropIntList: unknown field: " ) + p_field );

	field -> setIntList( p_val , n , setOwn );
}

void SerializeObject::setPropIntList( FlatList<int>& p_val , const char *p_field )
{
	setPropIntList( p_val.getAll() , p_val.count() , p_field , false );
}

void SerializeObject::setPropIntListFromStructList( void *p_val , int n , int sizeStruct , int offset , const char *p_field )
{
	if( n == 0 )
		return;

	int *valNew = ( int * )malloc( n * sizeof( int ) );

	int shift = offset;
	int *pvSet = valNew;
	for( int k = 0; k < n; k++ ) {
		int *pvGet = ( int * )( ( ( char * )p_val ) + shift );
		*pvSet++ = *pvGet;
		shift += sizeStruct;
	}

	setPropIntList( valNew , n , p_field , true );
}

void SerializeObject::setPropIntListFromStructPtrList( void **p_val , int n , int sizeStruct , int offset , const char *p_field )
{
	if( n == 0 )
		return;

	int *valNew = ( int * )malloc( n * sizeof( int ) );

	int *pvSet = valNew;
	for( int k = 0; k < n; k++ ) {
		int *pvGet = ( int * )( ( ( char * )p_val[ k ] ) + offset );
		*pvSet++ = *pvGet;
	}

	setPropIntList( valNew , n , p_field , true );
}

void SerializeObject::setPropString( const char *p_val , const char *p_field , bool setOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropString: unknown field: " ) + p_field );

	field -> setString( p_val , setOwn );
}

void SerializeObject::setPropStringList( const char **p_val , int n , const char *p_field , bool setOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropStringList: unknown field: " ) + p_field );

	field -> setStringList( p_val , n , setOwn );
}

void SerializeObject::setPropFloat( float p_val , const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropFloat: unknown field: " ) + p_field );

	field -> setFloat( p_val );
}

void SerializeObject::setPropFloatList( float *p_val , int n , const char *p_field , bool setOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropFloatList: unknown field: " ) + p_field );

	field -> setFloatList( p_val , n , setOwn );
}

void SerializeObject::setPropFloatList( FlatList<float>& p_val , const char *p_field )
{
	setPropFloatList( p_val.getAll() , p_val.count() , p_field , false );
}

void SerializeObject::setPropFloatListFromStructList( void *p_val , int n , int sizeStruct , int offset , const char *p_field )
{
	if( n == 0 )
		return;

	float *valNew = ( float * )malloc( n * sizeof( float ) );

	int shift = offset;
	float *pvSet = valNew;
	for( int k = 0; k < n; k++ ) {
		float *pvGet = ( float * )( ( ( char * )p_val ) + shift );
		*pvSet++ = *pvGet;
		shift += sizeStruct;
	}

	setPropFloatList( valNew , n , p_field , true );
}

void SerializeObject::setPropFloatListFromStructPtrList( void **p_val , int n , int sizeStruct , int offset , const char *p_field )
{
	if( n == 0 )
		return;

	float *valNew = ( float * )malloc( n * sizeof( float ) );

	float *pvSet = valNew;
	for( int k = 0; k < n; k++ ) {
		float *pvGet = ( float * )( ( ( char * )p_val[ k ] ) + offset );
		*pvSet++ = *pvGet;
	}

	setPropFloatList( valNew , n , p_field , true );
}

void SerializeObject::setPropObject( Object *p_val , const char *p_field , bool setOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropObject: unknown field: " ) + p_field );

	field -> setObject( p_val , setOwn );
}

void SerializeObject::setPropObject( Object& p_val , const char *p_field )
{
	setPropObject( &p_val , p_field , false );
}

void SerializeObject::setPropObjectList( ClassList<Object>& p_val , const char *p_field , bool setOwn )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropObjectList: unknown field: " ) + p_field );

	field -> setObjectList( &p_val , setOwn );
}

void SerializeObject::getPropObjectList( ClassList<Object>& p_val , const char *p_field , bool clearOwn )
{
	ASSERT( p_val.count() == 0 );

	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropObjectList: unknown field: " ) + p_field );

	FlatList<Object *> *data = field -> getObjectList();

	// copy
	int n = data -> count();
	p_val.allocate( n );
	Object **pv = data -> getAll();
	for( int k = 0; k < n; k++ )
		p_val.add( *pv++ );

	if( clearOwn )
		data -> clear();
}

ObjectField *SerializeObject::addFieldFloatTwoIndexArray( const char *name , int id )
{
	if( id <= 0 )
		id = fieldList.count() + 1;

	addFieldInt( String( name ) + "#N1" , 1000 + id );
	addFieldInt( String( name ) + "#N2" , 2000 + id );
	return( addFieldList( id , name , TP_FLOAT ) );
}

void SerializeObject::setPropFloatTwoIndexArray( TwoIndexArray<float> *p_val , const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::setPropFloatTwoIndexArray: unknown field: " ) + p_field );

	ObjectField *fieldN1 = getFieldByName( String( p_field ) + "#N1" );
	if( fieldN1 == NULL )
		throw RuntimeError( String( "SerializeObject::setPropFloatTwoIndexArray: unknown N1 for field: " ) + p_field );

	ObjectField *fieldN2 = getFieldByName( String( p_field ) + "#N2" );
	if( fieldN2 == NULL )
		throw RuntimeError( String( "SerializeObject::setPropFloatTwoIndexArray: unknown N2 for field: " ) + p_field );

	// save as list
	int N1 = p_val -> getN1();
	int N2 = p_val -> getN2();
	int n = N1 * N2;
	field -> setFloatList( p_val -> getData() , n , false );
	fieldN1 -> setInt( N1 );
	fieldN2 -> setInt( N2 );
}

TwoIndexArray<float> *SerializeObject::getPropFloatTwoIndexArray( const char *p_field )
{
	ObjectField *field = getFieldByName( p_field );
	if( field == NULL )
		throw RuntimeError( String( "SerializeObject::getPropFloatTwoIndexArray: unknown field: " ) + p_field );

	ObjectField *fieldN1 = getFieldByName( String( p_field ) + "#N1" );
	if( fieldN1 == NULL )
		throw RuntimeError( String( "SerializeObject::getPropFloatTwoIndexArray: unknown N1 for field: " ) + p_field );

	ObjectField *fieldN2 = getFieldByName( String( p_field ) + "#N2" );
	if( fieldN2 == NULL )
		throw RuntimeError( String( "SerializeObject::getPropFloatTwoIndexArray: unknown N2 for field: " ) + p_field );

	// save as list
	int N1 = fieldN1 -> getInt();
	int N2 = fieldN2 -> getInt();
	int n;

	float *fv = field -> getFloatList( &n , false );
	if( n != N1 * N2 )
		throw RuntimeError( String( "SerializeObject::getPropFloatTwoIndexArray: wrong indexes for field: " ) + p_field );

	TwoIndexArray<float> *av = new TwoIndexArray<float>( N1 , N2 );
	memcpy( av -> getData() , fv , n * sizeof( float ) );

	return( av );
}

String SerializeObject::getDataStringNameToValue()
{
	String s = "";
	fillMapString( s , true , 1 );
	return( s );
}

String SerializeObject::getDataStringIdToValue()
{
	String s = "";
	fillMapString( s , false , 0 );
	return( s );
}

void SerializeObject::fillMapString( String& s , bool useName , int level )
{
	s += TB_OPEN;

	// add class name
	s += String( ( useName )? "class" : "0" ) + "=\"" + effectiveObjectClass + "\"";

	char l_buf[ 12 ];
	int nListItems = 0;
	for( int k = 0; k < fieldList.count(); k++ ) {
		// delimiter
		s += TB_LISTDELIMITER_AND_SPACE;
		s += "\n";
		s.append( level , '\t' );

		// field prefix
		ObjectField *field = fieldList.get( k );
		int id = field -> getId();
			
		if( useName )
			s += field -> getName();
		else {
			sprintf( l_buf , "%d" , id );
			s += l_buf;
		}
		s += "=";

		// field data
		if( field -> isList() )
			s += TB_OPEN;

		switch( field -> getBaseType() ) {
			case TP_CHAR :
				addCharEscaped( s , field -> getChar() );
				break;
			case TP_INT :
				if( field -> isList() ) {
					int *v = field -> getIntList( &nListItems , false );
					for( int m = 0; m < nListItems; m++ ) {
						if( m > 0 )
							s += TB_LISTDELIMITER_AND_SPACE;
						addInt( s , v[ m ] );
					}
				}
				else
					addInt( s , field -> getInt() );
				break;
			case TP_FLOAT :
				if( field -> isList() ) {
					float *v = field -> getFloatList( &nListItems , false );
					for( int m = 0; m < nListItems; m++ ) {
						if( m > 0 )
							s += TB_LISTDELIMITER_AND_SPACE;
						addFloat( s , v[ m ] );
					}
				}
				else
					addFloat( s , field -> getFloat() );
				break;
			case TP_STRING :
				if( field -> isList() ) {
					const char **v = field -> getStringList( &nListItems , false );
					for( int m = 0; m < nListItems; m++ ) {
						if( m > 0 )
							s += TB_LISTDELIMITER_AND_SPACE;
						addStringEscaped( s , v[ m ] );
					}
				}
				else {
					addStringEscaped( s , field -> getString( false ) );
				}
				break;
			case TP_OBJECT :
				if( field -> isList() ) {
					FlatList<Object *> *v = field -> getObjectList();
					SerializeObject *so = field -> getFieldSerializeObject();
					nListItems = v -> count();
					Object **va = v -> getAll();

					for( int m = 0; m < nListItems; m++ ) {
						if( m > 0 )
							s += TB_LISTDELIMITER_AND_SPACE;

						s += "\n";
						s.append( level + 1 , '\t' );

						// serialize using object SO - if field's one is empty
						Object *o = *va++;
						SerializeObject *soUsed = ( so != NULL )? so : o -> getSerializeObject();
						Object::serialize( o , *soUsed );

						// add object data from new line
						soUsed -> fillMapString( s , useName , level + 1 );
					}
				}
				else {
					SerializeObject *so = field -> getFieldSerializeObject();
					Object *o = field -> getObject( false );

					if( o != NULL ) {
						// serialize using object SO - if field's one is empty
						SerializeObject *soUsed = ( so != NULL )? so : o -> getSerializeObject();
						Object::serialize( o , *soUsed );
						soUsed -> fillMapString( s , useName , level + 1 );
					}
					else {
						// add NULL object representaion
						s += TB_OPEN;
						s += TB_CLOSE;
					}
				}
				break;
		}

		// list terminator
		if( field -> isList() )
			s += TB_CLOSE;
	}

	s += TB_CLOSE;
}

void SerializeObject::addInt( String& s , int v )
{
	char l_buf[ 12 ];
	sprintf( l_buf , "%d" , v );
	s += l_buf;
}

void SerializeObject::addFloat( String& s , float v )
{
	char l_buf[ 64 ];
	int len = sprintf( l_buf , "%#.6f" , v );

	// ignore trailing zeros
	while( l_buf[ --len ] == '0' );

	// skip decimal if not required
	if( l_buf[ len ] == '.' )
		l_buf[ len ] = 0;
	else
		l_buf[ len + 1 ] = 0;

	s += l_buf;
}

const char *SerializeObject::readFromString( const char *p , SerializeObject *so , SerializeObject *soSrc , Object **pv )
{
	char c;

	int part = 0;
	int len = 0;

	const int MAX_NAMELEN = 128;
	char l_buf[ MAX_NAMELEN ];
	int key;
	ObjectField *field;

	// object data
	const int MAX_FIELDS = 256;
	FieldData dataValue[ MAX_FIELDS ];
	ObjectField *dataField[ MAX_FIELDS ];
	int dataListCount[ MAX_FIELDS ];
	int dataCount = 0;

	if( so != NULL )
		so -> clearData();

	// skip spaces
	p = skipSpaces( p );
	bool endOfList = false;
	while( c = *p ) {
		// possibles: "   {  x    =y,    x=y, x=y, x="y", x = {y} }"
		switch( part ) {
			case 0 :
				if( c != TB_OPEN )
					throw RuntimeError( "SerializeObject::readFromString: missed { symbol" );

				part = 1;
				len = 0;

				// skip spaces
				p = skipSpaces( ++p );

				// check empty list
				if( *p == TB_CLOSE )
					return( ++p );
				break;
			case 1 :
				if( c == '=' || c == ' ' ) {
					// terminates key ID
					if( len == 0 )
						throw RuntimeError( "SerializeObject::readFromString: missed field id/name" );

					l_buf[ len ] = 0;
					len = 0;

					bool isClassName = false;
					bool ignoreFieldData = false;
					if( soSrc == NULL ) {
						if( strcmp( l_buf , "class" ) ) {
							ASSERT( so != NULL );

							// read using field names
							field = so -> getFieldByName( l_buf );
							if( field == NULL )
								ignoreFieldData = true;
						}
						else
							isClassName = true;
					}
					else {
						key = atoi( l_buf );
						if( key > 0 ) {
							ASSERT( so != NULL );
							field = so -> getFieldById( key , soSrc );
							if( field == NULL )
								ignoreFieldData = true;
						}
						else
							isClassName = true;
					}

					// skip spaces=spaces
					if( c == ' ' ) {
						// find =
						p = skipSpaces( p );
						if( *p++ != '=' )
							throw RuntimeError( "SerializeObject::readFromString: missed = after field id/name" );
					}
					else {	
						// skip =
						p++;
					}
					p = skipSpaces( p );

					if( ignoreFieldData ) {
						// ignore/skip field data
						p = skipFieldData( p );
						part = 2;
						continue;
					}

					if( isClassName ) {
						// read special property - class name
						char *className;
						p = readString( p , true , &className , NULL );

						if( so == NULL )
							so = ObjectService::getService() -> getSerializeObject( className );
						so -> setEffectiveObjectClass( className );

						if( className != NULL )
							free( className );
					}
					else {
						ASSERT( so != NULL );

						// create object if first field
						if( *pv == NULL )
							*pv = so -> createObject();

						ObjectField *fieldSrc = NULL;
						if( soSrc != NULL )
							fieldSrc = soSrc -> getFieldById( key );

						if( dataCount >= MAX_FIELDS )
							throw RuntimeError( "SerializeObject::readFromString: too many fields" );

						// postpone object translation to avoid recursion
						dataField[ dataCount ] = field;
						dataValue[ dataCount ].V = NULL;
						dataListCount[ dataCount ] = 0;

						p = so -> readFieldData( *pv , p , field , fieldSrc , &dataValue[ dataCount ] , &dataListCount[ dataCount ] );
						dataCount++;
					}
					part = 2;
				}
				else {
					if( soSrc != NULL ) {
						if( !isdigit( c ) )
							throw RuntimeError( "SerializeObject::readFromString: non-digit found in field id" );

						if( len == 10 )
							throw RuntimeError( "SerializeObject::readFromString: too long field id" );
					}
					else
						if( len >= MAX_NAMELEN )
							throw RuntimeError( "SerializeObject::readFromString: too long field name" );

					l_buf[ len++ ] = c;
					p++;
				}
				break;
			case 2 :
				p = skipListDelimiter( p , &endOfList );
				if( endOfList ) {
					if( *p++ != TB_CLOSE )
						throw RuntimeError( "SerializeObject::readObjectFromString: no end of String" );

					// set data
					for( int k = 0; k < dataCount; k++ )
						so -> setFieldData( dataField[ k ] , &dataValue[ k ] , dataListCount[ k ] );

					return( p );
				}

				part = 1;
				break;
		}
	}

	throw RuntimeError( "SerializeObject::readObjectFromString: unexpected end of row" );
}

void SerializeObject::setFieldData( ObjectField *field , void *dataValue , int dataListCount )
{
	FieldData& v = *( FieldData * )dataValue;

	switch( field -> getBaseType() ) {
		case TP_CHAR :
			field -> setChar( v.charV );
			break;

		case TP_INT :
			if( field -> isList() )
				field -> setIntList( v.intVL , dataListCount , true );
			else
				field -> setInt( v.intV );
			break;

		case TP_FLOAT :
			if( field -> isList() )
				field -> setFloatList( v.floatVL , dataListCount , true );
			else
				field -> setFloat( v.floatV );
			break;

		case TP_STRING :
			if( field -> isList() )
				field -> setStringList( ( const char ** )v.stringVL , dataListCount , true );
			else
				field -> setString( v.stringV , true );
			break;

		case TP_OBJECT :
			if( field -> isList() )
				field -> setObjectList( v.objectVL , true );
			else
				field -> setObject( v.objectV , true );
			break;

		default :
			throw RuntimeError( "SerializeObject::readFieldData: unknown datatype" );
	}
}

const char *SerializeObject::skipListDelimiter( const char *p , bool *endOfList )
{
	if( TB_LISTDELIMITER != ' ' )
		p = skipSpaces( p );

	char c = *p;

	// if end of list
	if( c == TB_CLOSE ) {
		*endOfList = true;
		return( p );
	}

	*endOfList = false;

	// treat tab as space
	if( c == '\t' || c == '\n' )
		c = ' ';

	// expect list delimiter
	if( c != TB_LISTDELIMITER )
		throw RuntimeError( "SerializeObject::skipListDelimiter: list delimiter not found" );

	p = skipSpaces( ++p );
	return( p );
}

const char *SerializeObject::skipSpaces( const char *p )
{
	return( p + strspn( p , " \t\n" ) );
}

const char *SerializeObject::skipFieldData( const char *p )
{
	// find start of field data
	p = skipSpaces( p );

	// if String
	if( *p == '"' )
		return( skipString( p ) );

	// if block
	if( *p == TB_OPEN )
		return( skipBlock( p ) );

	// no data
	if( *p == 0 )
		throw RuntimeError( "SerializeObject::skipFieldData: no data" );

	// if simple data - skip until end of String/block margin/list delimiter
	char c;
	while( c = *p++ )
		if( c == TB_OPEN || c == TB_CLOSE || c == TB_LISTDELIMITER )
			break;

	return( --p );
}

const char *SerializeObject::readString( const char *p , bool pAlloc , char **pvAlloc , char *pvNoAlloc )
{
	// max size
	int len = skipString( p ) - p - 2;

	// empty String
	if( len == 0 ) {
		if( pAlloc )
			*pvAlloc = NULL;
		else
			*pvNoAlloc = 0;
		return( p );
	}

	// allocate
	char *v;
	if( pAlloc )
		v = ( char * )malloc( len + 1 );
	else
		v = pvNoAlloc;
	char *lv = v;

	// read String "
	*p++;

	while( *p ) {
		const char *px = strpbrk( p , "\"\\" );
		if( px == NULL )
			throw RuntimeError( "SerializeObject::readString: end of String not found" );
			
		// copy all between
		int n = px - p;
		strncpy( lv , p , n );
		lv += n;

		p = px;
		if( *p++ == '"' )
			break;

		if( !*p )
			throw RuntimeError( "SerializeObject::readString: unexpected end of String" );

		// escaped symbol
		*lv++ = *p++;
	}

	*lv = 0;

	// special handling for allocated
	if( pAlloc )
		if( lv == v ) {
			// empty String
			free( v );
			*pvAlloc = NULL;
		}
		else
			*pvAlloc = v;

	return( p );
}

const char *SerializeObject::skipString( const char *p )
{
	// skip spaces
	while( *p == ' ' )
		p++;

	if( *p++ != '"' )
		throw RuntimeError( "SerializeObject::skipString: expected String start symbol" );

	while( *p ) {
		const char *px = strpbrk( p , "\"\\" );
			
		// if no end String mark
		if( px == NULL )
			throw RuntimeError( "SerializeObject::skipString: no end String mark" );

		if( *px == '"' )
			return( ++px );

		// if escaped symbol
		if( !*++px )
			throw RuntimeError( "SerializeObject::skipString: unexpected end of String" );
			
		p = ++px;
	}

	throw RuntimeError( "SerializeObject::skipString: end of String not found" );
}

const char *SerializeObject::skipBlock( const char *p )
{
	// skip spaces
	while( *p == ' ' )
		p++;

	// expect start of block
	if( *p++ != TB_OPEN )
		throw RuntimeError( "SerializeObject::skipBlock: start of block expected" );

	// count block marks, but ignore inside strings
	int bc = 1;
	char eob[4] = { TB_OPEN , TB_CLOSE , '"' , 0 };
	while( bc ) {
		// find special symbol
		const char *px = strpbrk( p , eob );
		if( px == NULL )
			throw RuntimeError( "SerializeObject::skipBlock: no end block mark" );

		switch( *px ) {
			case TB_OPEN :
				bc++;
				p = ++px;
				break;
					
			case TB_CLOSE :
				bc--;
				p = ++px;
				break;

			case '"' :
				p = skipString( px );
				break;
		}
	}

	return( p );
}

const char *SerializeObject::readFieldData( Object *o , const char *p , ObjectField *field , ObjectField *fieldSrc , FieldData *data , int *pn )
{
	// read list data
	if( field -> isList() )
		return( readFieldDataList( o , p , field , fieldSrc , data , pn ) );

	*pn = 0;
	p = skipSpaces( p );

	// read single item
	FieldData& v = *( FieldData * )data;

	SerializeObject *soSrc = NULL;
	switch( field -> getBaseType() ) {
		case TP_CHAR :
			p = readChar( p , &v.charV );
			break;

		case TP_INT :
			p = readInt( p , &v.intV );
			break;

		case TP_FLOAT :
			p = readFloat( p , &v.floatV );
			break;

		case TP_STRING :
			p = readString( p , true , &v.stringV , NULL );
			break;

		case TP_OBJECT :
			if( fieldSrc != NULL )
				soSrc = fieldSrc -> getFieldSerializeObject();
			p = readObject( o , p , &v.objectV , field -> getFieldSerializeObject() , soSrc );
			break;

		default :
			throw RuntimeError( "SerializeObject::readFieldData: unknown datatype" );
	}

	return( p );
}

const char *SerializeObject::readInt( const char *p , int *p_v )
{
	// find start of data
	p = skipSpaces( p );

	// read chars related to buffer
	char l_buf[ 12 ];
	char *s = l_buf;
	char c;
	int n = 0;
	while( c = *p ) {
		if( n == 11 )
			throw RuntimeError( "SerializeObject::readInt: too long for int data" );

		if( isdigit( c ) || c == '-' )
			n++ , *s++ = *p++;
		else
			break;
	}
	if( c == 0 )
		throw RuntimeError( "SerializeObject::readInt: unexpected end of String" );
	
	// read int
	*s = 0;
	if( sscanf( l_buf , "%d%c" , p_v , &c ) != 1 )
		throw RuntimeError( "SerializeObject::readInt: wrong int data" );

	return( p );
}

const char *SerializeObject::readFloat( const char *p , float *p_v )
{
	// find start of data
	p = skipSpaces( p );

	// read chars related to buffer
	char l_buf[ 64 ];
	char *s = l_buf;
	char c;
	int n = 0;
	while( c = *p ) {
		if( n == 63 )
			throw RuntimeError( "SerializeObject::readFloat: too long for float data" );

		if( isdigit( c ) || c == '-' || c == '.' )
			n++ , *s++ = *p++;
		else
			break;
	}
	if( c == 0 )
		throw RuntimeError( "SerializeObject::readFloat: unexpected end of String" );
	
	// read int
	*s = 0;
	if( sscanf( l_buf , "%f%c" , p_v , &c ) != 1 )
		throw RuntimeError( "SerializeObject::readFloat: wrong float data" );

	return( p );
}

const char *SerializeObject::readFieldDataList( Object *o , const char *p , ObjectField *field , ObjectField *fieldSrc , FieldData *data , int *pn )
{
	p = skipSpaces( p );

	// check size
	int size = skipBlock( p ) - p;

	if( *p++ != TB_OPEN )
		throw RuntimeError( "SerializeObject::readFieldDataList: no start block mark" );

	p = skipSpaces( p );

	// read object list
	FieldData& v = *( FieldData * )data;
	int n = *pn = readListCount( p );
	
	// empty list
	if( n == 0 ) {
		p = skipSpaces( p );
		if( *p++ != TB_CLOSE )
			throw RuntimeError( "SerializeObject::readFieldDataList: no stop block mark" );
		return( p );
	}

	// allocate list memory
	char *charData;
	switch( field -> getBaseType() ) {
		case TP_CHAR :
			throw RuntimeError( "SerializeObject::readFieldDataList: wrong datatype - char list" );
		case TP_INT :
			v.intVL = ( int * )calloc( n , sizeof( int ) );
			break;

		case TP_FLOAT :
			v.floatVL = ( float * )calloc( n , sizeof( float ) );
			break;

		case TP_STRING :
			v.stringVL = ( char ** )calloc( 1 , sizeof( const char * ) * n + size );
			charData = ( char * )( v.stringVL + n );
			break;

		case TP_OBJECT :
			v.objectVL = new ClassList<Object>;
			v.objectVL -> allocate( n );

			// set field later - when filled
			break;
	}

	// save ptr
	FieldData vw;
	vw.V = v.V;

	// read data
	Object *objectData;
	SerializeObject *soSrc = NULL;
	for( int k = 0; k < n; k++ ) {
		switch( field -> getBaseType() ) {
			case TP_CHAR :
				throw RuntimeError( "SerializeObject::readFieldDataList: wrong datatype - char list" );
			case TP_INT :
				p = readInt( p , vw.intVL++ );
				break;

			case TP_FLOAT :
				p = readFloat( p , vw.floatVL++ );
				break;

			case TP_STRING :
				*vw.stringVL++ = charData;
				p = readString( p , false , NULL , charData );
				charData += strlen( charData ) + 1;
				break;

			case TP_OBJECT :
				if( fieldSrc != NULL )
					soSrc = fieldSrc -> getFieldSerializeObject();
				p = readObject( o , p , &objectData , field -> getFieldSerializeObject() , soSrc );
				vw.objectVL -> add( objectData );
				break;
		}

		bool endOfList = false;
		p = skipListDelimiter( p , &endOfList );

		if( !endOfList )
			if( k == ( n - 1 ) )
				throw RuntimeError( "SerializeObject::readFieldDataList: no end of list after last item" );
	}

	// check list terminator
	if( *p++ != TB_CLOSE )
		throw RuntimeError( "SerializeObject::readFieldDataList: no stop block mark" );

	return( p );
}

const char *SerializeObject::readObject( Object *parent , const char *p , Object **pv , SerializeObject *so , SerializeObject *soSrc )
{
	// deserialize object from String
	*pv = NULL;
	p = SerializeObject::readFromString( p , so , soSrc , pv );
	
	Object *o = *pv;
	if( o != NULL ) {
		// set effective class by default
		if( so != NULL )
			so -> setEffectiveObjectClass( o -> getClass() );
		else
			so = o -> getSerializeObject();

		// read data
		Object::deserialize( parent , o , *so );
	}

	return( p );
}

void SerializeObject::addStringEscaped( String& s , const char *p )
{
	int x;
	s += '"';

	while( *p ) {
		x = strcspn( p , "\\\"" );
		if( x > 0 ) {
			s.append( p , x );
			p += x;
		}
		else {
			s += '\\';
			s += *p++;
		}
	}

	s += '"';
}

const char *SerializeObject::getObjectClass()
{
	return( objectClass );
}

int SerializeObject::readListCount( const char *p )
{
	p = skipSpaces( p );
	if( *p == TB_CLOSE )
		return( 0 );

	int n = 0;
	while( *p ) {
		p = skipFieldData( p );
		n++;

		bool endOfList = false;
		p = skipListDelimiter( p , &endOfList );
		if( endOfList )
			return( n );
	}

	throw RuntimeError( "SerializeObject::readListCount: no list stop mark" );
}

Object *SerializeObject::createObject()
{
	if( pfnCreateObject == NULL )
		throw RuntimeError( "SerializeObject::createObject: factory method is NULL" );

	return( ( *pfnCreateObject )( effectiveObjectClass ) );
}

const char *SerializeObject::getClass()
{
	return( "SerializeObject" );
}

void SerializeObject::createSerializeObject()
{
	SerializeObject *so = new SerializeObject( "SerializeObject" );

	// create dependent objects
	ObjectField::createSerializeObject();

	// add fields
	so -> setFactoryMethod( SerializeObject::onCreate );
	so -> addFieldString( "className" , 1 );
	so -> addFieldObjectList( "fieldList" , ObjectField::getSerializeObject() , 2 );
}

SerializeObject *SerializeObject::getSerializeObject()
{ 
	return( ObjectService::getService() -> getSerializeObject( "SerializeObject" ) ); 
}

void SerializeObject::serialize( SerializeObject& so )
{
	so.setPropString( objectClass , "className" , false );
	so.setPropObjectList( ( ClassList<Object>& )fieldList , "fieldList" , false );
}

void SerializeObject::deserialize( Object *parent , SerializeObject& so )
{
	objectClass = so.getPropString( "className" , false );

	// delete old field list
	fieldList.destroy();
	so.getPropObjectList( ( ClassList<Object>& )fieldList , "fieldList" , true );

	// rebuild maps
	rfc_map_ptrclear( mapIdToField );
	rfc_map_strclear( mapNameToField );
	for( int k = 0; k < fieldList.count(); k++ ) {
		ObjectField *field = fieldList.get( k );
		field -> attach( this );

		rfc_map_stradd( mapNameToField , field -> getName() , ( void * )field );
		rfc_map_ptradd( mapIdToField , ( void * )field -> getId() , ( unsigned long )field );
	}
}

String SerializeObject::getPK()
{
	return( getClass() );
}

void SerializeObject::setFactoryMethod( Object *( *p_pfnCreateObject )( const char *className ) )
{
	pfnCreateObject = p_pfnCreateObject;
}

void SerializeObject::addCharEscaped( String& s , char v )
{
	if( v == 0 )
		throw RuntimeError( "SerializeObject::addCharEscaped: cannot save zero char" );

	// escape specials and spaces
	if( v == TB_OPEN || v == TB_CLOSE || v == TB_LISTDELIMITER || v == '"' || isspace( v ) )
		s += '\\';
	s += v;
}

const char *SerializeObject::readChar( const char *p , char *p_v )
{
	p = skipSpaces( p );

	// check escaped
	char c = *p++;
	if( c == '\\' ) {
		c = *p++;
		if( c == 0 )
			throw RuntimeError( "SerializeObject::readChar: zero char found in data" );
	}
	*p_v = c;
	return( p );
}

void SerializeObject::clearData()
{
	for( int k = 0; k < fieldList.count(); k++ ) {
		ObjectField *f = fieldList.get( k );
		f -> clearData();
	}
}

const char *SerializeObject::getEffectiveObjectClass()
{
	return( effectiveObjectClass );
}

void SerializeObject::setEffectiveObjectClass( const char *className )
{
	effectiveObjectClass = className;
}
