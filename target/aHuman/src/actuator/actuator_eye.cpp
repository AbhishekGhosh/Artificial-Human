#include "stdafx.h"
#include "ahumantarget.h"

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
	virtual void createSensor( MindRegionCreateInfo *createInfo );
	virtual void configureSensor( Xml config );
	virtual void startSensor() {};
	virtual void stopSensor() {};
	virtual void processSensorControl( NeuroLink *link , NeuroSignal *signal ) {};
	virtual void produceSensorData() {};
	virtual void pollSensor() {};

private:
	bool executeSensorControl( NeuroSignal *signal );
	void sendSignal( int action , String value );

	void createSourceConnector( MindRegionCreateInfo *createInfo , MindRegionConnectorDef *connector );
	void createTargetConnector( MindRegionCreateInfo *createInfo , MindRegionConnectorDef *connector );

	NeuroSignal *sourceHandler( NeuroLink *link , NeuroLinkSource *point );
	NeuroSignalSet *targetHandler( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *srcData );

private:
	Xml config;
};

MindSensor *AHumanTarget::createEye( SensorArea *area ) {
	return( new SensorEye( area ) );
}

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

void SensorEye::configureSensor( Xml p_config ) {
	config = p_config;
}

void SensorEye::createSensor( MindRegionCreateInfo *createInfo ) {
	// set connectors
	TargetRegionDef *info = MindSensor::getSensorInfo();
	MindRegionTypeDef *type = info -> getType();
	ClassList<MindRegionConnectorDef>& connectors = type -> getConnectors();
	for( int k = 0; k < connectors.count(); k++ ) {
		MindRegionConnectorDef *connector = connectors.get( k );
		if( connector -> isTarget() )
			createTargetConnector( createInfo , connector );
		else
			createSourceConnector( createInfo , connector );
	}
}

void SensorEye::createSourceConnector( MindRegionCreateInfo *createInfo , MindRegionConnectorDef *connector ) {
	NeuroLinkSource *source = new NeuroLinkSource();
	source -> create( this , connector -> getId() );
	source -> setHandler( ( MindRegion::NeuroLinkSourceHandler )&SensorEye::sourceHandler );
}

void SensorEye::createTargetConnector( MindRegionCreateInfo *createInfo , MindRegionConnectorDef *connector ) {
	NeuroLinkTarget *target = new NeuroLinkTarget();
	target -> create( this , connector -> getId() );
	target -> setHandler( ( MindRegion::NeuroLinkTargetHandler )&SensorEye::targetHandler );
}

NeuroSignal *SensorEye::sourceHandler( NeuroLink *link , NeuroLinkSource *point ) {
	return( NULL );
}

NeuroSignalSet *SensorEye::targetHandler( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *srcData ) {
	return( NULL );
}
