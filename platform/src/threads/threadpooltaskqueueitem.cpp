#include <ah_platform.h>
#include <ah_threads_impl.h>

/*#########################################################################*/
/*#########################################################################*/

ThreadPoolTaskQueueItem::ThreadPoolTaskQueueItem( String p_name , int p_threadPoolItem , ResourcePool<ThreadPoolTask>& p_pool )
:	ThreadPoolItem( p_name , p_threadPoolItem ) {
	res = &p_pool;
}

ThreadPoolTask *ThreadPoolTaskQueueItem::getThreadTask() {
	return( res -> get() );
}

void ThreadPoolTaskQueueItem::executedThreadTask( ThreadPoolTask *task ) {
	delete task;
}
