#include <ah_platform.h>

// #############################################################################
// #############################################################################

typedef struct {
	int size;
} StringData;

static StringData *getData( char *p ) {
	return( ( StringData * )( p - sizeof( StringData ) ) );
}

static StringData *ensureFreeSpace( char *& p , int space ) {
	// create new
	if( p == NULL ) {
		StringData *d = ( StringData * )malloc( sizeof( StringData ) + space + 1 );
		p = ( char * )( d + 1 );
		*p = 0;

		d -> size = space;
		return( d );
	}
				
	// if space already exists
	StringData *d = getData( p );
	if( d -> size - strlen( p ) >= ( unsigned )space )
		return( d );

	// reallocation
	if( space < 16 )
		space = 16;

	int sizeNew = strlen( p ) + space;
	d = ( StringData * )realloc( d , sizeof( StringData ) + sizeNew + 1 );
	d -> size = sizeNew;

	p = ( char * )( d + 1 );
	return( d );
}

// class String
String::String() {
	v = NULL;
}

String::~String() {
	if( v != NULL )
		free( getData( v ) );
}

String::String( const char *s ) {
	v = NULL;
	if( s != NULL )
		assign( s , strlen( s ) );
}

String::String( const char *s , int len ) {
	v = NULL;
	if( len > 0 )
		assign( s , len );
}

void String::createFromString( const char *s ) {
	assign( s , strlen( s ) );
}

String::String( const String& s ) {
	v = NULL;
	assign( s , s.length() );
}

char String::getChar( int index ) const {
	if( v == NULL )
		return( 0 );
	ASSERT( index < length() );
	return( v[ index ] );
}

bool String::equals( const char *s ) const {
	if( v == NULL )
		return( s == NULL || *s == 0 );
	if( s == NULL )
		return( *v == 0 );

	return( strcmp( s , v ) == 0 );
}

String& String::operator +=( const char *s ) {
	if( s == NULL )
		return( *this );

	if( v == NULL )
		return( assign( s , strlen( s ) ) );

	return( append( s , strlen( s ) ) );
}

String& String::append( const char *s ,  int n2 ) {
	if( s == NULL )
		return( *this );

	if( v == NULL )
		return( assign( s , n2 ) );

	StringData *d = ensureFreeSpace( v , n2 );

	int len = strlen( v );
	memcpy( v + len , s , n2 );
	v[ len + n2 ] = 0;

	return( *this );
}

void String::remove( int from ) {
	remove( from , -1 );
}

void String::remove( int from , int n ) {
	if( v == NULL )
		return;

	int len = strlen( v );
	if( from >= len )
		return;

	if( n < 0 )
		n = len - from;

	if( n >= len - from ) {
		v[ from ] = 0;
		return;
	}

	memmove( v + from , v + from + n , len - from - n );
	v[ len - n ] = 0;
}

String& String::operator +=( char c ) {
	return( append( &c , 1 ) );
}

String& String::operator +=( int v ) {
	char l_buf[ 12 ];
	sprintf( l_buf , "%d" , v );
	return( append( l_buf , strlen( l_buf ) ) );
}

String& String::operator +=( float v ) {
	char l_buf[ 30 ];
	sprintf( l_buf , "%g" , v );
	return( append( l_buf , strlen( l_buf ) ) );
}

String::operator const char *() const {
	if( v == NULL )
		return( "" );
	return( v );
}

String& String::operator =( const char *s ) {
	if( s == NULL )
		return( *this );

	return( assign( s , strlen( s ) ) );
}

String& String::operator =( String& s ) {
	if( s == NULL )
		return( *this );

	return( assign( s , strlen( s ) ) );
}

void String::clear() {
	if( v == NULL )
		return;

	*v = 0;
}

String& String::assign( const char *s ,  int n2 ) {
	if( v != NULL )
		*v = 0;

	StringData *d = ensureFreeSpace( v , n2 );
	memcpy( v , s , n2 );
	v[ n2 ] = 0;

	return( *this );
}

String& String::append( int count , char c ) {
	StringData *d = ensureFreeSpace( v , count );

	int len = strlen( v );
	memset( v + len , c , count );
	v[ len + count ] = 0;

	return( *this );
}

void String::resize( int nn ) {
	if( v == NULL ) {
		ensureFreeSpace( v , nn );
		return;
	}
			
	ensureFreeSpace( v , nn - length() );
}

char *String::getBuffer() {
	return( v );
}

