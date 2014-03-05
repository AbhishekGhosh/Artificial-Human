/*#######################################################*/
/*#######################################################*/

/* this header - string functions */
/* version 2.02.0001 */
#define RFC__GENSTR_H

/*#######################################################*/
/*#######################################################*/

#define RFC_STR_FPOPT_NOWSTR		1

/*#######################################################*/
/*#######################################################*/

typedef struct _rfc_string_format		rfc_string_format;
typedef struct _rfc_string_format_el	rfc_string_format_el;

/*#######################################################*/
/*#######################################################*/

/* string format part */
struct _rfc_string_format_el
{
	int			s_from;		/* segment start position */
	int			s_to;		/* next segment start position */
	short		s_isarg;	/* set if segment of argument */
	char		s_arg_t;	/* argument type */
	RFC_TYPE	s_arg_v;	/* value if argument segment */
	int			s_arg_w;	/* argument width */
	int			s_arg_p;	/* argument precesion */
	int			s_arg_s;	/* argument max size */
};

/* string format information */
struct _rfc_string_format
{
	int			s_n;		/* argument cound */
	rfc_string_format_el	/* format string parts */
				s_parts[ 255 ];
};

/*#######################################################*/
/*#######################################################*/
/* ANSI string functions */

/* create string */
extern rfc_string
	rfc_str_create( const char *p_s , int p_len , int p_allocate );
/* reallocate buffer if needed */
extern rfc_string
	rfc_str_allocate( rfc_string p_str , int p_size );
/* destroy string */
extern void
	rfc_str_drop( rfc_string p_str );

/* append string */
extern rfc_string
	rfc_str_append( rfc_string p_str , int p_pos , const char *p_s , int p_slen );
/* add char to string */
extern rfc_string
	rfc_str_addchar( rfc_string p_str , int p_pos , char p_c );
/* remove part of string */
extern void
	rfc_str_remove( rfc_string p_str , int p_pos , int p_len );
/* clear string */
extern void
	rfc_str_clear( rfc_string p_str );
/* set string */
extern rfc_string
	rfc_str_set( rfc_string p_str , const char *p_s , int p_len );
/* extract left part of string */
extern rfc_string
	rfc_str_extract_left( const char *p_s , int p_count );
/* extract right part of string */
extern rfc_string
	rfc_str_extract_right( const char *p_s , int p_count );
/* extract middle part of string */
extern rfc_string
	rfc_str_extract_middle( const char *p_s , int p_start , int p_count );
/* get char by pos from end */
extern char
	rfc_str_rchar( const char *p_s , int p_rpos );
/* check string empty */
#define rfc_str_isempty( /* rfc_string */ p_s ) \
	( ( *p_s == 0 )? 1 : 0 )
/* make format string */
extern rfc_string
	rfc_str_format( rfc_string p_str , const char *p_format , ... );
/* make format string with valist*/
extern rfc_string
	rfc_str_format_va( rfc_string p_str , const char *p_format , va_list p_va );

/* parse format string */
extern void
	rfc_str_format_parse( const char *p_format , va_list p_va , rfc_string_format *p_fi , int p_opts );
/* print parsed element */
extern int
	rfc_str_format_parse_printf( char *p_s , const char *p_format , rfc_string_format_el *p_fi );

/* translate from unicode using codepage */
extern short
	rfc_str_setcp( char *p_s , const unsigned short *p_v , int p_cp );

/*#######################################################*/
/*#######################################################*/
/* UNICODE USC-2 string functions */

/* create string */
extern rfc_wstring
	rfc_wstr_create( const unsigned short *p_s , int p_len , int p_allocate );
/* reallocate buffer if needed */
extern rfc_wstring
	rfc_wstr_allocate( rfc_wstring p_str , int p_size );
/* destroy string */
extern void
	rfc_wstr_drop( rfc_wstring p_str );

/* append string */
extern rfc_wstring
	rfc_wstr_append( rfc_wstring p_str , int p_pos , const unsigned short *p_s , int p_slen );
/* add char to string */
extern rfc_wstring
	rfc_wstr_addchar( rfc_wstring p_str , int p_pos , unsigned short p_c );
/* remove part of string */
extern void
	rfc_wstr_remove( rfc_wstring p_str , int p_pos , int p_len );
/* clear string */
extern void
	rfc_wstr_clear( rfc_wstring p_str );
/* set string */
extern rfc_wstring
	rfc_wstr_set( rfc_wstring p_str , const unsigned short *p_s , int p_len );
/* extract left part of string */
extern rfc_wstring
	rfc_wstr_extract_left( const unsigned short *p_s , int p_count );
/* extract right part of string */
extern rfc_wstring
	rfc_wstr_extract_right( const unsigned short *p_s , int p_count );
