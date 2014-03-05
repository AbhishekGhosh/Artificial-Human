#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindRegion::MindRegion( MindArea *p_area ) {
	attachLogger();

	area = p_area;

	regionInfo = NULL;
	regionType = NULL;
	regionMasterLinkSet = new MindRegionLinkSet();
	regionSlaveLinkSet = new MindRegionLinkSet();
	poolSet = NULL;
	linkSet = NULL;
	location = NULL;
}

MindRegion::~MindRegion() {
}

void MindRegion::exit() {
	exitRegion();
}

void MindRegion::destroy() {
	destroyRegion();

	delete regionMasterLinkSet;
	delete regionSlaveLinkSet;
	regionMasterLinkSet = NULL;
	regionSlaveLinkSet = NULL;
}

String MindRegion::getFullRegionId() {
	return( area -> getAreaId() + "." + id );
}

void MindRegion::addPrivateNeuroLink( NeuroLink *nt ) {
	// add link to set
	linkSet -> addSetItem( nt );
}

NeuroLinkSource *MindRegion::getNeuroLinkSource( String entity ) {
	NeuroLinkSource *connector = sourceConnectorMap.get( entity );
	return( connector );
}

NeuroLinkTarget *MindRegion::getNeuroLinkTarget( String entity ) {
	NeuroLinkTarget *connector = targetConnectorMap.get( entity );
	return( connector );
}

ClassList<MindRegionLink>& MindRegion::getMasterRegionLinks() {
	return( regionMasterLinkSet -> getLinks() );
}

ClassList<MindRegionLink>& MindRegion::getSlaveRegionLinks() {
	return( regionSlaveLinkSet -> getLinks() );
}

void MindRegion::addSourceEntity( String entity , NeuroLinkSource *connector ) {
	ASSERTMSG( sourceConnectorMap.get( entity ) == NULL , "Duplicate entity=" + entity + " in region id=" + id );
	// check metadata
	MindRegionConnectorDef *connectorInfo = regionType -> getConnector( entity );
	sourceConnectorMap.add( entity , connector );
}

void MindRegion::addTargetEntity( String entity , NeuroLinkTarget *connector ) {
	ASSERTMSG( targetConnectorMap.get( entity ) == NULL , "Duplicate entity=" + entity + " in region id=" + id );
	// check metadata
	MindRegionConnectorDef *connectorInfo = regionType -> getConnector( entity );
	targetConnectorMap.add( entity , connector );
}

void MindRegion::sendMessage( MindMessage *msg ) {
}

void MindRegion::addMasterRegionLink( MindRegionLink *link ) {
	regionMasterLinkSet -> addSetItem( link );
}

void MindRegion::addSlaveRegionLink( MindRegionLink *link ) {
	regionSlaveLinkSet -> addSetItem( link );
}

bool MindRegion::checkLinkedTo( MindRegion *dst ) {
	if( regionMasterLinkSet -> checkLinked( dst , this ) )
		return( true );
	if( regionSlaveLinkSet -> checkLinked( this , dst ) )
		return( true );

	return( false );
}
