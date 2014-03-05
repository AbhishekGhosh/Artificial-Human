#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindActiveMemory::MindActiveMemory() {
	attachLogger();
}

MindActiveMemory::~MindActiveMemory() {
	memoryObjects.destroy();
}

void MindActiveMemory::create( Xml config ) {
	Xml xmlTP = config.getFirstChild( "thread-pool" );
	threadPoolName = xmlTP.getProperty( "name" );

	Xml xmlFocus = config.getFirstChild( "memory-focus" );
	int focusSize = xmlFocus.getIntProperty( "objectCount" );

	// create memory objects
	for( int k = 0; k < focusSize; k++ ) {
		MindActiveMemoryObject *object = new MindActiveMemoryObject( k );
		memoryObjects.add( object );
	}

	// create thread pool
	ThreadService *ts = ThreadService::getService();
	ts -> createThreadPool( threadPoolName , xmlTP , ( ClassList<ThreadPoolTask>& )memoryObjects );
}

void MindActiveMemory::start() {
	ThreadService *ts = ThreadService::getService();
	ts -> startThreadPool( threadPoolName );
}

void MindActiveMemory::stop() {
	ThreadService *ts = ThreadService::getService();
	ts -> stopThreadPool( threadPoolName );
}

void MindActiveMemory::execute( MindMessage *msg ) {
	NeuroLink *link = msg -> getNeuroLink();
	NeuroSignal *signal = msg -> getMsgData();

	String extId = signal -> getId();
	if( extId.isEmpty() ) {
		extId = msg -> getSourceMessageId();
		signal -> setId( extId );
	}

	logger.logDebug( "execute: process link=" + link -> getId() + ", message id=" + msg -> getChannelMessageId() );

	NeuroLinkTarget *target = link -> getTarget();
	NeuroSignalSet *set = target -> execute( link , signal );
	if( set == NULL ) {
		logger.logDebug( signal -> getId() + ": there are no derived signals from signal id=" + signal -> getId() + ", link id=" + link -> getId() );
		return;
	}

	// define IDs
	ClassList<NeuroSignal>& signals = set -> getSignals();
	for( int k = 0; k < signals.count(); k++ ) {
		NeuroSignal *signalExecuted = signals.get( k );
		signalExecuted -> setId( signal -> getId() + ".S" + (k+1) );
	}

	// follow links
	MindRegion *region = target -> getRegion();
	MindArea *area = region -> getArea();
	area -> followLinks( signal -> getId() , region , set );
	set -> destroy();
}
