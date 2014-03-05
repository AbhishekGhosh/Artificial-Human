#include "stdafx.h"
#include "ahumantarget.h"

static const int S_FIBER_MAX = 7;
static const char *staticFiberTypeNerveMods[ S_FIBER_MAX ][6] = {
	{ "GSE" , "cranial motor" , "flexor motor" , "extensor motor" , NULL } ,
	{ "GSA" , "general sensory" , NULL , NULL , NULL } ,
	{ "SVA" , "general sensory" , NULL , NULL , NULL } ,
	{ "GVA" , "autonomic sensory" , NULL , NULL , NULL } ,
	{ "GVE" , "sympathetic motor" , "parasympathetic motor" , "cranial motor" , NULL } ,
	{ "SSA" , "special sensory" , NULL , NULL , NULL } ,
	{ "SVE" , "visceral motor" , NULL , NULL , NULL }
};

/*#########################################################################*/
/*#########################################################################*/

void ModelVerifier::checkNerves() {
	// check nerves use correct components
	logger.logInfo( "checkNerves: CHECK NERVES ..." );

	bool checkAll = true;
	StringList nerves;
	nervesxml.getNerveList( nerves );

	for( int k = 0; k < nerves.count(); k++ ) {
		String id = nerves.get( k );

		// verify
		logger.logInfo( "checkNerves: verify nerve id=" + id );
		if( !checkNerves_verifyComponents( id ) )
			checkAll = false;
	}

	// veriy modaility
	ClassList<XmlNerveInfo>& divs = nervesxml.getDivisions();
	for( int k = 0; k < divs.count(); k++ ) {
		XmlNerveInfo& div = divs.getRef( k );
		for( int z = 0; z < div.childs.count(); z++ ) {
			MapStringToPtr fibers;
			if( !checkNerves_verifyModalityByChilds( div.childs.getClassRefByIndex( z ) , fibers ) )
				checkAll = false;
		}
	}

	if( checkAll )
		logger.logInfo( "checkNerves: NERVES ARE OK" );
	else
		logger.logInfo( "checkNerves: NERVES HAVE ERRORS" );
}

bool ModelVerifier::checkNerves_verifyComponents( String nerve ) {
	XmlNerveInfo& info = nervesxml.getNerveInfo( nerve );

	bool res = true;

	// check nerve modality
	for( int k = 0; k < info.fibers.count(); k++ ) {
		XmlNerveFiberInfo& nf = info.fibers.getRef( k );

		logger.logInfo( "checkNerves: check fiber src=" + nf.src + ", dst=" + nf.dst );

		// check items
		if( !checkFiberComp( info , nf , nf.src ) )
			res = false;
		if( !checkFiberComp( info , nf , nf.dst ) )
			res = false;

		for( int m = 0; m < nf.mids.count(); m++ ) {
			String mid = nf.mids.get( m );
			if( !checkFiberComp( info , nf , mid ) )
				res = false;
		}

		// check type
		if( !checkFiberType( info , nf , nf.type ) )
			res = false;

		// check links
		if( res ) {
			if( !checkNerves_verifyLinks( info , nf ) )
				res = false;
		}
	}

	return( res );
}

