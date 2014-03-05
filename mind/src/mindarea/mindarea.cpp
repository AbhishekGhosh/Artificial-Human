#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindArea::MindArea() {
	info = NULL;
	targetArea = false;
	regionSet = new MindRegionSet;
	regionLinkSet = new MindRegionLinkSet;
	areaMasterLinkSet = new MindAreaLinkSet;
	areaSlaveLinkSet = new MindAreaLinkSet;
}

MindArea::~MindArea() {
	delete regionSet;
	delete regionLinkSet;
	delete areaMasterLinkSet;
	delete areaSlaveLinkSet;
}

void MindArea::configure( MindAreaDef *p_info ) {
	info = p_info;
	areaId = info -> getAreaId();
	Object::setInstance( areaId );
}

MapStringToClass<MindRegion>& MindArea::getRegions() {
	return( regionSet -> getRegions() );
}

ClassList<MindRegionLink>& MindArea::getInternalRegionLinks() {
	return( regionLinkSet -> getLinks() );
}

ClassList<MindAreaLink>& MindArea::getMasterAreaLinks() {
	return( areaMasterLinkSet -> getLinks() );
}

ClassList<MindAreaLink>& MindArea::getSlaveAreaLinks() {
	return( areaSlaveLinkSet -> getLinks() );
}

void MindArea::createRegions( MindTarget *target ) {
	// create regions
	ClassList<MindRegionDef>& regionDefs = info -> getRegions();
	for( int k = 0; k < regionDefs.count(); k++ )
		createRegion( target , regionDefs.get( k ) );
}

void MindArea::createRegion( MindTarget *target , MindRegionDef *regionDef ) {
	MindService *ms = MindService::getService();
	MindRegionTypeDef *typeDef = regionDef -> getType();

	String type = typeDef -> getName();

	// create region depending from type
	MindRegion *region = NULL;
	MindRegionCreateInfo info;
	info.setId( regionDef -> getId() );
	info.setInfo( regionDef );
	info.setType( regionDef -> getType() );
	region = ms -> createRegion( typeDef -> getImplementation() , type , this , &info );

	if( region == NULL )
		return;

	// add to collections
	regionSet -> addSetItem( region );
}

void MindArea::wakeupArea( MindActiveMemory *activeMemory ) {
}

void MindArea::suspendArea() {
}

void MindArea::exit() {
	if( regionSet != NULL )
		regionSet -> exitRegionSet();
	if( regionLinkSet != NULL )
		regionLinkSet -> exitRegionLinkSet();
}

void MindArea::destroy() {
	if( regionSet != NULL )
		regionSet -> destroyRegionSet();
	if( regionLinkSet != NULL )
		regionLinkSet -> destroyRegionLinkSet();
}

MindRegion *MindArea::getRegion( String group , String id ) {
	String regionId = group + "." + id;
	return( regionSet -> getSetItemById( regionId ) );
}

void MindArea::addRegion( MindRegion *region ) {
	// add to collections
	regionSet -> addSetItem( region );
}

void MindArea::addInternalRegionLink( MindRegionLink *link ) {
	regionLinkSet -> addSetItem( link );
}

void MindArea::addMasterLink( MindAreaLink *link ) {
	areaMasterLinkSet -> addSetItem( link );
}

void MindArea::addSlaveLink( MindAreaLink *link ) {
	areaSlaveLinkSet -> addSetItem( link );
}

