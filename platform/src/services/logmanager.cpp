#include <ah_platform.h>
#include <ah_services_impl.h>

#include <sys/types.h>
#include <sys/timeb.h>

// #############################################################################
// #############################################################################

static unsigned __stdcall async_logger( void *p_arg ) {
	LogManager *lm = ( LogManager * )p_arg;
	lm -> run( NULL );
	return( 0 );
}

// #############################################################################
// #############################################################################

LogManager::LogManager() {
	started = false;
	configSyncMode = false;

	logFileStream = NULL;
	lock = rfc_hnd_semcreate();
	stopEvent = rfc_hnd_evcreate();

	rfc_hnd_evsignal( stopEvent );

	isFileLoggingEnabled = false;
	stopAll = false;

	va = 1000;
	v = ( LogRecord * )calloc( va , sizeof( LogRecord ) );
	extraMode = false;
	syncMode = true;
	memset( &asyncThread , 0 , sizeof( RFC_THREAD ) );

	n1e = 0; n3e = va;
	n2f = n4f = n5f = n6e = 0;

	startAdd = startGet = 0;
	logger.attachRoot();
}

LogManager::~LogManager() {
	// check no filled data
	ASSERT( n2f == 0 && n4f == 0 && n5f == 0 );

	if( logFileStream != NULL ) {
		fprintf( logFileStream , "--------------\nStopped logger name=" + name + "\n" );
		fclose( logFileStream );
	}

	rfc_hnd_semdestroy( lock );
	rfc_hnd_evdestroy( stopEvent );
}

void LogManager::configure( Xml config ) {
	name = config.getAttribute( "name" );
	configSyncMode = config.getBooleanProperty( "syncMode" );
	configLogFile = config.getProperty( "filename" );
	configLogFormat = config.getProperty( "format" );
}

// sync/async mode
void LogManager::setSyncMode( bool p_syncMode ) {
	if( syncMode == p_syncMode )
		return;

	syncMode = p_syncMode;
	if( syncMode == false ) {
		rfc_hnd_evreset( stopEvent );
		logger.logInfo( "LogManager::setSyncMode: start asynchronous logging..." );
		if( !rfc_thr_process( &asyncThread , this , async_logger ) ) {
			// wait till log thread successfully started
			rfc_hnd_waitevent( stopEvent , -1 );
			rfc_hnd_evreset( stopEvent );
		}
		else
			rfc_hnd_evsignal( stopEvent );
	}
	else {
		// wait till writer thread is ended
		rfc_hnd_waitevent( stopEvent , -1 );
	}
}

bool LogManager::getSyncMode() {
	return( syncMode );
}

bool LogManager::getConfiguredSyncMode() {
	return( configSyncMode );
}

bool LogManager::start() {
	String fileName = configLogFile;

	logFileStream = NULL;
	logFileStream = fopen( fileName , "at" );
	if( logFileStream == NULL ) {
		String error = "LogManager::logStart: cannot initialize logging: cannot open file - " + fileName + "\n";
		fprintf( stderr , error );
		return( false );
	}

	// enable logging to screen and to file
	isFileLoggingEnabled = true;
	stopAll = false;
	fprintf( logFileStream , "Started logger name=" + name + "\n--------------\n" );
	fflush( logFileStream );

	started = true;

	return( true );
}

void LogManager::stopAsync() {
	if( syncMode )
		return;

	setSyncMode( true );
}

void LogManager::stop() {
	stopAsync();
	stopAll = true;
	started = false;
}

void LogManager::run( void * ) {
	// signal thread is started
	rfc_hnd_evsignal( stopEvent );
	logger.logInfo( "LogManager::run: asynchronous logging thread started" );

	try {
		// do get in cycle
		while( syncMode == false )
			get( true );

		// cleanup async queue before exit
		rfc_hnd_semlock( lock );
		while( get( false ) );
		rfc_hnd_semunlock( lock );
	}
	catch ( RuntimeException& e ) {
		logger.printStack( e );
		fprintf( stderr , "LogManager::run: exception in logging: " , e.getMsg() );
	}
	catch ( ... ) {
		logger.printStack();
		fprintf( stderr , "LogManager::run: unknown exception in logging" );
	}

	logger.logInfo( "LogManager::run: asynchronous logging thread stopped" );
	rfc_hnd_evsignal( stopEvent );
}

