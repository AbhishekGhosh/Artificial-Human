#include <ah_platform.h>
#include <ah_statistics_impl.h>

// #############################################################################
// #############################################################################

StatLogTask::StatLogTask() {
	attachLogger();
}

StatLogTask::~StatLogTask() {
	logRecordList.destroy();
}

void StatLogTask::configure( Xml config ) {
	name = config.getAttribute( "name" );
	intervalSec = config.getIntAttribute( "intervalSec" );

	// read log records
	for( Xml xmlChild = config.getFirstChild( "logRecord" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "logRecord" ) ) {
		StatLogRecord *record = new StatLogRecord();
		record -> configure( xmlChild );
		logRecordList.add( record );
	}
}

void StatLogTask::create() {
	// setup log records
	for( int k = 0; k < logRecordList.count(); k++ ) {
		StatLogRecord *record = logRecordList.get( k );
		record -> create();
	}
}

void StatLogTask::start() {
	logger.logInfo( "start: start log task name=" + name + ", intervalSec=" + intervalSec + "..." );

	// create scheduler task
	SchedulerService *ss = SchedulerService::getService();
	ss -> createTimerTask( "stat." + name , intervalSec , this );
}

void StatLogTask::stop() {
	logger.logInfo( "stop: stop log task name=" + name + "..." );

	SchedulerService *ss = SchedulerService::getService();
	ss -> dropTimerTask( "stat." + name );
}

void StatLogTask::onTextMessage( TextMessage *msg ) {
	logger.logInfo( "STATISTICS: statTask=" + name , Logger::LogStart );

	for( int k = 0; k < logRecordList.count(); k++ ) {
		StatLogRecord *record = logRecordList.get( k );
		String log = record -> getLogString();
		Logger::LogOutputMode mode = ( k == ( logRecordList.count() - 1 ) )? Logger::LogStop : Logger::LogLine;
		logger.logInfo( log );
	}

	resetMetrics();
}

void StatLogTask::resetMetrics() {
	for( int k = 0; k < logRecordList.count(); k++ ) {
		StatLogRecord *record = logRecordList.get( k );
		record -> resetMetric();
	}
}

