#include <ah_platform.h>
#include <ah_services_impl.h>

/*#########################################################################*/
/*#########################################################################*/

Xml::Xml() {
	doc = NULL;
	node = NULL;
}

void Xml::attach( void *p_doc , void *p_node ) {
	doc = p_doc;
	node = p_node;
}

void Xml::destroy() {
	if( !exists() )
		return;

	TiXmlDocument *l_doc = ( TiXmlDocument * )doc;
	delete l_doc;

	doc = NULL;
	node = NULL;
}

String Xml::getName() {
	ASSERT( node != NULL );

	TiXmlElement *xml = ( TiXmlElement * )node;
	return( xml -> Value() );
}

String Xml::getValue() {
	ASSERT( node != NULL );

	TiXmlElement *xml = ( TiXmlElement * )node;
	return( xml -> GetText() );
}

String Xml::getHeading() {
	ASSERT( node != NULL );

	TiXmlElement *xml = ( TiXmlElement * )node;

	std::string x;
	xml -> PrintWithoutChilds( &x , 0 );

	String s = x.c_str();
	return( s );
}

Xml Xml::getChildNode( String s ) {
	ASSERT( node != NULL );

	TiXmlElement *xml = ( TiXmlElement * )node;
	TiXmlElement *xmlChild = NULL;
	try {
		xmlChild = xml -> FirstChildElement( s );
	}
	catch( ... ) {
		// ignore - handle by null value returned
	}

	Xml x;
	x.attach( doc , xmlChild );

	return( x );
}

Xml Xml::getParentNode() {
	ASSERT( node != NULL );

	TiXmlNode *xml = ( TiXmlNode * )node;
	TiXmlNode *xmlParent = NULL;
	try {
		xmlParent = xml -> Parent();
		if( xmlParent -> Type() == TiXmlNode::DOCUMENT )
			xmlParent = NULL;
	}
	catch( ... ) {
		// ignore - handle by null value returned
	}

	Xml x;
	x.attach( doc , xmlParent );

	return( x );
}

String Xml::getXName() {
	ASSERTMSG( node != NULL , "node is null" );
	String name = getName();
	String attr = getAttribute( "name" , "" );
	if( !attr.isEmpty() )
		name += "[@name='" + attr + "']";
	
	return( name );
}

String Xml::getXPath() {
	ASSERTMSG( node != NULL , "node is null" );
	String xpath = getXName();
	Xml parent = *this;
	while( ( parent = parent.getParentNode() ).exists() ) {
		String xpathadd = parent.getXName();
		xpath = xpathadd + "/" + xpath;
	}
	return( xpath );
}

Xml Xml::getChildNamedNode( String element , String name ) {
	return( findChildByPathAttr( element , "name" , name ) );
}

String Xml::getAttribute( String a ) {
	ASSERTMSG( node != NULL , a + " attribute parent node is null" );

	TiXmlElement *xml = ( TiXmlElement * )node;
	const char *v = xml -> Attribute( a );

	ASSERTMSG( v != NULL , String( "attribute " ) + a + " not found" );
	return( v );
}

String Xml::getAttribute( String a , String defaultValue ) {
	ASSERTMSG( node != NULL , a + " attribute parent node is null" );

	TiXmlElement *xml = ( TiXmlElement * )node;
	const char *v = xml -> Attribute( a );

	if( v == NULL )
		return( defaultValue );

	return( v );
}

bool Xml::getBooleanAttribute( String a ) {
	ASSERTMSG( node != NULL , a + " attribute parent node is null" );

	TiXmlElement *xml = ( TiXmlElement * )node;
	return( getAttribute( a , "false" ).equals( "true" ) );
}

bool Xml::getBooleanAttribute( String a , bool defaultValue ) {
	ASSERTMSG( node != NULL , a + " attribute parent node is null" );

	TiXmlElement *xml = ( TiXmlElement * )node;
	return( getAttribute( a , ( ( defaultValue )? "true" : "false" ) ).equals( "true" ) );
}

