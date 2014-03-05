#ifndef	INCLUDE_AH_MINDSTREAM_H
#define INCLUDE_AH_MINDSTREAM_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_mindbase.h"

class MindAreaLink;
class MindAreaLinkSet;
class MindRegionLink;
class MindRegionLinkSet;
class MindActiveMemory;
class MindMessage;
class NeuroLinkInfo;

/*#########################################################################*/
/*#########################################################################*/

class MindArea;
class MindRegion;
class NeuroLinkSet;
class MindActiveMemoryObject;

/*#########################################################################*/
/*#########################################################################*/

// created by static setup on start
// however each inter-area connection has its own lifecycle in terms of connection details
// connection details declare which cortexes are connected, and specific interneuron connections in these intercortex connections can emerge or disappear
// inter-area connection connects outputs of one area and inputs of another area
// dynamics of connection is directed by brain
// how inputs/outputs are connected to cortexes - it is defined by related component
class MindAreaLink : public Object , public MessageSubscriber {
public:
	MindAreaLink();
	~MindAreaLink();
	const char *getClass() { return( "MindAreaLink" ); };

	void create( MindArea *p_masterArea , MindArea *p_slaveArea );

// operations
public:
	virtual void onMessage( Message *msg );

public:
	void addRegionLink( MindRegionLink *link );
	void open( MessageSession *session , String channel );
	void sendMessage( MindMessage *msg );

	MindArea *getMasterArea() { return( masterArea ); };
	MindArea *getSlaveArea() { return( slaveArea ); };
	MessagePublisher *getPublisher() { return( iopub ); };
	MindRegionLinkSet *getRegionLinkSet() { return( links ); };

	ClassList<MindRegionLink>& getRegionLinks();
	
private:
// utility
	MessageSession *session;
	MessagePublisher *iopub;
	MessageSubscription *iosub;

// own data
	MindRegionLinkSet *links;

// references
	MindActiveMemory *activeMemory;
	MindArea *masterArea;
	MindArea *slaveArea;
};

/*#########################################################################*/
/*#########################################################################*/

class MindAreaLinkSet : public Object {
public:
	const char *getClass() { return( "MindAreaLinkSet" ); };

	void addSetItem( MindAreaLink *link );
	ClassList<MindAreaLink>& getLinks() { return( list ); };

public:
// own data
	ClassList<MindAreaLink> list;
};

/*#########################################################################*/
/*#########################################################################*/

class MindRegionLink : public Object {
public:
	MindRegionLink( MindAreaLink *areaLink );
	const char *getClass() { return( "MindRegionLink" ); };

	String getId() { return( id ); };
	MindRegion *getSrcRegion();
	MindRegion *getDstRegion();
	MindAreaLink *getAreaLink() { return( areaLink ); };
	MindConnectionTypeDef *getConnectionType() { return( connectionType ); };
	void setPrimary( bool p_primary ) { primary = p_primary; };
	bool isPrimary() { return( primary ); };

	void createRegionLink( MindConnectionTypeDef *def , MindRegion *srcRegion , MindRegion *dstRegion );
	void exitRegionLink();
	void destroyRegionLink();

	void addNeuroLink( NeuroLink *link );

private:
// references
	MindAreaLink *areaLink;
	MindConnectionTypeDef *connectionType;

	bool primary;
	MindRegion *src;
	MindRegion *dst;

	// links within region link
	ClassList<NeuroLink> links;

// utilities
	String id;
};

/*#########################################################################*/
/*#########################################################################*/

class MindRegionLinkSet : public Object {
public:
	const char *getClass() { return( "MindRegionLinkSet" ); };

	void addSetItem( MindRegionLink *link );
	void exitRegionLinkSet();
	void destroyRegionLinkSet();
	int getCount();
	MindRegionLink *getSetItem( int k );

	ClassList<MindRegionLink>& getLinks() { return( list ); };
	bool checkLinked( MindRegion *src , MindRegion *dst );

public:
// own data
	ClassList<MindRegionLink> list;
};

/*#########################################################################*/
/*#########################################################################*/

class MindActiveMemory : public Object {
public:
	MindActiveMemory();
	~MindActiveMemory();
	const char *getClass() { return( "MindActiveMemory" ); };

	// lifecycle
	void create( Xml config );
	void start();
	void stop();
	void destroy();

	// task execution
	void execute( MindMessage *msg );

public:
// utility
	String threadPoolName;

// own data
	ClassList<MindActiveMemoryObject> memoryObjects;
};

/*#########################################################################*/
/*#########################################################################*/

// class to snapshot outputs from cortex and transfer to other cortexes
class MindMessage : public Message {
public:
	MindMessage( NeuroLink *p_link );
	MindMessage( NeuroLink *p_link , NeuroSignal *p_data );
	virtual ~MindMessage();
	virtual const char *getClass() { return( "MindMessage" ); };

public:
	NeuroLink *getNeuroLink() { return( link ); };
	NeuroSignal *getMsgData();
	MindAreaLink *getAreaLink() { return( areaLink ); };

	void send();

private:
// own data
	NeuroSignal *data;

// references
	MindAreaLink *areaLink;
	NeuroLink *link;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_MINDSTREAM_H
