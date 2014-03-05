#ifndef	INCLUDE_AH_OBJECTS_IMPL_H
#define INCLUDE_AH_OBJECTS_IMPL_H

/*#########################################################################*/
/*#########################################################################*/

#include <ah_objects.h>

class ObjectField;

#define TP_CHAR			'c'
#define TP_INT			'd'
#define TP_STRING		's'
#define TP_FLOAT		'f'
#define TP_OBJECT		'o'

#define TP_INTLIST		'D'
#define TP_STRINGLIST	'S'
#define TP_FLOATLIST	'F'
#define TP_OBJECTLIST	'O'

/*#########################################################################*/
/*#########################################################################*/

class ObjectField : public Object
{
public:
	// Object interface
	virtual const char *getClass();
	virtual void serialize( SerializeObject& so );
	virtual void deserialize( Object *parent , SerializeObject& so );
	virtual String getPK();

	static Object *onCreate( const char *className ) { return( new ObjectField ); };
	static void createSerializeObject();
	static SerializeObject *getSerializeObject();

public:
	ObjectField();
	ObjectField( int p_id , const char *p_name , char p_type , bool isList );
	ObjectField( int p_id , const char *p_name , SerializeObject *p_so , bool isList );
	~ObjectField();
	void init();

	bool isChar();
	bool isInt();
	bool isFloat();
	bool isString();
	bool isObject();
	bool isList();
	char getBaseType();

	int getId();
	const char *getName();
	SerializeObject *getFieldSerializeObject();
	void attach( SerializeObject *so );

	char getChar();
	int	getInt();
	float getFloat();
	const char *getString( bool clearOwn );
	Object *getObject( bool clearOwn );

	int	*getIntList( int *p_size , bool clearOwn );
	float *getFloatList( int *p_size , bool clearOwn );
	const char **getStringList( int *p_size , bool clearOwn );
	FlatList<Object *> *getObjectList();

	void setChar( char v );
	void setInt( int v );
	void setFloat( float v );
	void setString( const char *v , bool setOwn );
	void setObject( Object *v , bool setOwn );

	void setIntList( int *v , int p_size , bool setOwn );
	void setFloatList( float *v , int p_size , bool setOwn );
	void setStringList( const char **v , int p_size , bool setOwn );
	void setObjectList( ClassList<Object> *v , bool setOwn );
	void clearData( bool pFree = true );

private:
	int id;
	String name;
	char type;
	SerializeObject *soField;
	SerializeObject *soFieldSrc;
	bool isOwnData;

	// value
	union 
	{
		void *value;

		char charValue;
		int intValue;
		int *intListValue;
		float floatValue;
		float *floatListValue;
		const char *stringValue;
		const char **stringListValue;
		Object *objectValue;
		FlatList<Object *> *objectListValue;
	} value;

	int sizeOfList; // for all lists, except object list
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_OBJECTS_IMPL_H
