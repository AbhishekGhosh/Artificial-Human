#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

NeuroPool::NeuroPool() {
	attachLogger();
	pendingFire = NULL;
}

NeuroPool::~NeuroPool() {
}

void NeuroPool::createNeurons( int nx , int ny ) {
	neurons.create( nx , ny );
	pendingFire = new NeuroSignal( nx , ny );

	// set initial level of threshold
	setDefaultThreshold();
}

void NeuroPool::setDefaultThreshold() {
	NEURON_DATA *data = neurons.getData();
	int size = neurons.count();
	while( size-- ) {
		data -> synaptic_threshold = NEURON_SYNAPTIC_THRESHOLD_INITIAL_pQ;
		data -> membrane_threshold = NEURON_MEMBRANE_THRESHOLD_INITIAL_pQ;
		data++;
	}
}

void NeuroPool::setParent( MindRegion *p_region ) {
	region = p_region;
}

MindRegion *NeuroPool::getRegion() {
	return( region );
}

void NeuroPool::setId( String p_id ) {
	id = p_id;
}

String NeuroPool::getId() {
	return( id );
}

void NeuroPool::getNeuronDimensions( int *px , int *py ) {
	*px = neurons.getN1();
	*py = neurons.getN2();
}

TwoIndexArray<NEURON_DATA>& NeuroPool::getNeuronData() {
	return( neurons );
}

int NeuroPool::getNeuronDataSize() {
	return( neurons.count() );
}

NeuroSignal *NeuroPool::fire( NeuroSignal *srcSignal ) {
	// create generated feed forward signal
	NeuroSignal *ffSignal = new NeuroSignal( neurons.getN1() , neurons.getN2() );
	ffSignal -> setId( srcSignal -> getId() );

	// process signal
	NEURON_DATA *data = neurons.getData();
	int sn = srcSignal -> getDataSize();
	int stotal = srcSignal -> getMaxSize();
	int *sv = srcSignal -> getIndexRawData();
	int dtotal = neurons.count();
	RFC_INT64 msNow = Timer::getCurrentTimeMillis();

	for( int k = 0; k < sn; k++ , sv++ ) {
		int sk = *sv;
		int dk = ( int )( ( sk * (RFC_INT64)dtotal ) / stotal );

		NEURON_DATA *dv = data + dk;
		RFC_INT64 msSilentTill = dv -> silent_till;

		// check membrane potential
		if( dv -> membrane_potential < dv -> membrane_threshold ) {
			LOGDEBUG( String( "fire: pending fire index=" ) + dk + ", membrane_potential=" + dv -> membrane_potential + ", threshold=" + dv -> membrane_threshold );
			pendingFire -> addIndexData( dk );
			continue;
		}

		// fire
		dv -> silent_till = msNow + NEURON_FIRE_OUTPUT_SILENT_ms;
		dv -> firepower = NEURON_FIRE_OUTPUT_BY_MEMBRANE_POTENTIAL_pQ;
		ffSignal -> addIndexData( dk );
		LOGDEBUG( String( "fire: done index=" ) + dk );
	}

	// clear pending if fired
	int dn = pendingFire -> getDataSize();
	int *pendingFireData = pendingFire -> getIndexRawData();
	for( int k = 0; k < dn; k++ ) {
		int dk = *pendingFireData++;
		NEURON_DATA *dv = data + dk;

		// clear from pending
		if( dv -> firepower > 0 )
			pendingFireData[ -1 ] = -1;
	}

	// sort, remove duplicates and deleted items
	pendingFire -> arrangeNormal();

	// clear fire state
	int fn = ffSignal -> getDataSize();
	int *fireData = ffSignal -> getIndexRawData();
	for( int k = 0; k < fn; k++ ) {
		int dk = *fireData++;
		NEURON_DATA *dv = data + dk;
		dv -> firepower = 0;
	}

	MindArea *area = region -> getArea();
	logger.logInfo( "fire: MindArea id=" + area -> getAreaId() + " NeuroPool id=" + id + " state after signal id=" + srcSignal -> getId() + ", pendingSize=" + pendingFire -> getDataSize() + ", signal=" + ffSignal -> getNumberDataString() );

	return( ffSignal );
}
