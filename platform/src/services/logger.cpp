#include <ah_platform.h>
#include <ah_services_impl.h>

/*#########################################################################*/
/*#########################################################################*/

// class Logger
// construction
Logger::Logger() {
	loggerName = NULL;
	settings = NULL;
}

Logger::~Logger() {
}

void Logger::attachRoot() {
	loggerName = ".";
	settings = LogDispatcher::getRootSettings();
}

void Logger::setLoggerName( const char *name ) {
	loggerName = name;
}

void Logger::attachService( const char *p_s , const char *p_loggerName ) {
	loggerName = ( p_loggerName != NULL && *p_loggerName != 0 )? p_loggerName : p_s;
	LogDispatcher *logDispatcher = ServiceManager::getInstance().getLogDispatcher();
	settings = logDispatcher -> getServiceLogSettings( p_s );
}

void Logger::attachObjectInstance( const char *className , const char *classInstance , const char *p_loggerName ) {
	loggerName = ( p_loggerName != NULL && *p_loggerName != 0 )? p_loggerName : ( ( classInstance == NULL || *classInstance == 0 )? className : classInstance );
	LogDispatcher *logDispatcher = ServiceManager::getInstance().getLogDispatcher();
	settings = logDispatcher -> getObjectLogSettings( className , classInstance );
}

void Logger::attachObject( const char *className , const char *p_loggerName ) {
	loggerName = ( p_loggerName != NULL && *p_loggerName != 0 )? p_loggerName : className;
	LogDispatcher *logDispatcher = ServiceManager::getInstance().getLogDispatcher();
	settings = logDispatcher -> getObjectLogSettings( className , NULL );
}

void Logger::attachCustom( const char *customName , const char *p_loggerName ) {
	loggerName = ( p_loggerName != NULL && *p_loggerName != 0 )? p_loggerName : customName;
	LogDispatcher *logDispatcher = ServiceManager::getInstance().getLogDispatcher();

	// attach custom only if set
	LogSettingsItem *settingsNew = logDispatcher -> getCustomLogSettings( customName );

	// set to default if none currently
	if( settingsNew == NULL )
		if( settings == NULL )
			settingsNew = logDispatcher -> getCustomDefaultLogSettings();

	if( settingsNew != NULL )
		settings = settingsNew;
}

// stack
void Logger::printStack() {
	rfc_threadstack *stack = rfc_thr_stackget( 1 );
	printStackInternal( stack , 0 , true );
	rfc_thr_stackfree( stack );
}

void Logger::printStack( rfc_threadstack *stack , int skipTop ) {
	printStackInternal( stack , skipTop , false );
}

void Logger::printStackInplace( rfc_threadstack *stack , int skipTop ) {
	printStackInternal( stack , skipTop , true );
}

void Logger::printStackInternal( rfc_threadstack *stack , int skipTop , bool printInplace ) {
	int startItem = rfc_thr_stackfulldepth( stack ) - 1;

	StringList lines;
	lines.add( String( "CALL STACK:" ) );

	if( skipTop == 0 )
		skipTop = stack -> extraLevels;
	
	if( skipTop > 0 ) {
		lines.add( String( "\t...skipped..." ) );
		startItem -= skipTop;
	}

	bool skipLast = false;
	for( int k = startItem; k >= 0; k-- ) {
		rfc_threadstacklevel *sl = rfc_thr_stacklevel( stack , k );

		// extract short name
		String moduleName = sl -> moduleName;
		int from = moduleName.findLastAny( "/\\" );
		int to = moduleName.findLast( '.' );

		String moduleNameShort = moduleName;
		if( from >= 0 && to >= 0 )
			moduleNameShort = moduleNameShort.getMid( from + 1 , to - from - 1 );

		if( !strcmp( moduleNameShort , "unknown" ) )
			continue;

		lines.add( String( "\t" ) + sl -> className + 
			"::" + sl -> functionName + 
			" (" + moduleNameShort + 
			", " + sl -> message + ")" );

		// stop after main function
		String functionName = sl -> functionName;
		if( strcmp( functionName , "_main" ) == 0 ||
			strcmp( functionName , "threadMainFunction" ) == 0 ||
			strcmp( functionName , "runThread" ) == 0 ) {
			if( k > 0 )
				skipLast = true;
			break;
		}
	}

	if( skipLast )
		lines.add( "\t...skipped..." );

	// create final string
	String output;
	for( int k = 0; k < lines.count(); k++ ) {
		if( k > 0 )
			output += "\n";
		output += lines.get( k );
	}
	LogOutputMode mode = ( printInplace )? Logger::LogLine : Logger::LogDefault;
	logInternal( output , mode , Logger::LogLevelInfo );
}

