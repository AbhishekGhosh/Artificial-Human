#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

ModelVerifier::ModelVerifier( Xml p_modelArea ) {
	attachLogger();
	modelArea = p_modelArea;
}

ModelVerifier::~ModelVerifier() {
}

void ModelVerifier::verify() {
	logger.logInfo( "verify: VERIFY MIND SETUP..." );

	MindModel::load();

	// collect mind regions
	MindService *ms = MindService::getService();
	MindMap *mm = ms -> getMindMap();
	mm -> getMapRegions( regionMap );

	bool verifyHierarchy = modelArea.getBooleanProperty( "verifyHierarchy" , true );
	bool verifyCircuits = modelArea.getBooleanProperty( "verifyCircuits" , true );
	bool verifyNerves = modelArea.getBooleanProperty( "verifyNerves" , true );
	bool verifyMuscles = modelArea.getBooleanProperty( "verifyMuscles" , true );
	bool verifyMindModel = modelArea.getBooleanProperty( "verifyMindModel" , true );

	// verify
	if( verifyHierarchy )
		checkHierarchy();
	if( verifyCircuits )
		checkCircuits();
	if( verifyNerves )
		checkNerves();
	if( verifyMuscles )
		checkMuscles();
	if( verifyMindModel )
		checkMindModel();
}

