#include <ah_platform.h>
#include <ah_services_impl.h>

#include <windows.h>

ServiceManager *ServiceManager::instance = NULL;

/*#########################################################################*/
/*#########################################################################*/

static void UnhandledExceptionTranslator( unsigned int exceptionCode , struct _EXCEPTION_POINTERS *exceptionInfo ) {
	throw RuntimeException( exceptionCode , 1 , exceptionInfo -> ExceptionRecord -> ExceptionAddress );
}

/*#########################################################################*/
/*#########################################################################*/

bool ServiceManager::configureDefault( String etcpath ) {
	try {
		EnvService *es = ( EnvService * )findServiceByName( "EnvService" );
		ASSERTMSG( es != NULL , "EnvService was not added to the list of services" );
		es -> configureAll( etcpath );
		return( true );
	}
	catch( RuntimeException& e ) {
		logger.printStack( e );
	}
	catch( ... ) {
		logger.logError( "createServices: unknown exception" );
	}

	return( false );
}

void ServiceManager::configureLifecycle( Xml config ) {
}

void ServiceManager::configureLogging( Xml config ) {
	logDispatcher -> configure( config );
}

void ServiceManager::setRootLogLevel( Logger::LogLevel p_logLevel ) {
	LogSettingsItem *rootSettings = LogDispatcher::getRootSettings();
	rootSettings -> setLevel( p_logLevel );
}

void ServiceManager::addService( Service *svc ) {
	serviceSetOther.addService( svc );
}

Service *ServiceManager::findServiceByName( String name ) {
	return( serviceSetPlatform.findServiceByName( name ) );
}

void ServiceManager::addPlatformService( Service *svc ) {
	svc -> isPlatformService = true;
	serviceSetPlatform.addService( svc );
}

void ServiceManager::addPlatformServices() {
	addPlatformService( ThreadService::newService() );
	addPlatformService( EnvService::newService() );
	addPlatformService( ObjectService::newService() );
	addPlatformService( MessagingService::newService() );
	addPlatformService( MediaService::newService() );
	addPlatformService( PersistingService::newService() );
	addPlatformService( MathService::newService() );
	addPlatformService( SchedulerService::newService() );
	addPlatformService( StatService::newService() );
	addPlatformService( TestService::newService() );
}

Service *ServiceManager::getService( const char *serviceName ) {
	Service *svc = services.get( serviceName );
	ASSERTMSG( svc != NULL , "service " + String( serviceName ) + " is not available" );
	return( svc );
}

ClassList<Service>& ServiceManager::getPlatformServices() {
	return( serviceSetPlatform.getServiceList() );
}

ClassList<Service>& ServiceManager::getOtherServices() {
	return( serviceSetOther.getServiceList() );
}

void ServiceManager::getServices( ClassList<Service>& list ) {
	ClassList<Service>& listsetplatform = serviceSetPlatform.getServiceList();
	list.add( &listsetplatform );
	ClassList<Service>& listsetother = serviceSetOther.getServiceList();
	list.add( &listsetother );
}

void ServiceManager::waitRunDefault() {
	ThreadService *ts = ThreadService::getService();
	ASSERTMSG( ts != NULL , "Thread service is not created" );

	ts -> waitExitSignal();
	serviceSetOther.stopServices();
	serviceSetPlatform.stopServices();
	ts -> waitAllThreads();
}

LogDispatcher *ServiceManager::getLogDispatcher() {
	return( logDispatcher );
}

bool ServiceManager::canStartThread() {
	return( serviceSetPlatform.isInitialized() );
}

bool ServiceManager::isRunning() {
	return( serviceSetPlatform.isRunning() || serviceSetOther.isRunning() );
}

bool ServiceManager::isCreated() {
	return( serviceSetPlatform.isCreated() || serviceSetOther.isCreated() );
}

/*#########################################################################*/
/*#########################################################################*/

// constructor
ServiceManager::ServiceManager() {
	ServiceManager::instance = this;

	logDispatcher = new LogDispatcher();
	logger.attachRoot();

	// enable exception handling
	rfc_thr_initstackhandle();
	::_set_se_translator( UnhandledExceptionTranslator );

	// create service sets
	serviceSetPlatform.create( "platform" );
	serviceSetOther.create( "other" );
}

ServiceManager::~ServiceManager() {
	delete logDispatcher;

	rfc_thr_exitstackhandle();
}

ServiceManager& ServiceManager::getInstance() {
	return( *ServiceManager::instance );
}

void ServiceManager::logStart( Xml configLogging ) {
	// read configuration file
	ASSERTMSG( configLogging.exists() , "logStart: logging is not configured, empty configuration" );

	// open file
	logDispatcher -> configure( configLogging );
	if( !logDispatcher -> start() )
		throw RuntimeError( "logStart: cannot initialize logging - unknown reason" );

	logger.logInfo( "logStart: LOGGING STARTED" );
}

void ServiceManager::logStop() {
	// stop logging
	logger.logInfo( "logStop: LOGGING STOPPED" );
	logDispatcher -> stop();
}

void ServiceManager::execute() {
	try {
		logger.logInfo( "--------------------" );
		logger.logInfo( "execute: STARTING PLATFORM ..." );
		logger.logInfo( "--------------------" );

		// start platform services
		serviceSetPlatform.createServices( services );
		serviceSetPlatform.initServices( services );
		logDispatcher -> start();
		serviceSetPlatform.runServices();

		// wait for completion
		logger.logInfo( "--------------------" );
		logger.logInfo( "execute: RUN PLATFORM TARGET ..." );
		logger.logInfo( "--------------------" );
		serviceSetOther.createServices( services );
		serviceSetOther.initServices( services );
		serviceSetOther.runServices();

		logger.logInfo( "--------------------" );
		logger.logInfo( "execute: STARTED, WAITING FOR COMPLETION..." );
		logger.logInfo( "-------------------------------------------" );
		logDispatcher -> enableAsyncMode();
		waitRunDefault();
	}
	catch( RuntimeException& e ) {
		logger.logInfo( "----------------------------------" );
		logger.logInfo( "execute: STARTUP/RUNTIME EXCEPTION" );
		logger.logInfo( "----------------------------------" );

		logger.printStack( e );
	}

	// cleanup
	try {
		logger.logInfo( "--------------------" );
		logger.logInfo( "execute: EXITING..." );
		logger.logInfo( "-------------------" );

		serviceSetOther.stopServices();
		serviceSetOther.exitServices();

		serviceSetPlatform.stopServices();
		logDispatcher -> disableAsyncMode();
		serviceSetPlatform.exitServices();

		serviceSetOther.destroyServices( services );
		logDispatcher -> stop();
		serviceSetPlatform.destroyServices( services );
		services.clear();

		logger.logInfo( "----------------" );
		logger.logInfo( "execute: STOPPED" );
		logger.logInfo( "----------------" );
	}
	catch( RuntimeException& e ) {
		logger.logInfo( "------------------------------" );
		logger.logInfo( "execute: EXCEPTION ON SHUTDOWN" );
		logger.logInfo( "------------------------------" );
		logger.printStack( e );
	}
}

void ServiceManager::stop() {
	serviceSetOther.stopServices();
	serviceSetPlatform.stopServices();
}
