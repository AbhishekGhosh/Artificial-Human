#include <ah_mind.h>
#include <ah_mind_impl.h>

static int lastRegionId = 0;

/*#########################################################################*/
/*#########################################################################*/

MindRegionLink::MindRegionLink( MindAreaLink *p_areaLink ) {
	attachLogger();

	areaLink = p_areaLink;
	connectionType = NULL;

	primary = false;
	src = NULL;
	dst = NULL;
}

MindRegion *MindRegionLink::getSrcRegion() {
	return( src );
}

MindRegion *MindRegionLink::getDstRegion() {
	return( dst );
}

void MindRegionLink::createRegionLink( MindConnectionTypeDef *def , MindRegion *srcRegion , MindRegion *dstRegion ) {
	id = String( "RL" ) + ++lastRegionId;
	Object::setInstance( id );
	src = srcRegion;
	dst = dstRegion;
	connectionType = def;

	logger.logDebug( "createRegionLink: region link created id=" + id + " from region id=" + srcRegion -> getFullRegionId() + " to region id=" + dstRegion -> getFullRegionId()  );
}

void MindRegionLink::addNeuroLink( NeuroLink *link ) {
	links.add( link );
}

void MindRegionLink::exitRegionLink() {
}

void MindRegionLink::destroyRegionLink() {
	links.destroy();
}
