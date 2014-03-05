#include <ah_platform.h>

/*#########################################################################*/
/*#########################################################################*/

void Service::configureLifecycle( Xml config ) {
	isCreate = config.getBooleanAttribute( "create" , true );
	isInit = ( isCreate )? config.getBooleanAttribute( "init" , true ) : false;
	isRun = ( isInit )? config.getBooleanAttribute( "run" , true ) : false;
}

