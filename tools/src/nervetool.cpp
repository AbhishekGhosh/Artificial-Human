#include <ah_tools.h>
#include <ah_nervetool.h>
#include <platform\include\ah_platform.h>

/*#########################################################################*/
/*#########################################################################*/

NerveTool::NerveTool()
:	ToolBase( "NerveTool" ) {
}

NerveTool::~NerveTool() {
}

int NerveTool::execute( int nargs , char **args ) {
	// parameters
	String url;
	String fin;
	String fout;
	for( int k = 0; k < nargs; k++ ) {
		String arg = args[ k ];
		if( arg.equals( "-i" ) ) {
			fin = args[ ++k ];
			continue;
		}

		if( arg.equals( "-o" ) ) {
			fout = args[ ++k ];
			continue;
		}

		printf( "unknown option: %s\n" , args[ k ] );
		return( -1 );
	}

	FILE *sin = NULL;
	if( !fin.isEmpty() ) {
		sin = fopen( fin , "rt" );
		if( sin == NULL ) {
			printf( "cannot open input file (%s)\n" , ( const char * )fin );
			return( -2 );
		}
	}

	FILE *sout = NULL;
	if( !fout.isEmpty() ) {
		sout = fopen( fout , "at" );
		if( sout == NULL ) {
			printf( "cannot open output file (%s)\n" , ( const char * )fout );
			return( -2 );
		}
	}

	execute_file( sin , sout );

	if( sin != NULL )
		fclose( sin );
	if( sout != NULL )
		fclose( sout );

	return( 0 );
}

void NerveTool::execute_file( FILE *fin , FILE *fout ) {
	const int SIZE = 4096;
	char buffer[ SIZE ];

	StringList data; 
	while( !feof( fin ) ) {
		*buffer = 0;
		fgets( buffer , SIZE , fin );
		int len = strlen( buffer );

		data.add( buffer );
	}

	execute_data( data , fout );
}

void NerveTool::execute_data( StringList& data , FILE *sout ) {
	ClassList<Nerve> nn;

	// parse
	for( int k = 0; k < data.count(); k++ ) {
		String so = data.get( k );
		if( so.isEmpty() )
			continue;

		String s = so;
		int res = execute_line( s , sout , nn );
		if( res != 0 )
			fprintf( sout , "invalid string (%d): error=%s, full=%s\n" , res , ( const char * )s , ( const char * )so );
	}

	// output
	output_data( sout , nn );
}

int NerveTool::execute_line( String& s , FILE *sout , ClassList<Nerve>& nn ) {
	if( s.startsFrom( "<img" ) )
		return( execute_img( s , sout , 0 , nn ) );

	// parse string
	int level = s.find( "*" );
	if( level < 0 )
		return( 1 );

	if( level < 2 )
		return( 13 );

	s.remove( 0 , level + 1 );
	level -= 2;

	if( nn.count() > 0 ) {
		Nerve *np = nn.last();
		if( np -> level < level )
			if( level != np -> level + 1 )
				return( 14 );
	}

	s.trim();
	if( s.startsFrom( "<img" ) )
		return( execute_img( s , sout , level , nn ) );

	if( !s.startsFrom( "*" ) )
		return( 2 );

	s.remove( 0 , 1 );

	// name
	int idx = s.find( "*" );
	if( idx < 0 )
		return( 3 );

	String name = s.getMid( 0 , idx );
	s.remove( 0 , idx + 1 );

	// synonyms
	String synonyms;
	if( s.startsFrom( " (" ) ) {
		s.remove( 0 , 2 );
		idx = s.find( ")" );
		if( idx < 0 )
			return( 4 );

		synonyms = s.getMid( 0 , idx );
		s.remove( 0 , idx + 1 );
	}

	String origin;
	String branches;
	String distribution;
	String modality;
	String fibers;
	if( s.startsFrom( "; " ) ) {
		s.remove( 0 , 2 );

		if( !extract_item( sout , origin , s , "ORIGIN" ) )
			return( 6 );

		if( !extract_item( sout , branches , s , "BRANCHES" ) )
			return( 8 );

		if( !extract_item( sout , distribution , s , "DISTRIBUTION" ) )
			return( 7 );

		if( !extract_item( sout , modality , s , "MODALITY" ) )
			return( 9 );

		if( !extract_item( sout , fibers , s , "FIBERS" ) )
			return( 10 );
	}

	if( !s.isEmpty() )
		return( 11 );

	Nerve *n = new Nerve;
	n -> fibers = fibers;

	String fibersinfo;
	if( !fibers.isEmpty() )
		if( !extract_fibers( sout , n -> fibersinfo , fibers ) ) {
			fprintf( sout , "wrong fibers=%s\n" , ( const char * )fibers );
			delete n;
			return( 12 );
		}

	n -> name = name;
	n -> synonyms = synonyms;
	n -> level = level;
	n -> origin = origin;
	n -> branches = branches;
	n -> distribution = distribution;
	n -> modality = modality;
	nn.add( n );

	return( 0 );
}

