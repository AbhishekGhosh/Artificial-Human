#include <ah_mind.h>

/*#########################################################################*/
/*#########################################################################*/

MindLocation MindLocation::getAbsoluteLocation( const MindLocation& relativeLocation ) const {
	MindLocation location = relativeLocation;

	// offset lower corner for own coords
	location.x += x;
	location.y += y;
	location.z += z;
	return( location );
}

MindLocation MindLocation::getRelativeLocation( const MindLocation& absoluteLocation ) const {
	MindLocation location = absoluteLocation;

	// offset lower corner for own coords
	location.x -= x;
	location.y -= y;
	location.z -= z;
	return( location );
}

bool MindLocation::placeLocation( MindLocation& cover , MindLocation& add ) const {
	if( cover.getSize() == 0 )
		return( placeLocationFirst( cover , add ) );
	return( false );
}

bool MindLocation::placeLocationFirst( MindLocation& cover , MindLocation& add ) const {
	// first placement - use lower left corner
	if( add.dx > dx || add.dy > dy || add.dz > dz )
		return( false );
	
	// align by lower corner
	add.x = 0;
	add.y = 0;
	add.z = 0;
	
	// cover is the same
	cover = add;
	return( true );
}

void MindLocation::getCenter( int& cx , int& cy , int& cz ) const {
	cx = x + dx/2;
	cy = y + dy/2;
	cz = z + dz/2;
}

void MindLocation::movePosition( int cx , int cy , int cz ) {
	x += cx;
	y += cy;
	z += cz;
}

void MindLocation::resize( int cx , int cy , int cz ) {
	dx += cx;
	dy += cy;
	dz += cz;
}

void MindLocation::center( const MindLocation& parent ) {
	int cx , cy , cz;
	parent.getCenter( cx , cy , cz );

	x = cx - dx / 2;
	y = cy - dy / 2;
	z = cz - dz / 2;
}

void MindLocation::moveInside( MindLocation& relativePosition ) const {
	// check able to move in
	ASSERTMSG( relativePosition.dx <= dx &&
		relativePosition.dy <= dy &&
		relativePosition.dz <= dz , "Unable to place given location inside parent location" );

	// check boundaries
	if( relativePosition.x < 0 )
		relativePosition.x = 0;
	else
	if( relativePosition.x + relativePosition.dx > x + dx )
		relativePosition.x = x + dx - relativePosition.dx;

	if( relativePosition.y < 0 )
		relativePosition.y = 0;
	else
	if( relativePosition.y + relativePosition.dy > y + dy )
		relativePosition.y = y + dy - relativePosition.dy;

	if( relativePosition.z < 0 )
		relativePosition.z = 0;
	else
	if( relativePosition.z + relativePosition.dz > z + dz )
		relativePosition.z = z + dz - relativePosition.dz;
}