void MindArea::followLinks( String spykeTrainId , MindRegion *region , NeuroSignalSet *signalSet ) {
	// parameters: region has produced set of output signals
	// area actions:
	//		1. set of output signals will initiate set of links
	//		2. intra-area links targeting specific region is executed resulting in derived set of output signals and links
	//		3. intra-area links leading to already executed regions are ignored
	//		4. intra-area links leading to still not executed regions are executed immediately (2.)
	//		5. inter-area links produce messages

	logger.logInfo( spykeTrainId + ": execute spyke train set count=" + signalSet -> getSignals().count()  + "..." );
	MapStringToClass<MindRegion> regionsExecuted;
	regionsExecuted.add( region -> getRegionId() , region );

	// process input signals while present
	NeuroSignalSet awaitingLocalExecution;
	ClassList<MindMessage> remoteMessages;
	int signalNum = 0;
	followLinksInternalSet( spykeTrainId , signalNum , signalSet , regionsExecuted , awaitingLocalExecution , remoteMessages );

	// process derived inter-area signals
	NeuroSignalSet setLocalExecuted;
	while( !awaitingLocalExecution.isEmpty() ) {
		followLinksInternalSet( spykeTrainId , signalNum , &awaitingLocalExecution , regionsExecuted , setLocalExecuted , remoteMessages );
		awaitingLocalExecution.destroy();

		// move awaiting local execution to main set
		setLocalExecuted.moveTo( &awaitingLocalExecution );
	}

	// proceed with remote execution
	for( int k = 0; k < remoteMessages.count(); k++ ) {
		MindMessage *msg = remoteMessages.get( k );
		msg -> send();
	}
	remoteMessages.clear();
	logger.logInfo( spykeTrainId + ": finished" );
}

void MindArea::followLinksInternalSet( String spykeTrainId , int& signalNum , NeuroSignalSet *signalSet , MapStringToClass<MindRegion>& regionsExecuted , NeuroSignalSet& setLocalExecuted , ClassList<MindMessage>& remoteMessages ) {
	// iterate signals
	ClassList<NeuroSignal>& signals = signalSet -> getSignals();
	for( int k = 0; k < signals.count(); k++ ) {
		NeuroSignal *signal = signals.get( k );
		NeuroLinkSource *source = signal -> getSource();
		ASSERTMSG( source != NULL , spykeTrainId + ": signal has no source" );

		ClassList<NeuroLink>& links = source -> getLinks();
		followLinksInternalOne( spykeTrainId , signalNum , signal , links , regionsExecuted , setLocalExecuted , remoteMessages );
	}
}

void MindArea::followLinksInternalOne( String spykeTrainId , int& signalNum , NeuroSignal *signal , ClassList<NeuroLink>& links , MapStringToClass<MindRegion>& regionsExecuted , NeuroSignalSet& setLocalExecuted , ClassList<MindMessage>& remoteMessages ) {
	// follow links
	for( int k = 0; k < links.count(); k++ ) {
		NeuroLink *link = links.get( k );
		NeuroLinkTarget *target = link -> getTarget();
		MindRegion *targetRegion = target -> getRegion();

		// ignore if region is already affected
		if( regionsExecuted.get( targetRegion -> getRegionId() ) != NULL ) {
			logger.logDebug( spykeTrainId + ": skip signal id=" + signal -> getId() + ", link id=" + link -> getId() + " because region id=" + targetRegion -> getRegionId() + " is already affected" );
			continue;
		}

		regionsExecuted.add( targetRegion -> getRegionId() , targetRegion );

		// if to another area - add to async
		MindArea *targetArea = targetRegion -> getArea();
		if( targetArea != this ) {
			NeuroSignal *sentSignal = new NeuroSignal( signal );
			sentSignal -> setId( spykeTrainId + ".T" + (++signalNum) );
			MindMessage *msg = new MindMessage( link , sentSignal );

			remoteMessages.add( msg );
			logger.logInfo( spykeTrainId + ": create pending remote signal id=" + sentSignal -> getId() + " from signal id=" + signal -> getId() + ", link id=" + link -> getId() );
			continue;
		}

		// execute immediately
		NeuroSignalSet *set = target -> execute( link , signal );
		if( set == NULL ) {
			logger.logDebug( spykeTrainId + ": there are no derived signals from signal id=" + signal -> getId() + ", link id=" + link -> getId() );
			continue;
		}

		ClassList<NeuroSignal>& signals = set -> getSignals();
		for( int k = 0; k < signals.count(); k++ ) {
			NeuroSignal *signalExecuted = signals.get( k );
			signalExecuted -> setId( spykeTrainId + ".X" + (++signalNum) );
			logger.logInfo( spykeTrainId + ": executed local signal id=" + signalExecuted -> getId() + " from signal id=" + signal -> getId() + ", link id=" + link -> getId() );
		}

		set -> moveTo( &setLocalExecuted );
		delete set;
	}
}
