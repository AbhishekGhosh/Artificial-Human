#ifndef	INCLUDE_AH_CONSOLETOOL_H
#define INCLUDE_AH_CONSOLETOOL_H

/*#########################################################################*/
/*#########################################################################*/

#include <ah_tools.h>

class ConsoleTool;

/*#########################################################################*/
/*#########################################################################*/

class AdminApi;

class ConsoleTool : public ToolBase {
public:
	virtual int execute( int nargs , char **args );

public:
	ConsoleTool();
	virtual ~ConsoleTool();

// internals
private:
	int executeRequests( FILE *sin , FILE *sout );

	void logGreeting( FILE *sout );
	String readNextRequest( FILE *f );
	void logXml( FILE *f , String prefix , Xml& xml );
	void logInfo( FILE *f , String s );
	void logError( FILE *f , String s );
	void logException( FILE *f , String s );
	bool makeRequest( FILE *sin , FILE *sout );

private:
	String name;
	AdminApi *api;
	bool cmdMode;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_CONSOLETOOL_H
