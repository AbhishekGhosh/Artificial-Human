#ifndef	INCLUDE_AH_SCHEDULER_H
#define INCLUDE_AH_SCHEDULER_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_services.h"

class SchedulerService;

/*#########################################################################*/
/*#########################################################################*/

class SchedulerTask;
class MessageSubscription;
class MessageSubscriber;
class MessagePublisher;

class SchedulerService : public Service {
public:
	virtual const char *getServiceName() { return( "SchedulerService" ); };
	virtual void configureService( Xml config );
	virtual void createService();
	virtual void initService();
	virtual void runService();
	virtual void stopService();
	virtual void exitService();
	virtual void destroyService();

public:
	String getNotifyChannel();
	MessageSubscription *createTimerTask( String taskName , int intervalSec , MessageSubscriber *sub );
	void createTimerTask( String taskName , int intervalSec );
	MessageSubscription *subscribeTimerTask( String taskName , MessageSubscriber *sub );
	void dropTimerTask( String taskName );

// internals
protected:
	SchedulerService();
public:
	static Service *newService();
	static SchedulerService *getService() { return( ( SchedulerService * )ServiceManager::getInstance().getService( "SchedulerService" ) ); };

private:
// utility
	Xml config;
	String notifyChannel;
	MessagePublisher *notifyPublisher;

// own data
	MapStringToClass<SchedulerTask> taskMap;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_SCHEDULER_H
