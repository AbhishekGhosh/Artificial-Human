#include <ah_platform.h>
#include <ah_services_impl.h>

// #############################################################################
// #############################################################################

LogSettingsItem *LogDispatcher::rootSettings = NULL;

// #############################################################################
// #############################################################################

LogDispatcher::LogDispatcher() {
	// create default
	configured = false;
	rootSettings = logSettings.getDefaultSettings();
	rootSettings -> setLogManager( &defaultLog );
}

LogDispatcher::~LogDispatcher() {
	logMap.destroy();
	LogDispatcher::rootSettings = NULL;
}

void LogDispatcher::configure( Xml config ) {
	// init log settings
	logSettings.load( config );

	// create and configure logging
	Xml xmlLoggers = config.getChildNode( "loggers" );
	for( Xml xmlChild = xmlLoggers.getFirstChild( "logger" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "logger" ) ) {
		String name = xmlChild.getAttribute( "name" );
		ASSERTMSG( logMap.get( name ) == NULL , "duplicate LogManager name=" + name );

		LogManager *manager = new LogManager();
		manager -> configure( xmlChild );
		logMap.add( name , manager );
	}

	logSettings.attachLogManagers( this );
	configured = true;
}

LogManager *LogDispatcher::getDefaultLogManager() {
	return( getLogManager( "default" ) );
}

LogManager *LogDispatcher::getLogManager( String name ) {
	LogManager *manager = logMap.get( name );
	ASSERTMSG( manager != NULL , "not found LogManager name=" + name );
	return( manager );
}

LogSettingsItem *LogDispatcher::getRootSettings() {
	return( rootSettings );
}

LogSettingsItem *LogDispatcher::getObjectLogSettings( const char *className , const char *classInstance ) {
	if( !configured )
		return( rootSettings );
	return( logSettings.getObjectSettings( className , classInstance ) );
}

LogSettingsItem  *LogDispatcher::getServiceLogSettings( const char *serviceName ) {
	if( !configured )
		return( rootSettings );
	return( logSettings.getServiceSettings( serviceName ) );
}

LogSettingsItem *LogDispatcher::getCustomLogSettings( const char *loggerName ) {
	if( !configured )
		return( rootSettings );
	return( logSettings.getCustomSettings( loggerName ) );
}

LogSettingsItem *LogDispatcher::getCustomDefaultLogSettings() {
	if( !configured )
		return( rootSettings );
	return( logSettings.getCustomDefaultSettings() );
}

bool LogDispatcher::start() {
	bool result = defaultLog.start();

	// start all log managers
	for( int k = 0; k < logMap.count(); k++ ) {
		LogManager *manager = logMap.getClassByIndex( k );
		if( !manager -> start() )
			result = false;
	}

	return( result );
}

void LogDispatcher::stop() {
	defaultLog.stop();

	// start all log managers
	for( int k = 0; k < logMap.count(); k++ ) {
		LogManager *manager = logMap.getClassByIndex( k );
		manager -> stop();
	}
}

void LogDispatcher::enableAsyncMode() {
	// set logging to configured mode
	for( int k = 0; k < logMap.count(); k++ ) {
		LogManager *manager = logMap.getClassByIndex( k );
		if( !manager -> getConfiguredSyncMode() )
			manager -> setSyncMode( false );
	}
}

void LogDispatcher::disableAsyncMode() {
	// set logging to sync mode
	for( int k = 0; k < logMap.count(); k++ ) {
		LogManager *manager = logMap.getClassByIndex( k );
		manager -> setSyncMode( true );
	}
}
