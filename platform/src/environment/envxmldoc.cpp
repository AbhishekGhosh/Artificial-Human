#include <ah_platform.h>
#include <ah_env_impl.h>

EnvXmlDoc::EnvXmlDoc( Xml p_xml , RFC_INT64 timestamp ) {
	xml = p_xml;
	fileModifiedTimestamp = timestamp;
}

EnvXmlDoc::~EnvXmlDoc() {
	if( xml.exists() )
		xml.destroy();
}

bool EnvXmlDoc::isModified( RFC_INT64 timestamp ) {
	if( fileModifiedTimestamp == timestamp )
		return( false );

	return( true );
}
