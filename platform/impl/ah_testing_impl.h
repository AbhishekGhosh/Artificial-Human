#ifndef	INCLUDE_AH_TESTING_IMPL_H
#define INCLUDE_AH_TESTING_IMPL_H

/*#########################################################################*/
/*#########################################################################*/

#include <ah_platform.h>

// core classes
class TestUnit;
class TestMethod;
class TestExecutor;

typedef void ( TestUnit::*TestUnitMethodType )( XmlCall& );
#define ADD_METHOD( method ) TestUnit::addMethod( #method , ( TestUnitMethodType )&method )

/*#########################################################################*/
/*#########################################################################*/

class TestMethod
{
public:
	TestMethod( String p_name , TestUnitMethodType p_fptr ) {
		name = p_name;
		fptr = p_fptr;
	}
	
	String name;
	TestUnitMethodType fptr;
};

/*#########################################################################*/
/*#########################################################################*/

class TestUnit : public Object
{
// generic
public:
	TestUnit( String p_name );
	virtual ~TestUnit();

	virtual const char *getClass() { return( testUnitName ); };

	virtual void onInit() {};
	virtual void onExit() {};
	virtual void onCreate() = 0;

	TestMethod *getMethod( String methodName );
	void callMethod( TestMethod *method , XmlCall& call );
	
protected:
	void addMethod( String methodName , TestUnitMethodType methodFunction );

// test units
private:
	bool initialized;
	String testUnitName;

public:
	MapStringToClass<TestMethod> methods;

	static TestUnit *createEngineThreadsTest();
	static TestUnit *createDirectChannelsTest();
	static TestUnit *createThreadPoolTest();
};

/*#########################################################################*/
/*#########################################################################*/

// derives knowledge from io, activates mind
class TestExecutor : public Object , public MessageSubscriber {
public:
	void addTestUnit( TestUnit *p_unit );
	void connectChannels( String channelIn , String channelOut );
	void start();
	void stop();
	void exit();

protected:
	// object
	virtual const char *getClass() { return( "TestExecutor" ); };

	// message subscriber
	virtual void onXmlCall( XmlCall *msg );

public:
	TestExecutor();

// internals
private:
	bool enabled;

	// internals
	MessagePublisher *callPub;
	MessageSubscription *callSub;
	MapStringToClass<TestUnit> units;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_TESTING_IMPL_H
