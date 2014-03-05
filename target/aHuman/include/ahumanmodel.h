#include "stdafx.h"

#ifndef	INCLUDE_AHUMANMODEL_H
#define INCLUDE_AHUMANMODEL_H

#include "xmlhuman.h"

/*#########################################################################*/
/*#########################################################################*/

class MindModel;

/*#########################################################################*/
/*#########################################################################*/

class MindModel : public Object {
public:
	MindModel();
	virtual ~MindModel();
	virtual const char *getClass() { return "MindModel"; };

public:
	void load();

	bool checkLinkCoveredByModel( String compSrc , String compDst );

public:
	XmlHMind hmindxml;
	XmlCircuits circuitsxml;
	XmlNerves nervesxml;
	XmlMuscles musclesxml;
};

/*#########################################################################*/
/*#########################################################################*/

#endif // INCLUDE_AHUMANMODEL_H