bool ModelVerifier::checkNerves_verifyModalityByChilds( XmlNerveInfo& nerve , MapStringToPtr& parentFibers ) {
	bool res = true;

	// add own fibers
	MapStringToPtr fibers;
	fibers.addnew( parentFibers );
	for( int k = 0; k < nerve.fibers.count(); k++ )
		fibers.addnew( nerve.fibers.getRef( k ).type , this );

	// get child modalities
	MapStringToPtr childMods;
	if( nerve.type.equals( "root" ) ) {
		for( int k = 0; k < nerve.rootNerves.count(); k++ ) {
			String rn = nerve.rootNerves.get( k );
			XmlNerveInfo& rootnerve = nervesxml.getNerveInfo( rn );
			childMods.addnew( rootnerve.mods , this );
		}
	}
	else {
		// verify childs if not root
		for( int k = 0; k < nerve.childs.count(); k++ ) {
			XmlNerveInfo& child = nerve.childs.getClassRefByIndex( k );
			if( !checkNerves_verifyModalityByChilds( child , fibers ) )
				res = false;

			// get modality from childs
			childMods.addnew( child.mods , this );
		}

		// check childs are present in nerve
		for( int k = 0; k < childMods.count(); k++ ) {
			String mod = childMods.getKeyByIndex( k );
			if( nerve.mods.find( mod ) < 0 ) {
				logger.logError( "checkNerves_verifyModalityByChilds: child nerve modality=" + mod + " is not found in nerve=" + nerve.name );
				res = false;
			}
		}
	}

	// check fibers are covered by nerve
	for( int k = 0; k < fibers.count(); k++ ) {
		String ft = fibers.getKeyByIndex( k );
		if( !checkNerves_verifyFiberIsCovered( ft , nerve.mods ) ) {
			logger.logError( "checkNerves_verifyModalityByChilds: fiber type=" + ft + " is not covered by modality in nerve=" + nerve.name );
			res = false;
		}
	}

	// own modality should be covered by child modality or by fibers
	for( int k = 0; k < nerve.mods.count(); k++ ) {
		String mod = nerve.mods.get( k );

		// check childs
		if( childMods.get( mod ) != NULL )
			continue;
		
		// check fibers or nerves
		if( !checkNerves_verifyModalityIsCoveredByFibers( mod , fibers ) ) {
			logger.logError( "checkNerves_verifyModalityByChilds: nerve modality=" + mod + " is not found in childs or fibers, nerve=" + nerve.name );
			res = false;
		}
	}

	return( res );
}

bool ModelVerifier::checkNerves_verifyFiberIsCovered( String ft , StringList& mods ) {
	for( int k = 0; k < S_FIBER_MAX; k++ )
		if( ft.equals( staticFiberTypeNerveMods[ k ][ 0 ] ) ) {
			for( int z = 1; staticFiberTypeNerveMods[ k ][ z ] != NULL; z++ )
				if( mods.find( staticFiberTypeNerveMods[ k ][ z ] ) >= 0 )
					return( true );
			return( false );
		}
	return( false );
}

bool ModelVerifier::checkNerves_verifyModalityIsCoveredByFibers( String mod , MapStringToPtr& fibers ) {
	for( int v = 0; v < fibers.count(); v++ ) {
		String ft = fibers.getKeyByIndex( v );
		for( int k = 0; k < S_FIBER_MAX; k++ )
			if( ft.equals( staticFiberTypeNerveMods[ k ][ 0 ] ) ) {
				for( int z = 1; staticFiberTypeNerveMods[ k ][ z ] != NULL; z++ )
					if( mod.find( staticFiberTypeNerveMods[ k ][ z ] ) >= 0 )
						return( true );
			}
	}
	return( false );
}

bool ModelVerifier::checkFiberComp( XmlNerveInfo& info , XmlNerveFiberInfo& nf , String comp ) {
	if( hmindxml.isComponent( comp ) == false ) {
		logger.logError( "checkFiberComp: nerve=" + info.name + ", comp=" + comp + " - is not found in hierarchy" );
		return( false );
	}
	
	// find mapped regions
	String region = hmindxml.getMappedRegion( comp );
	if( region.isEmpty() ) {
		logger.logError( "checkFiberComp: nerve=" + info.name + ", comp=" + comp + " - is not mapped to region" );
		return( false );
	}

	return( true );
}

bool ModelVerifier::checkNerves_verifyLinks( XmlNerveInfo& info , XmlNerveFiberInfo& nf ) {
	String src = hmindxml.getMappedRegion( nf.src );
	String dst;

	bool res = true;

	ModelVerifierFiberChainPosEnum pos;
	int midleft;
	int midright;
	for( int k = 0; k <= nf.mids.count(); k++ ) {
		if( k == nf.mids.count() )
			dst = nf.dst;
		else
			dst = nf.mids.get( k );
		dst = hmindxml.getMappedRegion( dst );

		midleft = k;
		midright = nf.mids.count() - k;
		if( k == 0 )
			pos = FIBER_CHAIN_POS_BEGIN;
		else if( k == nf.mids.count() )
			pos = FIBER_CHAIN_POS_END;
		else
			pos = FIBER_CHAIN_POS_MID;

		if( !checkNerves_verifyFiberChain( info , nf , src , dst , pos , midleft , midright ) ) {
			logger.logError( "checkNerves_verifyLinks: nerve=" + info.name + ", src=" + src + ", dst=" + dst + " - is not covered by mind" );
			res = false;
		}

		src = dst;
	}

	return( res );
}

