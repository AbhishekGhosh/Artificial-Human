#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

NeuroSignalSet::NeuroSignalSet() {
}

NeuroSignalSet::~NeuroSignalSet() {
}

void NeuroSignalSet::addSetItem( NeuroSignal *signal ) {
	signals.add( signal );
}

void NeuroSignalSet::addSetItem( NeuroLinkSource *source , NeuroSignal *signal ) {
	ASSERTMSG( source != NULL , "source is NULL" );
	signal -> setSource( source );
	signals.add( signal );
}

void NeuroSignalSet::addSetItem( MindRegion *region , String sourceConnector , NeuroSignal *signal ) {
	NeuroLinkSource *source = region -> getNeuroLinkSource( sourceConnector );
	ASSERTMSG( source != NULL , "unknown connector=" + sourceConnector );
	signal -> setSource( source );
	signals.add( signal );
}

void NeuroSignalSet::addSet( NeuroSignalSet *signalSet ) {
	signals.add( &signalSet -> signals );
}

void NeuroSignalSet::moveTo( NeuroSignalSet *signalSet ) {
	signalSet -> signals.add( &signals );
	signals.clear();
}

void NeuroSignalSet::clear() {
	signals.clear();
}

bool NeuroSignalSet::isEmpty() {
	return( signals.count() == 0 );
}

void NeuroSignalSet::destroy() {
	signals.destroy();
}
