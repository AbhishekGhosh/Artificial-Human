#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

TargetAreaDef::TargetAreaDef() {
}

TargetAreaDef::~TargetAreaDef() {
}

void TargetAreaDef::defineSensorArea() {
	MindAreaDef::enabled = true;
	MindAreaDef::areaId = "TSA";
	MindAreaDef::areaName = "Sensor Area";
	MindAreaDef::areaType = "Target";
	MindAreaDef::areaFunction = "All sensory regions";
}

void TargetAreaDef::defineEffectorArea() {
	MindAreaDef::enabled = true;
	MindAreaDef::areaId = "TEA";
	MindAreaDef::areaName = "Effector Area";
	MindAreaDef::areaType = "Target";
	MindAreaDef::areaFunction = "All effector regions";
}

void TargetAreaDef::addRegion( TargetRegionDef *regionDef ) {
	MindAreaDef::regions.add( regionDef );
	MindAreaDef::regionMap.add( regionDef -> getId() , regionDef );
}

