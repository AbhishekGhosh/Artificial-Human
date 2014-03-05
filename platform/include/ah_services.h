#ifndef	INCLUDE_AH_SERVICES_H
#define INCLUDE_AH_SERVICES_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_basetypes.h"
#include "ah_templates.h"
#include "ah_logging.h"

class ServiceManager;
class Service;
class PropertyContainer;

/*#########################################################################*/
/*#########################################################################*/

class LogDispatcher;

class ServiceState {
public:
	typedef enum {
		AH_COLD = 1 ,			// enabled: create->CREATING->CREATED
		AH_CREATING = 2 ,		// enabled: create->CREATED, destroy->COLD
		AH_CREATED = 3 ,		// enabled: init->INITIALIZING->INITIALIZED, destroy->COLD
		AH_INITIALIZING = 4 ,	// enabled: init->INITIALIZED, exit->EXITING->CREATED
		AH_INITIALIZED = 5 ,	// enabled: run->RUNNING, exit->EXITING->CREATED
		AH_RUNNING = 6 ,		// enabled: stop->STOPPED
		AH_STOPPED = 7 ,		// enabled: exit->EXITING->CREATED
		AH_EXITING = 8			// enabled: exit->CREATED
	} ServiceStateType;

private:
	ServiceStateType state;

public:
	ServiceState() { state = AH_COLD; };
	ServiceStateType getState() { return( state ); };

	void setState( ServiceStateType value ) {
		bool checkTransition = false;
		if( state == AH_COLD )
			checkTransition = ( value == AH_CREATING );
		else
		if( state == AH_CREATING )
			checkTransition = ( value == AH_CREATED || value == AH_COLD );
		else
		if( state == AH_CREATED )
			checkTransition = ( value == AH_INITIALIZING || value == AH_COLD );
		else
		if( state == AH_INITIALIZING )
			checkTransition = ( value == AH_INITIALIZED || value == AH_EXITING );
		else
		if( state == AH_INITIALIZED )
			checkTransition = ( value == AH_RUNNING || value == AH_EXITING );
		else
		if( state == AH_RUNNING )
			checkTransition = ( value == AH_STOPPED );
		else
		if( state == AH_STOPPED )
			checkTransition = ( value == AH_EXITING );
		else
		if( state == AH_EXITING )
			checkTransition = ( value == AH_CREATED );

		ASSERTMSG( checkTransition , String( "Invalid state transition: stateFrom=" )  + state + ", stateTo=" + value );
		state = value;
	};

	bool readyForExit() {
		if( state == AH_INITIALIZING || state == AH_INITIALIZED || state == AH_STOPPED || state == AH_EXITING )
			return( true );
		return( false );
	};

	bool exitRequired() {
		if( state == AH_COLD || state == AH_CREATING || state == AH_CREATED )
			return( false );
		return( true );
	};
};

/*#########################################################################*/
/*#########################################################################*/

class ServiceSet {
public:
	ServiceSet();
	~ServiceSet();

	void create( String name );
	void addService( Service *src );
	Service *findServiceByName( String name );	
	ClassList<Service>& getServiceList() { return( serviceList ); };

	// operations for all added and optionally configured services
	void createServices( MapStringToClass<Service>& services );		// create/set internal items according to configuration if any
	void initServices( MapStringToClass<Service>& services );		// create contextual links among services, can start threads here
	void runServices();			// enable to generate events inside service, can start threads here
	void stopServices();		// stop generating events, wait for completion of all threads created
	void exitServices();		// wait for completion of all threads created, cleanup without dropping interfaces, last chance to use links between services
	void destroyServices( MapStringToClass<Service>& services );		// drop internal data

	bool isInitialized();
	bool isRunning();
	bool isCreated();

private:
	String name;
	Logger logger;
	ServiceState state;
	ClassList<Service> serviceList;
};
	
/*#########################################################################*/
/*#########################################################################*/

class ServiceManager {
public:
	// add services
	void addService( Service *src );
	void addPlatformServices();
	Service *getService( const char *serviceName );
	void getServices( ClassList<Service>& list );
	ClassList<Service>& getPlatformServices();
	ClassList<Service>& getOtherServices();

	// configuring
	bool configureDefault( String etcpath );
	void configureLifecycle( Xml config );
	void configureLogging( Xml config );

	// runtime operations
	void execute();
	void stop();

	bool isRunning();
	bool isCreated();
	void waitRunDefault();
	bool canStartThread();
	void setRootLogLevel( Logger::LogLevel p_logLevel );
	LogDispatcher *getLogDispatcher();

public:
	ServiceManager();
	~ServiceManager();
	static ServiceManager& getInstance();

// class internals
private:
	// log manager
	void logStart( Xml configLogging );
	void logStop();

	void addPlatformService( Service *src );
	Service *findServiceByName( String name );	

// data
private:
	static ServiceManager *instance;

	String logLineFormat;
	LogDispatcher *logDispatcher;
	Logger logger;

	// services
	ServiceSet serviceSetPlatform;
	ServiceSet serviceSetOther;
	MapStringToClass<Service> services;
};

/*#########################################################################*/
/*#########################################################################*/

class Service {
// interface plus engine-managed properties
public:
	Service() {
		isCreate = true;
		isInit = true;
		isRun = true;
	};

	virtual const char *getServiceName() = 0;
	virtual void configureService( Xml config ) = 0;
	virtual void createService() = 0;
	virtual void initService() = 0;
	virtual void runService() = 0;
	virtual void stopService() = 0;
	virtual void exitService() = 0;
	virtual void destroyService() = 0;

	// configuration
	ServiceState state;
	bool isPlatformService;
	bool isCreate;
	bool isInit;
	bool isRun;

	void setConfigMain( Xml p_config ) { configMain = p_config; };
	void setConfigService( Xml p_config ) { configService = p_config; };
	Xml getConfigMain() { return( configMain ); };
	Xml getConfigService() { return( configService ); };
	void configureLifecycle( Xml config );

	// logger
	void attachLogger() { logger.attachService( getServiceName() ); };
	Logger& getLogger() { return( logger ); };

protected:
	Logger logger;

private:
	// configuration
	Xml configMain;
	Xml configService;
	Xml configLogging;
};

/*#########################################################################*/
/*#########################################################################*/

class PropertyContainer {
private:
	MapStringToClass<Value> params;

public:
	PropertyContainer() {};
	~PropertyContainer() { params.destroy(); };

	void setInt( const char *key , int value ) { 
		ASSERTMSG( params.get( key ) == NULL , "Already used key=" + String( key ) );
		params.set( key , new Value( ( long )value ) ); 
	};
	int getInt( const char *key ) { 
		Value *pv = params.get( key ); 
		ASSERTMSG( pv != NULL , "Not found key=" + String( key ) );
		return( pv -> getLong() );
	}

	void setString( const char *key , const char *value ) { 
		ASSERTMSG( params.get( key ) == NULL , "Already used key=" + String( key ) );
		params.set( key , new Value( value ) ); 
	};
	const char *getString( const char *key ) { 
		Value *pv = params.get( key ); 
		ASSERTMSG( pv != NULL , "Not found key=" + String( key ) );
		return( pv -> getString() );
	}
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_SERVICES_H
