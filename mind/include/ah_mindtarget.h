#ifndef	INCLUDE_AH_MINDTARGET_H
#define INCLUDE_AH_MINDTARGET_H

/*#########################################################################*/
/*#########################################################################*/

#include <platform/include/ah_services.h>
#include "ah_mindarea.h"
#include "ah_mindregion.h"
#include "ah_mindmatter.h"

class MindTarget;
class MindSensorSet;
class MindEffectorSet;
class MindSensor;
class MindEffector;
class SensorArea;
class EffectorArea;

class TargetAreaDef;

/*#########################################################################*/
/*#########################################################################*/

class MindSensorSetTracker;
class MindArea;

class MindTarget : public Service {
public:
	MindTarget();

	virtual void configureTarget( Xml config ) = 0;
	virtual void createTarget( SensorArea *sensorArea , EffectorArea *effectorArea ) = 0;
	virtual void initTarget() = 0;
	virtual void runTarget() = 0;
	virtual void stopTarget() = 0;
	virtual void exitTarget() = 0;
	virtual void destroyTarget() = 0;

private:
	// target service virtuals
	virtual const char *getServiceName() { return( "MindTarget" ); };
	virtual void configureService( Xml config );
	virtual void createService();
	virtual void initService();
	virtual void runService();
	virtual void stopService();
	virtual void exitService();
	virtual void destroyService();

public:
	void addSensor( String name , MindSensor *sensor );
	void addEffector( String name , MindEffector *effector );

	SensorArea *getSensorArea() { return( sensorArea ); };
	EffectorArea *getEffectorArea() { return( effectorArea ); };
	
private:
// utility
	Xml config;

// own data
	SensorArea *sensorArea;
	EffectorArea *effectorArea;

// references
	MindSensorSetTracker *sensorTracker;
};

/*#########################################################################*/
/*#########################################################################*/

class MindSensorSet : public Object {
public:
	MindSensorSet( SensorArea *area );
	virtual const char *getClass() { return( "MindSensorSet" ); };

	// lifecycle
	void createSensorSet();
	void startSensorSet();
	void stopSensorSet();

public:
	int getCount();
	MindSensor *getSetItem( int k );
	void addSetItem( MindSensor *sensor );

	void pollSensorSet( int timeNowMs , int *minPollNextMs );
	MindSensor *getSensor( String name );

private:
// own data
	ClassList<MindSensor> list;

// references
	SensorArea *area;
	MapStringToClass<MindSensor> map;
};

/*#########################################################################*/
/*#########################################################################*/

class MindEffectorSet : public Object {
public:
	MindEffectorSet( EffectorArea *area );
	virtual const char *getClass() { return( "MindEffectorSet" ); };

	// lifecycle
	void createEffectorSet();
	void startEffectorSet();
	void stopEffectorSet();

public:
	int getCount();
	MindEffector *getSetItem( int k );
	void addSetItem( MindEffector *effector );

	MindEffector *getEffector( String name );

private:
// own data
	ClassList<MindEffector> list;

// references
	EffectorArea *area;
	MapStringToClass<MindEffector> map;
};

/*#########################################################################*/
/*#########################################################################*/

class MindSensor : public MindRegion {
public:
	MindSensor( SensorArea *p_area );

	// Object
	virtual const char *getClass() = 0;

	// MindSensor
	virtual void createSensor( MindRegionCreateInfo *info ) = 0;
	virtual void configureSensor( Xml config ) = 0;
	virtual void startSensor() = 0;
	virtual void stopSensor() = 0;
	virtual void processSensorControl( NeuroLink *link , NeuroSignal *signal ) = 0;
	virtual void produceSensorData() = 0;
	virtual void pollSensor() = 0;

private:
	// MindRegion
	virtual void createRegion( MindRegionCreateInfo *info );
	virtual void exitRegion();
	virtual void destroyRegion();

public:
	void configure( Xml config );
	Xml getConfig() { return( config ); };

	void setSensorInfo( TargetRegionDef *info ) { targetInfo = info; };
	TargetRegionDef *getSensorInfo() { return( targetInfo ); };
	void setSensorId( String p_id ) { sensorId = p_id; };
	String getSensorId() { return( sensorId ); };

	// capture data and and send via NeuroLink
	NeuroSignal *getSensorySignal( NeuroLinkSource *src );
	void processSensorData( NeuroLinkSource *src , String id );
	void applySensorControl( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *srcData );

	// poll handling
	void setPollState( bool state );
	bool getPollState();
	int getPollIntervalMs( int timeNowMs );

private:
// references
	TargetRegionDef *targetInfo;

// utility
	String sensorId;
	Xml config;
	int msgId;
	// auto-polling
	bool pollState;
	int pollNextMs;
	int pollIntervalMs;
};

/*#########################################################################*/
/*#########################################################################*/

