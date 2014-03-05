#include <ah_platform.h>
#include <ah_threads_impl.h>

/*#########################################################################*/
/*#########################################################################*/

ThreadData::ThreadData() {
	memset( &threadExtId , 0 , sizeof( RFC_THREAD ) );
	threadId = ( unsigned long )NULL;
	object = NULL;
	objectFunction = NULL;
	objectFunctionArg = NULL;
	sleepEvent = rfc_hnd_evcreate();
	stopped = false;
}

ThreadData::~ThreadData() {
	rfc_hnd_evdestroy( sleepEvent );
}

void ThreadData::stopThread() {
	stopped = true;
	rfc_hnd_evsignal( sleepEvent );
}

bool ThreadData::checkStopped() {
	return( stopped );
}