/* extract middle part of string */
extern rfc_wstring
	rfc_wstr_extract_middle( const unsigned short *p_s , int p_start , int p_count );
/* get char by pos from end */
extern unsigned short
	rfc_wstr_rchar( const unsigned short *p_s , int p_rpos );
/* check string empty */
#define rfc_wstr_isempty( /* rfc_wstring */ p_s ) \
	( ( *p_s == 0 )? 1 : 0 )
/* make format string */
extern rfc_wstring
	rfc_wstr_format( rfc_wstring p_str , const unsigned short *p_format , ... );
/* make format string with valist*/
extern rfc_wstring
	rfc_wstr_format_va( rfc_wstring p_str , const unsigned short *p_format , va_list p_va );

/* special */
/* get string length */
extern int
	rfc_wstr_len( const unsigned short *p_s );
/* convert string to integer */
extern int
	rfc_wstr_atoi( const unsigned short *p_s );
/* check character is digit */
extern int
	rfc_wstr_isdigit( const unsigned short p_c );
/* compare strings */
extern int
	rfc_wstr_ncmp( const unsigned short *p_s1 , const unsigned short *p_s2 , int p_n );
/* compare strings */
extern int
	rfc_wstr_cmp( const unsigned short *p_s1 , const unsigned short *p_s2 );
/* format string */
extern int
	rfc_wstr_sprintf( unsigned short *p_s , const unsigned short *p_fmt , ... );
extern int
	rfc_wstr_vsprintf( unsigned short *p_s , const unsigned short *p_fmt , va_list p_va );
/* copy string */
extern unsigned short *
	rfc_wstr_cpy( unsigned short *p_dst , const unsigned short *p_src );
/* find character */
extern unsigned short *
	rfc_wstr_chr( const unsigned short *p_s , unsigned short p_c );
/* find string */
extern unsigned short *
	rfc_wstr_str( const unsigned short *p_s , const unsigned short *p_v );

/* translate from ansi using codepage */
extern short
	rfc_wstr_setcp( unsigned short *p_s , const char *p_v , int p_cp );
/* translate from UTF8 */
extern short
	rfc_wstr_setutf8( rfc_wchar *p_s , const rfc_uchar *p_v );

/*#######################################################*/
/*#######################################################*/
/* UNICODE UTF-8 string functions */

/* create string */
extern rfc_ustring
	rfc_ustr_create( const rfc_uchar *p_s , int p_len , int p_allocate );
/* reallocate buffer if needed */
extern rfc_ustring
	rfc_ustr_allocate( rfc_ustring p_str , int p_size );
/* destroy string */
extern void
	rfc_ustr_drop( rfc_ustring p_str );

/* append string */
extern rfc_ustring
	rfc_ustr_append( rfc_ustring p_str , int p_pos , const rfc_uchar *p_s , int p_slen );
/* remove part of string */
extern void
	rfc_ustr_remove( rfc_ustring p_str , int p_pos , int p_len );
/* clear string */
extern void
	rfc_ustr_clear( rfc_ustring p_str );
/* set string */
extern rfc_ustring
	rfc_ustr_set( rfc_ustring p_str , const rfc_uchar *p_s , int p_len );
/* extract left part of string */
extern rfc_ustring
	rfc_ustr_extract_left( const rfc_uchar *p_s , int p_count );
/* extract right part of string */
extern rfc_ustring
	rfc_ustr_extract_right( const rfc_uchar *p_s , int p_count );
/* extract middle part of string */
extern rfc_ustring
	rfc_ustr_extract_middle( const rfc_uchar *p_s , int p_start , int p_count );
/* check string empty */
#define rfc_ustr_isempty( /* rfc_ustring */ p_s ) \
	( ( *p_s == 0 )? 1 : 0 )
/* make format string */
extern rfc_ustring
	rfc_ustr_format( rfc_ustring p_str , const rfc_uchar *p_format , ... );
/* make format string with valist*/
extern rfc_ustring
	rfc_ustr_format_va( rfc_ustring p_str , const rfc_uchar *p_format , va_list p_va );

/* special */
/* get string length */
extern int
	rfc_ustr_len( const rfc_uchar *p_s );
/* get string size for first p_n characters */
extern int
	rfc_ustr_size( const rfc_uchar *p_s , int p_n );
/* compare strings */
extern int
	rfc_ustr_ncmp( const rfc_uchar *p_s1 , const rfc_uchar *p_s2 , int p_n );
/* get next character pointer */
extern rfc_uchar *
	rfc_ustr_next( const rfc_uchar *p_s );

/* translate from USC-2 */
extern short
	rfc_ustr_setusc2( rfc_uchar *p_s , const rfc_wchar *p_v );
/* calculate UTF8 string size by USC-2 string */
extern int
	rfc_ustr_usc2size( const rfc_wchar *p_v );
