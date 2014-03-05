#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

NeocortexRegion::NeocortexRegion( MindArea *p_area )
:	MindRegion( p_area ) {
	attachLogger();

	sizeX = 0;
	sizeY = 0;
	useSpatialPooler = false;
	useTemporalPooler = false;
	temporalDepth = false;
	spatialPatternExpected = -1;

	spatialPooler = new CortexSpatialPooler();
	temporalPooler = new CortexTemporalPooler();
}

String NeocortexRegion::getRegionType() {
	return( "NeocortexRegion" );
}

void NeocortexRegion::createRegion( MindRegionCreateInfo *info ) {
	const int SPACIAL_MATCH_PATTERN_TOLERANCE = 10;
	const int SPACIAL_MATCH_NEURONSTATE_TOLERANCE = 30;
	const int SPATIAL_PROTECTED_PATTERN_USAGE = 100;
	const int TEMPORAL_MATCH_PATTERN_TOLERANCE = 20;
	const int TEMPORAL_PROTECTED_PATTERN_USAGE = 100;
	const int TEMPORAL_PREDICTION_PROBABILITY_TOLERANCE = 50;
	const int TEMPORAL_PREDICTION_MATCH_TOLERANCE = 75;

	// get parameters
	info -> getSizeInfo( &sizeX , &sizeY );
	useSpatialPooler = info -> isUsingSpatialPooler();
	useTemporalPooler = info -> isUsingTemporalPooler();
	temporalDepth = info -> getTemporalDepth();

	// create pools
	inputPool.setParent( this );
	inputPool.createNeurons( sizeX , sizeY );
	feedbackPool.setParent( this );
	feedbackPool.createNeurons( sizeX , sizeY );

	// create and setup spatial and temporal poolers
	spatialPooler -> create( info -> getSpatialPoolerSize() , sizeX , sizeY );
	spatialPooler -> setMatchTolerance( SPACIAL_MATCH_PATTERN_TOLERANCE , SPACIAL_MATCH_NEURONSTATE_TOLERANCE );
	spatialPooler -> setProtectedUsage( SPATIAL_PROTECTED_PATTERN_USAGE );

	temporalPooler -> create( info -> getTemporalPoolerSize() , temporalDepth );
	temporalPooler -> setMatchTolerance( TEMPORAL_MATCH_PATTERN_TOLERANCE );
	temporalPooler -> setProtectedUsage( TEMPORAL_PROTECTED_PATTERN_USAGE );
	temporalPooler -> setPredictionProbabilityTolerance( TEMPORAL_PREDICTION_PROBABILITY_TOLERANCE );
	temporalPooler -> setPredictionMatchTolerance( TEMPORAL_PREDICTION_MATCH_TOLERANCE );

	// set identity
	// MindRegion::create( p_id );
	// inputPool.setId( p_id + ".ff" );
	// feedbackPool.setId( p_id + ".fb" );

	sourceFeedForward.create( this , "neocortex.ffout" );
	sourceFeedBack.setSourcePool( &inputPool );
	sourceFeedBack.create( this , "neocortex.fbout" );
	sourceFeedBack.setSourcePool( &feedbackPool );
	targetFeedForward.create( this , "neocortex.ffin" );
	targetFeedForward.setHandler( ( MindRegion::NeuroLinkTargetHandler )&NeocortexRegion::handleFeedForwardNeuroLinkMessage );
	targetFeedBack.create( this , "neocortex.fbin" );
	targetFeedBack.setHandler( ( MindRegion::NeuroLinkTargetHandler )&NeocortexRegion::handleFeedBackNeuroLinkMessage );

	// sourceFeedForward = new NeuroLinkSource( this );
	// sourceAttention = new NeuroLinkSource( this );
	// targetFeedBack = new NeuroLinkTarget( this );
	// targetAttention = new NeuroLinkTarget( this );
	// create input/output vectors
	// sourceFeedForward -> setSourceSignal( &vectorFeedForwardOutput );
	// sourceFeedBack -> setSourceSignal( &vectorFeedBackOutput );
	// sourceAttention -> setSourceSignal( &vectorAttentionOutput );
	// targetFeedBack -> setHandler( ( MindRegion::NeuroLinkTargetHandler )&NeocortexRegion::handleFeedBackNeuroLinkMessage );
	// targetAttention -> setHandler( ( MindRegion::NeuroLinkTargetHandler )&NeocortexRegion::handleAttentionNeuroLinkMessage );

	logger.logDebug( String( "createRegion: created neocortex region: sizeX=" ) + sizeX + ", sizeY=" + sizeY );
}

void NeocortexRegion::getSourceSizes( String entity , int *p_sizeX , int *p_sizeY ) {
	*p_sizeX = sizeX;
	*p_sizeY = sizeY;
}

void NeocortexRegion::exitRegion() {
}

