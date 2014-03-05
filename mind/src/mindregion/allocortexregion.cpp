#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

AllocortexRegion::AllocortexRegion( MindArea *p_area )
:	MindRegion( p_area ) {
	attachLogger();
}

String AllocortexRegion::getRegionType() {
	return( "AllocortexRegion" );
}

void AllocortexRegion::createRegion( MindRegionCreateInfo *info ) {
	logger.logDebug( String( "createRegion: created allocortex region" ) );
}

void AllocortexRegion::getSourceSizes( String entity , int *p_sizeX , int *p_sizeY ) {
	*p_sizeX = 0;
	*p_sizeY = 0;
}

void AllocortexRegion::exitRegion() {
}

void AllocortexRegion::destroyRegion() {
}

void AllocortexRegion::handleFeedForwardNeuroLinkMessage( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *inputSignal ) {
}

// void AllocortexRegion::handleFeedBackNeuroLinkMessage( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *data ) {
// 	sourceFeedBack -> sendMessage();
// }

// void AllocortexRegion::handleAttentionNeuroLinkMessage( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *data ) {
// 	sourceAttention -> sendMessage();
// }

