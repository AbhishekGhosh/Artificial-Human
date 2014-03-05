#include <ah_platform.h>

// #############################################################################
// #############################################################################

Timer::Timer() {
	timeStarted = 0;
	timeStartedTicks = 0;
	waitTimeMs = 0;
	waitCount = 0;
}

void Timer::createWithStartingTimestamp() {
	timeStarted = clock();
	rfc_hpt_setpoint( &timeStartedTicks );
}

void Timer::createRunWindowMs( int p_waitTimeMs ) {
	timeStarted = clock();
	waitTimeMs = p_waitTimeMs;
}

void Timer::createRunWindowSec( int p_waitTimeSec ) {
	timeStarted = clock();
	waitTimeMs = p_waitTimeSec * 1000;
}

// time passed from process start - in ms
int Timer::clockSinceProcessStartMs() {
	long timeNow = clock();
	return( ( int )( timeNow * 1000 / CLOCKS_PER_SEC ) );
}

RFC_INT64 Timer::getCurrentTimeMillis() {
	RFC_INT64 ticksNow;
	rfc_hpt_setpoint( &ticksNow );
	RFC_INT64 msNow = rfc_hpt_ticks2ms( ticksNow );
	return( msNow );
}

int Timer::timeCreatedMs() {
	return( ( int )( timeStarted * 1000 / CLOCKS_PER_SEC ) );
}

void Timer::startAdjustment() {
	rfc_hpt_startadjustment();
}

void Timer::stopAdjustment() {
	rfc_hpt_stopadjustment();
}

int Timer::timePassedMs() {
	long timeNow = clock();
	return( ( int )( ( timeNow - timeStarted ) * 1000 ) / CLOCKS_PER_SEC );
}

// time passed - in clocks
int Timer::timePassedClocks() {
	return( ( int )( clock() - timeStarted ) );
}

// time passed - in ticks
int Timer::timePassedTicks() {
	return( ( int )rfc_hpt_timepassed( &timeStartedTicks ) );
}

// convert clocks to ms
int Timer::timeClocksToMs( int clocks ) {
	return( ( int )( ( clocks * 1000 ) / CLOCKS_PER_SEC ) );
}

// convert ms to clocks
int Timer::timeMsToClocks( int ms ) {
	return( ( int )( ( ms * CLOCKS_PER_SEC ) / 1000 ) );
}

int Timer::timeRemainedSec() {
	return( ( waitTimeMs - timePassedMs() ) / 1000 );
}

bool Timer::go() {
	return( timePassedMs() < waitTimeMs );
}

// convert ticks to ms
int Timer::timeTicksToMs( int ticks ) {
	return( ( int )rfc_hpt_ticks2ms( ticks ) );
}

// convert ms to ticks
int Timer::timeMsToTicks( int ms ) {
	return( ( int )rfc_hpt_ms2ticks( ms ) );
}

int Timer::waitNext() {
	ThreadService *ts = ThreadService::getService();
	ts -> threadSleepMs( waitTimeMs );
	return( ++waitCount );
}
