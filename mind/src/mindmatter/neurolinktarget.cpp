#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

NeuroLinkTarget::NeuroLinkTarget() {
	attachLogger();

	region = NULL;
	pfn = NULL;
}

void NeuroLinkTarget::create( MindRegion *p_region , String p_entity ) {
	attachLogger();

	region = p_region;
	entity = p_entity;
	Object::setInstance( region -> getRegionId() + "-" + entity );

	// setup connector
	p_region -> addTargetEntity( p_entity , this );
}

void NeuroLinkTarget::setHandler( MindRegion::NeuroLinkTargetHandler p_pfn ) {
	pfn = p_pfn;
}

void NeuroLinkTarget::addNeuroLink( NeuroLink *link ) {
	links.add( link );
}

NeuroSignalSet *NeuroLinkTarget::execute( NeuroLink *link , NeuroSignal *sourceData ) {
	// execute target handler
	try {
		NeuroSignalSet *set = ( region ->* pfn )( link , this , sourceData );
		return( set );
	}
	catch( RuntimeException& e ) {
		logger.logError( "execute: exception in handling signal id=" + sourceData -> getId() + " for NeuroLink id=" + link -> getId() );
		logger.printStack( e );
	}
	catch( ... ) {
		logger.logError( "execute: unknown exception in handling signal id=" + sourceData -> getId() + " for NeuroLink id=" + link -> getId() );
	}

	return( NULL );
}
