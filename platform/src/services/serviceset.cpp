#include <ah_platform.h>
#include <ah_services_impl.h>

/*#########################################################################*/
/*#########################################################################*/

ServiceSet::ServiceSet() {
}

ServiceSet::~ServiceSet() {
}

void ServiceSet::create( String p_name ) {
	name = p_name;
	logger.attachRoot();
}

bool ServiceSet::isInitialized() {
	return( state.getState() == ServiceState::AH_INITIALIZED || state.getState() == ServiceState::AH_RUNNING );
}

bool ServiceSet::isRunning() {
	return( state.getState() == ServiceState::AH_RUNNING );
}

bool ServiceSet::isCreated() {
	return( state.getState() != ServiceState::AH_COLD );
}

void ServiceSet::addService( Service *svc ) {
	serviceList.add( svc );
	svc -> attachLogger();
}

Service *ServiceSet::findServiceByName( String name ) {
	for( int k = 0; k < serviceList.count(); k++ ) {
		Service *svc = serviceList.get( k );
		if( name.equals( svc -> getServiceName() ) )
			return( svc );
	}
	return( NULL );
}

void ServiceSet::createServices( MapStringToClass<Service>& services ) {
	state.setState( ServiceState::AH_CREATING );
	logger.logInfo( "createServices: create " + name + " services ..." );

	try {
		// attach loggers and run creation event
		for( int k = 0; k < serviceList.count(); k++ ) {
			Service *svc = serviceList.get( k );

			// check creation is blocked
			if( !svc -> isCreate )
				continue;

			// ensure created service is available via manager
			services.add( svc -> getServiceName() , svc );

			// setup service logging
			svc -> attachLogger();

			// internal data creation
			svc -> state.setState( ServiceState::AH_CREATING );
			logger.logInfo( String( "createServices: create service - name=" ) + svc -> getServiceName() + String( "..." ) );
			svc -> createService();
			logger.logInfo( String( "createServices: create service - name=" ) + svc -> getServiceName() + String( " - done" ) );
			svc -> state.setState( ServiceState::AH_CREATED );

			// no cross-service links are permitted on creation
			services.remove( svc -> getServiceName() );
		}

		logger.logInfo( "createServices: create " + name + " services - done" );
		state.setState( ServiceState::AH_CREATED );
		return;
	}
	catch( RuntimeException& e ) {
		logger.printStack( e );
	}
	catch( ... ) {
		logger.logError( "createServices: unknown exception when creating " + name + " services" );
	}

	// in case of failure
	for( int k = 0; k < serviceList.count(); k++ ) {
		Service *svc = serviceList.get( k );
		services.remove( svc -> getServiceName() );
	}
	ASSERTFAILED( "createServices: fail to create " + name + " services" );
}

void ServiceSet::initServices( MapStringToClass<Service>& services ) {
	logger.logInfo( "initServices: init " + name + " services ..." );
	state.setState( ServiceState::AH_INITIALIZING );

	// allow cross-links
	for( int k = 0; k < serviceList.count(); k++ ) {
		Service *svc = serviceList.get( k );

		// add created service to cross-link map
		if( svc -> state.getState() != ServiceState::AH_CREATED )
			continue;

		services.add( svc -> getServiceName() , svc );
	}

	// init services
	for( int k = 0; k < serviceList.count(); k++ ) {
		Service *svc = serviceList.get( k );

		// check initialization is blocked
		if( !svc -> isInit )
			continue;

		// call service init procedure
		logger.logInfo( String( "initServices: init service name=" ) + svc -> getServiceName() + String( "..." ) );
		svc -> state.setState( ServiceState::AH_INITIALIZING );
		svc -> initService(); 
		svc -> state.setState( ServiceState::AH_INITIALIZED );
		logger.logInfo( String( "initServices: init service name=" ) + svc -> getServiceName() + String( " - done" ) );
	}

	state.setState( ServiceState::AH_INITIALIZED );
	logger.logInfo( "initServices: init " + name + " services - done" );
}

void ServiceSet::runServices() {
	logger.logInfo( "runServices: run " + name + " services..." );
	state.setState( ServiceState::AH_RUNNING );

	// run all instances
	for( int k = 0; k < serviceList.count(); k++ ) {
		Service *svc = serviceList.get( k );

		// check run is blocked
		if( !svc -> isRun )
			continue;

		logger.logInfo( String( "runServices: run service name=" ) + svc -> getServiceName() + String( "..." ) );
		svc -> state.setState( ServiceState::AH_RUNNING );
		svc -> runService();
		logger.logInfo( String( "runServices: run service name=" ) + svc -> getServiceName() + String( " - done" ) );
	}

	logger.logInfo( "runServices: run " + name + " services - done" );
}

