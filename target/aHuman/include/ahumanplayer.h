#include "stdafx.h"
#include "xmlhuman.h"
#include "ahumanmodel.h"

/*#########################################################################*/
/*#########################################################################*/

class ScenarioPlayer;

/*#########################################################################*/
/*#########################################################################*/

class ScenarioPlayer : public MindModel {
public:
	ScenarioPlayer();
	virtual ~ScenarioPlayer();
	virtual const char *getClass() { return "ScenarioPlayer"; };

public:
	void play( Xml scenario );

private:
	void playSignal( Xml cmd );
};

/*#########################################################################*/
/*#########################################################################*/
