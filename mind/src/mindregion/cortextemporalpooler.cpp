#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

CortexTemporalPooler::CortexTemporalPooler() {
	attachLogger();

	maxPoolSize = 0;
	maxDepth = 0;
	streamData = NULL;
	streamDataSize = 0;
	matchTolerancePattern = 0;
	predictionProbabilityTolerance = 0;
	predictionMatchTolerance = 0;
	protectedUsage = 0;
}

CortexTemporalPooler::~CortexTemporalPooler() {
	if( streamData != NULL )
		free( streamData );
}

void CortexTemporalPooler::create( int poolSize , int depth ) {
	maxPoolSize = poolSize;
	maxDepth = depth;

	// create empty history array
	streamData = ( int * )malloc( maxDepth * sizeof( int ) );
	for( int k = 0; k < maxDepth; k++ )
		streamData[ k ] = -1;
}

void CortexTemporalPooler::forgetSpatialPattern( int spatialPattern ) {
	// delete from all temporal patterns
	for( int k = 0; k < items.count(); k++ ) {
		CortexTemporalPoolerItem *item = items.get( k );
		item -> deleteSpatialPatternIfAny( spatialPattern );
	}

	// delete from current history
	for( int k = 0; k < streamDataSize; ) {
		if( streamData[ k ] != spatialPattern ) {
			k++;
			continue;
		}

		// delete and shift
		streamDataSize--;
		for( int m = k; m < streamDataSize; )
			streamData[ k ] = streamData[ k + 1 ];
		streamData[ streamDataSize ] = -1;
	}
}

int CortexTemporalPooler::matchPattern( int spatialPattern ,  int *spatialPatternPredicted , int *predictionProbability , int *temporalPatternForgotten ) {
	// make temporal pattern
	if( streamDataSize == maxDepth ) {
		// shift current stream if already filled in
		for( int k = 0; k < ( streamDataSize - 1 ); k++ )
			streamData[ k ] = streamData[ k + 1 ];
		streamData[ streamDataSize - 1 ] = spatialPattern;
	}
	else {
		// add to the end
		streamData[ streamDataSize++ ] = spatialPattern;
	}

	// make predition
	int predictedSpatialPattern;
	int predictedTemporalPattern = predictPattern( streamData , streamDataSize , &predictedSpatialPattern , predictionProbability );
	*spatialPatternPredicted = predictedSpatialPattern;

	// match and learn new pattern logic
	CortexTemporalPoolerItem *bestItem = matchFillPattern( streamData , streamDataSize , spatialPattern , temporalPatternForgotten );

	// return temporal pattern id
	if( bestItem == NULL )
		return( -1 );

	return( bestItem -> getId() );
}

CortexTemporalPoolerItem *CortexTemporalPooler::matchFillPattern( int *data , int dataSize , int spatialPattern , int *temporalPatternForgotten ) {
	// find closest match
	int difference;
	int matchItems;
	CortexTemporalPoolerItem *bestItem = findBestMatch( 0 , data , dataSize , &difference , &matchItems );
	if( bestItem != NULL ) {
		if( difference <= matchTolerancePattern ) {
			*temporalPatternForgotten = -1;
			bestItem -> addUsage();
			return( bestItem );
		}
	}

	// check free space
	if( items.count() < maxPoolSize ) {
		// add item
		CortexTemporalPoolerItem *item = new CortexTemporalPoolerItem( maxDepth );
		item -> setData( data , dataSize );
		int id = items.add( item );
		item -> setId( id );
		item -> addUsage();
		*temporalPatternForgotten = -1;
		return( item );
	}

	// check can free some
	CortexTemporalPoolerItem *leastUsed = findLeastUsed();
	if( leastUsed -> getUsage() >= protectedUsage )
		return( NULL );

	// free (replace)
	int id = leastUsed -> getId();
	leastUsed -> setData( data , dataSize );
	leastUsed -> setUsage( 1 );

	*temporalPatternForgotten = id;
	return( leastUsed );
}