int Xml::getIntAttribute( String a ) {
	ASSERTMSG( node != NULL , a + " attribute parent node is null" );

	TiXmlElement *xml = ( TiXmlElement * )node;
	const char *v = xml -> Attribute( a );

	ASSERTMSG( v != NULL , a + " attribute is null" );
	int value = 0;
	ASSERTMSG( sscanf( v , "%d" , &value ) == 1 , "unable to parse integer in string: " + a );
	return( value );
}

int Xml::getIntAttribute( String a , int defaultValue ) {
	ASSERT( node != NULL );

	TiXmlElement *xml = ( TiXmlElement * )node;
	const char *v = xml -> Attribute( a );

	if( v == NULL )
		return( defaultValue );

	int value = 0;
	ASSERTMSG( sscanf( v , "%d" , &value ) == 1 , "unable to parse integer in string: " + a );
	return( value );
}

String Xml::getProperty( String name ) {
	ASSERTMSG( node != NULL , name + " property parent node is null" );

	TiXmlElement *xml = ( TiXmlElement * )node;
	TiXmlElement *xmlChild = xml -> FirstChildElement( "property" );
	while( xmlChild != NULL ) {
		String nameFound = xmlChild -> Attribute( "name" );
		if( nameFound.equals( name ) )
			break;

		xmlChild = xmlChild -> NextSiblingElement( "property" );
	}

	ASSERTMSG( xmlChild != NULL , String( "property " ) + name + " not found" );

	const char *v = xmlChild -> Attribute( "value" );
	ASSERTMSG( v != NULL , String( "property " ) + name + " has no value attribute" );
	return( v );
}

bool Xml::getBooleanProperty( String name ) {
	String prop = getProperty( name , "false" );
	return( prop.equals( "true" ) );
}

bool Xml::getBooleanProperty( String name , bool defaultValue ) {
	String prop = getProperty( name , ( ( defaultValue )? "true" : "false" ) );
	return( prop.equals( "true" ) );
}

int Xml::getIntProperty( String name ) {
	String prop = getProperty( name );
	char c;
	int v;
	ASSERTMSG( sscanf( prop , "%d%c" , &v , &c ) == 1 , name + " property is not available" );
	return( v );
}

int Xml::getIntProperty( String name , int defaultValue ) {
	char buf[ 12 ];
	sprintf( buf , "%d" , defaultValue );
	String prop = getProperty( name , buf );
	char c;
	int v;
	ASSERTMSG( sscanf( prop , "%d%c" , &v , &c ) == 1 , name + " property is not available" );
	return( v );
}

float Xml::getFloatProperty( String name ) {
	String prop = getProperty( name );
	char c;
	float v;
	ASSERTMSG( sscanf( prop , "%f%c" , &v , &c ) == 1 , name + " property is not available" );
	return( v );
}

float Xml::getFloatProperty( String name , float defaultValue ) {
	char buf[ 40 ];
	sprintf( buf , "%f" , defaultValue );
	String prop = getProperty( name , buf );
	char c;
	float v;
	ASSERTMSG( sscanf( prop , "%f%c" , &v , &c ) == 1 , name + " property is not available" );
	return( v );
}

String Xml::getProperty( String name , String defaultValue ) {
	ASSERTMSG( node != NULL , name + " property parent node is null" );

	TiXmlElement *xml = ( TiXmlElement * )node;
	TiXmlElement *xmlChild = xml -> FirstChildElement( "property" );
	while( xmlChild != NULL ) {
		String nameFound = xmlChild -> Attribute( "name" );
		if( nameFound.equals( name ) )
			break;

		xmlChild = xmlChild -> NextSiblingElement( "property" );
	}

	if( xmlChild == NULL )
		return( defaultValue );

	const char *v = xmlChild -> Attribute( "value" );
	ASSERTMSG( v != NULL , String( "property " ) + name + " has no value attribute" );

	if( *v == 0 )
		return( defaultValue );

	return( v );
}

bool Xml::exists() {
	return( node != NULL );
}

