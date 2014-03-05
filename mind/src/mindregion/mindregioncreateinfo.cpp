#include <ah_mind.h>
#include <ah_mind_impl.h>

/*#########################################################################*/
/*#########################################################################*/

MindRegionCreateInfo::MindRegionCreateInfo() {
	sizeX = 0;
	sizeY = 0;
	useSpatialPooler = true;
	useTemporalPooler = true;
	temporalDepth = 0;
}

MindRegionCreateInfo::~MindRegionCreateInfo() {
}

void MindRegionCreateInfo::setId( String p_id ) {
	id = p_id;
	Object::setInstance( id );
}

void MindRegionCreateInfo::setInfo( MindRegionDef *p_typeInfo ) {
	typeInfo = p_typeInfo;
}

void MindRegionCreateInfo::setType( MindRegionTypeDef *p_typeDef ) {
	typeDef = p_typeDef;
}

void MindRegionCreateInfo::setUsingSpatialPooler( bool p_useSpatialPooler ) {
	useSpatialPooler = p_useSpatialPooler;
}

void MindRegionCreateInfo::setUsingTemporalPooler( bool p_useTemporalPooler ) {
	useTemporalPooler = p_useTemporalPooler;
}

void MindRegionCreateInfo::setTemporalDepth( int p_nDepth ) {
	temporalDepth = p_nDepth;
}

void MindRegionCreateInfo::setSizeInfo( int nx , int ny ) {
	sizeX = nx;
	sizeY = ny;
}

void MindRegionCreateInfo::getSizeInfo( int *nx , int *ny ) {
	*nx = sizeX;
	*ny = sizeY;
}

void MindRegionCreateInfo::setSpatialPoolerSize( int size ) {
	spatialPoolerSize = size;
}

void MindRegionCreateInfo::setTemporalPoolerSize( int size ) {
	temporalPoolerSize = size;
}