int NerveTool::execute_img( String& s , FILE *sout , int level , ClassList<Nerve>& nn ) {
	// extract
	if( !s.startsFrom( "<img src=\"" ) )
		return( 101 );

	s.remove( 0 , strlen( "<img src=\"" ) );

	int idx = s.find( "\"" );
	if( idx < 0 )
		return( 102 );

	String s1 = s.getMid( 0 , idx );
	s.remove( 0 , idx + 1 );
	s.trim();

	s.trimTrailing( '>' );
	String height;
	if( s.startsFrom( "height=" ) )
		height = s.getMid( strlen( "height=" ) );

	// create string
	String img = "imgsrc=\"" + s1 + "\"";
	if( !height.isEmpty() )
		img += " imgheight=\"" + height + "\"";

	// find nerve to add to - upper with level - 1
	for( int k = nn.count() - 1; k >= 0; k-- ) {
		Nerve& n = nn.getRef( k );
		if( n.level < level ) {
			if( !n.imginfo.isEmpty() )
				return( false );

			n.imginfo = img;
			return( 0 );
		}
	}

	return( 100 );
}

bool NerveTool::extract_item( FILE *sout , String& item , String& s , String p ) {
	s.trimStarting( ' ' );
	if( !s.startsFrom( p + "={" ) ) {
		item.clear();
		return( true );
	}

	s.remove( 0 , p.length() + 2 );
	int idx = s.find( "}" );
	if( idx < 0 )
		return( false );

	item = s.getMid( 0 , idx );
	s.remove( 0 , idx + 1 );
	if( s.startsFrom( "; " ) )
		s.remove( 0 , 2 );

	return( true );
}

bool NerveTool::extract_fibers( FILE *sout , StringList& fibersinfo , String& s ) {
	// format: 
	// <type>:<item>[,item...] -> <item>[,item...] ... -> <item>[,item...]; <type>:...

	String type;
	s.trim();
	while( !s.isEmpty() ) {
		int idx = s.find( ":" );
		if( idx < 0 )
			return( false );

		// parse type and its value
		type = s.getMid( 0 , idx );

		int l_typeid = 0;
		if( type.equals( "GSE" ) )
			l_typeid = 1;
		else if( type.equals( "GSA" ) )
			l_typeid = 2;
		else if( type.equals( "GVE" ) )
			l_typeid = 3;
		else if( type.equals( "GVA" ) )
			l_typeid = 4;
		else if( type.equals( "SSA" ) )
			l_typeid = 5;
		else if( type.equals( "SVA" ) )
			l_typeid = 6;
		else if( type.equals( "SVE" ) )
			l_typeid = 7;
		else
			return( false );

		s.remove( 0 , idx + 1 );
		s.trim();

		String value;
		idx = s.find( ";" );
		if( idx < 0 ) {
			value = s;
			s.clear();
		}
		else {
			value = s.getMid( 0 , idx );
			s.remove( 0 , idx + 1 );
			s.trim();
			value.trim();
		}

		if( !extract_fiberitems( sout , fibersinfo , type , value ) ) {
			fprintf( sout , "wrong fibers typeinfo=%s\n" , ( const char * )value );
			return( false );
		}
	}

	return( true );
}

bool NerveTool::extract_fiberitems( FILE *sout , StringList& fibersinfo , String type , String& value ) {
	// parse value: x,y -> x,y -> x,y ...
	ClassList<StringList> chain;

	value.trim();
	while( !value.isEmpty() ) {
		String part;
		int idx = value.find( "->" );
		if( idx < 0 ) {
			if( chain.count() == 0 )
				return( false );

			part = value;
			value.clear();
		}
		else {
			part = value.getMid( 0 , idx );
			value.remove( 0 , idx + 2 );
			value.trim();

			if( value.isEmpty() )
				return( false );
		}

		// parse part
		StringList *z = new StringList;
		chain.add( z );
		if( !extract_codes( part , z ) ) {
			fprintf( sout , "wrong part=%s\n" , ( const char * )part );
			return( false );
		}

		// prohibit many-to-many		
		if( z -> count() > 1 && chain.count() > 1 ) {
			StringList& zp = chain.getRef( chain.count() - 2 );
			if( zp.count() > 1 )
				return( false );
		}
	}

	// chain of more than one
	if( chain.count() < 2 )
		return( false );

	// split chain
	int startChain = 0;
	int startChainCount = 0;
	for( int k = 0; k < chain.count(); k++ ) {
		StringList& z = chain.getRef( k );
		int zn = z.count();

		// starter
		if( k == 0 ) {
			startChainCount = zn;
			continue;
		}

		// many to one - split
		if( startChainCount > 1 ) {
			if( zn != 1 )
				return( false );

			addManyToOne( fibersinfo , type , chain.getRef( startChainCount ) , z.get( 0 ) );
			startChain = k;
			startChainCount = zn;
			continue;
		}

		// allow x -> y -> z as is
		if( zn == 1 ) {
			if( k == chain.count() - 1 ) {
				addSingleChain( fibersinfo , type , chain , startChain , k );
				break;
			}

			continue;
		}

		// x -> y -> x,y - split to x -> y and y -> x,y
		if( ( k - 1 ) > startChain ) {
			addSingleChain( fibersinfo , type , chain , startChain , k - 1 );
			startChain = k - 1;
			startChainCount = 1;
		}

		addOneToMany( fibersinfo , type , chain.getRef( startChain ).get( 0 ) , z );
		startChain = k;
		startChainCount = zn;
	}

	chain.destroy();

	return( true );
}

