#include <ah_platform.h>
#include <ah_services_impl.h>

/*#########################################################################*/
/*#########################################################################*/

// class LogSettingsItem
LogSettingsItem::LogSettingsItem() {
	logManager = NULL;
	level = Logger::LogLevelDebug;
}

LogSettingsItem::~LogSettingsItem() {
}

void LogSettingsItem::setLogManager( LogManager *manager ) {
	logManager = manager;
}

Logger::LogLevel LogSettingsItem::getLevel() {
	return( level );
}

void LogSettingsItem::setName( String p_name ) {
	name = p_name;
}

void LogSettingsItem::setLevel( Logger::LogLevel p_level ) {
	level = p_level;
}

void LogSettingsItem::setLevelByName( String p_level ) {
	p_level = p_level.toUpper();
	
	if( p_level.equals( "ERROR" ) )
		level = Logger::LogLevelError;
	else if( p_level.equals( "INFO" ) )
		level = Logger::LogLevelInfo;
	else if( p_level.equals( "DEBUG" ) )
		level = Logger::LogLevelDebug;
	else
		ASSERTFAILED( "Unknown log level=" + p_level );
}

Xml LogSettingsItem::getXml() {
	return( settings );
}

void LogSettingsItem::configure( Xml xml , String defaultLevel , String p_logger ) {
	settings = xml;
	logger = p_logger;
	setLevelByName( defaultLevel );

	// read exclude settings if any
	for( Xml item = xml.getFirstChild( "exclude" ); item.exists(); item = item.getNextChild( "exclude" ) ) {
		String value = item.getAttribute( "string" );
		excludeList.add( value , this );
	}

	// read instances if any
	for( Xml instance = xml.getFirstChild( "instance" ); instance.exists(); instance = instance.getNextChild( "instance" ) ) {
		String value = instance.getAttribute( "name" );

		LogSettingsItem *lsi = new LogSettingsItem;
		lsi -> name = name;
		lsi -> instance = value;

		String level = instance.getAttribute( "level" );
		String instanceLogger = instance.getAttribute( "logger" , "" );
		if( instanceLogger.isEmpty() )
			instanceLogger = logger;

		lsi -> configure( instance , level , logger );
		instanceSettings.add( value , lsi );
	}
}

bool LogSettingsItem::logDisabled( Logger::LogLevel p_level ) {
	return( p_level > level );
}

bool LogSettingsItem::isExcluded( const char *s ) {
	return( excludeList.getPartial( s ) != NULL );
}

LogSettingsItem *LogSettingsItem::getSettings( const char *instance ) {
	if( instance == NULL || *instance == 0 )
		return( this );

	LogSettingsItem *is = instanceSettings.get( instance );
	if( is == NULL )
		return( this );

	return( is );
}

LogManager *LogSettingsItem::getLogManager() {
	return( logManager );
}

void LogSettingsItem::attachLogManagers( LogDispatcher *dispatcher ) {
	attachOwnLogManager( dispatcher );
	for( int k = 0; k < instanceSettings.count(); k++ ) {
		LogSettingsItem *item = instanceSettings.getClassByIndex( k );
		item -> attachOwnLogManager( dispatcher );
	}
}

void LogSettingsItem::attachOwnLogManager( LogDispatcher *dispatcher ) {
	if( logger.isEmpty() )
		logManager = dispatcher -> getDefaultLogManager();
	else
		logManager = dispatcher -> getLogManager( logger );
}
