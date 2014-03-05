#ifndef	INCLUDE_AH_OBJECTBASE_H
#define INCLUDE_AH_OBJECTBASE_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_services.h"

// object interface
class ObjectService;
class Object;
class SerializeObject;

/*#########################################################################*/
/*#########################################################################*/

class ObjectService : public Service {
public:
	// object serializations
	void registerSerializeObject( SerializeObject *sop );
	SerializeObject *getSerializeObject( const char *className );
	SerializeObject *getSerializeObjectInstance( const char *p_inst );
	void addSerializeObjectInstance( SerializeObject *sop , const char *p_inst );
	void replaceSerializeObjectInstance( SerializeObject *sop , const char *p_inst );
	void replaceSerializeObjectInstanceSrc( SerializeObject *sop );

	virtual const char *getServiceName() { return( "ObjectService" ); };
	virtual void configureService( Xml config ) {};
	virtual void createService();
	virtual void initService() {};
	virtual void runService() {};
	virtual void stopService() {};
	virtual void exitService() {};
	virtual void destroyService();

private:
	void destroySerializeObjectInstances();

// engine helpers
protected:
	ObjectService();
public:
	static Service *newService();
	static ObjectService *getService() { return( ( ObjectService * )ServiceManager::getInstance().getService( "ObjectService" ) ); };

private:
	// serializable objects
	rfc_strmap *mapObjectTypeIdToSerializeObject;
};

/*#########################################################################*/
/*#########################################################################*/

class Object
{
protected:
	Object() {};
	Logger logger;
	String instance;

private:
	// used when attached to instance
	String loggerName;

public:
	virtual const char *getClass() = 0;
	virtual void serialize( SerializeObject& so );
	virtual void deserialize( Object *parent , SerializeObject& so );
	virtual String getPK();

public:
	SerializeObject *getSerializeObject();
	void attachLogger();
	void setLoggerName( String name );
	Logger& getLogger() { return( logger ); };
	const char *getLoggerName();
	void log( const char *prompt , Logger::LogLevel p_logLevel );

	static void serialize( Object *o , SerializeObject& so );
	static void deserialize( Object *parent , Object *o , SerializeObject& so );
	static Object *createObject( const char *className );

	const char *getInstance();
	void setInstance( const char *instance );
};

/*#########################################################################*/
/*#########################################################################*/

class ObjectField;

class SerializeObject : public Object
{
	// read single item
	typedef union {
		void *V;

		char charV;
		int intV;
		float floatV;
		char *stringV;
		Object *objectV;

		int *intVL;
		float *floatVL;
		char **stringVL;
		ClassList<Object> *objectVL;
	} FieldData;

public:
	// Object interface
	virtual const char *getClass();
	virtual void serialize( SerializeObject& so );
	virtual void deserialize( Object *parent , SerializeObject& so );
	virtual String getPK();

	static Object *onCreate( const char *className ) { return( new SerializeObject ); };
	static void createSerializeObject();
	static SerializeObject *getSerializeObject();

public:
	SerializeObject();
	SerializeObject( Object *o );
	SerializeObject( const char *className );
	~SerializeObject();

	void setFactoryMethod( Object *( *pfnCreateObject )( const char *className ) );
	Object *createObject();
	const char *getObjectClass();
	const char *getEffectiveObjectClass();
	void setEffectiveObjectClass( const char *className );
	void clearData();

	String getDataStringNameToValue();
	String getDataStringIdToValue();

	static const char *readFromString( const char *p , SerializeObject *so , SerializeObject *soSrc , Object **pv );

	ObjectField *addFieldChar( const char *name , int id = 0 );
	ObjectField *addFieldInt( const char *name , int id = 0 );
	ObjectField *addFieldIntList( const char *name , int id = 0 );
	ObjectField *addFieldFloat( const char *name , int id = 0 );
	ObjectField *addFieldFloatList( const char *name , int id = 0 );
	ObjectField *addFieldFloatTwoIndexArray( const char *name , int id = 0 );
	ObjectField *addFieldString( const char *name , int id = 0 );
	ObjectField *addFieldStringList( const char *name , int id = 0 );
	ObjectField *addFieldObject( const char *name , SerializeObject *so , int id = 0 );
	ObjectField *addFieldObjectList( const char *name , SerializeObject *so , int id = 0 );
	ObjectField *addFieldBool( const char *name , int id = 0 );