void Xml::setAttribute( String a , String value ) {
	ASSERTMSG( node != NULL , a + " attribute parent node is null" );
	TiXmlElement *xml = ( TiXmlElement * )node;

	xml -> SetAttribute( a , value );
}

void Xml::setBooleanAttribute( String a , bool value ) {
	ASSERTMSG( node != NULL , a + " attribute parent node is null" );
	TiXmlElement *xml = ( TiXmlElement * )node;

	const char *valueStr = ( value )? "true" : "false";
	xml -> SetAttribute( a , valueStr );
}

void Xml::setIntAttribute( String a , int value ) {
	ASSERTMSG( node != NULL , a + " attribute parent node is null" );
	TiXmlElement *xml = ( TiXmlElement * )node;

	char v[ 15 ];
	sprintf( v , "%d" , value );
	xml -> SetAttribute( a , v );
}

Xml Xml::getFirstChild( String name ) {
	ASSERTMSG( node != NULL , name + " node parent is null" );

	TiXmlElement *xml = ( TiXmlElement * )node;

	TiXmlElement *xmlChild = NULL;
	try {
		xmlChild = xml -> FirstChildElement( name );
	}
	catch( ... ) {
		// ignore - handle by null value returned
	}

	Xml item;
	item.attach( doc , xmlChild );
	return( item );
}

Xml Xml::getNextChild( String name ) {
	ASSERTMSG( node != NULL , name + " node parent is null" );

	TiXmlElement *xml = ( TiXmlElement * )node;
	TiXmlElement *xmlChild = xml -> NextSiblingElement( name );

	Xml item;
	item.attach( doc , xmlChild );
	return( item );
}

void Xml::getProperties( MapStringToString& map ) {
	ASSERT( node != NULL );

	TiXmlElement *xml = ( TiXmlElement * )node;
	TiXmlElement *xmlChild = xml -> FirstChildElement( "property" );
	while( xmlChild != NULL ) {
		String name = xmlChild -> Attribute( "name" );
		String value = xmlChild -> Attribute( "value" );
		map.add( name , value );

		xmlChild = xmlChild -> NextSiblingElement( "property" );
	}
}

String Xml::serialize() {
	TiXmlDocument *l_doc = ( TiXmlDocument * )doc;

	std::string x;
	l_doc -> Print( &x , 0 );

	String s = x.c_str();
	return( s );
}

Xml Xml::addTextElement( String name , String value ) {
	ASSERTMSG( node != NULL , name + " node parent is null" );

	TiXmlElement *xml = ( TiXmlElement * )node;
	TiXmlElement *xmlElement = new TiXmlElement( name );
	xml -> LinkEndChild( xmlElement );
	TiXmlText *xmlValue = new TiXmlText( value );
	xmlElement -> LinkEndChild( xmlValue );

	Xml ret;
	ret.attach( doc , xmlValue );
	return( ret );
}

Xml Xml::addElement( String name ) {
	ASSERTMSG( node != NULL , name + " node parent is null" );

	TiXmlNode *xml = ( TiXmlNode * )node;
	TiXmlElement *xmlProp = new TiXmlElement( name );
	xml -> LinkEndChild( xmlProp );

	Xml ret;
	ret.attach( doc , xmlProp );
	return( ret );
}

void Xml::setProperty( String name , String value ) {
	ASSERTMSG( node != NULL , name + " property node parent is null" );

	TiXmlNode *xml = ( TiXmlNode * )node;
	TiXmlElement *xmlProp = new TiXmlElement( "property" );
	xmlProp -> SetAttribute( "name" , name );
	xmlProp -> SetAttribute( "value" , value );
	xml -> LinkEndChild( xmlProp );
}

void Xml::setBooleanProperty( String name , bool value ) {
	setProperty( name , ( ( value )? "true" : "false" ) );
}

void Xml::setIntProperty( String name , int value ) {
	char l_buf[ 12 ];
	sprintf( l_buf , "%d" , value );
	setProperty( name , l_buf );
}

