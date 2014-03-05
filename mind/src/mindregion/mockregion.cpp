#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MockRegion::MockRegion( String p_typeName , MindArea *p_area )
:	MindRegion( p_area ) {
	typeName = p_typeName;
	attachLogger();
	msLast = 0;
	msLastModulation = 0;
}

String MockRegion::getRegionType() {
	return( typeName );
}

void MockRegion::createRegion( MindRegionCreateInfo *info ) {
	logger.logDebug( String( "createRegion: created mock region original type=" + typeName ) );

	// get prototype definition
	MindRegionTypeDef *regionDef = info -> getType();
	MindRegionTypeDef *regionOriginalDef = regionDef -> getOriginalTypeDef();

	ClassList<MindRegionConnectorDef>& connectors = regionOriginalDef -> getConnectors();
	for( int k = 0; k < connectors.count(); k++ ) {
		MindRegionConnectorDef *connectorDef = connectors.get( k );
		String connectorType = connectorDef -> getType();

		if( connectorType.equals( "source" ) || connectorType.equals( "any" ) ) {
			NeuroLinkSource *source = new NeuroLinkSource;
			sourceConnectors.add( source );
			source -> create( this , connectorDef -> getId() );
			source -> setHandler( ( MindRegion::NeuroLinkSourceHandler )&MockRegion::handleGetNeuroLinkMessage );
		}

		if( connectorType.equals( "target" ) || connectorType.equals( "any" ) ) {
			NeuroLinkTarget *target = new NeuroLinkTarget;
			targetConnectors.add( target );
			target -> create( this , connectorDef -> getId() );
			target -> setHandler( ( MindRegion::NeuroLinkTargetHandler )&MockRegion::handleApplyNeuroLinkMessage );
		}
	}
}

void MockRegion::getSourceSizes( String entity , int *p_sizeX , int *p_sizeY ) {
	*p_sizeX = 0;
	*p_sizeY = 0;
}

void MockRegion::exitRegion() {
}

void MockRegion::destroyRegion() {
}

NeuroSignalSet *MockRegion::handleApplyNeuroLinkMessage( NeuroLink *link , NeuroLinkTarget *point , NeuroSignal *inputSignal ) {
	String entity = point -> getEntity();

	RFC_INT64 msNow = Timer::getCurrentTimeMillis();
	RFC_INT64 msDelta = msNow - msLast;
	msLast = msNow;

	// handle by region type
	NeuroSignalSet *set = NULL;
	if( typeName.equals( "neocortex" ) ) {
		// do not generate repeated signal within NEURON_FULL_RELAX_ms
		if( msDelta < NEURON_FULL_RELAX_ms ) {
			logger.logInfo( inputSignal -> getId() + ": signal is ignored, as coming just immediately after another one" );
			return( NULL );
		}

		set = new NeuroSignalSet;
		if( entity.equals( "neocortex.ffin" ) ) {
			set -> addSetItem( getNeuroLinkSource( "neocortex.ffout" ) , new NeuroSignal() );
		}
		else if( entity.equals( "neocortex.fbin" ) ) {
			set -> addSetItem( getNeuroLinkSource( "neocortex.fbout" ) , new NeuroSignal() );
		}
	}
	else if( typeName.equals( "nucleus" ) ) {
		if( entity.equals( "nucleus.input" ) ) {
			// do not generate repeated signal within NEURON_FULL_RELAX_ms
			if( msDelta < NEURON_FULL_RELAX_ms ) {
				logger.logInfo( inputSignal -> getId() + ": signal is ignored, as coming just immediately after another one" );
				return( NULL );
			}

			// ignore if modulation exists
			RFC_INT64 msDeltaModulation = msNow - msLastModulation;
			if( msDeltaModulation < NEURON_INHIBIT_DELAY_ms ) {
				logger.logInfo( inputSignal -> getId() + ": signal is ignored, as blocked by modulatory signal" );
				return( NULL );
			}

			set = new NeuroSignalSet;
			set -> addSetItem( getNeuroLinkSource( "nucleus.output" ) , new NeuroSignal() );
		}
		else if( entity.equals( "nucleus.interneurons" ) ) {
			msLastModulation = msNow;
			return( NULL );
		}
	}
	else if( typeName.equals( "ganglion" ) ) {
		if( entity.equals( "ganglion.input" ) ) {
			// do not generate repeated signal within NEURON_FULL_RELAX_ms
			if( msDelta < NEURON_FULL_RELAX_ms ) {
				logger.logInfo( inputSignal -> getId() + ": signal is ignored, as coming just immediately after another one" );
				return( NULL );
			}

			// ignore if modulation exists
			RFC_INT64 msDeltaModulation = msNow - msLastModulation;
			if( msDeltaModulation < NEURON_INHIBIT_DELAY_ms ) {
				logger.logInfo( inputSignal -> getId() + ": signal is ignored, as blocked by modulatory signal" );
				return( NULL );
			}

			set = new NeuroSignalSet;
			set -> addSetItem( getNeuroLinkSource( "ganglion.output" ) , new NeuroSignal() );
		}
		else if( entity.equals( "ganglion.feedback" ) ) {
			msLastModulation = msNow;
			return( NULL );
		}
	}

	return( set );
}

NeuroSignal *MockRegion::handleGetNeuroLinkMessage( NeuroLink *link , NeuroLinkSource *point ) {
	return( new NeuroSignal() );
}
