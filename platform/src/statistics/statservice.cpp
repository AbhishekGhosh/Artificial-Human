#include <ah_platform.h>
#include <ah_statistics_impl.h>

// #############################################################################
// #############################################################################

StatService::StatService() {
}

Service *StatService::newService() {
	return( new StatService() );
}

void StatService::configureService( Xml p_config ) {
	config = p_config;
}

void StatService::addMetricValue( const char *metricName , int metricValue ) {
	StatMetric *metric = metricMap.get( metricName );
	ASSERTMSG( metric != NULL , String( "invalid metric name=" ) + metricName );

	metric -> addMetricValue( metricValue );
}

void StatService::createService() {
	// load metrics
	Xml xmlMetrics = config.getChildNode( "metrics" );
	for( Xml xmlChild = xmlMetrics.getFirstChild( "metric" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "metric" ) ) {
		String name = xmlChild.getAttribute( "name" );
		ASSERTMSG( metricMap.get( name ) == NULL , "duplicate metric name=" + name );

		StatMetric *metric = new StatMetric();
		metric -> configure( xmlChild );
		metricMap.add( name , metric );
	}

	// load log tasks
	Xml xmlTasks = config.getChildNode( "logging" );
	for( Xml xmlChild = xmlTasks.getFirstChild( "logTask" ); xmlChild.exists(); xmlChild = xmlChild.getNextChild( "logTask" ) ) {
		String name = xmlChild.getAttribute( "name" );
		ASSERTMSG( logTaskMap.get( name ) == NULL , "duplicate task name=" + name );

		StatLogTask *task = new StatLogTask();
		task -> configure( xmlChild );
		logTaskMap.add( name , task );
	}
}

void StatService::initService() {
	// start logging
	for( int k = 0; k < logTaskMap.count(); k++ ) {
		StatLogTask *logTask = logTaskMap.getClassByIndex( k );
		logTask -> create();
	}
}

void StatService::runService() {
	logger.logInfo( "runService: start statistics logging tasks..." );
	// start logging
	for( int k = 0; k < logTaskMap.count(); k++ ) {
		StatLogTask *logTask = logTaskMap.getClassByIndex( k );
		logTask -> start();
	}
}

void StatService::stopService() {
	logger.logInfo( "runService: stop statistics logging tasks..." );
	// start logging
	for( int k = 0; k < logTaskMap.count(); k++ ) {
		StatLogTask *logTask = logTaskMap.getClassByIndex( k );
		logTask -> stop();
	}
}

void StatService::exitService() {
}

void StatService::destroyService() {
	metricMap.destroy();
	logTaskMap.destroy();
}

/*#########################################################################*/
/*#########################################################################*/

StatMetric *StatService::getMetricByName( String name ) {
	StatMetric *metric = metricMap.get( name );
	ASSERTMSG( metric != NULL , "invalid metric name=" + name );
	return( metric );
}

StatLogTask *StatService::getLogTaskByName( String name ) {
	StatLogTask *task = logTaskMap.get( name );
	ASSERTMSG( task != NULL , "invalid log task name=" + name );
	return( task );
}
