#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MockLink::MockLink( String p_type , NeuroLinkSource *src , NeuroLinkTarget *dst ) : NeuroLink( src , dst ) {
	attachLogger();
	type = p_type;
	opid = 0;
}

void MockLink::createNeuroLink( NeuroLinkInfo *info ) {
	NeuroLink::createNeuroLink( info );
}

NeuroSignal *MockLink::apply( NeuroSignal *srcData , NeuroPool *dstPool ) {
	opid++;

	// log 
	LOGDEBUG( String( "apply: project NeuroLink id=" ) + getId() + ", NeuroSignal id=" + srcData -> getId() + "..." );
	return( NULL );
}
