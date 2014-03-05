#include <ah_platform.h>
#include <ah_statistics_impl.h>

// #############################################################################
// #############################################################################

StatMetric::StatMetric() {
	type = StatMetric::MetricType_Unknown;
	calculateSum = false;

	sumValues = 0;
	count = 0;
}

StatMetric::~StatMetric() {
}

void StatMetric::configure( Xml config ) {
	name = config.getAttribute( "name" );
	
	String typeName = config.getAttribute( "type" );
	if( typeName.equals( "percentage" ) )
		type = StatMetric::MetricType_Percentage;
	else
	if( typeName.equals( "int" ) )
		type = StatMetric::MetricType_Int;
	else
		ASSERTFAILED( "Cannot parse type for metric name=" + name + ", value=" + typeName );
}

void StatMetric::addMetricValue( int value ) {
	count++;
	if( calculateSum )
		sumValues += value;
}

void StatMetric::prepareCalculate( MetricAggregate aggregateType ) {
	if( aggregateType == MetricAggregate_Average )
		calculateSum = true;
	else
	if( aggregateType == MetricAggregate_Sum )
		calculateSum = true;
}

int StatMetric::getCount() {
	return( count );
}

int StatMetric::calculate( MetricAggregate aggregateType ) {
	if( aggregateType == MetricAggregate_Average ) {
		if( count == 0 )
			return( 0 );

		int value = ( int )( sumValues / count );
		return( value );
	}

	if( aggregateType == MetricAggregate_Sum ) {
		int value = ( int )sumValues;
		return( value );
	}

	return( 0 );
}

void StatMetric::reset() {
	sumValues = 0;
	count = 0;
}
