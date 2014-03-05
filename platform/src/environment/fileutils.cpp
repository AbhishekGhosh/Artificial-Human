#include <ah_platform.h>
#include <ah_env_impl.h>

static int write_chunk( char *p_b , FILE *f , int maxread );

/*#########################################################################*/
/*#########################################################################*/

String aiutils_getnextlinefromfile( FILE *f )
{
	String s = "";
	char l_buf[ 1024 + 1 ];

	int l_written;
	do {
		l_written = write_chunk( l_buf , f , 1024 );
		l_buf[ l_written ] = 0;
		s += l_buf;
	}
	while( l_written > 0 );
	return( s );
}

static int write_chunk( char *p_b , FILE *f , int maxread )
{
	int l_written = maxread;
	int c;
	while( maxread && ( c = getc( f ) ) != EOF && c != '\n' ) {
		if( c == '\r' )
			continue;

		*p_b++ = c;
		maxread--;
	}
	return( l_written - maxread );
}

void freadString( String& s , FILE *f )
{
	// get size of file
	fseek( f , 0 , SEEK_END );
	int size = ftell( f );

	s.resize( size );
	memset( s.getBuffer() , 0 , size );
	rewind( f );
	fread( s.getBuffer() , 1 , size , f );
	rewind( f );
}

