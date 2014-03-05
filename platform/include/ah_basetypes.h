#ifndef	INCLUDE_AH_BASETYPES_H
#define INCLUDE_AH_BASETYPES_H

// #############################################################################
// #############################################################################

extern "C" {
#include <library/generic/include/__gen.h>
}

#include <stdexcept>
#include <iostream>

using namespace std;

class String;
class RuntimeException;
class Xml;
class MultiIndexIterator;
class MapStringToInt;
class MapStringToString;
class MapStringToPtr;
class StringList;
class Value;
class Random;
class Timer;

// #############################################################################
// #############################################################################

class String {
public:
	String();
	~String();
	String( const char *s );
	String( const String& s );
	String( const char *s , int len );

	String& operator +=( const char *v );
	String& operator +=( char v );
	String& operator +=( int v );
	String& operator +=( float v );
	operator const char *() const;
	String& operator =( const char *s );
	String& operator =( String& s );

	bool equals( const char *s ) const;
	void clear();
	String& append( int count , char c );
	String& append( const char *s , int nChars );
	String& assign( const char *s , int nChars );
	void remove( int from );
	void remove( int from , int n );
	void insert( int from , const char *s );

	void resize( int length );
	char *getBuffer();
	int size() const;
	int length() const;
	bool isEmpty() const;

	int find( const char *substring ) const;
	int find( int startfrom , const char *substring ) const;
	int findLastAny( const char *chars ) const;
	int findLast( char c ) const;
	String getMid( int from , int n ) const;
	String getMid( int from ) const;
	char getChar( int index ) const;
	String toUpper() const;
	String toLower() const;
	static String toHex( int value );
	int split( StringList& parts , String delimiter ) const;
	bool startsFrom( const char *substring ) const;
	void trim();
	void trim( char c );
	void trimStarting( char c );
	void trimTrailing( char c );
	int toInteger();
	String replicate( int count ) const;
	String replace( const char *textFrom , const char *textTo ) const;

	static String parseStringLiteral( const char *p );

private:
	void createFromString( const char *s );
private:
	char *v;
};

String operator +( const String& s1 , const char *s2 );
String operator +( const String& s1 , int value );
String operator +( const String& s1 , unsigned value );
String operator +( const String& s1 , float value );
String operator +( const String& s1 , double value );
String operator +( const String& s1 , char value );
String operator +( const String& s1 , bool value );

// #############################################################################
// #############################################################################

#define RuntimeError( msg ) RuntimeException( msg , __FILE__ , __LINE__ )

// utility functions & classes
#define ASSERT( x ) if( !(x) ) throw RuntimeError( String( "assertion failed (" ) + (#x) + ")" )
#define ASSERTMSG( x , msg ) if( !(x) ) throw RuntimeError( String( "assertion failed (" ) + (#x) + "): " + (msg) )
#define ASSERTFAILED( msg ) throw RuntimeError( String( "assertion failed: " ) + msg )

class RuntimeException {
public:
	RuntimeException( unsigned int code , int skipLevels , void *p_addr );
	RuntimeException( const char *p_msg , const char *p_file , int p_line );
	String printStackToString();

	bool isSEH();
	const char *getClassName();
	const char *getFunctionName();
	const char *getMsg();
	const char *getFileShort();
	int getLine();
	rfc_threadstack *getStack();
	int getSkipCount();

public:
	bool seh;

	String className;
	String functionName;
	String msg;
	String file;
	String fileShort;
	int line;
	rfc_threadstack *stack;
};

/*#########################################################################*/
/*#########################################################################*/

class Xml {
public:
	Xml();
	void attach( void *doc , void *node );
	bool exists();
	void destroy();

	static Xml parse( const char *src , int& len , bool& p_error );
	static Xml read( const char *data , const char *contentName );
	static Xml create( const char *contentName );
	static Xml load( const char *path );

	void *getDoc() { return( doc ); };
	void *getNode() { return( node ); };

	// direct data
	String getName();
	String getValue();
	String serialize();
	String getHeading();

