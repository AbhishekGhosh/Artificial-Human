#include <ah_platform.h>
#include <ah_media_impl.h>

/*#########################################################################*/
/*#########################################################################*/

String SocketUrl::getValidUrl() {
	String value = getValidUrlWithoutPage();
	if( !page.isEmpty() )
		value += "/" + page;

	return( value );
}

String SocketUrl::getValidUrlWithoutPage() {
	ASSERTMSG( !protocol.isEmpty() , "protocol is not defined" );
	ASSERTMSG( !host.isEmpty() , "host is not defined" );
	ASSERTMSG( port > 0 && port <= 32000 , "port should be between 1 and 32000" );

	String value = protocol + ":" + port + "//" + host;
	return( value );
}

String SocketUrl::getProtocol() {
	return( protocol );
}

String SocketUrl::getHost() {
	return( host );
}

int SocketUrl::getPort() {
	return( port );
}

String SocketUrl::getPage() {
	return( page );
}

void SocketUrl::setUrl( String url ) {
	// format is "[[[[<protocol>:[<port>]]//host]/][<page and params>]
	int index1 = url.find( ":" );
	if( index1 > 0 ) {
		String value = url.getMid( 0 , index1 );
		value.trim();
		if( !value.isEmpty() )
			protocol = value;
	}
	int index1e = ( index1 >= 0 )? index1 + 1 : 0;

	int index2 = url.find( index1e , "//" );
	if( index1 >= 0 && index2 >= 0 ) {
		String value = url.getMid( index1e , index2 - index1e );
		value.trim();
		if( !value.isEmpty() )
			port = value.toInteger();
	}
	int index2e = ( index2 > 0 )? index2 + 2 : index1;
	
	int index3 = url.find( index2e , "/" );
	if( index3 >= 0 ) {
		String value = url.getMid( index2 , index3 - index2 );
		value.trim();
		if( !value.isEmpty() )
			host = value;

		value = url.getMid( index3 + 1 );
		value.trim();
		if( !value.isEmpty() )
			page = value;
	}
	else {
		// check that host mark is present - then string represents host
		if( index2 >= 0 ) {
			String value = url.getMid( index2e );
			value.trim();
			if( !value.isEmpty() )
				host = value;
		}
		else {
			// string represents page
			String value = url.getMid( index2e );
			value.trim();
			if( !value.isEmpty() )
				page = value;
		}
	}
}

void SocketUrl::setProtocol( String p_protocol ) {
	protocol = p_protocol;
}

void SocketUrl::setHost( String p_host ) {
	host = p_host;
}

void SocketUrl::setPort( int p_port ) {
	port = p_port;
}

void SocketUrl::setPage( String p_page ) {
	page = p_page;
}

SocketUrl::SocketUrl() {
	port = 0;
}

SocketUrl::SocketUrl( String p_url ) {
	port = 0;
	setUrl( p_url );
}

