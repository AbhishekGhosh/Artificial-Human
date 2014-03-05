#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindRegion *MindRegionSet::getSetItemById( String regionId ) {
	return( map.get( regionId ) );
}

void MindRegionSet::addSetItem( MindRegion *region ) {
	list.add( region );
	map.add( region -> getRegionId() , region );
}

void MindRegionSet::addRegionSet( MindRegionSet *set ) {
	for( int k = 0; k < set -> getCount(); k++ ) {
		MindRegion *region = set -> getSetItem( k );
		if( map.get( region -> getRegionId() ) == NULL ) {
			list.add( region );
			map.add( region -> getRegionId() , region );
		}
	}
}

void MindRegionSet::exitRegionSet() {
	for( int k = 0; k < list.count(); k++ )
		list.get( k ) -> exit();
}

void MindRegionSet::destroyRegionSet() {
	for( int k = 0; k < list.count(); k++ )
		list.get( k ) -> destroy();
}

int MindRegionSet::getCount() {
	return( list.count() );
}

MindRegion *MindRegionSet::getSetItem( int k ) {
	return( list.get( k ) );
}