int String::size() const {
	if( v == NULL )
		return( 0 );

	StringData *d = getData( v );
	return( d -> size );
}

int String::length() const {
	if( v == NULL )
		return( 0 );

	return( strlen( v ) );
}

bool String::isEmpty() const {
	return( v == NULL || *v == 0 );
}

int String::find( const char *substring ) const {
	if( v == NULL || substring == NULL )
		return( -1 );

	char *ptr = strstr( v , substring );
	if( ptr == NULL )
		return( -1 );

	return( ptr - v );
}

int String::find( int startfrom , const char *substring ) const {
	if( v == NULL || substring == NULL )
		return( -1 );

	if( strlen( v ) <= ( unsigned )startfrom )
		return( -1 );

	char *ptr = strstr( v + startfrom , substring );
	if( ptr == NULL )
		return( -1 );

	return( ptr - v );
}

int String::findLastAny( const char *chars ) const {
	if( v == NULL )
		return( -1 );

	char c;
	char *pBest = NULL;
	while( c = *chars++ ) {
		char *p = strrchr( v , c );
		if( p != NULL )
			if( pBest == NULL || p > pBest )
				pBest = p;
	}

	if( pBest != NULL )
		return( pBest - v );

	return( -1 );
}

String String::toUpper() const {
	String x = v;
	_strupr( x.v );
	return( x );
}

String String::toLower() const {
	String x = v;
	_strlwr( x.v );
	return( x );
}

String String::toHex( int value ) {
	char l_buf[ 10 ];
	sprintf( l_buf , "%x" , value );
	return( l_buf );
}

int String::findLast( char c ) const {
	if( v == NULL )
		return( -1 );

	char *p = strrchr( v , c );
	if( p == NULL )
		return( -1 );

	return( p - v );
}

String String::getMid( int from , int n ) const {
	if( v == NULL )
		return( "" );

	int len = length();
	if( from >= len )
		return( "" );

	String s;
	if( n > len - from )
		n = len - from;
	s.assign( v + from  , n );
	return( s );
}
 
String String::getMid( int from ) const {
	if( v == NULL )
		return( "" );

	int len = length();
	if( from >= len )
		return( "" );

	String s;
	int n = len - from;
	s.assign( v + from  , n );
	return( s );
}

int String::split( StringList& parts , String delimiter ) const {
	if( isEmpty() )
		return( 0 );

	// scan string
	const char *p = v;
	while( *v ) {
		const char *pn = strstr( p , delimiter );
		if( pn == NULL ) {
			parts.add( p );
			break;
		}

		parts.add( getMid( p - v , pn - p ) );
		p = pn + delimiter.length();
	}

	return( parts.count() );
}

String String::parseStringLiteral( const char *p ) {
	String s;
	if( p == NULL )
		return( s );

	char c;
	char buf[ 10 ];
	int n;
	while( c = *p++ ) {
		if( c != '\\' ) {
			s += c;
			continue;
		}

		c = *p++;
		switch( c ) {
			case 0 : s += '\\'; return( s );
			default : s += c; break;
			case 'n' : s += '\n'; break;
			case 't' : s += '\t'; break;
			case 'v' : s += '\v'; break;
			case 'b' : s += '\b'; break;
			case 'r' : s += '\r'; break;
			case 'f' : s += '\f'; break;
			case 'a' : s += '\a'; break;
			case '\\' : s += '\\'; break;
			case '?' : s += '?'; break;
			case '\'' : s += '\''; break;
			case '"' : s += '"'; break;

			case 'x' : 
				n = 0;
				while( c = *p ) {
					if( n < 2 && isxdigit( c ) ) {
						buf[ n++ ] = c;
						p++;
					}
					else
						break;
				}
				buf[ n ] = 0;
				sscanf( buf , "%x" , &n );
				s += ( char )n;
				break;

			case '0' : case '1' : case '2' : case '3' : case '4' : case '5' : case '6' : case '7' : case '8' : case '9' : 
				n = 1;
				buf[ 0 ] = c;
				while( c = *p ) {
					if( n < 3 && isdigit( c ) ) {
						buf[ n++ ] = c;
						p++;
					}
					else
						break;
				}
				buf[ n ] = 0;
				sscanf( buf , "%d" , &n );
				s += ( char )n;
				break;
		}
	}

	return( s );
}

bool String::startsFrom( const char *substring ) const {
	if( v == NULL )
		return( false );

	return( strncmp( v , substring , strlen( substring ) ) == 0 );
}

