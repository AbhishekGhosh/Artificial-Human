#include <ah_platform.h>
#include <ah_threads_impl.h>

/*#########################################################################*/
/*#########################################################################*/

ThreadPoolPerformance::ThreadPoolPerformance( Logger& p_logger , ClassList<ThreadPoolItem>& p_threads )
:	logger( p_logger) ,
	threads( p_threads ) {

	lastActiveThreads = 0;
	lastAverageTimeWindowTicks = 0;
	lastAverageRunTicks = 0;
	lastAverageSleepTicks = 0;

	ticksPerSecond = Timer::timeMsToTicks( 1000 );
}

void ThreadPoolPerformance::gather() {
	float lastTotalWindowTicks = 0;
	float lastTotalRunTicks = 0;
	float lastTotalSleepTicks = 0;
	float lastTotalExecutionCount = 0;

	int nActiveThreads = 0;
	for( int k = 0; k < threads.count(); k++ ) {
		ThreadPoolItem *thread = threads.get( k );

		int executionTimeWindowTicks = thread -> executionTimeWindowTicks;
		if( executionTimeWindowTicks > 0 ) {
			nActiveThreads++;
			lastTotalWindowTicks += ( float )executionTimeWindowTicks;
			lastTotalRunTicks += ( float )thread -> ticksExecTimeTotal;
			lastTotalSleepTicks += ( float )thread -> ticksSleepTimeTotal;
			lastTotalExecutionCount += ( float )thread -> executionCount;
			thread -> clearStatistics();
		}
	}

	lastActiveThreads = nActiveThreads;
	lastAverageTimeWindowTicks = ( int )( lastTotalWindowTicks / nActiveThreads );
	lastAverageRunTicks = ( int )( lastTotalRunTicks / nActiveThreads );
	lastAverageSleepTicks = ( int )( lastTotalSleepTicks / nActiveThreads );
	lastAverageExecutionCount = ( int )( lastTotalExecutionCount / nActiveThreads );
}

void ThreadPoolPerformance::updateSpeedIfRequired( float prevLoadPercents , float lastLoadPercents , float maxLoadPercents , CPULOADINFO& loadinfo ) {
	if( lastActiveThreads == 0 || lastAverageExecutionCount == 0 ) {
		logger.logInfo( String( "updateSpeedIfRequired: ignore due to " ) + ( ( lastActiveThreads == 0 )? "lastActiveThreads=0" : "lastAverageExecutionCount=0" ) );
		return;
	}

	bool increase = ( lastLoadPercents >= maxLoadPercents )? false : true;
	float poolLoad = 100 * lastAverageRunTicks / ( float )( lastAverageRunTicks + lastAverageSleepTicks );

	// if pool load is only 1% - do not decrease load
	if( increase == false && poolLoad < 1 ) {
		logger.logInfo( String( "updateSpeedIfRequired: ignore due to decrease and poolLoad=" ) + poolLoad + " (<1)" );
		return;
	}

	// if current load is low and sleep time is large enough
	int averageExecutionTimeTicks = lastAverageRunTicks / lastAverageExecutionCount;
	if( increase == true && averageExecutionTimeTicks < lastAverageTimeWindowTicks / 2 ) {
		logger.logInfo( String( "updateSpeedIfRequired: ignore due to increase and low averageExecutionTimeTicks=" ) + averageExecutionTimeTicks + 
			" (lastAverageTimeWindowTicks=" + lastAverageTimeWindowTicks + ")" );
		return;
	}

	int ticks;
	if( increase ) {
		ticks = lastAverageTimeWindowTicks * 2;
		logger.logInfo( String( "updateSpeedIfRequired: increase window to executionTimeWindowTicks=" ) + ticks );
	}
	else {
		ticks = lastAverageTimeWindowTicks / 2;
		logger.logInfo( String( "updateSpeedIfRequired: decrease window to executionTimeWindowTicks=" ) + ticks );
	}

	for( int k = 0; k < threads.count(); k++ ) {
		ThreadPoolItem *thread = threads.get( k );
		if( thread -> executionTimeWindowTicks <= 0 )
			continue;

		// decrease or increase
		thread -> setExecutionTimeWindowTicks( ticks );
	}
}
