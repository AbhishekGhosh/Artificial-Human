#include <ah_platform.h>
#include <ah_services_impl.h>

/*#########################################################################*/
/*#########################################################################*/

// class LogSettings : public Object
LogSettings::LogSettings() {
}

LogSettings::~LogSettings() {
	objectData.destroy();
	serviceData.destroy();
	customData.destroy();
}

void LogSettings::load( Xml config ) {
	// read
	defaultSettings.configure( config , "DEBUG" , "" );
	readLevels( config , "objectLogLevel" , objectData , defaultObjectSettings );
	readLevels( config , "serviceLogLevel" , serviceData , defaultServiceSettings );
	readLevels( config , "customLogLevel" , customData , defaultCustomSettings );
}

void LogSettings::readLevels( Xml config , const char *listName , MapStringToClass<LogSettingsItem>& map , LogSettingsItem& p_defaultSettings ) {
	map.destroy();

	Xml list = config.getChildNode( listName );
	if( !list.exists() )
		return;

	// default settings
	String level = list.getAttribute( "level" );
	String logger = list.getAttribute( "logger" , "" );
	p_defaultSettings.configure( list , level , logger );

	// read list
	for( Xml item = list.getFirstChild( "class" ); item.exists(); item = item.getNextChild( "class" ) ) {
		String name = item.getAttribute( "name" );

		LogSettingsItem *lsi = new LogSettingsItem;
		lsi -> setName( name );

		String level = item.getAttribute( "level" );
		String logger = item.getAttribute( "logger" , "" );
		lsi -> configure( item , level , logger );
		map.add( name , lsi );
	}
}

LogSettingsItem *LogSettings::getDefaultSettings() {
	return( &defaultSettings );
}

LogSettingsItem *LogSettings::getCustomDefaultSettings() {
	return( &defaultCustomSettings );
}

LogSettingsItem *LogSettings::getObjectSettings( const char *className , const char *instance ) {
	// get class-based logger
	LogSettingsItem *item = objectData.get( className );
	if( item == NULL )
		return( &defaultObjectSettings );

	// get object settings
	return( item -> getSettings( instance ) );
}

LogSettingsItem *LogSettings::getServiceSettings( const char *className ) {
	// get class-based logger
	LogSettingsItem *item = serviceData.get( className );
	if( item == NULL )
		return( &defaultServiceSettings );

	// get object settings
	return( item );
}

LogSettingsItem *LogSettings::getCustomSettings( const char *loggerName ) {
	// get class-based logger - do not use default
	LogSettingsItem *item = customData.get( loggerName );
	return( item );
}

void LogSettings::attachLogManagers( LogDispatcher *dispatcher ) {
	defaultSettings.attachLogManagers( dispatcher );
	attachLogManagers( dispatcher , objectData , defaultObjectSettings );
	attachLogManagers( dispatcher , serviceData , defaultServiceSettings );
	attachLogManagers( dispatcher , customData , defaultCustomSettings );
}

void LogSettings::attachLogManagers( LogDispatcher *dispatcher , MapStringToClass<LogSettingsItem>& map , LogSettingsItem& settings ) {
	settings.attachLogManagers( dispatcher );
	for( int k = 0; k < map.count(); k++ ) {
		LogSettingsItem *item = map.getClassByIndex( k );
		item -> attachLogManagers( dispatcher );
	}
}

