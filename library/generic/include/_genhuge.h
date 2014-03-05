/*#######################################################*/
/*#######################################################*/

/* this header - huge array functions */
/* version 2.02.0001 */
#define RFC__GENHUGE_H

/*#######################################################*/
/*#######################################################*/
/* defined structures */

typedef struct _rfc_huge		rfc_huge;
typedef struct _rfc_hugeel		rfc_hugeel;

typedef int
	( *rfc_hm_compare )( void *p_userdata , const rfc_hugeel *p_e1 , const rfc_hugeel *p_e2 );
typedef void
	( *rfc_hm_delete )( void *p_struct, void *p_data );

/*#######################################################*/
/*#######################################################*/
/* huge map array */

struct _rfc_huge
{
	/* index to rfc_hugeel */
	rfc_hugeel *			s_map;		/* first level array */
	int						s_n;		/* data count */
	int						s_a;		/* allocated count */
	/* RFC_EXT_TYPE... value */
	int						s_type;		/* data type */
	rfc_huge *				s_parent;	/* parent array */
	int						s_key;		/* index in parent */
	rfc_hm_delete			s_fdelete;	/* delete callback */

	void *					s_userdata; /* user data: using in context */
	int						s_ref;		/* reference count */
};

/* huge map element struct */
struct _rfc_hugeel
{
	union
		{
			RFC_TYPE	u_v;		/* RFC_EXT_TYPE... value */
			rfc_huge *
						u_h;		/* embedded subarray */
		}					s_data;		/* element data */
	int						s_key;		/* array index */
	short					s_sub;		/* 0 - value , 1 - subarray */
};

/*#######################################################*/
/*#######################################################*/
/* huge map functions */

/* create huge map array */
extern rfc_huge *
	rfc_hm_create( int p_datatype /* RFC_EXT_TYPE... value */, rfc_hm_delete f_delete );
/* destroy huge map array */
extern void
	rfc_hm_destroy( rfc_huge *p_hm );
/* reference huge map array */
extern void
	rfc_hm_ref( rfc_huge *p_hm );

/* remove all array allocations */
extern void
	rfc_hm_init( rfc_huge *p_hm );
/* remove all array items without free memory */
extern void
	rfc_hm_clear( rfc_huge *p_hm );
/* allocate array items */
extern void
	rfc_hm_allocate( rfc_huge *p_hm , int p_count );
/* remove key holes on current level */
extern void
	rfc_hm_squeeze( rfc_huge *p_hm );
/* remove key holes recursive */
extern void
	rfc_hm_squeezeall( rfc_huge *p_hm );
/* remove key */
extern void
	rfc_hm_remove( rfc_huge *p_hm , int p_idx );

/* set current level array element */
extern void
	rfc_hm_set( rfc_huge *p_hm , int p_idx , const void *p_value );
extern void
	rfc_hm_set_d( rfc_huge *p_hm , int p_idx , short p_value );
extern void
	rfc_hm_set_l( rfc_huge *p_hm , int p_idx , int p_value );
extern void
	rfc_hm_set_m( rfc_huge *p_hm , int p_idx , RFC_INT64 p_value );
extern void
	rfc_hm_set_f( rfc_huge *p_hm , int p_idx , double p_value );
extern void
	rfc_hm_set_s( rfc_huge *p_hm , int p_idx , const char *p_value );
extern void
	rfc_hm_set_p( rfc_huge *p_hm , int p_idx , void *p_value );

/* get current level subarray */
extern rfc_huge *
	rfc_hm_getsub( rfc_huge *p_hm , int p_idx );

/* get current level array element */
extern const rfc_hugeel *
	rfc_hm_get( rfc_huge *p_hm , int p_idx );
extern short
	rfc_hm_get_d( rfc_huge *p_hm , int p_idx );
extern int
	rfc_hm_get_l( rfc_huge *p_hm , int p_idx );
extern RFC_INT64
	rfc_hm_get_m( rfc_huge *p_hm , int p_idx );
extern double
	rfc_hm_get_f( rfc_huge *p_hm , int p_idx );
extern const char *
	rfc_hm_get_s( rfc_huge *p_hm , int p_idx );
extern void *
	rfc_hm_get_p( rfc_huge *p_hm , int p_idx );
extern rfc_huge *
	rfc_hm_get_h( rfc_huge *p_hm , int p_idx );

/* set any level array element by index parameters */
extern void
	rfc_hm_mset( rfc_huge *p_hm , int p_level , const void *p_value , ... /*int p_idx1 , ... , int p_idx<p_level> */ );
/* set any level array element by va_list */
extern void
	rfc_hm_mset_va( rfc_huge *p_hm , int p_level , const void *p_value , va_list p_idx );
/* set any level array element by index array */
extern void
	rfc_hm_mset_a( rfc_huge *p_hm , int p_level , const void *p_value , int *p_idx );
/* get any level array element by index parameters */
extern const rfc_hugeel *
	rfc_hm_mget( rfc_huge *p_hm , int p_level , ... /*int p_idx1 , ... , int p_idx<p_level> */ );
/* get any level array element by va_list */
extern const rfc_hugeel *
	rfc_hm_mget_va( rfc_huge *p_hm , int p_level , va_list p_idx );
/* get any level array element by index array */
extern const rfc_hugeel *
	rfc_hm_mget_a( rfc_huge *p_hm , int p_level , int *p_idx );
/* get any level subarray by index parameters */
extern rfc_huge *
	rfc_hm_mgetsub( rfc_huge *p_hm , int p_level , ... /*int p_idx1 , ... , int p_idx<p_level> */ );
/* get any level subarray by va_list */
extern rfc_huge *
	rfc_hm_mgetsub_va( rfc_huge *p_hm , int p_level , va_list p_idx );
/* get any level subarray by index array */
extern rfc_huge *
	rfc_hm_mgetsub_a( rfc_huge *p_hm , int p_level , int *p_idx );

/* sort elements on given level by value */
extern void
	rfc_hm_sort( rfc_huge *p_hm , void *p_userdata , rfc_hm_compare f_compare );
/* sort elements recursive by value */
extern void
	rfc_hm_sortall( rfc_huge *p_hm , void *p_userdata , rfc_hm_compare f_compare );
