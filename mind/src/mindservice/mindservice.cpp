#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

neurovt_state	NEURON_SYNAPTIC_THRESHOLD_INITIAL_pQ;
neurovt_state	NEURON_SYNAPTIC_THRESHOLD_MIN_pQ;
neurovt_state	NEURON_ACTION_POTENTIAL_BY_SIGNAL_pQ;
neurovt_state	NEURON_MEMBRANE_POTENTIAL_BY_ACTION_POTENTIAL_pQ;
neurovt_state	NEURON_MEMBRANE_THRESHOLD_INITIAL_pQ;
neurovt_state	NEURON_FIRE_OUTPUT_BY_MEMBRANE_POTENTIAL_pQ;
neurovt_state	NEURON_INHIBIT_DELAY_ms;
neurovt_state	NEURON_FIRE_OUTPUT_SILENT_ms;
neurovt_state	NEURON_FIRE_IMPULSE_pQ;
neurovt_state	NEURON_POTENTIAL_DISCHARGE_RATE_pQ_per_ms;
neurovt_state	NEURON_OUTPUT_DISCHARGE_RATE_pQ_per_ms;
int				NEURON_CONNECTIVITY_UPDATE_FACTOR;
RFC_INT64		NEURON_FULL_RELAX_ms;

/*#########################################################################*/
/*#########################################################################*/

Service *MindService::newService() {
	return( new MindService() );
}

MindService::MindService() {
	lockStructure = rfc_hnd_semcreate();

	// construct items
	mindSpace = NULL;
	mindMap = NULL;
	areaSet = NULL;
	regionSet = NULL;
	linkSet = NULL;
	activeMemory = NULL;

	target = NULL;
	session = NULL;
	areaIdSeq = 0;
	regionIdSeq = 0;
}

void MindService::configureService( Xml p_config ) {
	config = p_config;

	// variables
	Xml configVar = config.getChildNode( "neural-networks" );
	NEURON_SYNAPTIC_THRESHOLD_INITIAL_pQ = configVar.getIntProperty( "NEURON_SYNAPTIC_THRESHOLD_INITIAL_pQ" );
	NEURON_SYNAPTIC_THRESHOLD_MIN_pQ = configVar.getIntProperty( "NEURON_SYNAPTIC_THRESHOLD_MIN_pQ" );
	NEURON_ACTION_POTENTIAL_BY_SIGNAL_pQ = configVar.getIntProperty( "NEURON_ACTION_POTENTIAL_BY_SIGNAL_pQ" );
	NEURON_MEMBRANE_POTENTIAL_BY_ACTION_POTENTIAL_pQ = configVar.getIntProperty( "NEURON_MEMBRANE_POTENTIAL_BY_ACTION_POTENTIAL_pQ" );
	NEURON_MEMBRANE_THRESHOLD_INITIAL_pQ = configVar.getIntProperty( "NEURON_MEMBRANE_THRESHOLD_INITIAL_pQ" );
	NEURON_FIRE_OUTPUT_BY_MEMBRANE_POTENTIAL_pQ = configVar.getIntProperty( "NEURON_FIRE_OUTPUT_BY_MEMBRANE_POTENTIAL_pQ" );
	NEURON_INHIBIT_DELAY_ms = configVar.getIntProperty( "NEURON_INHIBIT_DELAY_ms" );
	NEURON_FIRE_OUTPUT_SILENT_ms = configVar.getIntProperty( "NEURON_FIRE_OUTPUT_SILENT_ms" );
	NEURON_FIRE_IMPULSE_pQ = configVar.getIntProperty( "NEURON_FIRE_IMPULSE_pQ" );
	NEURON_POTENTIAL_DISCHARGE_RATE_pQ_per_ms = configVar.getIntProperty( "NEURON_POTENTIAL_DISCHARGE_RATE_pQ_per_ms" );
	NEURON_OUTPUT_DISCHARGE_RATE_pQ_per_ms = configVar.getIntProperty( "NEURON_OUTPUT_DISCHARGE_RATE_pQ_per_ms" );
	NEURON_CONNECTIVITY_UPDATE_FACTOR = configVar.getIntProperty( "NEURON_CONNECTIVITY_UPDATE_FACTOR" );
	NEURON_FULL_RELAX_ms = configVar.getIntProperty( "NEURON_FULL_RELAX_ms" );
}