bool ModelVerifier::checkFiberType( XmlNerveInfo& info , XmlNerveFiberInfo& nf , String type ) {
	// compare fiber type and nerve modality
	bool res = true;
	for( int k = 0; k < S_FIBER_MAX; k++ )
		if( type.equals( staticFiberTypeNerveMods[ k ][ 0 ] ) ) {
			for( int z = 1; staticFiberTypeNerveMods[ k ][ z ] != NULL; z++ )
				if( info.mods.find( staticFiberTypeNerveMods[ k ][ z ] ) >= 0 )
					return( true );

			logger.logError( "checkFiberType: nerve=" + info.name + ", type=" + type + " - is incompatible with modality=" + info.modality );
			return( false );
		}

	logger.logError( "checkFiberType: nerve=" + info.name + ", type=" + type + " - is unknown type" );
	return( false );
}

bool ModelVerifier::checkNerves_verifyFiberChain( XmlNerveInfo& info , XmlNerveFiberInfo& nf , String regionSrcId , String regionDstId , ModelVerifierFiberChainPosEnum pos , int midleft , int midright ) {
	MindService *ms = MindService::getService();

	// ignore check if no correct mapping
	if( ms -> isMindRegion( regionSrcId ) == false || ms -> isMindRegion( regionDstId ) == false )
		return( true );

	MindRegion *regionSrc = ms -> getMindRegion( regionSrcId );
	MindRegion *regionDst = ms -> getMindRegion( regionDstId );

	bool res = true;

	// check link exists from src to dst
	if( !regionSrc -> checkLinkedTo( regionDst ) ) {
		res = false;
		logger.logError( "checkNerves_verifyFiberChain: not found link from region=" + regionSrcId + " to region=" + regionDstId + ", from nerve=" + info.name );
	}

	// check fiber type is valid
	ModelFiberValidator fv;
	if( nf.type.equals( "GSE" ) || nf.type.equals( "SVE" ) ) {
		if( pos == FIBER_CHAIN_POS_END )
			if( !fv.isValid_GSE_end( info , regionSrc , regionDst ) )
				res = false;
	}
	else if( nf.type.equals( "GSA" ) || nf.type.equals( "GVA" ) ) {
		if( pos == FIBER_CHAIN_POS_BEGIN )
			if( !fv.isValid_GSA_begin( info , regionSrc , regionDst ) )
				res = false;
		if( midleft == 1 )
			if( !fv.isValid_GSA_afterbegin( info , regionSrc , regionDst ) )
				res = false;
	}
	else if( nf.type.equals( "GVE" ) ) {
		if( pos == FIBER_CHAIN_POS_END )
			if( !fv.isValid_GVE_end( info , regionSrc , regionDst ) )
				res = false;
		if( midright == 1 )
			if( !fv.isValid_GVE_beforeend( info , regionSrc , regionDst ) )
				res = false;
	}
	else if( nf.type.equals( "SSA" ) ) {
		if( pos == FIBER_CHAIN_POS_BEGIN )
			if( !fv.isValid_SSA_begin( info , regionSrc , regionDst ) )
				res = false;
	}
	else if( nf.type.equals( "SVA" ) ) {
		if( pos == FIBER_CHAIN_POS_BEGIN )
			if( !fv.isValid_SVA_begin( info , regionSrc , regionDst ) )
				res = false;
		if( midleft == 1 )
			if( !fv.isValid_SVA_afterbegin( info , regionSrc , regionDst ) )
				res = false;
	}

	return( res );
}

