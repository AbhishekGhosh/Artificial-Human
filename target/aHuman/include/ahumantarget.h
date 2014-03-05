#include "stdafx.h"

/*#########################################################################*/
/*#########################################################################*/

#include "xmlhuman.h"
#include "ahumanmodel.h"
#include "ahumanwiki.h"
#include "ahumanverify.h"
#include "ahumanplayer.h"

/*#########################################################################*/
/*#########################################################################*/

class AHumanTarget;

/*#########################################################################*/
/*#########################################################################*/

class MindRegionDef;

/*#########################################################################*/
/*#########################################################################*/

class AHumanTarget : public MindTarget , public MessageSubscriber {
public:
	AHumanTarget();

	// target lifecycle
	virtual void configureTarget( Xml config );
	virtual void createTarget( SensorArea *sensorArea , EffectorArea *effectorArea );
	virtual void initTarget();
	virtual void runTarget();
	virtual void stopTarget();
	virtual void exitTarget();
	virtual void destroyTarget();

protected:
	// commands
	void onXmlMessage( XmlMessage *msg );
	void cmdPlayCircuit( Xml scenario );
	void cmdVerifyModel( Xml modelArea );
	void cmdCreateWikiPages( Xml wiki );

private:
	// sensor construction
	MindSensor *createEye( SensorArea *sensorArea );
	MindSensor *createEar( SensorArea *sensorArea );
	MindSensor *createNose( SensorArea *sensorArea );

	// effector construction
	MindEffector *createHeart( EffectorArea *area );
	MindEffector *createProtector( EffectorArea *area );
	MindEffector *createFeeder( EffectorArea *area );
	MindEffector *createSpeaker( EffectorArea *area );
	MindEffector *createFace( EffectorArea *area );
	MindEffector *createNeck( EffectorArea *area );
	MindEffector *createLung( EffectorArea *area );
	MindEffector *createLiver( EffectorArea *area );
	MindEffector *createStomach( EffectorArea *area );
	MindEffector *createHand( EffectorArea *area );
	MindEffector *createLeg( EffectorArea *area );
	MindEffector *createTrunk( EffectorArea *area );
	MindEffector *createBreast( EffectorArea *area );
	MindEffector *createGenitalia( EffectorArea *area );
	MindEffector *createRectum( EffectorArea *area );
	MindEffector *createUreter( EffectorArea *area );
	MindEffector *createVessels( EffectorArea *area );
	MindEffector *createEndocrine( EffectorArea *area );
};

/*#########################################################################*/
/*#########################################################################*/
