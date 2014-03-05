#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindAreaLink::MindAreaLink() {
	attachLogger();

	masterArea = NULL;
	slaveArea = NULL;

	session = NULL;
	iopub = NULL;
	iosub = NULL;

	links = new MindRegionLinkSet();
}

MindAreaLink::~MindAreaLink() {
	delete links;
}

void MindAreaLink::create( MindArea *p_masterArea , MindArea *p_slaveArea ) {
	masterArea = p_masterArea;
	slaveArea = p_slaveArea;
	Object::setInstance( masterArea -> getAreaId() + "-" + slaveArea -> getAreaId() );
}

void MindAreaLink::open( MessageSession *p_session , String channel ) {
	session = p_session;

	MindService *ms = MindService::getService();
	activeMemory = ms -> getActiveMemory();

	MessagingService *msgs = MessagingService::getService();

	// use default channel
	if( !msgs -> checkChannelAvailable( channel ) )
		channel = "mind.default";

	// open source publisher
	String pubid = String( "PUB-" ) + getInstance();
	iopub = msgs -> createPublisher( session , channel , pubid , "xml" );

	// open master channel
	String subid = String( "SUB-" ) + getInstance();
	iosub = msgs -> subscribe( p_session , channel , subid , this );
}

void MindAreaLink::sendMessage( MindMessage *msg ) {
	iopub -> publish( session , msg );
}

void MindAreaLink::onMessage( Message *msg ) {
	MindMessage *mm = ( MindMessage * )msg;

	// ignore foreign links
	NeuroSignal *signal = mm ->getMsgData();
	if( mm -> getAreaLink() != this ) {
		logger.logInfo( signal -> getId() + ": ignore signal from different area link message id=" + mm -> getChannelMessageId() );
		return;
	}

	logger.logInfo( signal -> getId() + ": execute signal from area link message id=" + mm -> getChannelMessageId() );
	activeMemory -> execute( mm );
}

void MindAreaLink::addRegionLink( MindRegionLink *link ) {
	links -> addSetItem( link );
}

ClassList<MindRegionLink>& MindAreaLink::getRegionLinks() {
	return( links -> getLinks() );
}
