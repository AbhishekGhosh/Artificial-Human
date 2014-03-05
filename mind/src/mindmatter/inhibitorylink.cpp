#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

InhibitoryLink::InhibitoryLink( NeuroLinkSource *src , NeuroLinkTarget *dst ) : NeuroLink( src , dst ) {
	attachLogger();
	opid = 0;
}

void InhibitoryLink::createNeuroLink( NeuroLinkInfo *info ) {
}

NeuroSignal *InhibitoryLink::apply( NeuroSignal *srcData , NeuroPool *dstPool ) {
	opid++;

	// log 
	LOGDEBUG( String( "apply: projected NeuroLink id=" ) + getId() + ", NeuroSignal id=" + srcData -> getId() + "..." );

	// current timestamp
	RFC_INT64 msNow = Timer::getCurrentTimeMillis();

	// map source surface to target surface - as linear arrays
	int dn = dstPool -> getNeuronDataSize();
	TwoIndexArray<NEURON_DATA>& dstDataNeurons = dstPool -> getNeuronData();
	NEURON_DATA *dvdata = dstDataNeurons.getData();

	// project specific values
	int sn = srcData -> getMaxSize();

	// read activated indexes
	int n = srcData -> getDataSize();
	int *sv = srcData -> getIndexRawData();

	// no feed forward signal will be generated
	for( int k = 0; k < n; k++ ) {
		// get value and project
		int sk = *sv++;
		int dk = ( int )( ( sk * (RFC_INT64)dn ) / sn );

		// get current potential and handle timestamps
		NEURON_DATA *dv = dvdata + dk;
		dv -> silent_till = msNow + NEURON_INHIBIT_DELAY_ms;

		LOGDEBUG( String( "apply: projected NeuroLink id=" ) + getId() + ", NeuroSignal id=" + srcData -> getId() + ", spos=" + sk + ", dpos=" + dk + ", inhibited" );
	}

	// log 
	logger.logInfo( String( "apply: projected NeuroLink id=" ) + getId() + ", NeuroSignal id=" + srcData -> getId() + ", signalCount=" + srcData -> getDataSize() );

	return( NULL );
}
