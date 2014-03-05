#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

void ModelVerifier::checkHierarchy() {
	logger.logInfo( "checkHierarchy: CHECK HIERARCHY..." );

	hierarchyMap.clear();

	bool regionsMappedAll = true;
	StringList divisions;
	hmindxml.getDivisions( divisions );

	for( int k = 0; k < divisions.count(); k++ ) {
		String division = divisions.get( k );
		bool regionsMapped = checkHierarchy_verifyChild( division , true );
		if( regionsMapped == false )
			regionsMappedAll = false;
	}

	if( regionsMappedAll )
		logger.logInfo( "checkHierarchy: HIERARCHY IS OK" );
	else
		logger.logInfo( "checkHierarchy: HIERARCHY HAS ERRORS" );
}

bool ModelVerifier::checkHierarchy_verifyChild( String parentNode , bool checkMapping ) {
	const XmlHMindElementInfo& info = hmindxml.getElementInfo( parentNode );

	logger.logInfo( "check item=" + info.name );

	bool mappedOwn = false;
	if( checkMapping ) {
		// verify mind mapping
		if( info.mapped ) {
			MindRegionDef *rd = regionMap.get( info.id );

			if( rd == NULL ) {
				logger.logError( "checkHierarchy_verifyChild: " + info.name + " - hierarchy region with id=" + info.id + " is not mapped to mind model" );
				return( false );
			}

			if( !checkHierarchy_verifyConnectors( rd , info ) )
				return( false );

			hierarchyMap.add( info.id , rd );
		}

		// verify coverage
		mappedOwn = info.mapped;
	}

	bool mappedChildsAll = false;

	// check all childs
	StringList elements;
	hmindxml.getElements( parentNode , elements );

	if( elements.count() > 0 ) {
		mappedChildsAll = true;

		for( int k = 0; k < elements.count(); k++ ) {
			String node = elements.get( k );
			const XmlHMindElementInfo& childinfo = hmindxml.getElementInfo( node );

			bool checkMappingChilds = ( checkMapping == false || mappedOwn == true )? false : true;

			if( childinfo.ignore )
				checkMappingChilds = false;

			bool regionsMappedChilds = checkHierarchy_verifyChild( node , checkMappingChilds );
			if( childinfo.ignore == false && regionsMappedChilds == false )
				mappedChildsAll = false;
		}
	}

	if( mappedOwn )
		return( true );

	if( mappedChildsAll )
		return( true );

	if( checkMapping )
		logger.logError( "checkHierarchy_verifyChild: id=" + info.id + ", name=" + info.name + " - region has no complete mapping to mind map" );

	return( false );
}

bool ModelVerifier::checkHierarchy_verifyConnectors( MindRegionDef *rd , const XmlHMindElementInfo& info ) {
	MindRegionRoleEnum role = rd -> getRole();
	if( role != MIND_REGION_ROLE_TARGETSENSOR && role != MIND_REGION_ROLE_TARGETEFFECTOR )
		return( true );

	MindService *ms = MindService::getService();
	MindRegion *region = ms -> getMindRegion( rd -> getId() );

	StringList elements;
	hmindxml.getConnectors( info.id , elements );

	// check connectors are mapped to muscles and glands
	bool status = true;
	for( int k = 0; k < elements.count(); k++ ) {
		String connector = elements.get( k );
		const XmlHMindElementInfo& cinfo = hmindxml.getElementInfo( connector );

		// ignore sensors
		if( region -> getNeuroLinkSource( connector ) )
			continue;

		// ensure motor
		if( !region -> getNeuroLinkTarget( connector ) ) {
			logger.logError( "checkHierarchy_verifyConnectors: muscle id=" + info.id + " - unknown target connector id=" + connector );
			status = false;
		}

		// verify motor connector
		XmlMuscleInfo *muscle = musclesxml.findByConnector( cinfo.name );
		if( muscle == NULL ) {
			status = false;
			logger.logError( "checkHierarchy_verifyConnectors: element=" + info.id + ", connector=" + cinfo.name + " - is not found in muscles and glands" );
		}
	}

	return( status );
}

