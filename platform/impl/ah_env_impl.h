#ifndef	INCLUDE_AH_ENV_IMPL_H
#define INCLUDE_AH_ENV_IMPL_H

/*#########################################################################*/
/*#########################################################################*/

#include <ah_platform.h>

class EnvXmlDoc;

/*#########################################################################*/
/*#########################################################################*/

class EnvXmlDoc {
public:
	EnvXmlDoc( Xml xml , RFC_INT64 timestamp );
	~EnvXmlDoc();

	Xml getXml() { return( xml ); };

	bool isModified( RFC_INT64 timestamp );

private:
	Xml xml;
	RFC_INT64 fileModifiedTimestamp;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AH_ENV_IMPL_H
