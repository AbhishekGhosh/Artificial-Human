#ifndef	INCLUDE_AH_ADMINAPI_IMPL_H
#define INCLUDE_AH_ADMINAPI_IMPL_H

/*#########################################################################*/
/*#########################################################################*/

#include "ah_basetypes.h"

class AdminApi;

/*#########################################################################*/
/*#########################################################################*/

class AdminApiSocket;

class AdminApi {
public:
	// connnection
	void connect( String url );
	void disconnect();
	bool isConnected();

	// create request
	Xml createXmlRequest( const char *contenttype );
	Xml createXmlRequest( const char *data , const char *contenttype );
	Xml loadXmlRequest( String fileName , const char *contenttype );

	// modify request
	void setFunctionName( Xml request , String name );
	void setParam( Xml request , String name , String value );
	void setBooleanParam( Xml request , String name , bool value );
	void setIntParam( Xml request , String name , int value );
	void setFloatParam( Xml request , String name , float value );

	// execute request
	Xml execute( Xml request );
	void send( Xml msg );

public:
	AdminApi();
	~AdminApi();

private:
	// initialization
	void initapi();
	void exitapi();

private:
	AdminApiSocket *socket;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_ADMINAPI_IMPL_H
