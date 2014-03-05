#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

void MindAreaSet::addMindArea( MindArea *area ) {
	list.add( area );
	map.add( area -> getMindAreaDef() -> getAreaId() , area );
}

MindArea *MindAreaSet::getMindArea( String id ) {
	return( map.get( id ) );
}

void MindAreaSet::wakeupAreaSet( MindActiveMemory *activeMemory ) {
	for( int k = 0; k < list.count(); k++ )
		list.get( k ) -> wakeupArea( activeMemory );
}

void MindAreaSet::suspendAreaSet() {
	for( int k = 0; k < list.count(); k++ )
		list.get( k ) -> suspendArea();
}

void MindAreaSet::exitAreaSet() {
	for( int k = 0; k < list.count(); k++ )
		list.get( k ) -> exit();
}

void MindAreaSet::destroyAreaSet() {
	for( int k = 0; k < list.count(); k++ )
		list.get( k ) -> destroy();

	map.clear();
	list.destroy();
}

