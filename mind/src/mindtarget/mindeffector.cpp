#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindEffector::MindEffector( EffectorArea *p_area )
:	MindRegion( p_area ) {
}

void MindEffector::createRegion( MindRegionCreateInfo *info ) {
	// call effector creation
	createEffector( info );
	logger.logDebug( String( "createRegion: created effector region" ) );
}

void MindEffector::exitRegion() {
}

void MindEffector::destroyRegion() {
}