	// navigation
	Xml getParentNode();
	Xml getChildNode( String s );
	Xml getChildNamedNode( String element , String name );
	Xml getFirstChild( String name );
	Xml getNextChild( String name );
	// path items are delimited by '/' chars
	Xml findChildByPath( String path );
	Xml findChildByPathAttr( String path , String attr , String value );
	String getXPath();
	String getXName();

	// attributes
	String getAttribute( String a );
	String getAttribute( String a , String defaultValue );
	bool getBooleanAttribute( String a );
	bool getBooleanAttribute( String a , bool defaultValue );
	int getIntAttribute( String a );
	int getIntAttribute( String a , int defaultValue );
	void setAttribute( String a , String value );
	void setBooleanAttribute( String a , bool value );
	void setIntAttribute( String a , int value );

	// properties
	void getProperties( MapStringToString& map );
	String getProperty( String name );
	String getProperty( String name , String defaultValue );
	bool getBooleanProperty( String name );
	bool getBooleanProperty( String name , bool defaultValue );
	int getIntProperty( String name );
	int getIntProperty( String name , int defaultValue );
	float getFloatProperty( String name );
	float getFloatProperty( String name , float defaultValue );
	void setProperty( String name , String value );
	void setBooleanProperty( String name , bool value );
	void setIntProperty( String name , int value );
	void setFloatProperty( String name , float value );
	int getEnumProperty( String name , String pairList );

	// elements
	Xml addTextElement( String name , String value );
	Xml addElement( String name );

private:
	void *doc;
	void *node;
};

// #############################################################################
// #############################################################################

extern "C" static int lstcompareStringListAscend( void *p_userdata , const RFC_TYPE *p_e1 , const RFC_TYPE *p_e2 );
extern "C" static int lstcompareStringListDescend( void *p_userdata , const RFC_TYPE *p_e1 , const RFC_TYPE *p_e2 );
extern void trimString( char *p , const char *trimChars );

class StringList {
public:
	StringList() {
		data = rfc_lst_create( RFC_EXT_TYPESTRING );
	};
	~StringList() {
		rfc_lst_destroy( data );
	};

	int add( const char *value ) {
		RFC_TYPE v;
		v.u_c = value;
		int index = rfc_lst_add( data , &v );
		return( index );
	}
	int add( StringList *p ) {
		if( p == NULL )
			return( 0 );
		for( int k = 0; k < p -> count(); k++ )
			add( p -> get( k ) );
		return( p -> count() );
	}

	int count() {
		return( data -> s_n );
	}

	String get( int index ) {
		ASSERT( index >= 0 && index < data -> s_n );
		return( rfc_lst_get( data , index ) -> u_c );
	}

	void remove( int index ) {
		ASSERT( index >= 0 && index < data -> s_n );
		rfc_lst_remove( data , index );
	}

	void set( int index , const char *value ) {
		ASSERT( index >= 0 && index < data -> s_n );
		RFC_TYPE v;
		v.u_c = value;
		rfc_lst_replace( data , index , &v );
	}

	int find( String name ) {
		for( int k = 0; k < data -> s_n; k++ )
			if( name.equals( data -> s_p[ k ].u_c ) )
				return( k );
		return( -1 );
	}

	void trim() {
		for( int k = 0; k < data -> s_n; k++ )
			trimString( data -> s_p[ k ].u_s , " \t\n\r\v" );
	}

	void sort() {
		rfc_lst_sort( data , NULL , NULL );
	}

	void clear() {
		rfc_lst_clear( data );
	}

	String combine( String delimiter ) {
		if( data -> s_n == 0 )
			return( "" );

		String s = data -> s_p[ 0 ].u_s;
		for( int k = 1; k < data -> s_n; k++ )
			s += delimiter + data -> s_p[ k ].u_s;

		return( s );
	}

	rfc_list *data;
};

// #############################################################################
// #############################################################################

// map String to int
class MapStringToInt {
public:
	MapStringToInt() { mapData = rfc_map_strcreate(); };
	~MapStringToInt() { rfc_map_strdrop( mapData ); };

public:
	void allocate( int count ) { rfc_map_stralloc( mapData , count ); };