CortexTemporalPoolerItem *CortexTemporalPooler::findBestMatch( int minSize , int *data , int dataSize , int *difference , int *matchedCount ) {
	int minDiffValue = 0;
	CortexTemporalPoolerItem *minDiffItem = NULL;
	int diffMatchCount = 0;
	for( int k = 0; k < items.count(); k++ ) {
		// get difference
		CortexTemporalPoolerItem *item = items[ k ];
		if( item -> getDataSize() < minSize )
			continue;

		int diff = item -> getDifferencePercentage( data , dataSize );
		if( minDiffItem == NULL || diff <= minDiffValue ) {
			if( minDiffItem != NULL && diff == minDiffValue )
				diffMatchCount++;
			else
				diffMatchCount = 1;

			minDiffItem = item;
			minDiffValue = diff;
		}
	}

	*difference = minDiffValue;
	*matchedCount = diffMatchCount;
	return( minDiffItem );
}

CortexTemporalPoolerItem *CortexTemporalPooler::findPartialBestMatch( int minSize , int *data , int dataSize , int *difference , int *matchedCount ) {
	int minDiffValue = 0;
	CortexTemporalPoolerItem *minDiffItem = NULL;
	int diffMatchCount = 0;
	for( int k = 0; k < items.count(); k++ ) {
		// get difference
		CortexTemporalPoolerItem *item = items[ k ];
		if( item -> getDataSize() < minSize )
			continue;

		int diff = item -> getPartialDifferencePercentage( data , dataSize );
		if( minDiffItem == NULL || diff <= minDiffValue ) {
			if( minDiffItem != NULL && diff == minDiffValue )
				diffMatchCount++;
			else
				diffMatchCount = 1;

			minDiffItem = item;
			minDiffValue = diff;
		}
	}

	*difference = minDiffValue;
	*matchedCount = diffMatchCount;
	return( minDiffItem );
}

CortexTemporalPoolerItem *CortexTemporalPooler::findLeastUsed() {
	int minUsageValue = 0;
	CortexTemporalPoolerItem *minUsageItem = NULL;
	for( int k = 0; k < items.count(); k++ ) {
		// get usage
		CortexTemporalPoolerItem *item = items[ k ];
		int usage = item -> getUsage();
		if( minUsageItem == NULL || usage < minUsageValue ) {
			minUsageItem = item;
			minUsageValue = usage;
		}
	}
	return( minUsageItem );
}

int CortexTemporalPooler::predictPattern( int *data , int dataSize , int *spatialPatternPredicted , int *predictionProbability ) {
	// if history is already filled - first item should be ignored in search
	if( dataSize == maxDepth ) {
		data++;
		dataSize--;
	}

	// find first full sequence then partial
	CortexTemporalPoolerItem *nextItem = NULL;
	int difference = NULL;
	int matchItems;
	for( int k = 0; k < dataSize; k++ ) {
		int minSize = dataSize - k + 1;
		nextItem = findPartialBestMatch( minSize , data + k , dataSize - k , &difference , &matchItems );

		if( nextItem != NULL ) {
			if( difference <= predictionMatchTolerance ) {
				*spatialPatternPredicted = nextItem -> getSpatialPattern( dataSize - k );
				break;
			}
			else
				nextItem = NULL;
		}
	}

	// no similar item found
	if( nextItem == NULL ) {
		*spatialPatternPredicted = -1;
		*predictionProbability = 0;
		return( -1 );
	}

	// calculate probability
	nextItem -> addUsage();
	*predictionProbability = ( 100 - difference ) / matchItems;
	return( nextItem -> getId() );
}

void CortexTemporalPooler::setMatchTolerance( int tolerancePattern ) {
	matchTolerancePattern = tolerancePattern;
}

void CortexTemporalPooler::setProtectedUsage( int usage ) {
	protectedUsage = usage;
}

void CortexTemporalPooler::setPredictionProbabilityTolerance( int tolerance ) {
	predictionProbabilityTolerance = tolerance;
}

void CortexTemporalPooler::setPredictionMatchTolerance( int tolerance ) {
	predictionMatchTolerance = tolerance;
}

void CortexTemporalPooler::logItems() {
	// stream data
	String sd;
	for( int k = 0; k < streamDataSize; k++ ) {
		if( k > 0 )
			sd += ".";
		sd += streamData[ k ];
	}

	logger.logInfo( String( "logItems: temporal pooler items, total=" ) + items.count() + ", streamData=" + sd );
	for( int k = 0; k < items.count(); k++ ) {
		CortexTemporalPoolerItem *item = items[ k ];
		item -> logItem();
	}
}