void MindService::createService() {
	// construct items
	mindSpace = new MindSpace();
	mindMap = new MindMap();
	areaSet = new MindAreaSet();
	regionSet = new MindRegionSet();
	linkSet = new MindAreaLinkSet();
	activeMemory = new MindActiveMemory();

	// create mind space
	logger.logInfo( "createService: creating mind space..." );
	Xml xmlMindSpace = config.getFirstChild( "mind-space" );
	ASSERTMSG( xmlMindSpace.exists() , "createService: mind-space is not present in mind configuration" );
	mindSpace -> createFromXml( xmlMindSpace );

	// load mind map
	logger.logInfo( "createService: creating mind map..." );
	Xml xmlMindMap = config.getFirstChild( "mind-map" );
	ASSERTMSG( xmlMindMap.exists() , "createService: mind-map is not present in mind configuration" );
	mindMap -> createFromXml( xmlMindMap );

	// create areas
	createAreas();
}

void MindService::initService() {
	// create active memory
	Xml xmlActiveMemory = config.getFirstChild( "active-memory" );
	ASSERTMSG( xmlActiveMemory.exists() , "createService: active-memory is not present in brain configuration file" );
	activeMemory -> create( xmlActiveMemory );

	// create messaging session for mind services
	MessagingService *ms = MessagingService::getService();
	session = ms -> createSession();

	// create links
	establishAreaLinks();
}

void MindService::runService() {
	// set initial thinking
	areaSet -> wakeupAreaSet( activeMemory );

	// start thinking
	activeMemory -> start();
}

void MindService::stopService() {
	// sent areas to sleep
	areaSet -> suspendAreaSet();

	// stop thinking
	activeMemory -> stop();
}

void MindService::exitService() {
	// exit areas
	areaSet -> exitAreaSet();

	// stop session
	if( session != NULL ) {
		MessagingService *ms = MessagingService::getService();
		ms -> closeSession( session );
		session = NULL;
	}
}

void MindService::destroyService() {
	areaSet -> destroyAreaSet();

	delete mindSpace;
	delete mindMap;
	delete areaSet;
	delete regionSet;
	delete linkSet;
	delete activeMemory;

	rfc_hnd_semdestroy( lockStructure );
}

void MindService::createAreas() {
	// construct configured area set
	ClassList<MindAreaDef>& areaDefSet = mindMap -> getMindAreas();
	for( int k = 0; k < areaDefSet.count(); k++ ) {
		MindAreaDef *areaInfo = areaDefSet.get( k );
		createArea( areaInfo );
	}
}

void MindService::createArea( MindAreaDef *areaInfo ) {
	// check need running
	if( !areaInfo -> runEnabled() ) {
		logger.logInfo( "createArea: mind area is ignored, disabled in mind configuration name=" + areaInfo -> getAreaId() );
		return;
	}

	// construct area
	logger.logInfo( "createArea: create mind area id=" + areaInfo -> getAreaId() + " ..." );
	MindArea *area = new MindArea();
	area -> configure( areaInfo );

	area -> createRegions( target );
	addArea( area );
}

void MindService::addArea( MindArea *area ) {
	// add to list
	MindRegionSet *areaRegions = area -> getRegionSet();
	regionSet -> addRegionSet( areaRegions );
	areaSet -> addMindArea( area );
}

MapStringToClass<MindArea>& MindService::getMindAreas() {
	return( areaSet -> getMindAreas() );
}

MindArea *MindService::getMindArea( String areaId ) {
	return( areaSet -> getMindArea( areaId ) );
}

MindRegion *MindService::getMindRegion( String regionId ) {
	MindRegion *region = regionSet -> getSetItemById( regionId );
	ASSERTMSG( region != NULL , "getMindRegion: region is not found by id=" + regionId );
	return( region );
}

bool MindService::isMindRegion( String regionId ) {
	MindRegion *region = regionSet -> getSetItemById( regionId );
	if( region == NULL )
		return( false );
	return( true );
}

MindRegion *MindService::createRegion( String implementation , String type , MindArea *area , MindRegionCreateInfo *info ) {
	logger.logInfo( "createRegion: create mind region id=" + info -> getId() + " ..." );

	MindRegion *region = NULL;
	if( implementation.equals( "original" ) ) {
		if( type.equals( "neocortex" ) )
			region = new NeocortexRegion( area );
		else if( type.equals( "allocortex" ) )
			region = new AllocortexRegion( area );
		else if( type.equals( "nucleus" ) )
			region = new NucleiRegion( area );
		else
			ASSERTFAILED( "unknown region type=" + type );
	}
	else if( implementation.equals( "mock" ) ) {
		region = new MockRegion( type , area );
	}

	region -> setRegionId( info -> getId() );
	region -> setRegionInfo( info -> getInfo() );
	region -> setRegionType( info -> getType() );
	region -> createRegion( info );

	return( region );
}

