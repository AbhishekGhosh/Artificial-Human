#ifndef	INCLUDE_AH_TOOLS_H
#define INCLUDE_AH_TOOLS_H

/*#########################################################################*/
/*#########################################################################*/

#include <platform/include/ah_platform.h>

class ToolBase;

/*#########################################################################*/
/*#########################################################################*/

class ToolBase {
public:
	static ToolBase *getTool( String name );
	String getName() { return( name ); };
	virtual int execute( int nargs , char **args ) = 0;

public:
	ToolBase( String p_name );
	virtual ~ToolBase() {};

protected:
	Logger logger;

private:
	String name;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_TOOLS_H
