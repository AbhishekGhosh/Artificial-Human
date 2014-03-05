#include "stdafx.h"
#include "ahumantarget.h"

// #############################################################################
// #############################################################################

class SensorNose : public MindSensor {
private:
	bool continueRunFlag;

public:
	SensorNose( SensorArea *area );
	virtual ~SensorNose();
	virtual const char *getClass() { return( "SensorNose" ); };

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

MindSensor *AHumanTarget::createNose( SensorArea *area ) {
	return( new SensorNose( area ) );
}

// #############################################################################
// #############################################################################

SensorNose::SensorNose( SensorArea *p_area )
:	MindSensor( p_area ) {
	attachLogger();
	continueRunFlag = false;

	// autonomous, polling is not required
	MindSensor::setPollState( false );
}

SensorNose::~SensorNose() {
}

void SensorNose::configureSensor( Xml p_config ) {
	config = p_config;
}

void SensorNose::createSensor( MindRegionCreateInfo *createInfo ) {
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

void SensorNose::createSourceConnector( MindRegionCreateInfo *createInfo , MindRegionConnectorDef *connector ) {
	NeuroLinkSource *source = new NeuroLinkSource();
	source -> create( this , connector -> getId() );
	source -> setHandler( ( MindRegion::NeuroLinkSourceHandler )&SensorNose::sourceHandler );
}

void SensorNose::createTargetConnector( MindRegionCreateInfo *createInfo , MindRegionConnectorDef *connector ) {
	NeuroLinkTarget *target = new NeuroLinkTarget();
	target -> create( this , connector -> getId() );
	target -> setHandler( ( MindRegion::NeuroLinkTargetHandler )&SensorNose::targetHandler );
}

NeuroSignal *SensorNose::sourceHandler( NeuroLink *link , NeuroLinkSource *point ) {
	return( NULL );
}

NeuroSignalSet *SensorNose::targetHandler( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *srcData ) {
	return( NULL );
}