	int add( const char *key , int value ) {
		int l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) >= 0 )
			throw RuntimeError( "MapStringToInt::add - duplicate key=" + String( key ) );

		int index = rfc_map_strsetkey( mapData , key , ( void * )value );
		return( index );
	};

	int addnew( const char *key , int value ) {
		int l_value;
		int index = rfc_map_strcheck( mapData , key , ( void ** )&l_value );
		if( index < 0 )
			index = rfc_map_strsetkey( mapData , key , ( void * )value );
		return( index );
	}

	void add( MapStringToInt& a ) {
		rfc_strmap *s = a.mapData;
		for( int k = 0; k < rfc_map_strcount( s ); k++ )
			add( s -> s_p[ k ].s_x , ( int )s -> s_p[ k ].s_y );
	};

	void addnew( MapStringToInt& a ) {
		rfc_strmap *s = a.mapData;
		for( int k = 0; k < rfc_map_strcount( s ); k++ )
			addnew( s -> s_p[ k ].s_x , ( int )s -> s_p[ k ].s_y );
	};

	void add( StringList& a , int value ) {
		for( int k = 0; k < a.count(); k++ ) {
			String s = a.get( k );
			add( s , value );
		}
	};

	void addnew( StringList& a , int value ) {
		for( int k = 0; k < a.count(); k++ ) {
			String s = a.get( k );
			addnew( s , value );
		}
	};

	int set( const char *key , int value ) {
		int l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) < 0 )
			l_value = -1;

		rfc_map_strsetkey( mapData , key , ( void * )value );
		return( l_value );
	};

	int get( const char *key ) {
		int l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) < 0 )
			return( -1 );

		return( l_value );
	};
	// return item from which given string begins
	int getPartial( const char *key ) {
		int n = rfc_map_strcount( mapData );
		if( n == 0 )
			return( -1 );

		int fp = rfc_map_strinsertpos( mapData , key );

		// check returned - it could be equal to required
		if( fp < n ) {
			const char *kfp = mapData -> s_p[ fp ].s_x;
			if( !strcmp( kfp , key ) )
				return( ( int )mapData -> s_p[ fp ].s_y );
		}

		// check previous - it could be partially equal to returned
		if( fp > 0 ) {
			fp--;
			const char *kfp = mapData -> s_p[ fp ].s_x;
			if( !strncmp( kfp , key , strlen( kfp ) ) )
				return( ( int )mapData -> s_p[ fp ].s_y );
		}

		return( -1 );
	};

	int remove( const char *key ) { return( ( int )rfc_map_strremove( mapData , key ) ); };

	int count() { return( rfc_map_strcount( mapData ) ); };

	void destroy() { rfc_map_strclear( mapData ); };

	int getClassByIndex( int k ) {
		if( k < 0 || k >= rfc_map_strcount( mapData ) )
			throw RuntimeError( "MapStringToInt::getByIndex - invalid index" );

		return( ( int )mapData -> s_p[ k ].s_y );
	};
			
	const char *getKeyByIndex( int k ) {
		if( k < 0 || k >= rfc_map_strcount( mapData ) )
			throw RuntimeError( "MapStringToInt::getByIndex - invalid index" );

		return( mapData -> s_p[ k ].s_x );
	};
			
private:
	rfc_strmap *mapData;
};

// #############################################################################
// #############################################################################

// map String to String
class MapStringToString {
public:
	MapStringToString() { mapData = rfc_map_strcreate(); };
	~MapStringToString() {
		destroy();
		rfc_map_strdrop( mapData );
	};

public:
	void allocate( int count ) { rfc_map_stralloc( mapData , count ); };