void LogManager::output( LogRecord *p ) {
	struct tm lt;
	localtime_s( &lt , ( const time_t * )&(p -> time) );

	if( p -> count <= 0 )
		return;

	static const char *levelNames[ 4 ] = {
		" NONE" ,
		"ERROR" ,
		" INFO" ,
		"DEBUG"
	};

	char l_buf[ 100 ];
	sprintf( l_buf , "%2.2d:%2.2d:%2.2d,%3.3d [%s, 0x%4.4x] - " , 
		lt.tm_hour , lt.tm_min , lt.tm_sec , p -> time_ms ,
		levelNames[ p -> logLevel ] ,
		p -> threadId );

	// output
	FILE *cs = ( p -> logLevel == Logger::LogLevelError )? stderr : stdout;
	if( p -> count == 1 ) {
		fprintf( cs , "%s%s" , l_buf , p -> strings.one );
		if( p -> postfix != NULL )
			fprintf( cs , " [%s]\n" , p -> postfix );
		else
			fprintf( cs , "\n" );

		if( isFileLoggingEnabled ) {
			fprintf( logFileStream , "%s%s" , l_buf , p -> strings.one );
			if( p -> postfix != NULL )
				fprintf( logFileStream , " [%s]\n" , p -> postfix );
			else
				fprintf( logFileStream , "\n" );
		}
	}
	else {
		if( p -> postfix != NULL )
			fprintf( cs , "%s [%s]\n" , l_buf , p -> postfix );
		else
			fprintf( cs , "%s\n" , l_buf );

		if( isFileLoggingEnabled ) {
			if( p -> postfix != NULL )
				fprintf( logFileStream , "%s [%s]\n" , l_buf , p -> postfix );
			else
				fprintf( logFileStream , "%s\n" , l_buf );
		}

		for( int k = 0; k < p -> count; k++ ) {
			fprintf( cs , "%s\n" , p -> strings.many[ k ] );
			if( isFileLoggingEnabled )
				fprintf( logFileStream , "%s\n" , p -> strings.many[ k ] );
		}
	}

	if( isFileLoggingEnabled )
		fflush( logFileStream );
}

void LogManager::add( const char **chunkLines , int count , Logger::LogLevel p_logLevel , const char *postfix ) {
	// only exclusive
	rfc_hnd_semlock( lock );

	if( syncMode ) {
		// ensure no async messages
		showAsyncMessages();

		// print in sync
		LogRecord lr;
		set( &lr , false , chunkLines , count , p_logLevel , postfix );
		output( &lr );

		rfc_hnd_semunlock( lock );
		return;
	}

	ASSERTMSG( stopAll == false , "Logging is closed" );

	// simple mode
	if( !extraMode ) {
		// space available
		if( n1e || n3e ) {
			// has space for adding
			set( &v[ startAdd ] , true , chunkLines , count , p_logLevel , postfix );
			if( n4f ) {
				n3e--;
				n2f++;
				startAdd++;
			}
			else {
				n3e--;
				n2f++;
				startAdd++;

				// roll to top
				if( startAdd == va ) {
					startAdd = 0;
					n4f = n2f;
					n2f = 0;
					n3e = n1e;
					n1e = 0;
				}
			}

			// fprintf( logFileStream , "+n[%d, %d, %d, %d, %d, %d], s[%d, %d]\n" ,
			//	n1e , n2f , n3e , n4f , n5f , n6e , startAdd , startGet );
			// fflush( logFileStream );
			rfc_hnd_semunlock( lock );
			return;
		}

		// no space - start extended mode
		extraMode = true;
		startAdd = va;
	}

	// add in extra mode
	if( !n6e ) {
		// no empty slots - reallocation required
		n6e = 100;
		va += n6e;
		v = ( LogRecord * )realloc( v , va * sizeof( LogRecord ) );
		memset( &v[ va - n6e ] , 0 , sizeof( LogRecord ) * n6e );
	}

	// write log record
	set( &v[ startAdd ] , true , chunkLines , count , p_logLevel , postfix );
	startAdd++;
	n6e--;
	n5f++;

	// fprintf( logFileStream , "+n[%d, %d, %d, %d, %d, %d], s[%d, %d]\n" ,
	//	n1e , n2f , n3e , n4f , n5f , n6e , startAdd , startGet );
	// fflush( logFileStream );
	rfc_hnd_semunlock( lock );
}

