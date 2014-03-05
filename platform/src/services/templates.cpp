#include <ah_platform.h>

// #############################################################################
// #############################################################################

extern "C" int sortFlatList( void *p_userdata , const void *p_el1 , const void *p_el2 ) {
	int size = *( int * )p_userdata;
	return( memcmp( p_el1 , p_el2 , size ) );
}