	int add( const char *key , const char *value ) {
		ASSERTMSG( value != NULL , "value should not be NULL" );
		char *l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) >= 0 )
			throw RuntimeError( "MapStringToString::add - duplicate key=" + String( key ) );

		char *setValue = _strdup( value );
		int index = rfc_map_strsetkey( mapData , key , ( void * )setValue );
		return( index );
	};

	int addnew( const char *key , const char *value ) {
		ASSERTMSG( value != NULL , "value should not be NULL" );
		char *l_value;
		int index = rfc_map_strcheck( mapData , key , ( void ** )&l_value );
		if( index < 0 ) {
			char *setValue = _strdup( value );
			index = rfc_map_strsetkey( mapData , key , ( void * )setValue );
		}
		return( index );
	}

	void add( MapStringToString& a ) {
		rfc_strmap *s = a.mapData;
		for( int k = 0; k < rfc_map_strcount( s ); k++ ) {
			char *setValue = _strdup( ( char * )s -> s_p[ k ].s_y );
			add( s -> s_p[ k ].s_x , setValue );
		}
	};

	void addnew( MapStringToString& a ) {
		rfc_strmap *s = a.mapData;
		for( int k = 0; k < rfc_map_strcount( s ); k++ ) {
			char *setValue = _strdup( ( char * )s -> s_p[ k ].s_y );
			addnew( s -> s_p[ k ].s_x , setValue );
		}
	};

	void add( StringList& a , String value ) {
		for( int k = 0; k < a.count(); k++ ) {
			String s = a.get( k );
			add( s , value );
		}
	};

	void addnew( StringList& a , String value ) {
		for( int k = 0; k < a.count(); k++ ) {
			String s = a.get( k );
			addnew( s , value );
		}
	};

	void set( const char *key , const char *value ) {
		ASSERTMSG( value != NULL , "value should not be NULL" );
		char *l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) < 0 )
			l_value = NULL;
		else
			free( l_value );

		char *setValue = _strdup( value );
		rfc_map_strsetkey( mapData , key , ( void * )setValue );
	};

	const char *get( const char *key ) {
		char *l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) < 0 )
			return( NULL );

		return( l_value );
	};
	// return item from which given string begins
	const char *getPartial( const char *key ) {
		int n = rfc_map_strcount( mapData );
		if( n == 0 )
			return( NULL );

		int fp = rfc_map_strinsertpos( mapData , key );

		// check returned - it could be equal to required
		if( fp < n ) {
			const char *kfp = mapData -> s_p[ fp ].s_x;
			if( !strcmp( kfp , key ) )
				return( ( const char * )mapData -> s_p[ fp ].s_y );
		}

		// check previous - it could be partially equal to returned
		if( fp > 0 ) {
			fp--;
			const char *kfp = mapData -> s_p[ fp ].s_x;
			if( !strncmp( kfp , key , strlen( kfp ) ) )
				return( ( const char * )mapData -> s_p[ fp ].s_y );
		}

		return( NULL );
	};

	void remove( const char *key ) {
		char *value = ( char * )rfc_map_strremove( mapData , key );
		free( value );
	};

	int count() { return( rfc_map_strcount( mapData ) ); };

	void destroy() {
		for( int k = 0; k < mapData -> s_n; k++ )
			free( ( char * )mapData -> s_p[ k ].s_y );
		rfc_map_strclear( mapData );
	};

	const char *getClassByIndex( int k ) {
		if( k < 0 || k >= rfc_map_strcount( mapData ) )
			throw RuntimeError( "MapStringToString::getByIndex - invalid index" );

		return( ( const char * )mapData -> s_p[ k ].s_y );
	};
			
	const char *getKeyByIndex( int k ) {
		if( k < 0 || k >= rfc_map_strcount( mapData ) )
			throw RuntimeError( "MapStringToString::getByIndex - invalid index" );

		return( mapData -> s_p[ k ].s_x );
	};
			
private:
	rfc_strmap *mapData;
};

// #############################################################################
// #############################################################################

// map String to void *
class MapStringToPtr {
public:
	MapStringToPtr() { mapData = rfc_map_strcreate(); };
	~MapStringToPtr() {
		destroy();
		rfc_map_strdrop( mapData );
	};

public:
	void allocate( int count ) { rfc_map_stralloc( mapData , count ); };

