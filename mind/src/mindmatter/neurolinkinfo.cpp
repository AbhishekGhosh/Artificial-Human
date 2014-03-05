#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

NeuroLinkInfo::NeuroLinkInfo() {
	regionLink = NULL;
	linkDef = NULL;
}

void NeuroLinkInfo::setNeuroTransmitter( String p_nt ) {
	nt = p_nt;
}

void NeuroLinkInfo::setLinkDef( MindConnectionLinkTypeDef *p_linkDef ) {
	linkDef = p_linkDef;
}

void NeuroLinkInfo::setRegionLink( MindRegionLink *p_regionLink ) {
	regionLink = p_regionLink;
}