// log calls
void Logger::log( const char *s , Logger::LogLevel logLevel , LogOutputMode logMode ) {
	if( !settings -> logDisabled( logLevel ) )
		logInternal( s , logMode , logLevel );
}

void Logger::logInfo( const char *s , LogOutputMode mode ) {
	if( !settings -> logDisabled( Logger::LogLevelInfo ) )
		logInternal( s , mode , Logger::LogLevelInfo );
}

void Logger::logError( const char *s , LogOutputMode mode ) {
	if( !settings -> logDisabled( Logger::LogLevelError ) )
		logInternal( s , mode , Logger::LogLevelError );
}

void Logger::logDebug( const char *s , LogOutputMode mode ) {
	if( !settings -> logDisabled( Logger::LogLevelDebug ) )
		logInternal( s , mode , Logger::LogLevelDebug );
}

bool Logger::disabled( Logger::LogLevel logLevel ) {
	return( settings -> logDisabled( logLevel ) );
}

void Logger::logInternal( const char *s , LogOutputMode mode , Logger::LogLevel p_logLevel ) {
	// check needs to be excluded
	if( settings -> isExcluded( s ) )
		return;

	LogManager *logManager = settings -> getLogManager();
	ThreadHelper *logTail = NULL;
	if( logManager -> isStarted() )
		logTail = ThreadHelper::getThreadObject();

	if( logTail == NULL ) {
		logManager -> add( &s , 1 , p_logLevel , getPostfix() );
		return;
	}
	
	// release old
	String& w = logTail -> lastMsg;
	if( logTail -> remains )
		if( mode == Logger::LogStart || mode == Logger::LogDefault ) {
			const char *p = w;
			logManager -> add( &p , 1 , p_logLevel , getPostfix() );

			logTail -> remains = false;
			w.clear();
		}

	w += s;
	if( mode ==	Logger::LogLine || mode == Logger::LogStart )
		w += "\n";

	if( mode == Logger::LogStop || mode == Logger::LogDefault ) {
		const char *p = w;
		logManager -> add( &p , 1 , p_logLevel , getPostfix() );
		logTail -> remains = false;
		w.clear();
	}
	else
		logTail -> remains = true;
}

const char *Logger::getPostfix() {
	if( loggerName != NULL )
		return( loggerName );
	return( NULL );
}

bool Logger::isLogAll() {
	if( settings == NULL )
		attachRoot(); 
	return( settings -> getLevel() == LogLevelDebug );
}

Xml Logger::getLogSettings() {
	if( settings == NULL )
		attachRoot(); 
	return( settings -> getXml() );
}

void Logger::printStack( RuntimeException& e ) { 
	if( !e.isSEH() ) {
		logError( String( "printStack: C++ Exception - " ) + e.getMsg() + " (file=" + e.getFileShort() + ", line=" + e.getLine() + ")" );
		printStack( e.getStack() , 0 );
		return;
	}

	logError( String( "printStack: SEH Exception - " ) + e.getClassName() + "::" + e.getFunctionName() + " (" + e.getMsg() + ")" );

	// skip system levels
	printStack( e.getStack() , e.getSkipCount() );
}

