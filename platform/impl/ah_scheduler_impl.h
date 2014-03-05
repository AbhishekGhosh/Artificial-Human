#ifndef	INCLUDE_AH_SCHEDULER_IMPL_H
#define INCLUDE_AH_SCHEDULER_IMPL_H

/*#########################################################################*/
/*#########################################################################*/

#include <ah_platform.h>

// core classes
class SchedulerTask;

/*#########################################################################*/
/*#########################################################################*/

class SchedulerTask : public Object {
public:
	SchedulerTask( MessagePublisher *notifyPublisher );
	~SchedulerTask();
	virtual const char *getClass() { return( "SchedulerTask" ); };

	void setName( String name );
	void setIntervalSec( int nSec );

	void start();
	void stop();

private:
	void runSchedulerTask( void *p_arg );

private:
	MessagePublisher *notifyPublisher;
	String name;
	int intervalSec;
	RFC_HND taskThread;
	RFC_HND stopEvent;
	bool stopSignal;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_SCHEDULER_IMPL_H
