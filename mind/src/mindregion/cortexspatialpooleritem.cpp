#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

CortexSpatialPoolerItem::CortexSpatialPoolerItem() {
	attachLogger();

	id = -1;
	usage = 0;
}

void CortexSpatialPoolerItem::setId( int p_id ) {
	id = p_id;
}

int CortexSpatialPoolerItem::setStateFromPool( NeuroPool *pool ) {
	TwoIndexArray<NEURON_DATA>& nd = pool -> getNeuronData();
	int n = nd.getN1() * nd.getN2();
	NEURON_DATA *sv = nd.getData();

	RFC_INT64 msNow = Timer::getCurrentTimeMillis();
	while( n-- ) {
		// get state
		neurovt_state state = sv -> firepower;
		sv++;

		// copy state
		if( state >= sv -> membrane_threshold )
			indexList.add( state );
	}

	return( indexList.count() );
}

void CortexSpatialPoolerItem::setStateFromSignal( NeuroSignal *signal ) {
	int *sv = signal -> getIndexRawData();
	int sn = signal -> getDataSize();
	ASSERTMSG( sn > 0 , "invalid signal" );

	// copy from signal
	indexList.set( sv , sn );
}

int CortexSpatialPoolerItem::getDifferencePercentage( CortexSpatialPoolerItem *item , neurovt_state toleranceNeuronState ) {
	int sn = item -> indexList.count();
	int *sv = item -> indexList.getAll();
	int vn = indexList.count();
	int *dv = indexList.getAll();

	// number of union items
	int cn = sn + vn;
	// number of different items
	int d = 0;

	// read both ascending
	while( sn > 0 && vn > 0 ) {
		int sk = *sv;
		int vk = *dv;

		if( sk == vk ) {
			cn--;
			sn--; sv++;
			vn--; dv++;
		}
		else
		if( sk < vk ) {
			d++;
			sn--; sv++;
		}
		else {
			d++;
			vn--; dv++;
		}
	}

	// add remaining to difference
	d += sn + vn;

	ASSERTMSG( cn > 0 , "all item neurons are silent" );
	int pt = ( d * 100 ) / cn;
	return( pt );
}

int CortexSpatialPoolerItem::getId() {
	return( id );
}

int CortexSpatialPoolerItem::getUsage() {
	return( usage );
}

void CortexSpatialPoolerItem::addUsage() {
	usage++;
}

void CortexSpatialPoolerItem::setPoolState( NeuroPool *pool ) {
	TwoIndexArray<NEURON_DATA>& nd = pool -> getNeuronData();
	NEURON_DATA *dv = nd.getData();

	int *sv = indexList.getAll();
	int n = indexList.count();

	RFC_INT64 msNow = Timer::getCurrentTimeMillis();
	while( n-- ) {
		int index = *sv++;
		dv[ index ].firepower = NEURON_FIRE_OUTPUT_BY_MEMBRANE_POTENTIAL_pQ;
		dv[ index ].silent_till = msNow;
	}
}

void CortexSpatialPoolerItem::getSignalFromState( NeuroSignal *signal ) {
	int *dv = indexList.getAll();
	int n = indexList.count();
	signal -> setIndexData( dv , n );
}

void CortexSpatialPoolerItem::logItem() {
	logger.logInfo( String( "id=" ) +  id + ", usage=" + usage + ", state=" + getNumberedState() );
}

String CortexSpatialPoolerItem::getBinaryState() {
	String ps;

	int *s = indexList.getAll();
	int n = indexList.count();
	String zs = "0";
	for( int k = 0; k < n; k++ ) {
		// add intermediate zeros
		if( k == 0 )
			ps += zs.replicate( s[ k ] );
		else
			ps += zs.replicate( s[ k ] - s[ k - 1 ] - 1 );

		// add item
		ps += "1";
	}
	
	return( ps );
}

String CortexSpatialPoolerItem::getNumberedState() {
	String ps;

	int *s = indexList.getAll();

	bool first = true;
	int n = indexList.count();
	for( int k = 0; k < n; k++ ) {
		if( first )
			first = false;
		else
			ps += ".";

		ps += s[ k ];
	}
	
	return( ps );
}