	int add( const char *key , void *value ) {
		ASSERTMSG( value != NULL , "value should not be NULL" );
		void *l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) >= 0 )
			throw RuntimeError( "MapStringToPtr::add - duplicate key=" + String( key ) );

		int index = rfc_map_strsetkey( mapData , key , value );
		return( index );
	};

	int addnew( const char *key , void *value ) {
		ASSERTMSG( value != NULL , "value should not be NULL" );
		void *l_value;
		int index = rfc_map_strcheck( mapData , key , ( void ** )&l_value );
		if( index < 0 )
			index = rfc_map_strsetkey( mapData , key , value );
		return( index );
	};

	void add( MapStringToPtr& a ) {
		rfc_strmap *s = a.mapData;
		for( int k = 0; k < rfc_map_strcount( s ); k++ ) {
			void *setValue = s -> s_p[ k ].s_y;
			add( s -> s_p[ k ].s_x , setValue );
		}
	};

	void addnew( MapStringToPtr& a ) {
		rfc_strmap *s = a.mapData;
		for( int k = 0; k < rfc_map_strcount( s ); k++ ) {
			void *setValue = s -> s_p[ k ].s_y;
			addnew( s -> s_p[ k ].s_x , setValue );
		}
	};

	void add( StringList& a , void *value ) {
		for( int k = 0; k < a.count(); k++ ) {
			String s = a.get( k );
			add( s , value );
		}
	};

	void addnew( StringList& a , void *value ) {
		for( int k = 0; k < a.count(); k++ ) {
			String s = a.get( k );
			addnew( s , value );
		}
	};

	void set( const char *key , void *value ) {
		ASSERTMSG( value != NULL , "value should not be NULL" );
		rfc_map_strsetkey( mapData , key , value );
	};

	void *get( const char *key ) {
		void *l_value;
		if( rfc_map_strcheck( mapData , key , ( void ** )&l_value ) < 0 )
			return( NULL );

		return( l_value );
	};

	// return item from which given string begins
	const void *getPartial( const char *key ) {
		int n = rfc_map_strcount( mapData );
		if( n == 0 )
			return( NULL );

		int fp = rfc_map_strinsertpos( mapData , key );

		// check returned - it could be equal to required
		if( fp < n ) {
			const char *kfp = mapData -> s_p[ fp ].s_x;
			if( !strcmp( kfp , key ) )
				return( mapData -> s_p[ fp ].s_y );
		}

		// check previous - it could be partially equal to returned
		if( fp > 0 ) {
			fp--;
			const char *kfp = mapData -> s_p[ fp ].s_x;
			if( !strncmp( kfp , key , strlen( kfp ) ) )
				return( mapData -> s_p[ fp ].s_y );
		}

		return( NULL );
	};

	void remove( const char *key ) {
		rfc_map_strremove( mapData , key );
	};

	int count() { return( rfc_map_strcount( mapData ) ); };

	void destroy() {
		rfc_map_strclear( mapData );
	};

	const void *getClassByIndex( int k ) {
		if( k < 0 || k >= rfc_map_strcount( mapData ) )
			throw RuntimeError( "MapStringToPtr::getByIndex - invalid index" );

		return( mapData -> s_p[ k ].s_y );
	};
			
	const char *getKeyByIndex( int k ) {
		if( k < 0 || k >= rfc_map_strcount( mapData ) )
			throw RuntimeError( "MapStringToString::getByIndex - invalid index" );

		return( mapData -> s_p[ k ].s_x );
	};
			
private:
	rfc_strmap *mapData;
};

// #############################################################################
// #############################################################################

class Value {
public:
	RFC_TYPE value;
	int type;

public:
	Value( long v ) { value.u_l = v; type = RFC_EXT_TYPELONG; };
	Value( double v ) { value.u_f = v; type = RFC_EXT_TYPEDOUBLE; };
	Value( RFC_INT64 v ) { value.u_m = v; type = RFC_EXT_TYPEMONEY; };
	Value( const char *v ) { if( v == NULL ) v = ""; value.u_s = _strdup( v ); type = RFC_EXT_TYPECHAR; };
	Value( short v ) { value.u_d = v; type = RFC_EXT_TYPESHORT; };
	Value( float v ) { value.u_r = v; type = RFC_EXT_TYPESHORT; };
	~Value() { if( type == RFC_EXT_TYPECHAR ) free( value.u_s ); };

