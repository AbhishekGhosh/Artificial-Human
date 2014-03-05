#include "stdafx.h"

// #############################################################################
// #############################################################################

class EffectorWheeledLegs : public MindEffector {
private:
	bool continueRunFlag;

public:
	EffectorWheeledLegs( EffectorArea *area );
	virtual ~EffectorWheeledLegs();
	virtual const char *getClass() { return( "EffectorWheeledLegs" ); };

public:
	// effector lifecycle
	virtual void createEffector();
	virtual void configureEffector( Xml config );
	virtual void startEffector();
	virtual void stopEffector();
	virtual void processEffectorControl( NeuroLink *link , NeuroSignal *signal );

private:
	bool executeEffectorControl( NeuroSignal *signal );
	void sendSignal( int action , String value );

	unsigned char getCommand( NeuroSignal *signal , unsigned char *strength );
	void charToSignal( NeuroSignal *signal , int pos , unsigned char v );
};

// #############################################################################
// #############################################################################

EffectorWheeledLegs::EffectorWheeledLegs( EffectorArea *p_area )
:	MindEffector( p_area ) {
	attachLogger();
}

EffectorWheeledLegs::~EffectorWheeledLegs() {
}

// effector lifecycle
void EffectorWheeledLegs::createEffector() {
}

void EffectorWheeledLegs::configureEffector( Xml config ) {
}

void EffectorWheeledLegs::startEffector() {
}

void EffectorWheeledLegs::stopEffector() {
}

void EffectorWheeledLegs::processEffectorControl( NeuroLink *link , NeuroSignal *signal ) {
}

bool EffectorWheeledLegs::executeEffectorControl( NeuroSignal *signal ) {
	return( false );
}

void EffectorWheeledLegs::sendSignal( int action , String value ) {
}

unsigned char EffectorWheeledLegs::getCommand( NeuroSignal *signal , unsigned char *strength ) {
	return( ( unsigned char )0 );
}

void charToSignal( NeuroSignal *signal , int pos , unsigned char v ) {
}

// #############################################################################
// #############################################################################

MindEffector *ARobotTarget::createWheeledLegs( EffectorArea *area ) {
	return( new EffectorWheeledLegs( area ) );
}