void MindService::setMindTarget( MindTarget *p_target ) {
	target = p_target;
}

// mind links
void MindService::establishAreaLinks() {
	ClassList<MindCircuitDef>& circuits = mindMap -> getMindCircuits();
	for( int k = 0; k < circuits.count(); k++ )
		createCircuitLinks( circuits.get( k ) );
}

void MindService::createCircuitLinks( MindCircuitDef *circuitDef ) {
	logger.logInfo( "createCircuitLinks: create cicuit id=" + circuitDef -> getId() );

	ClassList<MindCircuitConnectionDef>& connections = circuitDef -> getConnections();
	for( int k = 0; k < connections.count(); k++ )
		createCircuitConnection( circuitDef , connections.get( k ) );
}

void MindService::createCircuitConnection( MindCircuitDef *circuitDef , MindCircuitConnectionDef *connectionDef ) {
	// get/create area link
	String srcRegionName = connectionDef -> getSrcRegion();
	MindRegion *srcRegion = regionSet -> getSetItemById( srcRegionName );
	ASSERTMSG( srcRegion != NULL , "circuit=" + circuitDef -> getName() + ": unknown src region=" + srcRegionName );
	String dstRegionName = connectionDef -> getDstRegion();
	MindRegion *dstRegion = regionSet -> getSetItemById( dstRegionName );
	ASSERTMSG( dstRegion != NULL , "circuit=" + circuitDef -> getName() + ": unknown dst region=" + dstRegionName );
	ASSERTMSG( srcRegion != dstRegion , "circuit=" + circuitDef -> getName() + ": cannot connect region to itself, name=" + dstRegionName );

	// check link like this is already created
	String type = connectionDef -> getTypeName();
	String key = type + "." + srcRegionName + "." + dstRegionName;
	if( regionConnectionMap.get( key ) != NULL )
		return;

	// create
	MindConnectionTypeDef *connectionType = connectionDef -> getType();
	ASSERTMSG( connectionType != NULL , "connectionType is null in circuit connection key=" + key );
	createRegionConnection( connectionType , srcRegion , dstRegion , connectionDef -> isPrimary() );

	// add to map
	regionConnectionMap.add( key , connectionType );
}

void MindService::createRegionConnection( MindConnectionTypeDef *connectionType , MindRegion *srcRegion , MindRegion *dstRegion , bool primary ) {
	ClassList<MindConnectionLinkTypeDef>& links = connectionType -> getLinks();
	for( int k = 0; k < links.count(); k++ )
		createNeuroLink( connectionType , links.get( k ) , srcRegion , dstRegion , primary );
}

NeuroLink *MindService::createNeuroLink( MindConnectionTypeDef *connectionTypeDef , MindConnectionLinkTypeDef *linkDef , MindRegion *srcRegion , MindRegion *dstRegion , bool primary ) {
	// handle direction
	MindRegion *linkSrcRegion = ( linkDef -> isBackward() )? dstRegion : srcRegion;
	MindRegion *linkDstRegion = ( linkDef -> isBackward() )? srcRegion : dstRegion;

	// check pools
	NeuroLinkSource *srcConnector = linkSrcRegion -> getNeuroLinkSource( linkDef -> getSrcConnector() );
	NeuroLinkTarget *dstConnector = linkDstRegion -> getNeuroLinkTarget( linkDef -> getDstConnector() );

	// ignore if any connector is not present
	if( srcConnector == NULL || dstConnector == NULL )
		return( NULL );

	// create forward region link - as specified in network connection, though neurolink can be of backward direction
	MindRegionLink *regionLink = createRegionLink( connectionTypeDef , srcRegion , dstRegion , primary );

	// check neurolink exists - by neurolink type and region pair, note that different connection types can produce the same link and duplicates will be avoided
	String linkType = linkDef -> getName();
	String key = linkType + "-" + linkSrcRegion -> getRegionId() + "-" + linkDstRegion -> getRegionId();
	NeuroLink *neurolink = regionNeuroLinkMap.get( key );
	if( neurolink != NULL ) {
		if( primary )
			neurolink -> setPrimary( primary );
		return( neurolink );
	}

	// create neurolink
	NeuroLinkInfo info;
	info.setLinkDef( linkDef );
	info.setNeuroTransmitter( linkDef -> getNeurotransmitter() );
	info.setRegionLink( regionLink );
	neurolink = createNeuroLink( linkDef -> getImplementation() , linkDef -> getType() , srcConnector , dstConnector , &info );
	if( neurolink == NULL )
		return( NULL );

	neurolink -> setPrimary( primary );
	regionNeuroLinkMap.add( key , neurolink );
	regionLink -> addNeuroLink( neurolink );

	logger.logInfo( "createNeuroLink: neurolink created srcRegion=" + linkSrcRegion -> getRegionId() + 
		", dstRegion=" + linkDstRegion -> getRegionId() + 
		", type=" + linkType +
		", direction=" + ( ( linkDef -> isBackward() )? "backward" : "forward" ) + 
		", implementation=" + linkDef -> getImplementation() );

	return( neurolink );
}

