#include <ah_platform.h>
#include <ah_threads_impl.h>

/*#########################################################################*/
/*#########################################################################*/

ThreadHelper::ThreadHelper()
{ 
	oldAIUnhandledExceptionTranslator = NULL;
	remains = false; 
}

ThreadHelper::~ThreadHelper()
{
}

void ThreadHelper::addThreadObject()
{
	ThreadService *ts = ThreadService::getService();
	ts -> addThreadObject( "ThreadHelper" , this );
}

ThreadHelper *ThreadHelper::getThreadObject()
{
	ThreadService *ts = ThreadService::getService();
	return( ( ThreadHelper * )ts -> getThreadObject( "ThreadHelper" ) );
}
