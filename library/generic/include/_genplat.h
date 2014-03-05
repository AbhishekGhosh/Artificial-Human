/*#######################################################*/
/*#######################################################*/
#define RFC__GENPLAT_H

/*#######################################################*/
/*#######################################################*/
/* force includes */

#if !defined(_VA_LIST_DEFINED) && !defined(__VARARGS_H) && !defined(__STDARG_H)
#include <stdarg.h>
#endif

#include <stdio.h>
#include <stddef.h>
#include <string.h>
#include <memory.h>
#include <malloc.h>
#include <stdlib.h>
#include <time.h>

/*#######################################################*/
/*#######################################################*/

/* codepages */
#define RFC_CODEPAGE_UNKNOWN	-1
#define RFC_CODEPAGE_ANSI		0
#define RFC_CODEPAGE_OEM		1

/*#######################################################*/
/*#######################################################*/

/* platform type definitions */
#ifndef INVALID_SOCKET
	typedef unsigned int	SOCKET;
#define INVALID_SOCKET		(SOCKET)(~0)
#endif

#ifndef BITS_PER_INT
#define BITS_PER_INT		32
#endif

#define BIG_STRING_SIZE		64535

typedef void *				RFC_HND;
typedef void				( *RFC_PROC )();

/*#######################################################*/
/*#######################################################*/
	
/* windows definitions */
#ifdef _WIN32

	typedef long long int			RFC_INT64;
	typedef struct { RFC_HND s_ih; unsigned s_ip; }
							RFC_THREAD;
	typedef unsigned		( __stdcall *RFC_THRFUNC )( void *p_arg );

	typedef struct fd_set	RFC_FD_SET;

#define RFC_INT64FMTBASE	"I64"
#define RFC_INT64FMT		"%I64d"
#define RFC_DIRSLASH		'\\'
#define RFC_DIRSLASHS		"\\"
#define RFC_WINT64FMTBASE	L"I64"
#define RFC_WINT64FMT		L"%I64d"
#define RFC_WDIRSLASH		L'\\'
#define RFC_WDIRSLASHS		L"\\"

/* POSIX definitions */
#else

	/* money type */
	typedef long long		RFC_INT64;

#if !defined( PTHREAD ) && !defined( _PTHREAD_DESCR_DEFINED ) && !defined( _POSIX_C_SOURCE_ )
	typedef struct { int s_i1; int s_i2; }
							pthread_t;
#endif

	typedef pthread_t		RFC_THREAD;
	typedef void *			( *RFC_THRFUNC )( void *p_arg );

	typedef int				RFC_FD_SET;

#define RFC_INT64FMTBASE	"ll"
#define RFC_INT64FMT		"%lld"
#define RFC_DIRSLASH		'/'
#define RFC_DIRSLASHS		"/"
#define RFC_INT64FMTBASE	L"ll"
#define RFC_INT64FMT		L"%lld"
#define RFC_DIRSLASH		L'/'
#define RFC_DIRSLASHS		L"/"

#endif

/*#######################################################*/
/*#######################################################*/

/* helper field types */
#define RFC_EXT_TYPENONE	0x00000000		/* void						*/
#define RFC_EXT_TYPESHORT	0x00000001		/* short[2]					*/
#define RFC_EXT_TYPELONG	0x00000002		/* int[4]					*/
#define RFC_EXT_TYPEMONEY	0x00000003		/* RFC_INT64[8]				*/
#define RFC_EXT_TYPEDOUBLE	0x00000004		/* double[8]				*/
#define RFC_EXT_TYPESTRING	0x00000005		/* rfc_string				*/
#define RFC_EXT_TYPEMOMENT	0x00000006		/* rfc_moment[4]			*/
#define RFC_EXT_TYPETIMET	0x00000007		/* time_t[4]				*/

#define RFC_EXT_TYPECHAR	0x00000008		/* char *					*/
#define RFC_EXT_TYPEPTR		0x00000009		/* void *					*/
#define RFC_EXT_TYPEBYTE	0x0000000A		/* char						*/
#define RFC_EXT_TYPEBLOB	0x0000000B		/* blob (struct _rfc_blob)	*/
#define RFC_EXT_TYPELCHAR	0x0000000C		/* string (struct _rfc_lchar)	*/
#define RFC_EXT_TYPEWCHAR	0x0000000D		/* unsigned short *			*/
#define RFC_EXT_TYPEUCHAR	0x0000000E		/* char * (UTF8)			*/
#define RFC_EXT_TYPEFLOAT	0x0000000F		/* float[4]					*/

/*#######################################################*/

/* helper generic types */
typedef unsigned short		rfc_wchar;
typedef unsigned char		rfc_uchar;
typedef char *				rfc_string;
typedef rfc_wchar *			rfc_wstring;
typedef rfc_uchar *			rfc_ustring;
typedef int					rfc_moment;

typedef struct _rfc_blob
{
	char *					s_ptr;	/* data pointer */
	int						s_size;	/* blob size */
} rfc_blob;

