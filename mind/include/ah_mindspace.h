#ifndef	INCLUDE_AH_MINDSPACE_H
#define INCLUDE_AH_MINDSPACE_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_mindbase.h"

class MindSpace;
class MindLocation;
class MindLocationInfo;

/*#########################################################################*/
/*#########################################################################*/

class MindSpace : public Object {
public:
	MindSpace();
	~MindSpace() {};
	virtual const char *getClass() { return( "MindSpace" ); };

// operations
public:
	void createFromXml( Xml xml );
};

/*#########################################################################*/
/*#########################################################################*/

// every mind area and cortex have exact location in brain 3D space
// location is parallelogramm, centered in given absolute point, and having X, Y, Z dimentions
// brain has no limits
// mind area has pre-configured location in brain space
// inter-area connections between neurons are established within specific mind link, all mind links are pre-configured
// inter-cortex connections between neurons are established within specific cortex link, all cortex links are dynamic
// connections evolve all the time
// probability of establishing inter-area and inter-cortex connections depends on distance between neurons
// to diminish rough shapes of mind areas and cortexes, distances are adjusted by factor equal to distance between cortex/area centers
// brain controls that areas/cortexes do not overlap
class MindLocation : public Object {
public:
	MindLocation() {
		x = y = z = 0;
		dx = dy = dz = 0;
	};
	virtual const char *getClass() { return( "MindLocation" ); };

	void setPosition( int p_x , int p_y , int p_z ) { x = p_x; y = p_y; z = p_z; };
	void setDimensions( int p_dx , int p_dy , int p_dz ) { dx = p_dx; dy = p_dy; dz = p_dz; };
	int getSize() const { return( dx * dy * dz ); };

	// relative is counted from lower corner
	MindLocation getAbsoluteLocation( const MindLocation& relativeLocation ) const;
	MindLocation getRelativeLocation( const MindLocation& absoluteLocation ) const;
	bool placeLocation( MindLocation& cover , MindLocation& add ) const;
	void getCenter( int& cx , int& cy , int& cz ) const;
	void movePosition( int cx , int cy , int cz );
	void resize( int cx , int cy , int cz );
	void setSurfaceDimensions( int d1 , int d2 );
	void center( const MindLocation& parent );
	void moveInside( MindLocation& relativePosition ) const;

	// get inputs/outputs surface
	int getSurfaceSize() const;

private:
	bool placeLocationFirst( MindLocation& cover , MindLocation& add ) const;

private:
// utility
	// lower corner position (relative to parent)
	int x;
	int y;
	int z;

	// dimensions
	int dx;
	int dy;
	int dz;
};

/*#########################################################################*/
/*#########################################################################*/

class MindLocationInfo : public Object {
public:
	MindLocationInfo();
	virtual const char *getClass() { return( "MindLocationInfo" ); };

// operations
public:
	void createFromXml( Xml xml );

	MindLocation *createLocation();

private:
	int posX , posY, posZ;
	int sizeX , sizeY , sizeZ;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_MINDSPACE_H
