#include "stdafx.h"

/*#########################################################################*/
/*#########################################################################*/

ARobotTarget::ARobotTarget() {
	attachLogger();
}

void ARobotTarget::configureTarget( Xml config ) {
}

void ARobotTarget::createTarget( SensorArea *sensorArea , EffectorArea *effectorArea ) {
	ASSERTMSG( nxt.create() , "unable to initiate connection to aRobot" );

	MindTarget::addSensor( "eye" , ARobotTarget::createEye( sensorArea ) );
	MindTarget::addEffector( "leg" , ARobotTarget::createWheeledLegs( effectorArea ) );
}

void ARobotTarget::initSensorsTarget( SensorArea *sensorArea ) {
}

void ARobotTarget::initEffectorsTarget( EffectorArea *effectorArea ) {
}

void ARobotTarget::runTarget() {
	Timer t;
	t.createRunWindowSec( 5 );

	for( int k = 0; k < 10; k++ ) {
		logger.logInfo( "turn forward 20 both" );
		nxt.motorSetPower( 2 , 20 );
		nxt.motorSetPower( 3 , 20 );
		logger.logInfo( "sleep" );

		t.waitNext();

		logger.logInfo( "turn backward 20 both" );
		nxt.motorSetPower( 2 , -20 );
		nxt.motorSetPower( 3 , -20 );
		logger.logInfo( "sleep" );

		t.waitNext();
	}

	nxt.destroy();
}

void ARobotTarget::stopTarget() {
	nxt.destroy();
}

void ARobotTarget::exitTarget() {
}

void ARobotTarget::destroyTarget() {
}