MindRegionLink *MindService::createRegionLink( MindConnectionTypeDef *connectionType , MindRegion *srcRegion , MindRegion *dstRegion , bool primary ) {
	// create area link if areas are different
	MindArea *linkSrcArea = srcRegion -> getArea();
	MindArea *linkDstArea = dstRegion -> getArea();
	MindAreaLink *areaLink = NULL;
	if( linkSrcArea != linkDstArea )
		areaLink = createAreaLink( linkSrcArea , linkDstArea );

	// check already created
	String key = srcRegion -> getRegionId() + "." + dstRegion -> getRegionId();
	MindRegionLink *regionLink = regionLinkMap.get( key );
	if( regionLink != NULL ) {
		if( primary )
			regionLink -> setPrimary( true );
		return( regionLink );
	}

	// create region link
	regionLink = new MindRegionLink( areaLink );
	regionLink -> setPrimary( primary );
	regionLink -> createRegionLink( connectionType , srcRegion , dstRegion );
	regionLinkMap.add( key , regionLink );
	
	// add to area link
	if( areaLink != NULL )
		areaLink -> addRegionLink( regionLink );
	else
		linkSrcArea -> addInternalRegionLink( regionLink );

	// add to region link
	srcRegion -> addSlaveRegionLink( regionLink );
	dstRegion -> addMasterRegionLink( regionLink );

	return( regionLink );
}

MindAreaLink *MindService::createAreaLink( MindArea *masterArea , MindArea *slaveArea ) {
	// check area link exists
	String key = masterArea -> getAreaId() + "." + slaveArea -> getAreaId();
	MindAreaLink *link = areaLinkMap.get( key );
	if( link != NULL )
		return( link );

	// create link
	link = new MindAreaLink;
	link -> create( masterArea , slaveArea );
	areaLinkMap.add( key , link );

	// add to link set
	masterArea -> addSlaveLink( link );
	slaveArea -> addMasterLink( link );
	linkSet -> addSetItem( link );
	logger.logInfo( "createMindAreaLink: create link masterArea=" + masterArea -> getAreaId() + ", slaveArea=" + slaveArea -> getAreaId() + "..." );

	// start process area link messages
	String channel = "CHANNEL." + masterArea -> getAreaId() + "." + slaveArea -> getAreaId();
	link -> open( session , channel );
	return( link );
}

NeuroLink *MindService::createNeuroLink( String implementation , String typeName , NeuroLinkSource *src , NeuroLinkTarget *dst , NeuroLinkInfo *info ) {
	NeuroLink *link = NULL;
	if( implementation.equals( "original" ) ) {
		if( typeName.equals( "excitatory" ) )
			link = new ExcitatoryLink( src , dst );
		else if( typeName.equals( "inhibitory" ) )
			link = new InhibitoryLink( src , dst );
		else if( typeName.equals( "modulatory" ) )
			link = new ModulatoryLink( src , dst );
		else
			ASSERTFAILED( "unknown region type=" + typeName );
	}
	else if( implementation.equals( "mock" ) ) {
		link = new MockLink( typeName , src , dst );
	}

	link -> createNeuroLink( info );
	src -> addNeuroLink( link );
	dst -> addNeuroLink( link );

	return( link );
}