	ObjectField *getFieldById( int id );
	ObjectField *getFieldById( int id , SerializeObject *so );
	ObjectField *getFieldByName( const char *name );

	char getPropChar( const char *p_field );
	int getPropInt( const char *p_field );
	int *getPropIntList( const char *p_field , int *n , bool clearOwn );
	const char *getPropString( const char *p_field , bool clearOwn );
	const char **getPropStringList( const char *p_field , int *n , bool clearOwn );
	float getPropFloat( const char *p_field );
	float *getPropFloatList( const char *p_field , int *n , bool clearOwn );
	void getPropFloatList( FlatList<float>& p_val , const char *p_field , bool clearOwn );
	Object *getPropObject( const char *p_field , bool clearOwn );
	const FlatList<Object *> *getPropObjectList( const char *p_field );
	void getPropObjectList( ClassList<Object>& p_val , const char *p_field , bool clearOwn );
	bool getPropBool( const char *p_field );
	TwoIndexClassArray<Object> *getPropTwoIndexClassArray( const char *p_field );
	TwoIndexArray<float> *getPropFloatTwoIndexArray( const char *p_field );

	void setPropChar( char p_val , const char *p_field );
	void setPropInt( int p_val , const char *p_field );
	void setPropIntList( int *p_val , int n , const char *p_field , bool setOwn );
	void setPropIntList( FlatList<int>& p_val , const char *p_field );
	void setPropIntListFromStructList( void *p_val , int n , int sizeStruct , int offset , const char *p_field );
	void setPropIntListFromStructPtrList( void ** p_val , int n , int sizeStruct , int offset , const char *p_field );
	void setPropString( const char *p_val , const char *p_field , bool setOwn );
	void setPropStringList( const char **p_val , int n , const char *p_field , bool setOwn );
	void setPropFloat( float p_val , const char *p_field );
	void setPropFloatList( float *p_val , int n , const char *p_field , bool setOwn );
	void setPropFloatListFromStructList( void *p_val , int n , int sizeStruct , int offset , const char *p_field );
	void setPropFloatListFromStructPtrList( void **p_val , int n , int sizeStruct , int offset , const char *p_field );
	void setPropFloatList( FlatList<float>& p_val , const char *p_field );
	void setPropObject( Object *p_val , const char *p_field , bool setOwn );
	void setPropObject( Object& p_val , const char *p_field );
	void setPropObjectList( ClassList<Object>& p_val , const char *p_field , bool setOwn );
	void setPropBool( bool p_val , const char *p_field );
	void setPropFloatTwoIndexArray( TwoIndexArray<float> *p_val , const char *p_field );

	void ref();
	void deref();

private:
	ObjectField *addField( int id , const char *name , char type );
	ObjectField *addFieldList( int id , const char *name , char type );
	ObjectField *addField( ObjectField *field , int id , const char *name );

	void init();
	void fillMapString( String& s , bool useName , int level );
	
	static const char *skipSpaces( const char *p );
	static const char *skipFieldData( const char *p );
	static const char *skipString( const char *p );
	static const char *skipBlock( const char *p );
	static const char *skipListDelimiter( const char *p , bool *endOfList );
	
	int readListCount( const char *p );
	const char *readFieldData( Object *o , const char *p , ObjectField *field , ObjectField *fieldSrc , FieldData *data , int *pn );
	const char *readFieldDataList( Object *o , const char *p , ObjectField *field , ObjectField *fieldSrc , FieldData *data , int *pn );
	static const char *readString( const char *p , bool pAlloc , char **pvAlloc , char *pvNoAlloc );
	const char *readObject( Object *parent , const char *p , Object **pv , SerializeObject *so , SerializeObject *soSrc );
	const char *readChar( const char *p , char *p_v );
	const char *readInt( const char *p , int *p_v );
	const char *readFloat( const char *p , float *p_v );
	void setFieldData( ObjectField *field , void *dataValue /* FieldData* */, int dataListCount );

	void addInt( String& s , int v );
	void addFloat( String& s , float v );
	void addCharEscaped( String& s , char v );
	void addStringEscaped( String& s , const char *p );

private:
	String objectClass;
	String effectiveObjectClass;
	Object *( *pfnCreateObject )( const char *className );

	String data;
	rfc_ptrmap *mapIdToField;
	rfc_strmap *mapNameToField;
	ClassList<ObjectField> fieldList;
	int refCount;
};

/*#########################################################################*/
/*#########################################################################*/

#endif	// INCLUDE_AH_OBJECTBASE_H
