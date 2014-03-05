/*#######################################################*/
/*#######################################################*/

/* this header - miscellaneous functions */
/* version 2.02.0001 */
#define RFC__GENMISC_H

/*#######################################################*/
/*#######################################################*/
/* defined structures */

typedef	struct _rfc_fmt_money	rfc_fmt_money;
typedef	struct _rfc_fmt_date	rfc_fmt_date;

/*#######################################################*/
/*#######################################################*/
/* datetime functions */

/* convert momemnt to string */
extern void
	rfc_dtm_moment2string ( rfc_moment p_mmt, const rfc_fmt_date * p_fmt , char * p_mmtstr );
/* convert time_t to string */
extern void 
	rfc_dtm_time2string( time_t p_time , const rfc_fmt_date * p_fmt , char * p_tmstr );
/* get moment for current time */
extern rfc_moment
	rfc_dtm_moment( short p_prec /* 0 - min set 95 (end of day ); 1 - current 15`min ; other - set 0 (start of day) */);

/* get default format */
extern void
	rfc_dtm_get_deformat( rfc_fmt_date * p_fmt );

/* make moment from string */
extern rfc_moment
	rfc_dtm_format_get( const char *p_date );
extern int
	rfc_dtm_get_days( rfc_moment p_moment );
/* get moment from days since 01.01.1900 */
extern rfc_moment
	rfc_dtm_get_moment( int p_days );
/* get day of the week (monday=0) */
extern int
	rfc_dtm_get_weekday( rfc_moment p_moment );
/* add days to moment */
extern rfc_moment
	rfc_dtm_add_days( rfc_moment p_moment , int p_days );

/*#######################################################*/
/*#######################################################*/
/* blob functions */

extern void
	rfc_blob_alloc( rfc_blob *p_blob , int p_size );
extern void
	rfc_blob_reserve( rfc_blob *p_blob , int p_size );
extern void
	rfc_blob_realloc( rfc_blob *p_blob , int p_size_add );
extern void
	rfc_blob_empty( rfc_blob *p_blob );
extern void
	rfc_blob_copy( rfc_blob *p_dest , const rfc_blob *p_source );
extern int
	rfc_blob_size( const rfc_blob *p_blob );
extern int
	rfc_blob_add( rfc_blob *p_blob , int p_type , const RFC_TYPE *p_data );
extern int
	rfc_blob_collect( rfc_blob *p_blob , rfc_blob *p_data );
extern int
	rfc_blob_extract( rfc_blob *p_blob , int p_type , RFC_TYPE *p_data , int *p_offset );
extern int
	rfc_blob_tail( rfc_blob *p_blob , rfc_blob *p_dest , int *p_offset );
extern void
	rfc_blob_free( rfc_blob *p_blob );

/*#######################################################*/
/*#######################################################*/
/* money functions */
/* double to money (int64) convertion */
extern RFC_INT64 
	rfc_money_double2money( double p_val );
/* money (int64) to double convertion */
extern double 
	rfc_money_money2double( RFC_INT64 p_val );
/* money (int64) to string convertion. p_buf - destination buffer */
extern void
	rfc_money_money2string( RFC_INT64 p_val, const rfc_fmt_money * p_fmt , char * p_mstr );
/* string to money (int64) convertion. p_buf - destination buffer */
extern RFC_INT64
	rfc_money_string2money( const char *p_buf );
/* money multiply "p_first" and "p_second" */
extern RFC_INT64 
	rfc_money_mul( RFC_INT64 p_first , RFC_INT64 p_second );
/* money division "p_first" and "p_second" */
extern RFC_INT64 
	rfc_money_div( RFC_INT64 p_first , RFC_INT64 p_second );

/* get default format */
extern void
	rfc_money_get_deformat( rfc_fmt_money * p_fmt );

/*#######################################################*/
/*#######################################################*/

/* options for main function */ 
extern int
	rfc_opt_get( int p_argc , char **p_argv , char *p_valid_opts , char **p_param );
/* convert phys type to phys type */
extern short
	rfc_type_cnv( const void *p_vfrom , int p_typefrom , void *p_vto , int p_typeto , int *p_tolen , void *p_fmt );
/* set type value */
extern short
	rfc_type_set( int p_type , RFC_TYPE *p_data , const void *p_value , int *p_tolen );
/* compare type values */
extern int
	rfc_type_cmp( int p_type , const void *p_v1 , const void *p_v2 );
/* copy buffer string */
extern short
	rfc_copy_char( char *p_to , const char *p_from , int *p_tolen );

/*#######################################################*/
/*#######################################################*/
/* quick sort with user data */

/* function for quick sort */
typedef int
	( *rfc_qsortcb )( void *p_userdata , const void *p_el1 , const void *p_el2 );

/* quick sort array with user data */
extern void
	rfc_qsort( void *p_userdata , void *p_base , unsigned p_num , unsigned p_width , rfc_qsortcb p_fcompare );

/*#######################################################*/
/*#######################################################*/
/* format defines */

#define RFC_FMT_NOT							0

#define RFC_FMT_DAY_SHORTDIG				1
#define RFC_FMT_DAY_LONGDIG					2
#define RFC_FMT_DAY_SHORTSTR				3
#define RFC_FMT_DAY_LONGSTR					4

#define RFC_FMT_MONTH_SHORTDIG				1
#define RFC_FMT_MONTH_LONGDIG				2
#define RFC_FMT_MONTH_SHORTSTR				3
#define RFC_FMT_MONTH_LONGSTR				4

#define RFC_FMT_YEAR_SHORTDIG				2
#define RFC_FMT_YEAR_LONGDIG				4

#define RFC_FMT_TIME_SHORTDIG				4
#define RFC_FMT_TIME_LONGDIG				6

#define RFC_FMT_MONEY_NEGNORM				0
#define RFC_FMT_MONEY_NEGCOLORUNSIGNED		1
#define RFC_FMT_MONEY_NEGCOLORSIGNED		2

#define RFC_FMT_MONEY_ROUNDTRUNC			0
#define RFC_FMT_MONEY_ROUNDNORM				1

struct _rfc_fmt_money 
{
	struct _fmt
		{
			char			s_chdiv[ 3 + 1 ];
			short			s_ndig;
		}	s_prec ,
			s_group;
	short	s_neg_fmt;
	short	s_rndtype;
};

struct _rfc_fmt_date
{
	struct _fmt_date_el
		{
			char			s_type;
			unsigned char	s_fmt;
		}	s_el[ 4 ];
	char	s_ddiv[ 3 + 1 ];
	char	s_tdiv[ 3 + 1 ];
};

/*#######################################################*/
/*#######################################################*/

/* MIN-MAX DATE */
#define	RFC_MOMENT_MIN	0
#define	RFC_MOMENT_MAX	2100123195

#ifndef max
#define max(a,b) (((a)>(b))?(a):(b))
#endif

#ifndef min
#define min(a,b) (((a)<(b))?(a):(b))
#endif

/*#######################################################*/
/*#######################################################*/