typedef struct _rfc_lchar
{
	const char *			s_ptr;		/* data pointer */
	int						s_cp;		/* code page */
} rfc_lchar;

/* generic type value */
typedef union _RFC_TYPE		RFC_TYPE;

/*#######################################################*/
/*#######################################################*/

/* money type */
typedef union	{
			struct { unsigned int s_low; int s_hi; }	
						s_part;
			double		s_double;
			RFC_INT64	s_int64;
				}			RFC_MONEY;

union _RFC_TYPE
{
	long					u_l;	/* RFC_EXT_TYPELONG , RFC_EXT_TYPEMOMENT */
	double					u_f;	/* RFC_EXT_TYPEDOUBLE */
	RFC_INT64				u_m;	/* RFC_EXT_TYPEMONEY */
	time_t					u_t;	/* RFC_EXT_TYPETIMET */
	char *					u_s;	/* RFC_EXT_TYPESTRING , RFC_EXT_TYPECHAR */
	short					u_d;	/* RFC_EXT_TYPESHORT */
	void *					u_p;	/* RFC_EXT_TYPEPTR */
	const char *			u_c;	/* RFC_EXT_TYPECHAR */
	char					u_b;	/* RFC_EXT_TYPEBYTE */
	rfc_blob				u_o;	/* RFC_EXT_TYPEBLOB */
	RFC_MONEY				u_mm;	/* RFC_EXT_TYPEMONEY (duplicated) */
	const rfc_wchar *		u_wc;	/* RFC_EXT_TYPEWCHAR */
	const rfc_uchar *		u_uc;	/* RFC_EXT_TYPEUCHAR */
	rfc_lchar				u_lc;	/* RFC_EXT_TYPELCHAR */
	float					u_r;	/* RFC_EXT_TYPEFLOAT */
};

/*#######################################################*/
/*#######################################################*/

/* platform independent macros */
#define _RFC_GET_SHORT( _v , _x ) \
	_v = ( ( *( (_x) + 0 ) << 8 ) | \
		   ( *( (_x) + 1 ) ) )

#define _RFC_SET_SHORT( _x , _v ) \
	*( (_x) + 0 ) = ( unsigned char )( (_v) >> 8 ) , \
	*( (_x) + 1 ) = ( unsigned char )( (_v) )

#define _RFC_DCL_SHORT( _x ) \
	unsigned char _x[ 2 ]

#define _RFC_GET_LONG( _v , _x ) \
	_v = ( ( *( (_x) + 0 ) << 24 ) | \
           ( *( (_x) + 1 ) << 16 ) | \
           ( *( (_x) + 2 ) <<  8 ) | \
           ( *( (_x) + 3 ) ) )

#define _RFC_SET_LONG( _x , _v ) \
	*( (_x) + 0 ) = ( unsigned char )( (_v) >> 24 ) , \
	*( (_x) + 1 ) = ( unsigned char )( (_v) >> 16 ) , \
	*( (_x) + 2 ) = ( unsigned char )( (_v) >>  8 ) , \
	*( (_x) + 3 ) = ( unsigned char )( (_v) )

#define _RFC_DCL_LONG( _x ) \
	unsigned char _x[ 4 ]

#define _RFC_GET_MONEY( _v , _x ) \
	_RFC_GET_LONG( _v.s_part.s_hi , _x ) , \
	_RFC_GET_LONG( _v.s_part.s_low , (_x) + 4 )

#define _RFC_SET_MONEY( _x , _v ) \
	_RFC_SET_LONG( _x , _v.s_part.s_hi ) , \
	_RFC_SET_LONG( (_x) + 4 , _v.s_part.s_low )

#define _RFC_DCL_MONEY( _x ) \
	unsigned char _x[ 8 ]

#define RFC_FREE( _m_ ) \
	if( _m_ != NULL ) free( _m_ ) , _m_ = NULL

/*#######################################################*/
/*#######################################################*/

/* stream money */
	typedef struct	{
			_RFC_DCL_MONEY( s_byte );
					}			RFC_MONEY_STREAM;

/*#######################################################*/
/*#######################################################*/

/* varargs handling */
#ifdef __cplusplus
#define RFC_VARARGS		...
#else
#define RFC_VARARGS
#endif

/*#######################################################*/
/*#######################################################*/
/* rfcctl control version */
#define	RFC_CTRL_VERSION		100

/*#######################################################*/
/*#######################################################*/

/* memory management */
#ifdef RFC_MEMCHECK

/* redefine allocation */
#define malloc		rfc_malloc
#define calloc		rfc_calloc
#define free		rfc_free
#define strdup		rfc_strdup
#define realloc		rfc_realloc

extern void *
	rfc_malloc( size_t p_count );
extern void *
	rfc_calloc( size_t p_num , size_t p_count );
extern void
	rfc_free( void *p_addr );
extern char *
	rfc_strdup( const char *p_src );
extern void *
	rfc_realloc( void *p_src , size_t p_count );
/* report inconsistancies */
extern void
	rfc_memcheck_report( void );
#endif

/*#######################################################*/
/*#######################################################*/
