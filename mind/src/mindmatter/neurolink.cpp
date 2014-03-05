#include <ah_mind.h>
#include <ah_mind_impl.h>

static int sequence_NL = 0;

/*#########################################################################*/
/*#########################################################################*/

NeuroLink::NeuroLink( NeuroLinkSource *src , NeuroLinkTarget *dst ) {
	source = src;
	target = dst;

	primary = false;
	regionLink = NULL;
	region = NULL;
	linkType = NULL;

	sizeX = 0;
	sizeY = 0;
}

void NeuroLink::createNeuroLink( NeuroLinkInfo *info ) {
	id = source -> getInstance() + String( "-" ) + target -> getInstance();
	Object::setInstance( id );

	linkType = info -> getLinkDef();
	regionLink = info -> getRegionLink();

	// set sizes if not defined by MindCircuitLinkDef
	sizeX = source -> getSizeX();
	sizeY = source -> getSizeY();
}

int NeuroLink::getSize() {
	return( sizeX * sizeY );
}