class MindEffector : public MindRegion {
public:
	MindEffector( EffectorArea *p_area );

	// Object
	virtual const char *getClass() = 0;

	// MindEffector
	virtual void createEffector( MindRegionCreateInfo *info ) = 0;
	virtual void configureEffector( Xml config ) = 0;
	virtual void startEffector() = 0;
	virtual void stopEffector() = 0;
	virtual void processEffectorControl( NeuroLink *link , NeuroSignal *signal ) = 0;

public:
	void setEffectorId( String p_id ) { effectorId = p_id; };
	String getEffectorId() { return( effectorId ); };

	void setEffectorInfo( TargetRegionDef *info ) { targetInfo = info; };
	TargetRegionDef *getEffectorInfo() { return( targetInfo ); };

private:
	// MindRegion
	virtual void createRegion( MindRegionCreateInfo *info );
	virtual void exitRegion();
	virtual void destroyRegion();

private:
// references
	TargetRegionDef *targetInfo;

// utility
	String effectorId;
};

/*#########################################################################*/
/*#########################################################################*/

class MindRegion;
class MindRegionLink;
class MindAreaNet;

class SensorArea : public MindArea {
public:
	SensorArea( MindTarget *target );
	virtual ~SensorArea();
	virtual const char *getClass() { return( "SensorArea" ); };

public:
	// lifecycle
	void createSensorArea();
	void initSensorArea();
	void startSensorArea();
	void stopSensorArea();

	// operations
	MindTarget *getTarget() { return( target ); };
	MindSensorSet *getSensors() { return( sensors ); };

	void addSensor( MindSensor *sensor , bool offline );
	MindSensor *getSensor( String name );

private:
	void createSensor( MindSensor *sensor );
	void createSensorLinks( MindSensor *sensor );

private:
// references
	MindTarget *target;

// own data
	MindSensorSet *sensors;
	MindSensorSet *sensorsOffline;
};

/*#########################################################################*/
/*#########################################################################*/

class EffectorArea : public MindArea {
public:
	EffectorArea( MindTarget *target );
	virtual ~EffectorArea();
	virtual const char *getClass() { return( "EffectorArea" ); };

public:
	// lifecycle
	void createEffectorArea();
	void initEffectorArea();
	void startEffectorArea();
	void stopEffectorArea();

	// operations
	MindTarget *getTarget() { return( target ); };
	MindEffectorSet *getEffectors() { return( effectors ); };

	void addEffector( MindEffector *effector , bool offline );
	MindEffector *getEffector( String name );

private:
	void createEffector( MindEffector *effector );
	void createEffectorLinks( MindEffector *effector );

private:
// references
	MindTarget *target;

// own data
	MindEffectorSet *effectors;
	MindEffectorSet *effectorsOffline;
};

// #############################################################################
// #############################################################################

/*
lifecycle:

	virtual void MindTarget::Service::configureService( Xml config );
		virtual void FinalTarget::MindTarget::configureTarget( Xml config ) = 0;
	virtual void MindTarget::Service::createService();
		void SensorArea::createSensorArea( MindTarget *target );
		virtual void FinalTarget::MindTarget::createTarget( SensorArea *sensorArea , EffectorArea *effectorArea ) = 0;
			void MindTarget::addSensor( String name , MindSensor *sensor );
				virtual void MindSensor::configureSensor( Xml config ) = 0;
	virtual void MindTarget::Service::initService();
		void SensorArea::initSensorArea();
			virtual void MindSensor::MindRegion::createRegion( MindRegionCreateInfo *info );
				virtual void MindSensor::createSensor( MindRegionCreateInfo *info ) = 0;
			void SensorArea::createSensorLinks();
		virtual void FinalTarget::MindTarget::initTarget() = 0;
	virtual void MindTarget::Service::runService();
		void SensorArea::startSensorArea();
			virtual void MindSensor::startSensor() = 0;
		virtual void FinalTarget::MindTarget::runTarget() = 0;
		virtual void MindSensor::processSensorControl( NeuroLink *link , NeuroSignal *signal ) = 0;
		virtual void MindSensor::produceSensorData() = 0;
		virtual void MindSensor::pollSensor() = 0;
	virtual void MindTarget::Service::stopService();
		void SensorArea::stopSensorArea();
			virtual void MindSensor::stopSensor() = 0;
		virtual void FinalTarget::MindTarget::stopTarget() = 0;
	virtual void MindTarget::Service::exitService();
		virtual void FinalTarget::MindTarget::exitTarget() = 0;
			virtual void MindSensor::MindRegion::exitRegion();
	virtual void MindTarget::Service::destroyService();
		virtual void FinalTarget::MindTarget::destroyTarget() = 0;
			virtual void MindSensor::MindRegion::destroyRegion();
			
*/

#endif // INCLUDE_AH_MINDTARGET_H
