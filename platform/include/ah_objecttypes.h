#ifndef	INCLUDE_AH_OBJECTTYPES_H
#define INCLUDE_AH_OBJECTTYPES_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_objects.h"

// object-based helper classes
class Scale;
class MultiIndexIterator;

/*#########################################################################*/
/*#########################################################################*/

class Scale : public Object
{
public:
	// Object interface
	virtual const char *getClass() { return( "Scale" ); };
	virtual void serialize( SerializeObject& so );
	virtual void deserialize( Object *parent , SerializeObject& so );

	static void createSerializeObject();
	static Object *onCreate( const char *className ) { return( new Scale ); };
	static SerializeObject *getSerializeObject()
		{ return( ObjectService::getService() -> getSerializeObject( "Scale" ) ); };

public:
	Scale();

	void set( const Scale& scale );
	void setRangeFrom( float minValue , float maxValue );
	void setRangeTo( float minValue , float maxValue );
	void setDoScale( bool p_doScale );
	bool getDoScale() { return( doScale ); };

	float getFromMin() { return( rangeFromMin ); };
	float getFromMax() { return( rangeFromMax ); };
	float getFromRange() { return( rangeFromMax - rangeFromMin ); };
	float getToMin() { return( rangeToMin ); };
	float getToMax() { return( rangeToMax ); };
	float getToRange() { return( rangeToMax - rangeToMin ); };
	float getRate() { return( rateFromTo ); };

	float scale( float value );
	float unscale( float value );
	float unscaleDiffNormalized( float toValue1 , float toValue2 );

private:
	void recalculateRate();

private:
	float rangeFromMin;
	float rangeFromMax;
	float rangeToMin;
	float rangeToMax;
	float rateFromTo;

	bool doScale;
};

// #############################################################################
// #############################################################################

class MultiIndexIterator : public Object {
public:
	MultiIndexIterator( int numberOfAxis , int axisPoints );
	~MultiIndexIterator();

public:
	void start();
	void startAround( MultiIndexIterator& point );
	void startDistinctUnsorted();

	bool next();
	bool nextAround();
	bool nextDistinctUnsorted();

	int getGlobalIndex();
	int getGlobalIndexAround();

	int getAxisIndex( int axis );
	int getAxisIndexAround( int axis );

	int getNumberOfAxis();
	int getAxisPoints();

	bool hasEqualIndexes();

private:
	bool isBeyondAround();

private:
	int totalPoints;
	int numberOfAxis;
	int axisPoints;

	int *axisIndex; // [numberOfAxis]
	int globalIndex;

	int *axisIndexAround; // [numberOfAxis], delta for axisIndex, 0=0,1=1,2=-1
	int globalIndexAround;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_OBJECTTYPES_H
