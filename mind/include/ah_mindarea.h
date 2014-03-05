#ifndef	INCLUDE_AH_MINDAREA_H
#define INCLUDE_AH_MINDAREA_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_mindbase.h"

class MindArea;
class MindAreaSet;

/*#########################################################################*/
/*#########################################################################*/

class MindTarget;
class MindRegionSet;
class MindRegionLinkSet;
class MindAreaLink;
class MindMessage;
class MindAreaDef;
class MessageSession;
class NeuroLinkSource;
class NeuroSignal;
class NeuroSignalSet;
class MindActiveMemory;

/*#########################################################################*/
/*#########################################################################*/

// brain provides implementation for mind areas
// each mind area is implemented by module components - next level folder under mod...
// each mind area has own name and own location in the brain
// mind area has pre-defined size as number of neurons, no matter of neuron type
// inter-area connections have own lifecycle, controlled by brain
// mind area consists a set of cortexes, where each cortex is neural or belief network of any type
// a number of cortexes, their types and inter-connections are defined dynamically by related modules

class MindArea : public Object {
public:
	MindArea();
	virtual ~MindArea();
	virtual const char *getClass() { return "MindArea"; };

public:
	// mind area lifecycle
	void wakeupArea( MindActiveMemory *activeMemory );
	void suspendArea();

public:
	void configure( MindAreaDef *info );
	void createRegions( MindTarget *target );
	void exit();
	void destroy();

	void followLinks( String spykeTrainId , MindRegion *region , NeuroSignalSet *signalSet );

	String getAreaId() { return( areaId ); };
	bool isTargetArea() { return( targetArea ); };
	MindAreaDef *getMindAreaDef() { return( info ); };
	MindRegionSet *getRegionSet() { return( regionSet ); };
	MindRegionLinkSet *getInternalRegionLinkSet() { return( regionLinkSet ); };
	MindAreaLinkSet *getMasterAreaLinkSet() { return( areaMasterLinkSet ); };
	MindAreaLinkSet *getSlaveAreaLinkSet() { return( areaSlaveLinkSet ); };

	MindRegion *getRegion( String group , String groupId );
	MapStringToClass<MindRegion>& getRegions();
	ClassList<MindRegionLink>& getInternalRegionLinks();
	ClassList<MindAreaLink>& getMasterAreaLinks();
	ClassList<MindAreaLink>& getSlaveAreaLinks();

	void addRegion( MindRegion *region );
	void addInternalRegionLink( MindRegionLink *link );
	void addMasterLink( MindAreaLink *link );
	void addSlaveLink( MindAreaLink *link );

private:
	void createRegion( MindTarget *target , MindRegionDef *regionDef );
	void followLinkSource( String spykeTrainId , NeuroLinkSource *linkSource , NeuroSignal *signal );
	void followLinksInternalSet( String spykeTrainId , int& signalNum , NeuroSignalSet *signalSet , MapStringToClass<MindRegion>& regionsExecuted , 
			NeuroSignalSet& setLocalExecuted , ClassList<MindMessage>& remoteMessages );
	void followLinksInternalOne( String spykeTrainId , int& signalNum , NeuroSignal *signal , ClassList<NeuroLink>& links , MapStringToClass<MindRegion>& regionsExecuted ,
			NeuroSignalSet& setAwaitingLocalExecution , ClassList<MindMessage>& remoteMessages );

protected:
// own data
	String areaId;
	bool targetArea;
	MindRegionSet *regionSet;
	MindRegionLinkSet *regionLinkSet;

// references
	MindAreaDef *info;
	MindAreaLinkSet *areaMasterLinkSet;
	MindAreaLinkSet *areaSlaveLinkSet;
};

/*#########################################################################*/
/*#########################################################################*/

class MindAreaSet : public Object {
public:
	virtual const char *getClass() { return( "MindAreaSet" ); };

	void addMindArea( MindArea *area );
	MindArea *getMindArea( String id );
	MapStringToClass<MindArea>& getMindAreas() { return( map ); };

	void wakeupAreaSet( MindActiveMemory *activeMemory );
	void suspendAreaSet();
	void exitAreaSet();
	void destroyAreaSet();

public:
// own data
	ClassList<MindArea> list;

// references
	MapStringToClass<MindArea> map;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_MINDAREA_H
