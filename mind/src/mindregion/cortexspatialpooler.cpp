#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

CortexSpatialPooler::CortexSpatialPooler() {
	attachLogger();

	maxPoolSize = 0;
	patternSizeX = 0;
	patternSizeY = 0;

	matchTolerancePattern = 0;
	matchToleranceNeuronState = 0;
	protectedUsage = 0;
}

CortexSpatialPooler::~CortexSpatialPooler() {
	items.destroy();
}

void CortexSpatialPooler::create( int p_poolSize , int sizeX , int sizeY ) {
	maxPoolSize = p_poolSize;
	patternSizeX = sizeX;
	patternSizeY = sizeY;
}

void CortexSpatialPooler::setMatchTolerance( int tolerancePattern , int toleranceNeuronState ) {
	matchTolerancePattern = tolerancePattern;
	matchToleranceNeuronState = toleranceNeuronState;
}

void CortexSpatialPooler::setProtectedUsage( int usage ) {
	protectedUsage = usage;
}

int CortexSpatialPooler::matchPattern( NeuroSignal *excitedSignal , int *matchProbability , int *patternForgotten ) {
	CortexSpatialPoolerItem *item = new CortexSpatialPoolerItem();

	// create item data from current pool state and generated signal
	item -> setStateFromSignal( excitedSignal );
	LOGDEBUG( String( "matchPattern: input=" ) + item -> getNumberedState() );

	// find closest match
	int difference;
	CortexSpatialPoolerItem *bestItem = findBestMatch( item , &difference );
	if( bestItem != NULL ) {
		if( difference <= matchTolerancePattern ) {
			LOGDEBUG( String( "matchPattern: matched item id=" ) + bestItem -> getId() + ", difference=" + difference );
			LOGDEBUG( String( "matchPattern: matched item data id=" ) + bestItem -> getId() + ", data=" + bestItem -> getNumberedState() );

			delete item;
			bestItem -> addUsage();
			*patternForgotten = -1;
			*matchProbability = 100 - difference;
			return( bestItem -> getId() );
		}
	}

	*matchProbability = -1;

	// check free space
	if( items.count() < maxPoolSize ) {
		// add item
		int id = items.add( item );
		item -> setId( id );
		item -> addUsage();
		*patternForgotten = -1;
		LOGDEBUG( String( "matchPattern: not matched, added id=" ) + id );
		return( id );
	}

	// check can free some
	CortexSpatialPoolerItem *leastUsed = findLeastUsed();
	if( leastUsed -> getUsage() >= protectedUsage ) {
		LOGDEBUG( String( "matchPattern: not matched, unable to replace id=" ) + leastUsed -> getId() + ", usage=" + leastUsed -> getUsage() );
		delete item;
		*patternForgotten = -1;
		return( -1 );
	}

	// free (replace)
	int id = leastUsed -> getId();
	item -> setId( id );
	item -> addUsage();
	items.set( id , item );
	LOGDEBUG( String( "matchPattern: not matched, replaced id=" ) + leastUsed -> getId() + ", usage=" + leastUsed -> getUsage() );
	delete leastUsed;

	*patternForgotten = id;
	return( id );
}

bool CortexSpatialPooler::getPattern( int pattern , NeuroSignal *signal ) {
	if( items.count() <= pattern )
		return( false );

	CortexSpatialPoolerItem *item = items[ pattern ];
	item -> getSignalFromState( signal );
	return( true );
}

CortexSpatialPoolerItem *CortexSpatialPooler::findBestMatch( CortexSpatialPoolerItem *data , int *difference ) {
	int minDiffValue = 0;
	CortexSpatialPoolerItem *minDiffItem = NULL;
	for( int k = 0; k < items.count(); k++ ) {
		// get difference
		CortexSpatialPoolerItem *item = items[ k ];
		int diff = item -> getDifferencePercentage( data , matchToleranceNeuronState );
		if( minDiffItem == NULL || diff < minDiffValue ) {
			minDiffItem = item;
			minDiffValue = diff;
		}
	}

	*difference = minDiffValue;
	return( minDiffItem );
}

CortexSpatialPoolerItem *CortexSpatialPooler::findLeastUsed() {
	int minUsageValue = 0;
	CortexSpatialPoolerItem *minUsageItem = NULL;
	for( int k = 0; k < items.count(); k++ ) {
		// get usage
		CortexSpatialPoolerItem *item = items[ k ];
		int usage = item -> getUsage();
		if( minUsageItem == NULL || usage < minUsageValue ) {
			minUsageItem = item;
			minUsageValue = usage;
		}
	}
	return( minUsageItem );
}

void CortexSpatialPooler::logItems() {
	logger.logInfo( String( "logItems: spatial pooler items, total=" ) + items.count() );
	for( int k = 0; k < items.count(); k++ ) {
		CortexSpatialPoolerItem *item = items[ k ];
		item -> logItem();
	}
}
