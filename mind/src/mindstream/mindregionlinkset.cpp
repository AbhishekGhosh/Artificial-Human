#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

void MindRegionLinkSet::exitRegionLinkSet() {
	for( int k = 0; k < list.count(); k++ )
		list.get( k ) -> exitRegionLink();
}

void MindRegionLinkSet::destroyRegionLinkSet() {
	for( int k = 0; k < list.count(); k++ )
		list.get( k ) -> destroyRegionLink();
}

void MindRegionLinkSet::addSetItem( MindRegionLink *link ) {
	list.add( link );
}

int MindRegionLinkSet::getCount() {
	return( list.count() );
}

MindRegionLink *MindRegionLinkSet::getSetItem( int k ) {
	return( list.get( k ) );
}

bool MindRegionLinkSet::checkLinked( MindRegion *src , MindRegion *dst ) {
	for( int k = 0; k < list.count(); k++ ) {
		MindRegionLink *link = list.get( k );
		if( link -> getSrcRegion() == src && link -> getDstRegion() == dst )
			return( true );
	}

	return( false );
}
