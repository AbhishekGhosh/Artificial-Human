#ifndef	INCLUDE_AH_MINDMATTER_H
#define INCLUDE_AH_MINDMATTER_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_mindbase.h"

class NeuroSignal;
class NeuroSignalSet;
class NeuroPool;
class NeuroPoolSet;
class NeuroLink;
class NeuroLinkSet;
class NeuroLinkSource;
class NeuroLinkTarget;
class NeuroLinkInfo;

/*#########################################################################*/
/*#########################################################################*/

class MindConnectionTypeDef;
class MindRegionLink;
class MindRegion;

/*#########################################################################*/
/*#########################################################################*/

typedef struct {
	neurovt_state synaptic_potential;	// accumulated action potential in axon terminal
	neurovt_state synaptic_threshold;	// synapse penetration threshold of accumulated action potential, initially NEURON_SYNAPTIC_THRESHOLD_pQ
	neurovt_state membrane_potential;	// accumulated membrane potential
	neurovt_state membrane_threshold;	// fire threshold of membrane potential, depends on neuron type and size
	neurovt_state firepower;			// if fired, output signal power
	RFC_INT64 updated_mp;				// membrane potential updated timestamp
	RFC_INT64 silent_till;				// should be silent till given timestamp
} NEURON_DATA;

/*#########################################################################*/
/*#########################################################################*/

class NeuroSignal : public Object {
public:
	virtual const char *getClass() { return( "NeuroSignal" ); };

	NeuroSignal();
	NeuroSignal( int sizeX , int sizeY );
	NeuroSignal( NeuroSignal *src );
	~NeuroSignal();

public:
	void create( int sizeX , int sizeY );
	void createFromPool( NeuroPool *pool );
	void copyDataFromSignal( NeuroSignal *signal );
	void setIndexData( int *indexData , int n );

	void setTs( RFC_INT64 ts );
	void setId( String id );
	void setSource( NeuroLinkSource *source );

	String getId() { return( id ); };
	NeuroLinkSource *getSource() { return( source ); };

	void getSizeInfo( int *nx , int *ny );
	int getMaxSize();
	int getDataSize();
	String getBinaryDataString();
	String getNumberDataString();
	int *getIndexRawData();
	String getCombinedState( NeuroSignal *srcSignal1 , NeuroSignal *srcSignal2 );

	void clearData();
	void addIndexData( int index );
	void addIndexDataSorted( NeuroSignal *srcSignal );
	void arrangeNormal();
	void removeNotFiringIndexData();

private:
// utility
	String id;
	RFC_INT64 ts;

	// original 2D index space
	int sizeX;
	int sizeY;