void Xml::setFloatProperty( String name , float value ) {
	char l_buf[ 40 ];
	sprintf( l_buf , "%f" , value );
	setProperty( name , l_buf );
}

Xml Xml::findChildByPath( String path ) {
	Xml xml = *this;

	const char *p = path;
	while( *p ) {
		const char *np = strchr( p , '/' );

		String key;
		if( np == NULL ) {
			key = p;
			p += strlen( p );
		}
		else {
			key = path.getMid( p - path , np - p );
			p = np + 1;
		}

		xml = xml.getFirstChild( key );
		if( !xml.exists() )
			return( xml );
	}

	return( xml );
}

Xml Xml::findChildByPathAttr( String path , String attr , String value ) {
	Xml xml = findChildByPath( path );
	if( !xml.exists() )
		return( xml );

	String key;
	int index = path.findLast( '/' );
	if( index < 0 )
		key = path;
	else
		key = path.getMid( index + 1 );

	while( xml.exists() ) {
		if( xml.getAttribute( attr , "" ).equals( value ) )
			return( xml );

		xml = xml.getNextChild( key );
	}
	return( xml );
}

Xml Xml::parse( const char *src , int& len , bool& p_error ) {
	if( src == NULL )
		return( Xml() );

	// parse
	TiXmlDocument *doc = new TiXmlDocument();

	Xml xml;
	try {
		const char *res = doc -> Parse( src );
		if( res != NULL ) {
			p_error = false;
			len = res - src;
			xml.attach( doc , doc -> FirstChildElement() );
		}
	}
	catch( RuntimeException& e ) {
		Logger log;
		log.attachRoot();
		log.printStack( e );
		p_error = true;
		len = 0;
	}

	return( xml );
}

Xml Xml::read( const char *data , const char *contentName ) {
	TiXmlDocument *doc = new TiXmlDocument();

	const char *p = doc -> Parse( data );
	if( doc -> Error() ) {
		delete doc;
		String err = "XML message cannot be read";
		throw RuntimeError( err );
	}

	// verify trailing data are only spaces
	if( p != NULL ) {
		char c;
		while( c = *p++ )
			if( !( c == ' ' || c == '\t' || c == '\v' || c == '\n' || c == '\r' ) )
				break;

		if( c ) {
			delete doc;
			String err = "XML message is malformed - data found behind message: ";
			err += data;
			throw RuntimeError( err );
		}
	}

	// verify message has given child element
	try {
		TiXmlElement *item = doc -> FirstChildElement( contentName );
		if( item != NULL )
			{
				Xml xml;
				xml.attach( doc , item );
				return( xml );
			}
	}
	catch( ... ) {
		// ignore - generate own exception instead
	}

	delete doc;
	String err = String( "XML message is malformed - cannot find required top item: " ) + contentName;
	throw RuntimeError( err );
}

Xml Xml::create( const char *contentName ) {
	TiXmlDocument *doc = new TiXmlDocument();
	TiXmlElement *top = new TiXmlElement( contentName );
	doc -> LinkEndChild( top );
	
	Xml xml;
	xml.attach( doc , top );

	return( xml );
}

Xml Xml::load( const char *path ) {
	TiXmlDocument *doc = new TiXmlDocument( path );
	if( !doc -> LoadFile() ) {
		String err = String( "loadXml: cannot load root configuration from path=" ) + path;
		delete doc;
		throw RuntimeError( err );
	}

	Xml xml;
	xml.attach( doc , doc -> FirstChildElement() );

	return( xml );
}

int Xml::getEnumProperty( String name , String pairList ) {
	String value = getProperty( name );

	StringList pairs;
	pairList.split( pairs , "," );

	for( int k = 0; k < pairs.count(); k++ ) {
		StringList items;
		pairs.get( k ).split( items , "=" );

		String key = items.get( 0 );
		key.trim();
		if( value.equals( key ) )
			return( items.get( 1 ).toInteger() );
	}

	ASSERTFAILED( "unable to find value=" + value + " in enum: " + pairList );
	return( 0 );
}
