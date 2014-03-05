#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindLocationInfo::MindLocationInfo() {
	posX = posY = posZ = 0;
	sizeX = sizeY = sizeZ = 0;
}

void MindLocationInfo::createFromXml( Xml xml ) {
	posX = xml.getIntAttribute( "posX" );
	posY = xml.getIntAttribute( "posY" );
	posZ = xml.getIntAttribute( "posZ" );

	sizeX = xml.getIntAttribute( "sizeX" );
	sizeY = xml.getIntAttribute( "sizeY" );
	sizeZ = xml.getIntAttribute( "sizeZ" );
}

MindLocation *MindLocationInfo::createLocation() {
	MindLocation *location = new MindLocation();
	location -> setPosition( posX , posY , posZ );
	location -> setDimensions( sizeX , sizeY , sizeZ );
	return( location );
}
