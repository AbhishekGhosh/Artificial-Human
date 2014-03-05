#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

void ModelVerifier::checkCircuits() {
	// check circuits use correct components
	logger.logInfo( "checkCircuits: CHECK CIRCUITS ..." );

	bool checkAll = true;
	StringList circuits;
	circuitsxml.getCircuitList( circuits );

	for( int k = 0; k < circuits.count(); k++ ) {
		String id = circuits.get( k );

		// verify
		logger.logInfo( "checkCircuits: verify circuit id=" + id );
		bool checkOne = checkCircuits_verifyComponents( id );
		if( checkOne == false )
			checkAll = false;

		checkOne = checkCircuits_verifyLinks( id );
		if( checkOne == false )
			checkAll = false;
	}

	if( checkAll )
		logger.logInfo( "checkCircuits: CIRCUITS ARE OK" );
	else
		logger.logInfo( "checkCircuits: CIRCUITS HAVE ERRORS" );
}

bool ModelVerifier::checkCircuits_verifyComponents( String circuit ) {
	XmlCircuitInfo& info = circuitsxml.getCircuitInfo( circuit );

	// check components
	bool checkAll = true;
	for( int k = 0; k < info.componentMapping.count(); k++ ) {
		String originalComponentId = info.componentMapping.getKeyByIndex( k );
		String modelComponentId = info.componentMapping.getClassByIndex( k );

		// check region in hierarchy
		if( !hmindxml.isComponent( modelComponentId ) ) {
			checkAll = false;
			logger.logError( "checkCircuits_verifyComponents: unknown region=" + modelComponentId + " mapped from component=" + originalComponentId );
		}
	}

	return( checkAll );
}

bool ModelVerifier::checkCircuits_verifyLinks( String circuit ) {
	XmlCircuitInfo& info = circuitsxml.getCircuitInfo( circuit );

	// check links
	bool checkAll = true;
	FlatList<Xml> links;
	circuitsxml.getCircuitLinks( circuit , links );

	for( int k = 0; k < links.count(); k++ ) {
		Xml link = links.get( k );

		XmlCircuitLinkInfo linkinfo;
		circuitsxml.getCircuitLinkInfo( link , linkinfo );

		bool checkOne = checkCircuits_verifyCircuitLink( info , linkinfo );
		if( checkOne == false )
			checkAll = false;
	}

	return( checkAll );
}

bool ModelVerifier::checkCircuits_verifyCircuitLink( XmlCircuitInfo& circuit , XmlCircuitLinkInfo& link ) {
	// use mapping
	String compSrc = circuitsxml.mapComponent( circuit , link.compSrc );
	String compDst = circuitsxml.mapComponent( circuit , link.compDst );

	// ignore mapping errors
	if( hmindxml.isComponent( compSrc ) == false || hmindxml.isComponent( compDst ) == false )
		return( true );
	
	// ignore if it is connection from one component part to another, not listed in mind tree
	if( compSrc.equals( compDst ) )
		return( true );

	// find mapped regions
	String regionSrcId = hmindxml.getMappedRegion( compSrc );
	String regionDstId = hmindxml.getMappedRegion( compDst );

	// specific check if mapped to upper items
	if( regionSrcId.isEmpty() || regionDstId.isEmpty() ) {
		bool check = checkLinkCoveredByModel( compSrc , compDst );
		if( check == false )
			logger.logError( "checkCircuits_verifyCircuitLink: not found covering child link element=" + compSrc + " to element=" + compDst + ", from circuit component=" + link.compSrc + " to component=" + link.compDst );
		return( check );
	}

	MindService *ms = MindService::getService();

	// ignore check if no correct mapping
	if( ms -> isMindRegion( regionSrcId ) == false || ms -> isMindRegion( regionDstId ) == false )
		return( true );

	MindRegion *regionSrc = ms -> getMindRegion( regionSrcId );
	MindRegion *regionDst = ms -> getMindRegion( regionDstId );

	// check link exists from src to dst
	if( regionSrc -> checkLinkedTo( regionDst ) )
		return( true );

	// this link does not exist in mind model
	logger.logError( "checkCircuits_verifyCircuitLink: not found link from region=" + regionSrcId + " to region=" + regionDstId + ", from circuit component=" + link.compSrc + " to component=" + link.compDst );
	return( false );
}

