#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

CortexTemporalPoolerItem::CortexTemporalPoolerItem( int p_maxDepth ) {
	attachLogger();

	maxDepth = p_maxDepth;
	id = -1;
	usage = 0;

	streamData = ( int * )malloc( maxDepth * sizeof( int ) );
	for( int k = 0; k < maxDepth; k++ )
		streamData[ k ] = -1;
	streamDataSize = 0;
}

CortexTemporalPoolerItem::~CortexTemporalPoolerItem() {
	delete streamData;
}

int CortexTemporalPoolerItem::getId() {
	return( id );
}

void CortexTemporalPoolerItem::setId( int p_id ) {
	id = p_id;
}

void CortexTemporalPoolerItem::deleteSpatialPatternIfAny( int spatialPattern ) {
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

int CortexTemporalPoolerItem::getDifferencePercentage( int *data , int dataSize ) {
	int len = min( dataSize , streamDataSize );
	int diff = maxDepth - len;

	for( int k = 0; k < len; k++ )
		if( data[ k ] != streamData[ k ] )
			diff++;

	int diffPercentage = ( diff * 100 ) / maxDepth;
	return( diffPercentage );
}

int CortexTemporalPoolerItem::getPartialDifferencePercentage( int *data , int dataSize ) {
	int len = min( dataSize , streamDataSize );
	int diff = 0;

	for( int k = 0; k < len; k++ )
		if( data[ k ] != streamData[ k ] )
			diff++;

	int diffPercentage = ( diff * 100 ) / dataSize;
	return( diffPercentage );
}

void CortexTemporalPoolerItem::addUsage() {
	usage++;
}

int CortexTemporalPoolerItem::getUsage() {
	return( usage );
}

void CortexTemporalPoolerItem::setUsage( int p_usage ) {
	usage = p_usage;
}

void CortexTemporalPoolerItem::setData( int *data , int dataSize ) {
	for( int k = 0; k < dataSize; k++ )
		streamData[ k ] = data[ k ];
	for( int k = dataSize; k < maxDepth; k++ )
		streamData[ k ] = -1;
	streamDataSize = dataSize;
}

int CortexTemporalPoolerItem::getDataSize() {
	return( streamDataSize );
}

int CortexTemporalPoolerItem::getSpatialPattern( int index ) {
	ASSERTMSG( index <= streamDataSize , "getSpatialPattern: invalid index" );
	return( streamData[ index ] );
}

void CortexTemporalPoolerItem::logItem() {
	logger.logInfo( String( "id=" ) + id + ", usage=" + usage + ", state=" + getNumberedState() );
}

String CortexTemporalPoolerItem::getNumberedState() {
	String ps;
	for( int k = 0; k < streamDataSize; k++ ) {
		if( k > 0 )
			ps += ".";
		ps += streamData[ k ];
	}
	return( ps );
}
