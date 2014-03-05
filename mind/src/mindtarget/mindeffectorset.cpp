#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindEffectorSet::MindEffectorSet( EffectorArea *p_area) {
	attachLogger();
	area = p_area;
}

void MindEffectorSet::createEffectorSet() {
	MindRegionSet *set = area -> getRegionSet();
	for( int k = 0; k < list.count(); k++ ) {
		MindEffector *effector = list.get( k );

		String id = String( area -> getClass() ) + "." + effector -> getClass();
		// sensor -> create( id );
		set -> addSetItem( effector );
	}
}

void MindEffectorSet::startEffectorSet() {
	for( int k = 0; k < list.count(); k++ ) {
		MindEffector *effector = list.get( k );
		effector -> startEffector();
		logger.logInfo( String( "startEffectorSet: effector started - name=" ) + effector -> getClass() );
	}
}

void MindEffectorSet::stopEffectorSet() {
	for( int k = 0; k < list.count(); k++ ) {
		MindEffector *effector = list.get( k );
		effector -> stopEffector();
		logger.logInfo( String( "stopEffectorSet: effector stopped - name=" ) + effector -> getClass() );
	}
}

int MindEffectorSet::getCount() {
	return( list.count() );
}

MindEffector *MindEffectorSet::getSetItem( int k ) {
	return( list.get( k ) );
}

void MindEffectorSet::addSetItem( MindEffector *effector ) {
	list.add( effector );
	map.add( effector -> getClass() , effector );
}

MindEffector *MindEffectorSet::getEffector( String name ) {
	return( map.get( name ) );
}