void LogManager::set( LogRecord *p , bool copy , const char **chunkLines , int count , Logger::LogLevel p_logLevel , const char *postfix ) {
	p -> count = count;

	struct _timeb timebuffer;
	_ftime_s( &timebuffer );
	p -> time = timebuffer.time;
	p -> time_ms = timebuffer.millitm;
	p -> logLevel = p_logLevel;

	p -> threadId = 0;
	if( started ) {
		ThreadService *ts = ThreadService::getService();
		p -> threadId = ts -> getThreadId();
	}

	if( count == 0 )
		p -> strings.one = NULL;
	else
	if( count == 1 ) {
		if( copy ) {
			p -> strings.one = _strdup( *chunkLines );
			p -> postfix = ( postfix == NULL || *postfix == 0 )? NULL : _strdup( postfix );
		}
		else {
			p -> strings.one = ( char * )*chunkLines;
			p -> postfix = ( postfix == NULL || *postfix == 0 )? NULL : ( char * )postfix;
		}
	}
	else {
		if( copy ) {
			p -> strings.many = ( char ** )calloc( count , sizeof( char * ) );
			for( int k = 0; k < count; k++ )
				p -> strings.many[ k ] = _strdup( chunkLines[ k ] );
			p -> postfix = ( postfix == NULL || *postfix == 0 )? NULL : _strdup( postfix );
		}
		else {
			p -> strings.many = ( char ** )chunkLines;
			p -> postfix = ( postfix == NULL || *postfix == 0 )? NULL : ( char * )postfix;
		}
	}
}

void LogManager::clear( LogRecord *p ) {
	if( p -> count == 1 ) {
		free( p -> strings.one );
		p -> strings.one = NULL;
	}
	else {
		for( int k = 0; k < p -> count; k++ )
			free( p -> strings.many[ k ] );
		free( p -> strings.many );
		p -> strings.many = NULL;
	}
	p -> count = 0;
}

bool LogManager::get( bool p_autolock ) {
	// await data
	while( 1 ) {
		// only exclusive
		if( p_autolock )
			rfc_hnd_semlock( lock );

		if( n2f || n4f || n5f )
			break;

		// do not wait in sync mode
		if( syncMode ) {
			if( p_autolock )
				rfc_hnd_semunlock( lock );
			return( false );
		}

		// no data - wait
		if( p_autolock )
			rfc_hnd_semunlock( lock );
		rfc_thr_sleep( 1 );
	}

	// get pending count
	int readFrom = startGet;
	int n = 0;
	const int maxRead = 100;
	LogRecord localRecords[ maxRead ];

	// read n2/n4 first
	if( n4f > 0 ) {
		// read n4
		if( n4f > maxRead ) {
			// partial read
			n4f -= maxRead;
			n3e += maxRead;
			n = maxRead;
			startGet += n;
		}
		else {
			// full read
			n = n4f;
			n3e += n4f;
			n4f = 0;

			// roll read to top
			startGet = n1e;
		}
	}
	else {
		// read n2
		if( n2f > maxRead ) {
			// partial read
			n2f -= maxRead;
			n1e += maxRead;
			n = maxRead;
			startGet += n;
		}
		else {
			// full read
			n = n2f;
			n1e += n2f;
			n2f = 0;
			startGet += n;

			// extra mode
			if( extraMode ) {
				startGet += n3e;
				n1e += n3e;
				n3e = n6e;
				n6e = 0;
				n2f = n5f;
				n5f = 0;

				extraMode = false;
			}
			else {
				// roll
				if( startGet == va )
					startGet = n1e;
			}
		}
	}

	// read
	memcpy( localRecords , &v[ readFrom ] , sizeof( LogRecord ) * n );
	memset( &v[ readFrom ] , 0 , sizeof( LogRecord ) * n );
	// fprintf( logFileStream , "-n[%d, %d, %d, %d, %d, %d], s[%d, %d]\n" ,
	//	n1e , n2f , n3e , n4f , n5f , n6e , startAdd , startGet );
	if( p_autolock )
		rfc_hnd_semunlock( lock );

	// write w/o lock
	for( int k = 0; k < n; k++ ) {
		output( &localRecords[ k ] );
		clear( &localRecords[ k ] );
	}

	return( true );
}

int LogManager::getLogRecordsPending() {
	int n = n2f + n4f + n5f;
	return( n );
}

void LogManager::showAsyncMessages() {
	while( getLogRecordsPending() > 0 )
		get( false );
}