void ServiceSet::stopServices() {
	if( !isRunning() )
		return;

	logger.logInfo( "stopServices: stop " + name + " services..." );

	// in back order
	for( int k = serviceList.count() - 1; k >= 0 ; k-- ) {
		// exit sepately from each other
		Service *svc = NULL;
		try {
			svc = serviceList.get( k );
			if( svc -> state.getState() != ServiceState::AH_RUNNING )
				continue;

			logger.logInfo( String( "stopServices: stop service name=" ) + svc -> getServiceName() + String( "..." ) );
			svc -> stopService();
			svc -> state.setState( ServiceState::AH_STOPPED );
			logger.logInfo( String( "stopServices: stop service name=" ) + svc -> getServiceName() + String( " - done" ) );
		}
		catch( RuntimeException& e ) {
			logger.printStack( e );
			logger.logInfo( String( "stopServices: exception while stopping service name=" ) + svc -> getServiceName() );
		}
		catch( ... ) {
			logger.logInfo( String( "stopServices: unknown exception while stopping service name=" ) + svc -> getServiceName() );
		}
	}
	
	state.setState( ServiceState::AH_STOPPED );
	logger.logInfo( "stopServices: stop " + name + " services - done" );
}

void ServiceSet::exitServices() {
	if( !state.readyForExit() )
		return;

	logger.logInfo( "exitServices: exit " + name + " services ..." );
	state.setState( ServiceState::AH_EXITING );

	// in back order
	bool everyExited = true;
	for( int k = serviceList.count() - 1; k >= 0 ; k-- ) {
		// exit sepately from each other
		Service *svc = NULL;
		try {
			svc = serviceList.get( k );
			if( !svc -> state.readyForExit() ) {
				if( svc -> state.exitRequired() )
					everyExited = false;
				continue;
			}

			logger.logInfo( String( "exitServices: exit service name=" ) + svc -> getServiceName() + String( "..." ) );
			svc -> state.setState( ServiceState::AH_EXITING );
			svc -> exitService();
			svc -> state.setState( ServiceState::AH_CREATED );
			logger.logInfo( String( "exitServices: exit service name=" ) + svc -> getServiceName() + String( " - done" ) );
		}
		catch( RuntimeException& e ) {
			everyExited = false;
			logger.printStack( e );
			logger.logInfo( String( "exitServices: exception while exiting service name=" ) + svc -> getServiceName() );
		}
		catch( ... ) {
			everyExited = false;
			logger.logInfo( String( "exitServices: unknown exception while exiting service name=" ) + svc -> getServiceName() );
		}
	}

	if( everyExited ) {
		state.setState( ServiceState::AH_CREATED );
		logger.logInfo( "exitServices: exit " + name + " services - done" );
	}
	else {
		logger.logInfo( "exitServices: exit " + name + " services - not fully completed" );
	}
}

void ServiceSet::destroyServices( MapStringToClass<Service>& services ) {
	if( !isCreated() )
		return;

	logger.logInfo( "destroyServices: destroy " + name + " services..." );

	// in back order
	for( int k = serviceList.count() - 1; k >= 0; k-- ) {
		// destroy sepately from each other
		Service *svc = NULL;
		try {
			svc = serviceList.get( k );

			// clear in service map
			services.set( svc -> getServiceName() , NULL );
			if( svc -> state.getState() == ServiceState::AH_COLD )
				continue;

			String name = svc -> getServiceName();
			logger.logInfo( String( "destroyServices: destroy service name= " ) + name + String( "..." ) );
			svc -> destroyService();
			svc -> state.setState( ServiceState::AH_COLD );
			logger.logInfo( String( "destroyServices: destroy service name=" ) + name + String( " - done" ) );
		}
		catch( RuntimeException& e ) {
			logger.printStack( e );
			logger.logInfo( String( "destroyServices: exception while destroying service name=" ) + svc -> getServiceName() );
		}
		catch( ... ) {
			logger.printStack();
			logger.logInfo( String( "destroyServices: unknown exception while destroying service name=" ) + svc -> getServiceName() );
		}
	}

	serviceList.clear();
	state.setState( ServiceState::AH_COLD );
	logger.logInfo( "destroyServices: destroy " + name + " services - done" );
}

