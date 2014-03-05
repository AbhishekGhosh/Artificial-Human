#ifndef	INCLUDE_AH_THREADS_IMPL_H
#define INCLUDE_AH_THREADS_IMPL_H

/*#########################################################################*/
/*#########################################################################*/

#include <ah_threads.h>

class ThreadData;
class ThreadPool;
class ThreadPoolItem;
class ThreadPoolFixedTaskListItem;
class ThreadPoolTaskQueueItem;
class ThreadPoolTask;

/*#########################################################################*/
/*#########################################################################*/

// thread data
class ThreadData {
public:
	RFC_THREAD threadExtId;
	String name;
	unsigned long threadId;
	Object *object;
	void ( Object::*objectFunction )( void *p_arg );
	void *objectFunctionArg;
	bool stopped;

	RFC_HND sleepEvent;
	MapStringToClass<ThreadObject> map;

public:
	ThreadData();
	~ThreadData();

	void stopThread();
	bool checkStopped();
};

/*#########################################################################*/
/*#########################################################################*/

class ThreadState {
public:
	typedef enum {
		THREAD_STATE_CREATED = 1 ,		// start -> running
		THREAD_STATE_RUNNING = 2 ,		// suspend -> suspended, stop -> stopping
		THREAD_STATE_SUSPENDED = 3 ,	// resume -> running, stop -> stopping
		THREAD_STATE_STOPPING = 4		// stop -> stopping -> created
	} ThreadStateType;

private:
	ThreadStateType state;

public:
	ThreadState() { state = THREAD_STATE_CREATED; };
	ThreadStateType getState() { return( state ); };

	bool isActive() {
		return( state == THREAD_STATE_RUNNING || state == THREAD_STATE_SUSPENDED );
	};

	void setState( ThreadStateType value ) {
		bool checkTransition = false;
		if( state == THREAD_STATE_CREATED )
			checkTransition = ( value == THREAD_STATE_RUNNING );
		else
		if( state == THREAD_STATE_RUNNING )
			checkTransition = ( value == THREAD_STATE_STOPPING || value == THREAD_STATE_SUSPENDED );
		else
		if( state == THREAD_STATE_SUSPENDED )
			checkTransition = ( value == THREAD_STATE_STOPPING || value == THREAD_STATE_RUNNING );
		else
		if( state == THREAD_STATE_STOPPING )
			checkTransition = ( value == THREAD_STATE_CREATED );

		ASSERTMSG( checkTransition , String( "Invalid state transition: stateFrom=" )  + state + ", stateTo=" + value );
		state = value;
	};
};

/*#########################################################################*/
/*#########################################################################*/

class ThreadPool : public Object {
private:
	String name;
	bool runEnabled;
	int nThreads;
	int secondsPerMonitoringCycle;
	int maxLoadPercents;

	ClassList<ThreadPoolItem> threads;
	RFC_HND monitorThread;
	CPULOADINFO cpuload;
	float lastCPULoad;
	ThreadState state;
	bool stopSignal;

public:
	void configure( Xml config );
	void create( ClassList<ThreadPoolTask>& tasks );
	void create( ResourcePool<ThreadPoolTask>& tasks );
	void ensureStopped();

	void start();
	void stop();
	void suspend();
	void resume();

// internals
protected:
	void runMonitorThread( void *arg );
	void runMonitorTask();

public:
	ThreadPool( String name );
	~ThreadPool();
	virtual const char *getClass() { return( "ThreadPool" ); };
};

/*#########################################################################*/
/*#########################################################################*/

class ThreadPoolItem : public Object {
public:
	String getThreadName();
	void setExecutionTimeWindowTicks( int ticks );
	void clearStatistics();

	void start();
	void stop();
	void suspend();
	void resume();
	void execute( ThreadPoolTask *task );
	void run( void *p_arg );

protected:
	virtual ThreadPoolTask *getThreadTask() = 0;
	virtual void executedThreadTask( ThreadPoolTask *task ) = 0;

public:
	ThreadPoolItem( String name , int threadPoolItem );
	~ThreadPoolItem();
	virtual const char *getClass() { return( "ThreadPoolItem" ); };

public:
	int ticksPerSecond;
	int executionCount;
	int ticksExecTimeTotal;
	int ticksSleepTimeTotal;
	int ticksWaitTimeRemained;
	int executionTimeWindowTicks;

protected:
	String name;
	ThreadState state;
	RFC_HND thread;
	int threadPoolId;

	RFC_HND runEvent;
	RFC_HND suspendEvent;
	bool suspendSignal;
	bool stopSignal;
};

/*#########################################################################*/
/*#########################################################################*/

class ThreadPoolFixedTaskListItem : public ThreadPoolItem {
private:
	ClassList<ThreadPoolTask> tasks;
	int ticksPerSecond;
	int executionCount;
	int ticksExecTimeTotal;
	int ticksSleepTimeTotal;
	int ticksWaitTimeRemained;
	int executionTimeWindowTicks;
	int currentTask;

public:
	ThreadPoolFixedTaskListItem( String name , int threadPoolItem , ClassList<ThreadPoolTask>& tasks );

protected:
	virtual ThreadPoolTask *getThreadTask();
	virtual void executedThreadTask( ThreadPoolTask *task );
};

/*#########################################################################*/
/*#########################################################################*/

class ThreadPoolTaskQueueItem : public ThreadPoolItem {
private:
	ResourcePool<ThreadPoolTask> *res;

public:
	ThreadPoolTaskQueueItem( String name , int threadPoolItem , ResourcePool<ThreadPoolTask>& res );

protected:
	virtual ThreadPoolTask *getThreadTask();
	virtual void executedThreadTask( ThreadPoolTask *task );
};

/*#########################################################################*/
/*#########################################################################*/

class ThreadPoolPerformance {
public:
	ThreadPoolPerformance( Logger& logger , ClassList<ThreadPoolItem>& threads );

public:
	void gather();
	void updateSpeedIfRequired( float prevLoadPercents , float lastLoadPercents , float maxLoadPercents , CPULOADINFO& loadinfo );

public:
	Logger& logger;
	ClassList<ThreadPoolItem>& threads;

	int lastAverageTimeWindowTicks;
	int lastAverageRunTicks;
	int lastAverageSleepTicks;
	int lastAverageExecutionCount;
	int lastActiveThreads;

	int ticksPerSecond;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_THREADS_IMPL_H
