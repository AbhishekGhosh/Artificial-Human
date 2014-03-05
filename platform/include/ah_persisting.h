#ifndef	INCLUDE_AH_PERSISTING_H
#define INCLUDE_AH_PERSISTING_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_services.h"
#include "ah_objects.h"

class PersistingService;
class PersistingFile;
class PersistingCursor;

/*#########################################################################*/
/*#########################################################################*/

// manages persistence of knowledge
class PersistingService : public Service
{
// interface
public:
	PersistingFile& getFile( SerializeObject *so );
	void save( Object *o , const char *p_file );
	bool load( Object *o , const char *p_file );
	void remove( Object *o );
	void update( Object *o );
	void insert( Object *o );
	void insert( Object **o , int n );
	bool select( Object *o , const char *pk );
	PersistingCursor& scan( SerializeObject *so );

public:
	virtual const char *getServiceName() { return( "PersistingService" ); };
	virtual void configureService( Xml config ) {};
	virtual void createService() {};
	virtual void initService() {};
	virtual void runService() {};
	virtual void stopService() {};
	virtual void exitService() {};
	virtual void destroyService() {};

// engine helpers
protected:
	PersistingService() {};
public:
	static Service *newService();
	static PersistingService *getService() { return( ( PersistingService * )ServiceManager::getInstance().getService( "PersistingService" ) ); };
};

/*#########################################################################*/
/*#########################################################################*/

class PersistingFile
{
public:
	virtual void open() = 0;
	virtual void close() = 0;

	virtual void truncate() = 0;
	virtual PersistingCursor& scan() = 0;

	virtual void remove( Object *o ) = 0;
	virtual void update( Object *o ) = 0;
	virtual void insert( Object *o , const char *pk ) = 0;
	virtual void insert( Object **o , int n ) = 0;
	virtual bool drop( Object *o ) = 0;
	virtual bool select( Object *o , const char *pk ) = 0;
};

/*#########################################################################*/
/*#########################################################################*/

class PersistingCursor
{
public:
	virtual bool next() = 0;
	virtual const char *getDataPK() = 0;
	virtual void getData( Object *o ) = 0;
};

/*#########################################################################*/
/*#########################################################################*/

#endif	// INCLUDE_AH_PERSISTING_H
