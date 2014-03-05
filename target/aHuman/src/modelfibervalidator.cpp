#include "stdafx.h"
#include "ahumantarget.h"

/*#########################################################################*/
/*#########################################################################*/

ModelFiberValidator::ModelFiberValidator() {
	attachLogger();
}

ModelFiberValidator::~ModelFiberValidator() {
}

bool ModelFiberValidator::isValid_GSE_end( XmlNerveInfo& info , MindRegion *src , MindRegion *dst ) {
	bool res = true;

	// src=somatic motor, dst=target
	MindRegionRoleEnum role = src -> getRegionInfo() -> getRole();
	if( role != MIND_REGION_ROLE_FLEXOR && role != MIND_REGION_ROLE_EXTENSOR ) {
		res = false;
		logger.logError( "isValid_GSE_end: invalid source role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	role = dst -> getRegionInfo() -> getRole();
	if( role != MIND_REGION_ROLE_TARGETSENSOR && role != MIND_REGION_ROLE_TARGETEFFECTOR ) {
		res = false;
		logger.logError( "isValid_GSE_end: invalid destination role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	return( res );
}

bool ModelFiberValidator::isValid_GSA_begin( XmlNerveInfo& info , MindRegion *src , MindRegion *dst ) {
	bool res = true;

	// src=target, dst=sensory
	MindRegionRoleEnum role = src -> getRegionInfo() -> getRole();
	if( role != MIND_REGION_ROLE_TARGETSENSOR && role != MIND_REGION_ROLE_TARGETEFFECTOR ) {
		res = false;
		logger.logError( "isValid_GSA_begin: invalid source role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	role = dst -> getRegionInfo() -> getRole();
	if( role != MIND_REGION_ROLE_SENSORY ) {
		res = false;
		logger.logError( "isValid_GSA_begin: invalid destination role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	return( res );
}

bool ModelFiberValidator::isValid_GSA_afterbegin( XmlNerveInfo& info , MindRegion *src , MindRegion *dst ) {
	bool res = true;

	// src=sensory ganglion, dst=not sensory/target
	MindRegionRoleEnum role = src -> getRegionInfo() -> getRole();
	if( role != MIND_REGION_ROLE_SENSORY ) {
		res = false;
		logger.logError( "isValid_GSA_afterbegin: invalid source role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	role = dst -> getRegionInfo() -> getRole();
	if( role == MIND_REGION_ROLE_TARGETSENSOR || role == MIND_REGION_ROLE_TARGETEFFECTOR  || role == MIND_REGION_ROLE_SENSORY ) {
		res = false;
		logger.logError( "isValid_GSA_afterbegin: invalid destination role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	return( res );
}

bool ModelFiberValidator::isValid_GVE_end( XmlNerveInfo& info , MindRegion *src , MindRegion *dst ) {
	bool res = true;

	// src=autonomic ganglion, dst=target
	MindRegionRoleEnum role = src -> getRegionInfo() -> getRole();
	if( ( role != MIND_REGION_ROLE_SYMPATHETIC && role != MIND_REGION_ROLE_PARASYMPATHETIC ) || !src -> getRegionInfo() -> getTypeName().equals( "ganglion" ) ) {
		res = false;
		logger.logError( "isValid_GVE_end: invalid source role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	role = dst -> getRegionInfo() -> getRole();
	if( role != MIND_REGION_ROLE_TARGETSENSOR && role != MIND_REGION_ROLE_TARGETEFFECTOR ) {
		res = false;
		logger.logError( "isValid_GVE_end: invalid destination role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	return( res );
}

bool ModelFiberValidator::isValid_GVE_beforeend( XmlNerveInfo& info , MindRegion *src , MindRegion *dst ) {
	bool res = true;

	// src=autonomic nucleus, dst=autonomic ganglion
	MindRegionRoleEnum role1 = src -> getRegionInfo() -> getRole();
	if( ( role1 != MIND_REGION_ROLE_SYMPATHETIC && role1 != MIND_REGION_ROLE_PARASYMPATHETIC ) || !src -> getRegionInfo() -> getTypeName().equals( "nucleus" ) ) {
		res = false;
		logger.logError( "isValid_GVE_beforeend: invalid source role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	MindRegionRoleEnum role2 = dst -> getRegionInfo() -> getRole();
	if( ( role2 != MIND_REGION_ROLE_SYMPATHETIC && role2 != MIND_REGION_ROLE_PARASYMPATHETIC ) || role2 != role1 || !dst -> getRegionInfo() -> getTypeName().equals( "ganglion" ) ) {
		res = false;
		logger.logError( "isValid_GVE_beforeend: invalid destination role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	return( res );
}

bool ModelFiberValidator::isValid_SSA_begin( XmlNerveInfo& info , MindRegion *src , MindRegion *dst ) {
	bool res = true;

	// src=target, dst=relay or processor nucleus
	MindRegionRoleEnum role1 = src -> getRegionInfo() -> getRole();
	if( role1 != MIND_REGION_ROLE_TARGETSENSOR && role1 != MIND_REGION_ROLE_TARGETEFFECTOR ) {
		res = false;
		logger.logError( "isValid_SSA_begin: invalid source role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	MindRegionRoleEnum role2 = dst -> getRegionInfo() -> getRole();
	if( ( role2 != MIND_REGION_ROLE_SENSORY && role2 != MIND_REGION_ROLE_RELAY && role2 != MIND_REGION_ROLE_PROCESSOR ) || 
		( dst -> getRegionInfo() -> getTypeName().equals( "nucleus" ) == false && dst -> getRegionInfo() -> getTypeName().equals( "ganglion" ) == false ) ) {
		res = false;
		logger.logError( "isValid_SSA_begin: invalid destination role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	return( res );
}

bool ModelFiberValidator::isValid_SVA_begin( XmlNerveInfo& info , MindRegion *src , MindRegion *dst ) {
	bool res = true;

	// src=target, dst=sensory ganglion
	MindRegionRoleEnum role1 = src -> getRegionInfo() -> getRole();
	if( role1 != MIND_REGION_ROLE_TARGETSENSOR && role1 != MIND_REGION_ROLE_TARGETEFFECTOR ) {
		res = false;
		logger.logError( "isValid_SVA_begin: invalid source role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	MindRegionRoleEnum role2 = dst -> getRegionInfo() -> getRole();
	if( ( role2 != MIND_REGION_ROLE_SENSORY ) || !dst -> getRegionInfo() -> getTypeName().equals( "ganglion" ) ) {
		res = false;
		logger.logError( "isValid_SVA_begin: invalid destination role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	return( res );
}

bool ModelFiberValidator::isValid_SVA_afterbegin( XmlNerveInfo& info , MindRegion *src , MindRegion *dst ) {
	bool res = true;

	// src=sensory ganglion, dst=relay or processor nucleus
	MindRegionRoleEnum role1 = src -> getRegionInfo() -> getRole();
	if( ( role1 != MIND_REGION_ROLE_SENSORY ) || !src -> getRegionInfo() -> getTypeName().equals( "ganglion" ) ) {
		res = false;
		logger.logError( "isValid_SVA_afterbegin: invalid source role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	MindRegionRoleEnum role2 = dst -> getRegionInfo() -> getRole();
	if( ( role2 != MIND_REGION_ROLE_RELAY && role2 != MIND_REGION_ROLE_PROCESSOR ) || !dst -> getRegionInfo() -> getTypeName().equals( "nucleus" ) ) {
		res = false;
		logger.logError( "isValid_SVA_afterbegin: invalid destination role in link from region=" + src -> getRegionId() + " to region=" + dst -> getRegionId() + ", from nerve=" + info.name );
	}

	return( res );
}