bool NerveTool::extract_codes( String& s , StringList *z ) {
	String so;

	// [zzz,code],[zzz,code]
	s.trim();
	while( !s.isEmpty() ) {
		if( !s.startsFrom( "[" ) )
			return( false );

		int idx = s.find( "]" );
		if( idx < 0 )
			return( false );

		String a = s.getMid( 1 , idx - 1 );
		s.remove( 0 , idx + 1 );
		s.trim();

		idx = a.find( "," );
		if( idx < 0 )
			return( false );

		a = a.getMid( idx + 1 );
		a.trim();
		if( a.isEmpty() )
			return( false );

		idx = s.find( "," );
		if( idx >= 0 ) {
			s.remove( 0 , idx + 1 );
			s.trim();

			if( s.isEmpty() )
				return( false );
		}

		// add to list
		z -> add( a );
	}

	return( true );
}

void NerveTool::output_data( FILE *sout , ClassList<Nerve>& nn ) {
	for( int k = 0; k < nn.count(); k++ )
		output_nerve( sout , nn , k );
}

void NerveTool::output_nerve( FILE *sout , ClassList<Nerve>& nn , int k ) {
	Nerve& n = nn.getRef( k );

	bool hasChilds = false;
	int upLevelCount = 0;
	if( nn.count() > ( k + 1 ) ) {
		Nerve& nx = nn.getRef( k + 1 );
		if( nx.level > n.level )
			hasChilds = true;
		else
			upLevelCount = n.level - nx.level;
	}
	else
		upLevelCount = n.level;

	String tabs = String( "\t" ).replicate( 2 + n.level );
	String s = tabs + "<element name=\"" + n.name + "\"";
	if( !n.synonyms.isEmpty() )
		s += "\n" + tabs + "\tsynonyms=\"" + n.synonyms + "\"";
	if( !n.origin.isEmpty() )
		s += "\n" + tabs + "\torigin=\"" + n.origin + "\"";
	if( !n.distribution.isEmpty() )
		s += "\n" + tabs + "\tdistribution=\"" + n.distribution + "\"";
	if( !n.imginfo.isEmpty() )
		s += "\n" + tabs + "\t" + n.imginfo;
	if( !n.modality.isEmpty() )
		s += "\n" + tabs + "\tmodality=\"" + n.modality + "\"";

	if( n.fibers.isEmpty() ) {
		if( hasChilds )
			s +="\n" + tabs + "\t>\n";
		else
			s +="\n" + tabs + "\t/>\n";
	}
	else {
		s += "\n" + tabs + "\t>";

		String fibersinfo;
		for( int z = 0; z < n.fibersinfo.count(); z++ )
			fibersinfo += tabs + "\t" + n.fibersinfo.get( z ) + "\n";

		s += "\n" + fibersinfo;
		if( !hasChilds )
			s += tabs + "</element>\n";
	}

	for( int z = 1; z <= upLevelCount; z++ ) {
		tabs = String( "\t" ).replicate( 2 + n.level - z );
		s += tabs + "</element>\n";
	}

	if( ( n.level == 0 && hasChilds == false ) || ( n.level > 0 && ( n.level - upLevelCount ) == 0 ) )
		s += "\n";

	fprintf( sout , "%s" , ( const char * )s );
}

void NerveTool::addManyToOne( StringList& fibersinfo , String type , StringList& from , String to ) {
	StringList mids;
	for( int k = 0; k < from.count(); k++ )
		addFiberItem( fibersinfo , type , from.get( k ) , to , mids );
}

void NerveTool::addOneToMany( StringList& fibersinfo , String type , String from , StringList& to ) {
	StringList mids;
	for( int k = 0; k < to.count(); k++ )
		addFiberItem( fibersinfo , type , from , to.get( k ) , mids );
}

void NerveTool::addSingleChain( StringList& fibersinfo , String type , ClassList<StringList>& chain , int from , int to ) {
	StringList mids;
	for( int k = from + 1; k < to; k++ )
		mids.add( chain.getRef( k ).get( 0 ) );

	addFiberItem( fibersinfo , type , chain.getRef( from ).get( 0 ) , chain.getRef( to ).get( 0 ) , mids );
}

void NerveTool::addFiberItem( StringList& fibersinfo , String type , String from , String to , StringList& mids ) {
	String s = "<fibers type=\"" + type + "\" src=\"" + from + "\" dst=\"" + to + "\">";
	for( int k = 0; k < mids.count(); k++ )
		s += "<mid id=\"" + mids.get( k ) + "\"/>";

	s += "</fibers>";
	fibersinfo.add( s );
}

