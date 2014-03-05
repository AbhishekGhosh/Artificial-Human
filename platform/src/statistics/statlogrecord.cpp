#include <ah_platform.h>
#include <ah_statistics_impl.h>

// #############################################################################
// #############################################################################

StatLogRecord::StatLogRecord() {
	metric = NULL;
	functionMetric1 = NULL;
	functionMetric2 = NULL;
	derived = false;
	aggregateType = StatMetric::MetricAggregate_Unknown;
	function = StatLogRecord::MetricFunction_Unknown;
}

StatLogRecord::~StatLogRecord() {
}

void StatLogRecord::configure( Xml config ) {
	metricName = config.getAttribute( "metric" );
	aggregateTypeName = config.getAttribute( "aggregate" );

	if( aggregateTypeName.equals( "average" ) )
		aggregateType = StatMetric::MetricAggregate_Average;
	else
	if( aggregateTypeName.equals( "sum" ) )
		aggregateType = StatMetric::MetricAggregate_Sum;
	else
	if( aggregateTypeName.equals( "derived" ) ) {
		derived = true;
		configureFunction( config );
	}
	else
		ASSERTFAILED( "Cannot parse type for log aggregate metric=" + metricName + ", value=" + aggregateTypeName );
}

void StatLogRecord::configureFunction( Xml config ) {
	String functionName = config.getAttribute( "function" );

	if( functionName.equals( "sumbysum2percentage" ) ) {
		function = StatLogRecord::MetricFunction_SumBySum2Percentage;
		functionMetric1Name = config.getAttribute( "metric1" );
		functionMetric2Name = config.getAttribute( "metric2" );
	}
	else
		ASSERTFAILED( "Cannot parse aggregate function for log metric=" + metricName + ", value=" + functionName );
}

void StatLogRecord::create() {
	StatService *ss = StatService::getService();
	metric = ss -> getMetricByName( metricName );
	metric -> prepareCalculate( aggregateType );

	if( !functionMetric1Name.isEmpty() ) {
		functionMetric1 = ss -> getMetricByName( functionMetric1Name );
		functionMetric1 -> prepareCalculate( StatMetric::MetricAggregate_Sum );
	}
	if( !functionMetric2Name.isEmpty() ) {
		functionMetric2 = ss -> getMetricByName( functionMetric2Name );
		functionMetric2 -> prepareCalculate( StatMetric::MetricAggregate_Sum );
	}
}

String StatLogRecord::getLogString() {
	String log = "metric=" + metricName + ", aggregate=" + aggregateTypeName + ", value=";

	if( derived == false ) {
		if( metric -> getCount() == 0 )
			log += "(no values)";
		else {
			int value = metric -> calculate( aggregateType );
			log += value;
		}
	}
	else {
		int value = calculateDerivedValue(); 
		log += value;
	}

	return( log );
}

void StatLogRecord::resetMetric() {
	metric -> reset();
	if( functionMetric1 != NULL )
		functionMetric1 -> reset();
	if( functionMetric2 != NULL )
		functionMetric2 -> reset();
}

int StatLogRecord::calculateDerivedValue() {
	// calculate derived
	if( function == StatLogRecord::MetricFunction_SumBySum2Percentage ) {
		RFC_INT64 value1 = functionMetric1 -> calculate( StatMetric::MetricAggregate_Sum );
		RFC_INT64 value2 = functionMetric2 -> calculate( StatMetric::MetricAggregate_Sum );
		if( value2 == 0 )
			return( 0 );

		int value = ( int )( ( value1 * 100 ) / value2 );
		return( value );
	}

	ASSERTFAILED( "Cannot calculate unknown derived function for log metric=" + metricName );
	return( 0 );
}
