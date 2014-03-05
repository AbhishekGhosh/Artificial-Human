#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

ModulatoryLink::ModulatoryLink( NeuroLinkSource *src , NeuroLinkTarget *dst ) : NeuroLink( src , dst ) {
	attachLogger();
}

void ModulatoryLink::createNeuroLink( NeuroLinkInfo *info ) {
}

NeuroSignal *ModulatoryLink::apply( NeuroSignal *srcData , NeuroPool *dstPool ) {
	logger.logError( "apply: apply message to NeuroPool - not implemented" );
	return( NULL );
}
