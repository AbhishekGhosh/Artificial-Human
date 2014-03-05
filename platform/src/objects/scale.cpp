#include <ah_platform.h>
#include <ah_objecttypes.h>

/*#########################################################################*/
/*#########################################################################*/

// class Scale
Scale::Scale()
{
	rangeFromMin = 0;
	rangeFromMax = 1;
	rangeToMin = 0;
	rangeToMax = 1;
	rateFromTo = 1;

	doScale = false;
}

void Scale::setRangeFrom( float minValue , float maxValue )
{
	rangeFromMin = minValue;
	rangeFromMax = maxValue;

	doScale = true;
	recalculateRate();
}

void Scale::setRangeTo( float minValue , float maxValue )
{
	rangeToMin = minValue;
	rangeToMax = maxValue;

	doScale = true;
	recalculateRate();
}

float Scale::scale( float value )
{
	if( !doScale )
		return( value );

	float v = rangeToMin + ( value - rangeFromMin ) * rateFromTo;
	return( v );
}

float Scale::unscale( float value )
{
	if( !doScale )
		return( value );

	float v = rangeFromMin + ( value - rangeToMin ) / rateFromTo;
	return( v );
}

void Scale::setDoScale( bool p_doScale )
{
	doScale = p_doScale;
}

void Scale::set( const Scale& scale )
{
	rangeFromMin = scale.rangeFromMin;
	rangeFromMax = scale.rangeFromMax;
	rangeToMin = scale.rangeToMin;
	rangeToMax = scale.rangeToMax;
	rateFromTo = scale.rateFromTo;

	doScale = scale.doScale;
}

float Scale::unscaleDiffNormalized( float toValue1 , float toValue2 )
{
	ASSERT( doScale );
	return( ( unscale( toValue1 ) - unscale( toValue2 ) ) / ( rangeFromMax - rangeFromMin ) );
}
	
/*#########################################################################*/
/*#########################################################################*/

void Scale::recalculateRate()
{
	rateFromTo = ( rangeToMax - rangeToMin ) / ( rangeFromMax - rangeFromMin );

	// check no scaling
	if( rateFromTo == 1 )
		doScale = false;
}

/*#########################################################################*/
/*#########################################################################*/

// AIObject interface
void Scale::createSerializeObject()
{
	SerializeObject *so = new SerializeObject( "Scale" );
	so -> setFactoryMethod( Scale::onCreate );

	so -> addFieldFloat( "rangeFromMin" );
	so -> addFieldFloat( "rangeFromMax" );
	so -> addFieldFloat( "rangeToMin" );
	so -> addFieldFloat( "rangeToMax" );
	so -> addFieldFloat( "rateFromTo" );
	so -> addFieldBool( "doScale" );
}

void Scale::serialize( SerializeObject& so )
{
	so.setPropFloat( rangeFromMin , "rangeFromMin" );
	so.setPropFloat( rangeFromMax , "rangeFromMax" );
	so.setPropFloat( rangeToMin , "rangeToMin" );
	so.setPropFloat( rangeToMax , "rangeToMax" );
	so.setPropFloat( rateFromTo , "rateFromTo" );
	so.setPropBool( doScale , "doScale" );
}

void Scale::deserialize( Object *parent , SerializeObject& so )
{
	rangeFromMin = so.getPropFloat( "rangeFromMin" );
	rangeFromMax = so.getPropFloat( "rangeFromMax" );
	rangeToMin = so.getPropFloat( "rangeToMin" );
	rangeToMax = so.getPropFloat( "rangeToMax" );
	rateFromTo = so.getPropFloat( "rateFromTo" );
	doScale = so.getPropBool( "doScale" );
}
