#ifndef	INCLUDE_AH_STATISTICS_H
#define INCLUDE_AH_STATISTICS_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_services.h"

class StatService;

/*#########################################################################*/
/*#########################################################################*/

class StatMetric;
class StatLogTask;

class StatService : public Service {
public:
	virtual const char *getServiceName() { return( "StatService" ); };
	virtual void configureService( Xml config );
	virtual void createService();
	virtual void initService();
	virtual void runService();
	virtual void stopService();
	virtual void exitService();
	virtual void destroyService();

public:

// internals
protected:
	StatService();
public:
	static Service *newService();
	static StatService *getService() { return( ( StatService * )ServiceManager::getInstance().getService( "StatService" ) ); };

	void addMetricValue( const char *metricName , int metricValue );
	StatMetric *getMetricByName( String name );
	StatLogTask *getLogTaskByName( String name );

private:
// utility
	Xml config;

// own data
	MapStringToClass<StatMetric> metricMap;
	MapStringToClass<StatLogTask> logTaskMap;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_STATISTICS_H
