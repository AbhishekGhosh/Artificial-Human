#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

NerveRegion::NerveRegion( MindArea *p_area )
:	MindRegion( p_area ) {
	attachLogger();
}

String NerveRegion::getRegionType() {
	return( "NerveRegion" );
}

void NerveRegion::createRegion( MindRegionCreateInfo *info ) {
	ASSERTFAILED( "createNerveRegion: not implemented" );
}

void NerveRegion::exitRegion() {
}

void NerveRegion::destroyRegion() {
}