void String::insert( int from , const char *s ) {
	if( s == NULL )
		return;

	int length = ( v == NULL )? 0 : strlen( v );
	ASSERTMSG( from <= length , "String::insert - invalid string operation" );

	int lengthAdd = strlen( s );
	resize( length + lengthAdd );

	memmove( v + from + lengthAdd , v + from , length - from );
	v[ length + lengthAdd ] = 0;
	memcpy( v + from , s , lengthAdd );
}

void String::trim() {
	if( v == NULL )
		return;

	const char *spaces = " \t\n\r";
	int skip = strspn( v , spaces );
	remove( 0 , skip );

	// from the end
	int n = strlen( v );
	for( skip = 0; skip < n && strchr( spaces , v[ n - skip - 1 ] ) != NULL; skip++ );
	remove( n - skip , skip );
}

int String::toInteger() {
	ASSERTMSG( v != NULL , "string is null" );

	int value;
	int argc = sscanf( v , "%d" , &value );
	ASSERTMSG( argc == 1 , String( "cannot get integer from string=" ) + v );

	return( value );
}

void String::trim( char c ) {
	trimTrailing( c );
	trimStarting( c );
}

void String::trimStarting( char c ) {
	if( v == NULL )
		return;

	int n = strlen( v );
	int nd = 0;
	char *xv = v;
	for( ; n; n-- , nd++ )
		if( *xv++ != c )
			break;

	if( nd > 0 )
		remove( 0 , nd );
}

void String::trimTrailing( char c ) {
	if( v == NULL )
		return;

	int n = strlen( v );
	int nd = 0;
	char *xv = v + n - 1;
	for( ; n; n-- , nd++ )
		if( *xv-- != c )
			break;

	if( nd > 0 )
		remove( n );
}

String String::replicate( int count ) const {
	String s;
	if( v == NULL )
		return( s );

	ASSERTMSG( count >= 0 , "invalid count" );
	int len = strlen( v );
	if( len == 0 )
		return( s );

	int total = len * count;
	s.resize( total );

	for( int k = 0; k < count; k++ )
		for( int m = k * len , z = 0; z < len; z++ )
			s.v[ m + z ] = v[ z ];
	s.v[ total ] = 0;
	return( s );
}

String String::replace( const char *textFrom , const char *textTo ) const {
	String res;

	int pos = 0;
	while( true ) {
		int next = find( pos , textFrom );
		if( next < 0 ) {
			res += getMid( pos );
			break;
		}

		if( next > pos )
			res += getMid( pos , next - pos );

		res += textTo;
		pos = next + strlen( textFrom );
	}

	return( res );
}

// #############################################################################
// #############################################################################

String operator +( String& s1 , const char *s2 ) {
	String s = s1;
	s += s2;
	return( s );
}

String operator +( const String& s1 , int value ) {
	char l_buf[ 12 ];
	sprintf( l_buf , "%d" , value );

	String s = s1;
	s += l_buf;
	return( s );
}

String operator +( const String& s1 , unsigned value ) {
	char l_buf[ 12 ];
	sprintf( l_buf , "%u" , value );

	String s = s1;
	s += l_buf;
	return( s );
}

String operator +( const String& s1 , float value ) {
	char l_buf[ 30 ];
	sprintf( l_buf , "%g" , value );

	String s = s1;
	s += l_buf;
	return( s );
}

String operator +( const String& s1 , double value ) {
	char l_buf[ 30 ];
	sprintf( l_buf , "%lf" , value );

	String s = s1;
	s += l_buf;
	return( s );
}

String operator +( const String& s1 , const char *s2 ) {
	String s = s1;
	s += s2;
	return( s );
}

String operator +( const String& s1 , char value ) {
	String s = s1;
	s += value;
	return( s );
}

String operator +( const String& s1 , bool value ) {
	String s = s1;
	s += ( value )? "true" : "false";
	return( s );
}

void trimString( char *p , const char *trimChars ) {
	if( p == NULL )
		return;

	// start
	char *pb = p;
	char c;
	while( c = *pb ) {
		if( strchr( trimChars , c ) == NULL )
			break;

		pb++;
	}

	// end
	int len = strlen( pb );
	char *px = pb + len - 1;
	for( ; len > 0; len-- , px-- )
		if( strchr( trimChars , *px ) == NULL ) {
			*++px = 0;
			break;
		}

	// move
	memmove( p , pb , px - pb + 1 );
}