void NeocortexRegion::destroyRegion() {
	delete spatialPooler;
	spatialPooler = NULL;

	delete temporalPooler;
	temporalPooler = NULL;
}

NeuroSignalSet *NeocortexRegion::handleFeedForwardNeuroLinkMessage( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *inputSignal ) {
	// mock algorithm:
	//	1. project neurolink
	NeuroSignal *forwardSignal = link -> apply( inputSignal , &inputPool );
	if( forwardSignal == NULL )
		return( NULL );

	NeuroSignalSet *signalSet = new NeuroSignalSet;
	signalSet -> addSetItem( &sourceFeedForward , forwardSignal );

	//	2. execute spacial pooler
	//		- max number of patterns, spatial pooler slots: MAX_SPACIAL_PATTERNS
	//		- size is defined by area, differs from neurolink width
	//		- add new patterns only if not matched for more than SPACIAL_MATCH_TOLERANCE=10
	//		- keep usage count for each pattern: usageCount
	//		- forget pattern only if usage is less than PROTECTED_SPATIAL_PATTERN_USAGE
	//		- forget least used stored pattern when adding new pattern and all spacial pooler slots are already filled in
	int spatialPatternForgotten = -1;
	int matchProbability = 0;
	int spatialPatternMatched = spatialPooler -> matchPattern( forwardSignal , &matchProbability , &spatialPatternForgotten );

	StatService *ss = StatService::getService();
	ss -> addMetricValue( "cortex.spatialmatch.rate" , matchProbability );

	//	3. execute temporal pooler
	//		- max number of patterns, spatial pooler slots: MAX_TEMPORAL_PATTERNS
	//		- use length: temporalDepth
	//		- use complete (max depth) and incomplete (less than depth) patterns
	//		- clear forgotten spatial pattern if any from temporal patterns
	//		- clear forgotten temporal pattern
	if( spatialPatternForgotten >= 0 )
		temporalPooler -> forgetSpatialPattern( spatialPatternForgotten );

	if( spatialPatternMatched < 0 ) {
		logger.logInfo( String( "handleFeedForwardNeuroLinkMessage: id=" ) + inputSignal -> getId() + " - not matched, ignored" );

		if( logger.isLogAll() ) {
			spatialPooler -> logItems();
			temporalPooler -> logItems();
		}
		return( signalSet );
	}

	int spatialPatternPredicted = -1;
	int predictionProbability = 0;
	int temporalPatternForgotten;
	int temporalspatialPatternMatched = temporalPooler -> matchPattern( spatialPatternMatched , &spatialPatternPredicted , &predictionProbability , &temporalPatternForgotten );
	
	ss -> addMetricValue( "cortex.temporalmatch.rate" , predictionProbability );
	int predictionRate = ( spatialPatternPredicted >= 0 && spatialPatternPredicted == spatialPatternExpected )? 100 : 0;
	ss -> addMetricValue( "cortex.prediction.rate" , predictionRate );

	if( spatialPatternPredicted < 0 ) {
		logger.logInfo( String( "handleFeedForwardNeuroLinkMessage: id=" ) + inputSignal -> getId() + ", spatialMatched=" + spatialPatternMatched + ", spatialExpected=" + spatialPatternExpected + ", temporalMatched=" + temporalspatialPatternMatched + ", spatialPredicted=-1, probability=" + matchProbability + "/-1" );

		if( logger.isLogAll() ) {
			spatialPooler -> logItems();
			temporalPooler -> logItems();
		}
		return( signalSet );
	}

	if( logger.isLogAll() ) {
		spatialPooler -> logItems();
		temporalPooler -> logItems();
	}

	// 4. generate feedback
	//		- get predicted pattern
	logger.logInfo( String( "handleFeedForwardNeuroLinkMessage: id=" ) + inputSignal -> getId() + ", spatialMatched=" + spatialPatternMatched + ", spatialExpected=" + spatialPatternExpected + ", temporalMatched=" + temporalspatialPatternMatched + ", spatialPredicted=" + spatialPatternPredicted + ", probability=" + matchProbability + "/" + predictionProbability );
	spatialPatternExpected = spatialPatternPredicted;

	//		- generate feedback message
	int fnx , fny;
	feedbackPool.getNeuronDimensions( &fnx , &fny );
	NeuroSignal *feedbackSignal = new NeuroSignal( fnx , fny );
	spatialPooler -> getPattern( spatialPatternPredicted , feedbackSignal );
	feedbackSignal -> setId( inputSignal -> getId() );
	feedbackSignal -> setTs( Timer::getCurrentTimeMillis() );

	signalSet -> addSetItem( &sourceFeedBack , feedbackSignal );
	return( signalSet );
}

NeuroSignalSet *NeocortexRegion::handleFeedBackNeuroLinkMessage( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *data ) {
	return( NULL );
}

// void NeocortexRegion::handleAttentionNeuroLinkMessage( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *data ) {
// 	sourceAttention -> sendMessage();
// }

