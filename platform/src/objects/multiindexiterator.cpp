#include <ah_platform.h>
#include <ah_objecttypes.h>

// #############################################################################
// #############################################################################

// class MultiIndexIterator
MultiIndexIterator::MultiIndexIterator( int p_numberOfAxis , int p_axisPoints )
{
	numberOfAxis = p_numberOfAxis;
	axisPoints = p_axisPoints;

	axisIndex = ( int * )calloc( numberOfAxis , sizeof( int ) );
	globalIndex = -1;

	axisIndexAround = ( int * )calloc( numberOfAxis , sizeof( int ) );
	globalIndexAround = -1;

	// calculate total count of points
	totalPoints = 1;
	for( int k = 0; k < numberOfAxis; k++ )
		totalPoints *= p_axisPoints;
}

MultiIndexIterator::~MultiIndexIterator()
{
	free( axisIndex );
	free( axisIndexAround );
}

void MultiIndexIterator::start()
{
	memset( axisIndex , 0 , numberOfAxis * sizeof( int ) );
	globalIndex = -1;
	axisIndex[ numberOfAxis - 1 ] = -1;
}

bool MultiIndexIterator::next()
{
	if( globalIndex < -1 )
		throw RuntimeError( "MultiIndexIterator::next: index < -1" );

	globalIndex++;

	// increment pos
	for( int k = numberOfAxis - 1; k >= 0; k-- ) {
		int v = ++(axisIndex[ k ]);
		if( v < axisPoints )
			return( true );

		if( k == 0 )
			break;

		axisIndex[ k ] = 0;
	}

	globalIndex = -2;
	return( false );
}

bool MultiIndexIterator::nextAround()
{
	// zero excluded from scan
	if( globalIndexAround < 0 || globalIndex < -1 )
		throw RuntimeError( "MultiIndexIterator::nextAround: index < 0" );

	// cycle to ignore marginal points
	bool scanEnded = false;
	do {
		globalIndexAround++;

		// increment pos (3 value by each axis)
		scanEnded = false;
		for( int k = numberOfAxis - 1; k >= 0; k-- ) {
			// check axis after update - is it within limits
			int v = ++(axisIndexAround[ k ]);

			// check in scan area for this axis
			if( v < 3 ) {
				// if beyond limits - go to next point
				if( isBeyondAround() )
					break;

				return( true );
			}

			if( k == 0 ) {
				scanEnded = true;
				break;
			}

			axisIndexAround[ k ] = 0;
		}
	}
	while( !scanEnded );

	globalIndexAround = -2;
	return( false );
}

bool MultiIndexIterator::isBeyondAround()
{
	for( int k = 0; k < numberOfAxis; k++ ) {
		int v = axisIndexAround[ k ];
		int newIndex = axisIndex[ k ] + ( ( v == 2 )? -1 : v );
		if( newIndex < 0 || newIndex >= axisPoints )
			return( true );
	}
	return( false );
}

void MultiIndexIterator::startAround( MultiIndexIterator& point )
{
	if( point.globalIndex < 0 )
		throw RuntimeError( "MultiIndexIterator::startAround: globalIndex < 0" );

	if( point.axisPoints != axisPoints || point.numberOfAxis != numberOfAxis )
		throw RuntimeError( "MultiIndexIterator::startAround: different dimentions" );

	memcpy( axisIndex , point.axisIndex , numberOfAxis * sizeof( int ) );
	globalIndex = point.globalIndex;

	memset( axisIndexAround , 0 , numberOfAxis * sizeof( int ) );
	globalIndexAround = 0;
}

int MultiIndexIterator::getGlobalIndex()
{
	if( globalIndex < 0 )
		throw RuntimeError( "MultiIndexIterator::getGlobalIndex: globalIndex < 0" );

	if( globalIndex >= totalPoints )
		throw RuntimeError( "MultiIndexIterator::getGlobalIndex: globalIndex >= totalPoints" );

	return( globalIndex );
}

int MultiIndexIterator::getGlobalIndexAround()
{
	// calculate global index
	int number = 0;
	int mul = 1;
	for( int k = numberOfAxis - 1; k >= 0; k-- ) {
		int v = axisIndexAround[ k ];
		int index = axisIndex[ k ] + ( ( v == 2 )? -1 : v );
			
		number += mul * index;
		mul *= axisPoints;
	}

	if( number < 0 || number >= totalPoints )
		throw RuntimeError( "MultiIndexIterator::getGlobalIndexAround: number < 0 || number >= totalPoints" );

	return( number );
}

int MultiIndexIterator::getAxisIndex( int axis )
{
	if( globalIndex < 0 )
		throw RuntimeError( "MultiIndexIterator::getAxisIndex: globalIndex < 0" );

	return( axisIndex[ axis ] );
}

int MultiIndexIterator::getAxisIndexAround( int axis )
{
	if( globalIndex < 0 || globalIndexAround < 0 )
		throw RuntimeError( "MultiIndexIterator::getAxisIndexAround: index < 0" );

	int v = axisIndexAround[ axis ];
	int index = axisIndex[ axis ] + ( ( v == 2 )? -1 : v );

	return( index );
}

int MultiIndexIterator::getNumberOfAxis()
{
	return( numberOfAxis );
}

int MultiIndexIterator::getAxisPoints()
{
	return( axisPoints );
}

bool MultiIndexIterator::hasEqualIndexes()
{
	for( int k = 0; k < numberOfAxis - 1; k++ ) {
		int index = axisIndex[ k ];
			
		// try find
		for( int m = k + 1; m < numberOfAxis; m++ )
			if( axisIndex[ m ] == index )
				return( true );
	}
	return( false );
}

void MultiIndexIterator::startDistinctUnsorted()
{
	memset( axisIndex , 0 , numberOfAxis * sizeof( int ) );
	globalIndex = -1;
}

bool MultiIndexIterator::nextDistinctUnsorted()
{
	// first point
	if( globalIndex == -1 ) {
		if( numberOfAxis > axisPoints )
			throw RuntimeError( "MultiIndexIterator::nextDistinctUnsorted: numberOfAxis > axisPoints" );

		int mul = 1;
		int val = 0;
		for( int k = numberOfAxis - 1; k >= 0; k-- , mul *= axisPoints ) {
			axisIndex[ k ] = k;
			val += mul * k;
		}

		globalIndex = val;
		return( true );
	}

	// next point
	for( int m = numberOfAxis - 1; m >= 0; m-- ) {
		int v = ++(axisIndex[ m ]);
			
		// v has value range [axisIndex[m-1]+1,axisPoints-numberOfAxis+m]
		// for m=0 it will be [0,axisPoints-numberOfAxis]
		// for m=numberOfAxis-1 it will be [...,axisPoints-1]

		if( v <= ( axisPoints - numberOfAxis + m ) ) {
			// set increasing values for all next
			for( int z = m + 1; z < numberOfAxis; z++ )
				axisIndex[ z ] = v + z - m;

			// calculate global index
			int mul = 1;
			int val = 0;
			for( int t = numberOfAxis - 1; t >= 0; t-- , mul *= axisPoints )
				val += axisIndex[ t ] * mul;

			globalIndex = val;
			return( true );
		}

		if( m == 0 )
			break;
	}

	globalIndex = -2;
	return( false );
}
