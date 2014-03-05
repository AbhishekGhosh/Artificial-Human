#include "stdafx.h"

// #############################################################################
// #############################################################################

class SensorEye : public MindSensor {
private:
	bool continueRunFlag;

public:
	SensorEye( SensorArea *area );
	virtual ~SensorEye();
	virtual const char *getClass() { return( "SensorEye" ); };

public:
	// sensor lifecycle
	virtual void createSensor();
	virtual void configureSensor( Xml config );
	virtual void startSensor();
	virtual void stopSensor();
	virtual void processSensorControl( NeuroLink *link , NeuroSignal *signal );
	virtual void produceSensorData();
	virtual void pollSensor() {};

private:
	bool executeSensorControl( NeuroSignal *signal );
	void sendSignal( int action , String value );

	unsigned char getCommand( NeuroSignal *signal , unsigned char *strength );
	void charToSignal( NeuroSignal *signal , int pos , unsigned char v );
};

// #############################################################################
// #############################################################################

SensorEye::SensorEye( SensorArea *p_area )
:	MindSensor( p_area ) {
	attachLogger();
	continueRunFlag = false;

	// autonomous, polling is not required
	MindSensor::setPollState( false );
}

SensorEye::~SensorEye() {
}

void SensorEye::createSensor() {
	// internal data
	const int BYTE_SIZE=8;
	MindSensor::createSensorySignal( BYTE_SIZE , BYTE_SIZE );
	MindSensor::createControlFeedbackSignal( BYTE_SIZE , BYTE_SIZE );
}

void SensorEye::configureSensor( Xml config ) {
	logger.logInfo( String( "configureSensor: " ) + "sensor=" + getClass() + " configured" );
}

void SensorEye::startSensor() {
	// start reading thread
	continueRunFlag = true;
}

void SensorEye::stopSensor() {
	continueRunFlag = false;
}

// called when inputs updated - i.e. control commands
void SensorEye::processSensorControl( NeuroLink *link , NeuroSignal *signal ) {
	if( executeSensorControl( signal ) ) {
		produceSensorData();
	}
}

// returns status - whether sensor data should be generated
bool SensorEye::executeSensorControl( NeuroSignal *signal ) {
	return( false );
}

void SensorEye::produceSensorData() {
}

// #############################################################################
// #############################################################################

MindSensor *ARobotTarget::createEye( SensorArea *area ) {
	return( new SensorEye( area ) );
}
