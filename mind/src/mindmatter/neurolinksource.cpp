#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

NeuroLinkSource::NeuroLinkSource() {
	attachLogger();

	sizeX = sizeY = 0;
	region = NULL;
	pfn = NULL;
	sourcePool = NULL;
}

void NeuroLinkSource::create( MindRegion *p_region , String p_entity ) {
	region = p_region;
	entity = p_entity;
	Object::setInstance( region -> getRegionId() + "-" + entity );
	pfn = NULL;

	// setup connector
	p_region -> addSourceEntity( p_entity , this );
}

void NeuroLinkSource::setHandler( MindRegion::NeuroLinkSourceHandler p_pfn ) {
	pfn = p_pfn;
}

void NeuroLinkSource::setSourcePool( NeuroPool *p_data ) {
	sourcePool = p_data;
}

void NeuroLinkSource::addNeuroLink( NeuroLink *link ) {
	links.add( link );
}

NeuroSignal *NeuroLinkSource::getLinkSignal( NeuroLink *link ) {
	// use callback function
	if( pfn != NULL ) {
		// execute source handler
		try {
			NeuroSignal *signal = ( region ->* pfn )( link , this );
			return( signal );
		}
		catch( RuntimeException& e ) {
			logger.logError( "getSourceSignal: exception in handling message for NeuroLink id=" + link -> getId() );
			logger.printStack( e );
		}
		catch( ... ) {
			logger.logError( "getSourceSignal: unknown exception in handling message for NeuroLink id=" + link -> getId() );
		}

		return( NULL );
	}

	// use source pool
	if( sourcePool != NULL ) {
		// create from pool
		NeuroSignal *signal = new NeuroSignal( link -> getSizeX() , link -> getSizeY() );
		signal -> createFromPool( sourcePool );
		return( signal );
	}

	return( NULL );
}

void NeuroLinkSource::sendMessage( NeuroSignal *sourceSignal ) {
	// check empty signal
	if( sourceSignal != NULL ) {
		if( sourceSignal -> getDataSize() == 0 ) {
			logger.logInfo( "sendMessage: ignore empty signal id=" + sourceSignal -> getId() );
			return;
		}
	}

	for( int k = 0; k < links.count(); k++ ) {
		NeuroLink *link = links.get( k );

		NeuroSignal *linkSignal = NULL;
		if( sourceSignal != NULL )
			linkSignal = new NeuroSignal( sourceSignal );

		MindMessage *msg = new MindMessage( link , linkSignal );
		region -> sendMessage( msg );
	}
}
