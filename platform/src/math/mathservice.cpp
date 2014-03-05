#include <ah_platform.h>

/*#########################################################################*/
/*#########################################################################*/

float getMin( float v1 , float v2 )
{
	return( ( v1 < v2 )? v1 : v2 );
}

float getMax( float v1 , float v2 )
{
	return( ( v1 > v2 )? v1 : v2 );
}

float getMinMul( float a1 , float a2 , float b1 , float b2 )
{
	float v1 = getMin( a1 * b1 , a2 * b2 );
	float v2 = getMin( a1 * b2 , a2 * b1 );
	return( getMin( v1 , v2 ) );
}

float getMaxMul( float a1 , float a2 , float b1 , float b2 )
{
	float v1 = getMax( a1 * b1 , a2 * b2 );
	float v2 = getMax( a1 * b2 , a2 * b1 );
	return( getMax( v1 , v2 ) );
}

int compareFloats( float v1 , float v2 , float tolerance )
{
	float delta = ( v1 > v2 )? v1 - v2 : v2 - v1;
	if( delta < tolerance )
		return( 0 );

	if( v1 < v2 )
		return( -1 );

	return( 1 );
}

/*#########################################################################*/
/*#########################################################################*/

Service *MathService::newService() {
	return( new MathService() );
}
