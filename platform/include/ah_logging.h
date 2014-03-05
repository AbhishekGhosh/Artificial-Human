#ifndef	INCLUDE_AH_LOGGING_H
#define INCLUDE_AH_LOGGING_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_basetypes.h"

class Logger;

#define LOGDEBUG( msg ) if( logger.isLogAll() ) logger.logDebug( msg )

/*#########################################################################*/
/*#########################################################################*/

class LogSettingsItem;

class Logger {
public:
	typedef enum {
		LogLevelNone = 0 ,
		LogLevelError = 1 ,
		LogLevelInfo = 2 ,
		LogLevelDebug = 3
	} LogLevel;

	typedef enum {
		LogStart = 1 ,
		LogLine = 0 ,
		LogStop = 2 ,
		LogDefault = -1
	} LogOutputMode;

public:
	// construction
	Logger();
	~Logger();

	Xml getLogSettings();
	void setLoggerName( const char *name );
	void attachRoot();
	void attachService( const char *serviceName , const char *loggerName = NULL );
	void attachObject( const char *className , const char *loggerName = NULL );
	void attachObjectInstance( const char *className , const char *classInstance , const char *loggerName = NULL );
	void attachCustom( const char *customName , const char *loggerName = NULL );

	// stack
	void printStack();
	void printStack( rfc_threadstack *stack , int skipTop );
	void printStackInplace( rfc_threadstack *stack , int skipTop );
	void printStackInternal( rfc_threadstack *stack , int skipTop , bool printInplace );
	void printStack( RuntimeException& e );

	// log calls
	void log( const char *s , Logger::LogLevel logLevel , LogOutputMode logMode = LogDefault );
	void logInfo( const char *s , LogOutputMode logMode = LogDefault );
	void logError( const char *s , LogOutputMode logMode = LogDefault );
	void logDebug( const char *s , LogOutputMode logMode = LogDefault );

	// check log status
	bool isLogAll();
	bool disabled( Logger::LogLevel logLevel );

private:
	void logInternal( const char *s , LogOutputMode logMode , Logger::LogLevel logLevel );
	const char *getPostfix();
	
private:
	const char *loggerName;
	LogSettingsItem *settings;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_LOGGING_H
