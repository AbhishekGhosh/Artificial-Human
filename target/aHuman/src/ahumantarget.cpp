#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

AHumanTarget::AHumanTarget() {
	attachLogger();
}

void AHumanTarget::configureTarget( Xml config ) {
}

void AHumanTarget::createTarget( SensorArea *sensorArea , EffectorArea *effectorArea ) {
	MindTarget::addSensor( "TARGET.TSA.EYE" , AHumanTarget::createEye( sensorArea ) );
	MindTarget::addSensor( "TARGET.TSA.EAR" , AHumanTarget::createEar( sensorArea ) );
	MindTarget::addSensor( "TARGET.TSA.NOSE" , AHumanTarget::createNose( sensorArea ) );

	MindTarget::addEffector( "TARGET.TEA.HEART" , AHumanTarget::createHeart( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.PROTECTOR" , AHumanTarget::createProtector( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.FEEDER" , AHumanTarget::createFeeder( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.SPEAKER" , AHumanTarget::createSpeaker( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.FACE" , AHumanTarget::createFace( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.NECK" , AHumanTarget::createNeck( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.LUNG" , AHumanTarget::createLung( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.LIVER" , AHumanTarget::createLiver( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.STOMACH" , AHumanTarget::createStomach( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.HAND" , AHumanTarget::createHand( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.LEG" , AHumanTarget::createLeg( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.TRUNK" , AHumanTarget::createTrunk( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.BREAST" , AHumanTarget::createBreast( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.GENITALIA" , AHumanTarget::createGenitalia( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.RECTUM" , AHumanTarget::createRectum( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.URETER" , AHumanTarget::createUreter( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.VESSELS" , AHumanTarget::createVessels( effectorArea ) );
	MindTarget::addEffector( "TARGET.TEA.ENDOCRINE" , AHumanTarget::createEndocrine( effectorArea ) );
}

void AHumanTarget::initTarget() {
}

void AHumanTarget::runTarget() {
	// subscribe command channel
	MessagingService *ms = MessagingService::getService();
	ms -> subscribe( NULL , "target.commands" , "target.commands.sub" , this );
}

void AHumanTarget::onXmlMessage( XmlMessage *msg ) {
	Xml xml = msg -> getXml();
	String cmdName = xml.getAttribute( "name" );

	logger.logInfo( "onXmlMessage: execute request=" + cmdName );

	if( cmdName.equals( "PlayCircuit" ) )
		cmdPlayCircuit( xml );
	else if( cmdName.equals( "VerifyModel" ) )
		cmdVerifyModel( xml );
	else if( cmdName.equals( "CreateWikiPages" ) )
		cmdCreateWikiPages( xml );
	else
		logger.logError( "unknown cmd=" + cmdName );
}

void AHumanTarget::cmdPlayCircuit( Xml scenario ) {
	ScenarioPlayer player;
	player.play( scenario );
	logger.logInfo( "cmdPlayCircuit: SUCCESSFULLY FINISHED" );
}

void AHumanTarget::cmdVerifyModel( Xml modelArea ) {
	ModelVerifier verifier( modelArea );
	verifier.verify();
	logger.logInfo( "cmdVerifyModel: SUCCESSFULLY FINISHED" );
}

void AHumanTarget::cmdCreateWikiPages( Xml wiki ) {
	WikiMaker maker( wiki );
	maker.createPages();
	logger.logInfo( "cmdCreateWikiPages: SUCCESSFULLY FINISHED" );
}

void AHumanTarget::stopTarget() {
}

void AHumanTarget::exitTarget() {
}

void AHumanTarget::destroyTarget() {
}