	// sorted list of activated 1D-indexes in source data
	FlatList<int> data;

// references
	NeuroLinkSource *source;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroSignalSet : public Object {
public:
	virtual const char *getClass() { return( "NeuroSignalSet" ); };

	NeuroSignalSet();
	~NeuroSignalSet();

public:
	void addSetItem( NeuroSignal *signal );
	void addSetItem( NeuroLinkSource *source , NeuroSignal *signal );
	void addSetItem( MindRegion *region , String sourceConnector , NeuroSignal *signal );
	void addSet( NeuroSignalSet *signalSet );
	void moveTo( NeuroSignalSet *signalSet );
	void clear();
	void destroy();

	bool isEmpty();
	ClassList<NeuroSignal>& getSignals() { return( signals ); };

private:
// own data
	ClassList<NeuroSignal> signals;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroPool : public Object {
public:
	NeuroPool();
	virtual ~NeuroPool();
	virtual const char *getClass() { return( "NeuroPool" ); };

	void createNeurons( int nx , int ny );
	void setDefaultThreshold();
	void setParent( MindRegion *p_region );
	void setId( String id );
	String getId();
	MindRegion *getRegion();

	TwoIndexArray<NEURON_DATA>& getNeuronData();
	int getNeuronDataSize();
	void getNeuronDimensions( int *nx , int *ny );
	NeuroSignal *fire( NeuroSignal *srcSignal );

private:
// own data
	NeuroSignal *pendingFire;

// references
	MindRegion *region;

// utilities
	String id;
	TwoIndexArray<NEURON_DATA> neurons;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroPoolSet : public Object {
public:
	virtual const char *getClass() { return( "NeuroPoolSet" ); };

private:
// own data
	ClassList<NeuroPool> list;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroLink : public Object {
public:
	NeuroLink( NeuroLinkSource *src , NeuroLinkTarget *dst );
	const char *getClass() { return( "NeuroLink" ); };

	virtual void createNeuroLink( NeuroLinkInfo *info ) = 0;
	virtual NeuroSignal *apply( NeuroSignal *srcData , NeuroPool *dstPool ) = 0;

public:

	// parameters
	String getId() { return( id ); };
	String getTransmitter() { return( transmitter ); };
	NeuroLinkSource *getSource() { return( source ); };
	NeuroLinkTarget *getTarget() { return( target ); };
	MindRegionLink *getRegionLink() { return( regionLink ); };
	int getSizeX() { return( sizeX ); };
	int getSizeY() { return( sizeY ); };
	void setPrimary( bool p_primary ) { primary = p_primary; };
	bool isPrimary() { return( primary ); };

	int getSize();

protected:
// utilities
	bool primary;
	String id;
	String transmitter;
	int sizeX;
	int sizeY;

// references
	MindRegionLink *regionLink;
	MindRegion *region;
	MindConnectionLinkTypeDef *linkType;
	NeuroLinkSource *source;
	NeuroLinkTarget *target;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroLinkSet : public Object {
public:
	const char *getClass() { return( "NeuroLinkSet" ); };

	void addSetItem( NeuroLink *link );

public:
// own data
	ClassList<NeuroLink> list;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroLinkSource : public Object {
public:
	NeuroLinkSource();
	virtual const char *getClass() { return( "NeuroLinkSource" ); };

public:
	void create( MindRegion *region , String entity );
	void setHandler( MindRegion::NeuroLinkSourceHandler pfn );

	MindRegion *getRegion() { return( region ); };
	int getSizeX() { return( sizeX ); };
	int getSizeY() { return( sizeY ); };
	String getEntity() { return( entity ); };
	ClassList<NeuroLink>& getLinks() { return( links ); };

	void addNeuroLink( NeuroLink *link );
	void setSourcePool( NeuroPool *pool );
	void sendMessage( NeuroSignal *sourceSignal );
	NeuroSignal *getLinkSignal( NeuroLink *link );

private:
// references
	MindRegion *region;
	String entity;
	int sizeX;
	int sizeY;
	NeuroPool *sourcePool;
	ClassList<NeuroLink> links;

// utilities
	MindRegion::NeuroLinkSourceHandler pfn;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroLinkTarget : public Object {
public:
	NeuroLinkTarget();
	virtual const char *getClass() { return( "NeuroLinkTarget" ); };

public:
	void create( MindRegion *region , String entity );
	void setHandler( MindRegion::NeuroLinkTargetHandler pfn );
	void addNeuroLink( NeuroLink *link );

	String getEntity() { return( entity ); };
	MindRegion *getRegion() { return( region ); };
	ClassList<NeuroLink>& getLinks() { return( links ); };

	NeuroSignalSet *execute( NeuroLink *link , NeuroSignal *sourceData );

private:
// references
	MindRegion *region;
	ClassList<NeuroLink> links;

// utilities
	String entity;
	MindRegion::NeuroLinkTargetHandler pfn;
};

/*#########################################################################*/
/*#########################################################################*/

class NeuroLinkInfo : public Object {
public:
	NeuroLinkInfo();
	virtual const char *getClass() { return( "NeuroLinkInfo" ); };

public:
	void setNeuroTransmitter( String nt );
	void setLinkDef( MindConnectionLinkTypeDef *linkDef );
	void setRegionLink( MindRegionLink *regionLink );

	String getNeuroTransmitter() { return( nt ); };
	MindConnectionLinkTypeDef *getLinkDef() { return( linkDef ); };
	MindRegionLink *getRegionLink() { return( regionLink ); };

private:
	String nt;
	MindConnectionLinkTypeDef *linkDef;
	MindRegionLink *regionLink;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_MINDMATTER_H
