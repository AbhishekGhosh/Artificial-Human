#ifndef	INCLUDE_AH_THREADS_H
#define INCLUDE_AH_THREADS_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_services.h"
#include "ah_objects.h"

class ThreadService;
class ThreadObject;
class ThreadHelper;
class ThreadPoolTask;

typedef void (Object::*ObjectThreadFunction)( void *p_arg );

/*#########################################################################*/
/*#########################################################################*/

class ThreadData;
class ThreadPool;

class ThreadService : public Service {
public:
	// thread management
	void threadDumpAll( bool showStackTrace );
	void threadDumpByName( String name , bool showStackTrace );

	RFC_HND runThread( String name , Object *object , ObjectThreadFunction function , void *p_arg );
	bool waitThreadExited( RFC_HND thread );
	RFC_HND getThreadHandle();
	int getThreadId();
	void addThreadObject( const char *key , ThreadObject *to );
	ThreadObject *getThreadObject( const char *key );
	void manageThreadCallStack();
	void setSignalHandlers();

	// thread pool operations
	void createThreadPool( String name , Xml configuration , ClassList<ThreadPoolTask>& tasks );
	void createThreadPool( String name , Xml configuration , ResourcePool<ThreadPoolTask>& tasks );
	void startThreadPool( String name );
	void stopThreadPool( String name );
	void suspendThreadPool( String name );
	void resumeThreadPool( String name );

	// interruptable sleep
	void threadSleepMs( int ms );
	void threadSleepInterruptAll();

// service
protected:
	virtual const char *getServiceName() { return( "ThreadService" ); };
	virtual void configureService( Xml config ) {};
	virtual void createService();
	virtual void initService() {};
	virtual void runService() {};
	virtual void stopService();
	virtual void exitService();
	virtual void destroyService();

// engine helpers
protected:
	ThreadService() {};
public:
	static Service *newService();
	static ThreadService *getService() { return( ( ThreadService * )ServiceManager::getInstance().getService( "ThreadService" ) ); };

// service implementation
public:
	unsigned threadFunction( ThreadData *td );
	void addMainThread();
	void stopServicesBySignal( int p_signal );
	void waitExitSignal();
	void waitAllThreads();
	void threadSleepInterrupt( ThreadData *td );

private:
	ThreadData *getThreadData();
	void printThreadStackTrace( ThreadData *td );

	void workerCreated();
	void workerStarted( ThreadData *threadData );
	void workerExited( int status );
	void workerExited( ThreadData *threadData , int status );
	String getActiveThreads();
	ThreadPool *createThreadPool( String name , Xml configuration );
	void destroyThreadPool( String name );

private:
	MapStringToClass<ThreadData> threads;
	MapStringToClass<ThreadPool> threadpools;

	// threads
	int workerStatus;
	RFC_HND lockExit;
	RFC_HND eventExit;
	int countExit;
};

/*#########################################################################*/
/*#########################################################################*/

class ThreadObject {
public:
	virtual ~ThreadObject() {};
};

/*#########################################################################*/
/*#########################################################################*/

class ThreadHelper : public ThreadObject {
public:
	ThreadHelper();
	virtual ~ThreadHelper();

	void addThreadObject();
	static ThreadHelper *getThreadObject();

public:
	void ( *oldAIUnhandledExceptionTranslator )(); // _se_translator_function
	String lastMsg;
	bool remains;
};

/*#########################################################################*/
/*#########################################################################*/

class ThreadPool;
class ThreadPoolItem;

// it is interface - inherit specific task from this class
class ThreadPoolTask : public Object {
public:
	virtual bool needExecution() { return( true ); };
	virtual bool finished() { return( false ); };

	virtual void execute() = 0;

public:
	ThreadPoolTask() {
		pool = NULL;
		thread = NULL;
	};
	virtual ~ThreadPoolTask() {};
	virtual const char *getClass() = 0;

public:
	ThreadPool *pool;
	ThreadPoolItem *thread;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_THREADS_H
