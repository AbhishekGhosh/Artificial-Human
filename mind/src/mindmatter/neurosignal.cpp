#include <ah_mind.h>
#include <ah_mind_impl.h>

static const int DEFAULT_ARRAY_SIZE = 256;

/*#########################################################################*/
/*#########################################################################*/

NeuroSignal::NeuroSignal() {
	ts = 0;
	sizeX = 0;
	sizeY = 0;
	source = NULL;
}

NeuroSignal::NeuroSignal( int p_sizeX , int p_sizeY ) {
	ts = 0;
	sizeX = p_sizeX;
	sizeY = p_sizeY;
	source = NULL;
	data.allocate( DEFAULT_ARRAY_SIZE );
}

NeuroSignal::NeuroSignal( NeuroSignal *src ) {
	id = src -> id;
	ts = src -> ts;
	sizeX = src -> sizeX;
	sizeY = src -> sizeY;
	source = src -> source;
	data.copy( src -> data );
}

NeuroSignal::~NeuroSignal() {
}

void NeuroSignal::copyDataFromSignal( NeuroSignal *src ) {
	data.copy( src -> data );
}

int *NeuroSignal::getIndexRawData() {
	return( data.getAll() );
}

void NeuroSignal::create( int p_sizeX , int p_sizeY ) {
	sizeX = p_sizeX;
	sizeY = p_sizeY;
	data.allocate( DEFAULT_ARRAY_SIZE );
	data.clear();
}

void NeuroSignal::setTs( RFC_INT64 p_ts ) {
	ts = p_ts;
}

void NeuroSignal::setId( String p_id ) {
	id = p_id;
	Object::setInstance( id );
}

void NeuroSignal::setSource( NeuroLinkSource *p_source ) {
	source = p_source;
}

void NeuroSignal::getSizeInfo( int *nx , int *ny ) {
	*nx = sizeX;
	*ny = sizeY;
}

int NeuroSignal::getMaxSize() {
	return( sizeX * sizeY );
}

int NeuroSignal::getDataSize() {
	return( data.count() );
}

void NeuroSignal::createFromPool( NeuroPool *pool ) {
	int snx , sny;
	pool -> getNeuronDimensions( &snx , &sny );
	int sn = snx * sny;
	TwoIndexArray<NEURON_DATA>& poolData = pool -> getNeuronData();

	create( snx , sny );
	data.clear();
	int *dv = data.getAll();
	int aSig = data.size();
	int nSig = 0;

	// copy activated pool data to signal data
	NEURON_DATA *sv = poolData.getData();

	RFC_INT64 msNow = Timer::getCurrentTimeMillis();
	ts = msNow;

	for( int sk = 0; sk < sn; sk++ , sv++ ) {
		// get current state
		neurovt_state state = sv -> firepower;

		// process only at or above threshold
		if( state >= NEURON_FIRE_OUTPUT_BY_MEMBRANE_POTENTIAL_pQ ) {
			// clear fire state
			sv -> firepower = 0;

			// allocate signal data
			if( nSig == aSig ) {
				aSig *= 2;
				data.allocate( aSig );
				dv = data.getAll();
			}

			// add to signal
			*dv++ = sk;
			nSig++;
		}
	}

	data.setCount( nSig );
}

String NeuroSignal::getBinaryDataString() {
	int sn = data.count();
	String ds;

	String sz = "0";
	for( int k = 0; k < sn; k++ ) {
		int idx = data.get( k );

		// zeros before
		int nz = ( k == 0 )? data.get( k ) : data.get( k ) - data.get( k - 1 ) - 1; 
		if( nz > 0 )
			ds += sz.replicate( nz );

		ds += '1';
	}

	return( ds );
}

String NeuroSignal::getNumberDataString() {
	int sn = data.count();
	String ds;

	for( int k = 0; k < sn; k++ ) {
		int idx = data.get( k );
		if( k > 0 )
			ds += ".";

		ds += idx;
	}

	return( ds );
}

void NeuroSignal::clearData() {
	data.clear();
}

void NeuroSignal::addIndexData( int index ) {
	data.add( index );
}

void NeuroSignal::arrangeNormal() {
	// sort
	data.sort();

	// remove duplicates
	data.removeDuplicates();

	// remove not firing
	data.removeValue( -1 );
}

void NeuroSignal::removeNotFiringIndexData() {
	// remove not firing
	data.removeValue( -1 );
}

void NeuroSignal::addIndexDataSorted( NeuroSignal *srcSignal ) {
	int dn = data.count();
	int sn = srcSignal -> data.count();
	int *dv = data.getAll();
	int *sv = srcSignal -> data.getAll();

	// get duplicate count
	int ndup = 0;
	int sk = 0;
	int dk = 0;
	while( sk < sn && dk < dn ) {
		int cc = sv[ sk ] - dv[ dk ];
		if( cc < 0 )
			sk++;
		else
		if( cc > 0 )
			dk++;
		else {
			ndup++;
			sk++;
			dk++;
		}
	}

	// check need to add smth
	int nadd = sn - ndup;
	if( nadd == 0 )
		return;

	// resize
	data.setCount( dn + nadd );
	dv = data.getAll();

	// fill index data
	sk = sn - 1;
	dk = dn - 1;
	int wk = dk + nadd;
	while( sk >= 0 ) {
		if( dk < 0 )
			dv[ wk-- ] = sv[ sk-- ];
		else {
			int cc = sv[ sk ] - dv[ dk ];
			if( cc > 0 )
				dv[ wk-- ] = sv[ sk-- ];
			else {
				dv[ wk-- ] = dv[ dk-- ];
				if( cc == 0 )
					sk--;
			}
		}
	}
}

void NeuroSignal::setIndexData( int *indexData , int n ) {
	ASSERTMSG( n > 0 , "invalid count" );
	data.set( indexData , n );
}

String NeuroSignal::getCombinedState( NeuroSignal *srcSignal1 , NeuroSignal *srcSignal2 ) {
	NeuroSignal data;
	data.addIndexDataSorted( srcSignal1 );
	data.addIndexDataSorted( srcSignal2 );

	String ps;

	int *sd = srcSignal1 -> getIndexRawData();
	int sdn = srcSignal1 -> getDataSize();

	int *pd = srcSignal2 -> getIndexRawData();
	int pdn = srcSignal2 -> getDataSize();

	bool first = true;
	int ks = 0;
	int kp = 0;
	for( ; ks < sdn || kp < pdn; ) {
		if( first  )
			first = false;
		else
			ps += ".";

		if( ks >= sdn ) {
			ps += pd[ kp++ ];
			ps += "p";
		}
		else
		if( kp >= pdn )
			ps += sd[ ks++ ];
		else {
			if( pd[ kp ] < sd[ ks ] ) {
				ps += pd[ kp++ ];
				ps += "p";
			}
			else
			if( pd[ kp ] > sd[ ks ] )
				ps += sd[ ks++ ];
			else {
				ps += sd[ ks++ ];
				kp++;
			}
		}
	}

	return( ps );
}