	long getLong() { ASSERTMSG( type == RFC_EXT_TYPELONG , "RFC_EXT_TYPELONG expected" ); return( value.u_l ); };
	double getDouble() { ASSERTMSG( type == RFC_EXT_TYPEDOUBLE , "RFC_EXT_TYPEDOUBLE expected" ); return( value.u_f ); };
	RFC_INT64 getInt64() { ASSERTMSG( type == RFC_EXT_TYPEMONEY , "RFC_EXT_TYPEMONEY expected" ); return( value.u_m ); };
	const char *getString() { ASSERTMSG( type == RFC_EXT_TYPECHAR , "RFC_EXT_TYPECHAR expected" ); return( value.u_s ); };
	short getShort() { ASSERTMSG( type == RFC_EXT_TYPESHORT , "RFC_EXT_TYPESHORT expected" ); return( value.u_d ); };
	float getFloat() { ASSERTMSG( type == RFC_EXT_TYPEFLOAT , "RFC_EXT_TYPEFLOAT expected" ); return( value.u_r ); };

	Value& operator =( const Value& cv ) {
		if( cv.type == RFC_EXT_TYPECHAR )
			value.u_s = _strdup( cv.value.u_s );
		else
			value.u_m = cv.value.u_m;
		type = cv.type;
		return( *this );
	}
};

// #############################################################################
// #############################################################################

class Random {
	typedef enum {
		RND_UNKNOWN = 0 ,
		RND_RANGEFLOAT = 1 ,
		RND_TWORANGEFLOAT = 2 ,
		RND_RANGEINT = 3 ,
		RND_TWORANGEINT = 4
	} RandomType;

public:
	Random();
	~Random();

	void createRangeRandomsFloat( float min , float max );
	void createTwoRangeRandomsFloat( float min1 , float max1 , float min2 , float max2 );
	void createRangeRandomsInt( int min , int max );
	void createTwoRangeRandomsInt( int min1 , int max1 , int min2 , int max2 );

	int getRandomInt();
	float getRandomFloat();
	float getRandomFloat( float min , float max );

	void collectStatisticsInt();
	void collectStatisticsFloat( int count );
	void showStatistics();

	static int getRandomIntStatic( int min , int max );
	static float getRandomFloatStatic( float min , float max );

private:
	static float randFloat();

private:
	static int instanceCount;
	RandomType type;
	RFC_TYPE min1;
	RFC_TYPE max1;
	RFC_TYPE min2;
	RFC_TYPE max2;

	// random stat
	bool collect;
	int *statCounts;
	int statBuckets;
	float bucketSize;
};

// #############################################################################
// #############################################################################

class Timer {
public:
	Timer();
	// do not set interval, set/not set starting timestamp
	void createWithStartingTimestamp();
	// set interval in ms and initial timestamp
	void createRunWindowMs( int runTimeMs );
	// set interval in seconds and initial timestamp
	void createRunWindowSec( int runTimeSec );

	// initial adjustments
	static void startAdjustment();
	static void stopAdjustment();
	// time passed from process start - in ms, using clock()
	static int clockSinceProcessStartMs();
	// time passed from process start - in ms, using rfc_hpt_setpoint
	static RFC_INT64 getCurrentTimeMillis();

public:
	// timestamp of timer construction - in ms
	int timeCreatedMs();
	// time passed - in ms
	int timePassedMs();
	// time passed - in clocks
	int timePassedClocks();
	// time passed - in ticks
	int timePassedTicks();
	// time remained - in secs
	int timeRemainedSec();

	// convert clocks to ms
	static int timeClocksToMs( int clocks );
	// convert ms to clocks
	static int timeMsToClocks( int ms );
	// convert ticks to ms
	static int timeTicksToMs( int ticks );
	// convert ms to ticks
	static int timeMsToTicks( int ms );

	// check current time is within run window
	bool go();
	// wait next interval
	int waitNext();

private:
	long timeStarted;
	RFC_INT64 timeStartedTicks;
	int waitTimeMs;
	int waitCount;
};

// #############################################################################
// #############################################################################

#endif	// INCLUDE_AH_BASETYPES_H
