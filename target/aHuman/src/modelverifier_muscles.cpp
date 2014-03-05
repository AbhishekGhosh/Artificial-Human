#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

void ModelVerifier::checkMuscles() {
	// check muscles use correct nerves
	logger.logInfo( "checkNerves: CHECK MUSCLES ..." );

	bool checkAll = true;
	StringList muscles;
	musclesxml.getMuscleList( muscles );

	for( int k = 0; k < muscles.count(); k++ ) {
		String id = muscles.get( k );

		// verify
		logger.logInfo( "checkMuscles: verify muscle id=" + id );
		bool checkOne = checkMuscles_verifyNerves( id );
		if( checkOne == false )
			checkAll = false;
	}

	if( checkAll )
		logger.logInfo( "checkMuscles: MUSCLES ARE OK" );
	else
		logger.logInfo( "checkMuscles: MUSCLES HAVE ERRORS" );
}

bool ModelVerifier::checkMuscles_verifyNerve( XmlMuscleInfo& muscle , String nervename , String cdef ) {
	bool res = true;

	if( !nervesxml.checkNerve( nervename ) ) {
		logger.logError( "checkMuscles_verifyNerve: muscle=" + muscle.name + ", nerve=" + nervename + " - is unknown nerve" );
		res = false;
	}

	XmlNerveInfo& nerve = nervesxml.getNerveInfo( nervename );

	// compare muscle type with nerve modality
	bool xres = true;
	if( muscle.type.equals( "flexor" ) ) {
		if( nerve.mods.find( "flexor motor" ) < 0 && nerve.mods.find( "sympathetic motor" ) < 0 )
			xres = false;
	}
	else if( muscle.type.equals( "extensor" ) ) {
		if( nerve.mods.find( "extensor motor" ) < 0 && nerve.mods.find( "parasympathetic motor" ) < 0 )
			xres = false;
	}
	else if( muscle.type.equals( "cranial" ) ) {
		if( nerve.mods.find( "cranial motor" ) < 0 && nerve.mods.find( "visceral motor" ) < 0 && nerve.mods.find( "sympathetic motor" ) < 0 && nerve.mods.find( "parasympathetic motor" ) < 0 )
			xres = false;
	}
	else if( muscle.type.equals( "gland" ) ) {
		if( nerve.mods.find( "sympathetic motor" ) < 0 && nerve.mods.find( "parasympathetic motor" ) < 0 )
			xres = false;
	}

	if( xres == false ) {
		res = false;
		logger.logError( "checkMuscles_verifyNerve: muscle=" + muscle.name + ", nerve=" + nervename + " - muscle type=" + muscle.type + " does not correspond to nerve modality=" + nerve.modality );
	}

	// verify muscle as connector
	String cname;
	if( cdef.equals( "default" ) )
		cname = muscle.name;
	else
		cname = muscle.name + ":" + cdef;

	const XmlHMindElementInfo *ni = hmindxml.getConnectorInfo( cname );
	if( ni == NULL ) {
		res = false;
		logger.logError( "checkMuscles_verifyNerve: muscle=" + muscle.name + ", nerve=" + nervename + " - cdef=" + cdef + " is not listed in connectors" );
	}

	return( res );
}

bool ModelVerifier::checkMuscles_verifyNerves( String muscle ) {
	XmlMuscleInfo& info = musclesxml.getMuscleInfo( muscle );

	if( info.nerves.count() == 0 )
		return( true );

	bool res = true;

	String mtype = info.type;
	if( mtype.isEmpty() ) {
		logger.logError( "checkMuscles_verifyNerves: muscle=" + info.name + ", type is not set" );
		res = false;
	}
	else if( ! ( mtype.equals( "flexor" ) || mtype.equals( "extensor" ) || mtype.equals( "cranial" ) || mtype.equals( "gland" ) ) ) {
		logger.logError( "checkMuscles_verifyNerves: muscle=" + info.name + ", invalid type=" + mtype );
		res = false;
	}

	for( int k = 0; k < info.nerves.count(); k++ ) {
		String nerve = info.nerves.getKeyByIndex( k );
		String cdef = info.nerves.getClassByIndex( k );
		if( !checkMuscles_verifyNerve( info , nerve , cdef ) )
			res = false;
	}

	return( res );
}

