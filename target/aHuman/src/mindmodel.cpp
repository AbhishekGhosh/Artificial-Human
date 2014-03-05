#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

MindModel::MindModel() {
	attachLogger();
}

MindModel::~MindModel() {
}

void MindModel::load() {
	// read definitions
	hmindxml.load();
	circuitsxml.load();
	nervesxml.load();
	musclesxml.load();
}

bool MindModel::checkLinkCoveredByModel( String compSrc , String compDst ) {
	MindService *ms = MindService::getService();

	// find mapped regions
	String regionSrcId = hmindxml.getMappedRegion( compSrc );
	String regionDstId = hmindxml.getMappedRegion( compDst );
	if( regionSrcId.isEmpty() == false && regionDstId.isEmpty() == false ) {
		MindRegion *srcRegion = ms -> getMindRegion( regionSrcId );
		MindRegion *dstRegion = ms -> getMindRegion( regionDstId );
		return( srcRegion -> checkLinkedTo( dstRegion ) );
	}

	// collect sets of child regions
	StringList srcRegions;
	if( regionSrcId.isEmpty() )
		hmindxml.getChildRegions( compSrc , srcRegions );
	else
		srcRegions.add( regionSrcId );

	StringList dstRegions;
	if( regionDstId.isEmpty() )
		hmindxml.getChildRegions( compDst , dstRegions );
	else
		dstRegions.add( regionDstId );

	// check mapping
	for( int k1 = 0; k1 < srcRegions.count(); k1++ ) {
		String srcRegionId = srcRegions.get( k1 );
		if( !ms -> isMindRegion( srcRegionId ) )
			continue;

		MindRegion *srcRegion = ms -> getMindRegion( srcRegionId );
		for( int k2 = 0; k2 < dstRegions.count(); k2++ ) {
			String dstRegionId = dstRegions.get( k2 );
			if( !ms -> isMindRegion( dstRegionId ) )
				continue;

			MindRegion *dstRegion = ms -> getMindRegion( dstRegionId );
			if( srcRegion -> checkLinkedTo( dstRegion ) )
				return( true );
		}
	}

	return( false );
}

