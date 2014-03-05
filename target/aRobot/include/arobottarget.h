/*#########################################################################*/
/*#########################################################################*/

#include <mind/include/ah_mind.h>

class ARobotTarget : public MindTarget {
public:
	ARobotTarget();

	// target lifecycle
	virtual void configureTarget( Xml config );
	virtual void createTarget( SensorArea *sensorArea , EffectorArea *effectorArea );
	virtual void initSensorsTarget( SensorArea *sensorArea );
	virtual void initEffectorsTarget( EffectorArea *effectorArea );
	virtual void runTarget();
	virtual void stopTarget();
	virtual void exitTarget();
	virtual void destroyTarget();

public:
	MindSensor *createEye( SensorArea *area );
	MindEffector *createWheeledLegs( EffectorArea *area );

private:
	NxtPP nxt;
};

/*#########################################################################*/
/*#########################################################################*/
