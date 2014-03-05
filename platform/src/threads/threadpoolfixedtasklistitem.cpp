#include <ah_platform.h>
#include <ah_threads_impl.h>

/*#########################################################################*/
/*#########################################################################*/

ThreadPoolFixedTaskListItem::ThreadPoolFixedTaskListItem( String p_name , int p_threadPoolItem , ClassList<ThreadPoolTask>& p_tasks )
:	ThreadPoolItem( p_name , p_threadPoolItem ) {
	currentTask = 0;

	// assign task list
	for( int k = 0; k < p_tasks.count(); k++ ) {
		ThreadPoolTask *task = p_tasks.get( k );
		task -> thread = this;
		tasks.add( task );
	}
}

ThreadPoolTask *ThreadPoolFixedTaskListItem::getThreadTask() {
	// no tasks - stop thread
	if( tasks.count() == 0 ) {
		stopSignal = true;
		return( NULL );
	}

	// get execution object
	for( int k = 0; k < tasks.count(); k++ ) {
		// run given task while it has work
		int nt = ( currentTask + k ) % tasks.count();
		ThreadPoolTask *task = tasks.get( nt );

		if( task -> needExecution() ) {
			currentTask = nt;
			return( task );
		}
	}

	// no active tasks - ensure sleep 100 ms
	rfc_hnd_waitevent( suspendEvent , 100 );
	return( NULL );
}

void ThreadPoolFixedTaskListItem::executedThreadTask( ThreadPoolTask *task ) {
	// delete unneeded task
	if( task -> finished() )
		tasks.remove( currentTask );
}

