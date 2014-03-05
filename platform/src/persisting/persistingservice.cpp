#include <ah_platform.h>

// #############################################################################
// #############################################################################

Service *PersistingService::newService() {
	return( new PersistingService() );
}
