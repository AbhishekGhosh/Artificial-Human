#include "stdafx.h"

/*#########################################################################*/
/*#########################################################################*/

AWeeTarget::AWeeTarget() {
}

void AWeeTarget::configureTarget( Xml config ) {
}

void AWeeTarget::createTarget( SensorArea *sensorArea , EffectorArea *effectorArea ) {
	MindTarget::addSensor( AWeeTarget::createFileSysWalker( sensorArea ) );
}

void AWeeTarget::initSensorsTarget( SensorArea *sensorArea ) {
}

void AWeeTarget::initEffectorsTarget( EffectorArea *effectorArea ) {
}

void AWeeTarget::runTarget() {
}

void AWeeTarget::stopTarget() {
}

void AWeeTarget::exitTarget() {
}

void AWeeTarget::destroyTarget() {
}
