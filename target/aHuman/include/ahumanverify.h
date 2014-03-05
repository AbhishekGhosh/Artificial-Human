#include "stdafx.h"
#include "xmlhuman.h"
#include "ahumanmodel.h"

/*#########################################################################*/
/*#########################################################################*/

class ModelVerifier;
class ModelFiberValidator;

/*#########################################################################*/
/*#########################################################################*/

typedef enum {
	FIBER_CHAIN_POS_BEGIN = 1 ,
	FIBER_CHAIN_POS_END = 2 ,
	FIBER_CHAIN_POS_MID = 3
} ModelVerifierFiberChainPosEnum;

class ModelVerifier : public MindModel {
public:
	ModelVerifier( Xml modelArea );
	virtual ~ModelVerifier();
	virtual const char *getClass() { return "ModelVerifier"; };

public:
	void verify();

private:
	void checkHierarchy();
	void checkCircuits();
	void checkNerves();
	void checkMuscles();
	void checkMindModel();

	bool checkHierarchy_verifyChild( String node , bool checkMapping );
	bool checkHierarchy_verifyConnectors( MindRegionDef *rd , const XmlHMindElementInfo& info );
	bool checkCircuits_verifyComponents( String circuit );
	bool checkCircuits_verifyLinks( String circuit );
	bool checkCircuits_verifyCircuitLink( XmlCircuitInfo& circuit , XmlCircuitLinkInfo& link );
	bool checkNerves_verifyComponents( String nerve );
	bool checkNerves_verifyModalityByChilds( XmlNerveInfo& nerve , MapStringToPtr& parentFibers );
	bool checkNerves_verifyFiberIsCovered( String ft , StringList& mods );
	bool checkNerves_verifyModalityIsCoveredByFibers( String mod , MapStringToPtr& fibers );
	bool checkNerves_verifyLinks( XmlNerveInfo& info , XmlNerveFiberInfo& nf );
	bool checkNerves_verifyFiberChain( XmlNerveInfo& info , XmlNerveFiberInfo& nf , String regionSrcId , String regionDstId , ModelVerifierFiberChainPosEnum pos , int midleft , int midright );
	bool checkMuscles_verifyNerves( String muscle );
	bool checkMuscles_verifyNerve( XmlMuscleInfo& muscle , String nerve , String cdef );
	bool checkFiberComp( XmlNerveInfo& info , XmlNerveFiberInfo& nf , String comp );
	bool checkFiberType( XmlNerveInfo& info , XmlNerveFiberInfo& nf , String type );
	bool checkMindModel_verifyRegion( MindRegionDef *regionDef );
	bool checkMindModel_verifyLinkedConnectors( MindRegionDef *regionDef , MindRegion *region );
	bool checkMindModel_verifyRegionCircuits( MindRegionDef *regionDef , MindRegion *region );

private:
	Xml modelArea;

	MapStringToClass<MindRegionDef> regionMap;
	MapStringToClass<MindRegionDef> hierarchyMap;
};

/*#########################################################################*/
/*#########################################################################*/

class ModelFiberValidator : public Object {
public:
	ModelFiberValidator();
	virtual ~ModelFiberValidator();
	virtual const char *getClass() { return "ModelFiberValidator"; };

public:
	bool isValid_GSE_end( XmlNerveInfo& info , MindRegion *src , MindRegion *dst );
	bool isValid_GSA_begin( XmlNerveInfo& info , MindRegion *src , MindRegion *dst );
	bool isValid_GSA_afterbegin( XmlNerveInfo& info , MindRegion *src , MindRegion *dst );
	bool isValid_GVE_end( XmlNerveInfo& info , MindRegion *src , MindRegion *dst );
	bool isValid_GVE_beforeend( XmlNerveInfo& info , MindRegion *src , MindRegion *dst );
	bool isValid_SSA_begin( XmlNerveInfo& info , MindRegion *src , MindRegion *dst );
	bool isValid_SVA_begin( XmlNerveInfo& info , MindRegion *src , MindRegion *dst );
	bool isValid_SVA_afterbegin( XmlNerveInfo& info , MindRegion *src , MindRegion *dst );
};

/*#########################################################################*/
/*#########################################################################*/
