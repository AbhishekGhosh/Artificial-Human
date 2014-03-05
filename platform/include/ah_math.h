#ifndef	INCLUDE_AH_MATHEMATICS_H
#define INCLUDE_AH_MATHEMATICS_H

// #############################################################################
// #############################################################################

#include "ah_services.h"

#define ABS( x ) ( ( (x) > 0 )? (x) : -(x) )

extern float getMin( float v1 , float v2 );
extern float getMax( float v1 , float v2 );
extern float getMinMul( float a1 , float a2 , float b1 , float b2 );
extern float getMaxMul( float a1 , float a2 , float b1 , float b2 );

extern int compareFloats( float v1 , float v2 , float tolerance = 0.000001 );

/*#########################################################################*/
/*#########################################################################*/

class MathService : public Service {
public:
	virtual const char *getServiceName() { return( "MathService" ); };
	virtual void configureService( Xml config ) {};
	virtual void createService() {};
	virtual void initService() {};
	virtual void runService() {};
	virtual void stopService() {};
	virtual void exitService() {};
	virtual void destroyService() {};

// engine helpers
protected:
	MathService() {};
public:
	static Service *newService();
	static MathService *getService() { return( ( MathService * )ServiceManager::getInstance().getService( "MathService" ) ); };
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_MATHEMATICS_H
