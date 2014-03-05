#ifndef	INCLUDE_AH_TESTING_H
#define INCLUDE_AH_TESTING_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_services.h"

/*#########################################################################*/
/*#########################################################################*/

class TestExecutor;

class TestService : public Service {
public:
	virtual const char *getServiceName() { return( "TestService" ); };
	virtual void configureService( Xml config );
	virtual void createService();
	virtual void initService();
	virtual void runService();
	virtual void stopService();
	virtual void exitService();
	virtual void destroyService();

// engine helpers
protected:
	TestService();
public:
	static Service *newService();
	static TestService *getService() { return( ( TestService * )ServiceManager::getInstance().getService( "TestService" ) ); };

// internals
private:
	TestExecutor *executor;

	// configuration
	String channelIn;
	String channelOut;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_TESTING_H
