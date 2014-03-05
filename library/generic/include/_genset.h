/*#######################################################*/
/*#######################################################*/

/* this header - set functions */
/* version 2.02.0001 */
#define RFC__GENSET_H

/*#######################################################*/
/*#######################################################*/
/* defined structures */

typedef struct _rfc_set				rfc_set;
typedef struct _rfc_setitem			rfc_setitem;

/*#######################################################*/
/*#######################################################*/
/* set */

/* element types */
#define RFC_SET_ONE					0x00	/* single entry */
#define RFC_SET_MINEXC				0x01	/* minimum entry of region (>) */
#define RFC_SET_MININC				0x02	/* minimum entry of region (>=) */
#define RFC_SET_MAXEXC				0x10	/* maximum entry of region (<) */
#define RFC_SET_MAXINC				0x20	/* maximum entry of region (<=) */

/* region types */
/* v1 < x < v2 */
#define RFC_SET_REGMINEXC_MAXEXC	(RFC_SET_MINEXC|RFC_SET_MAXEXC)
/* v1 < x <= v2 */
#define RFC_SET_REGMINEXC_MAXINC	(RFC_SET_MINEXC|RFC_SET_MAXINC)
/* v1 <= x < v2 */
#define RFC_SET_REGMININC_MAXEXC	(RFC_SET_MININC|RFC_SET_MAXEXC)
/* v1 <= x <= v2 */
#define RFC_SET_REGMININC_MAXINC	(RFC_SET_MININC|RFC_SET_MAXINC)

struct _rfc_set
{
	/* RFC_EXT_TYPE... value */
	int						s_type;		/* data type */

	rfc_setitem *			s_item;		/* item array ordered by key pointer */
	int						s_n;		/* item count */
	int						s_a;		/* item allocated */
	int						s_enum;		/* enumeration item */

	int						s_ref;		/* reference count */
};

/* set item struct */
struct _rfc_setitem
{
	RFC_TYPE				s_data;		/* RFC_EXT_TYPE... */
	short					s_regtype;	/* region type */
	short					s_mark;		/* item mark */

	int						s_unused;	/* unused space */
};

/*#######################################################*/
/*#######################################################*/
/* set functions */

/* create set */
extern rfc_set *
	rfc_set_create( int p_type );
/* create set as copy */
extern rfc_set *
	rfc_set_createcopy( rfc_set *p_set );
/* create set as union */
extern rfc_set *
	rfc_set_createunion( rfc_set *p_set1 , rfc_set *p_set2 );
/* create set as intersect */
extern rfc_set *
	rfc_set_createcross( rfc_set *p_set1 , rfc_set *p_set2 );
/* create set as union except intersect */
extern rfc_set *
	rfc_set_createouter( rfc_set *p_set1 , rfc_set *p_set2 );
/* create set as first except intersect */
extern rfc_set *
	rfc_set_createleft( rfc_set *p_set1 , rfc_set *p_set2 );
/* destroy set */
extern void
	rfc_set_destroy( rfc_set *p_set );
/* reference set */
extern void
	rfc_set_ref( rfc_set *p_set );

/* clear set without free memory */
extern void
	rfc_set_clear( rfc_set *p_set );
/* remove all items with free memory */
extern void
	rfc_set_init( rfc_set *p_set );
/* allocate item memory to given count */
extern void
	rfc_set_allocate( rfc_set *p_set , int p_count );

/* add item to set */
extern void
	rfc_set_add( rfc_set *p_set , const void *p_value );
extern void
	rfc_set_add_d( rfc_set *p_set , short p_value );
extern void
	rfc_set_add_l( rfc_set *p_set , int p_value );
extern void
	rfc_set_add_m( rfc_set *p_set , RFC_INT64 p_value );
extern void
	rfc_set_add_f( rfc_set *p_set , double p_value );
extern void
	rfc_set_add_s( rfc_set *p_set , const char *p_value );
extern void
	rfc_set_add_p( rfc_set *p_set , void *p_value );

/* add item region to set */
extern void
	rfc_set_addreg( rfc_set *p_set , int p_regtype , const void *p_vfrom , const void *p_vto );
extern void
	rfc_set_addreg_d( rfc_set *p_set , int p_regtype , short p_vfrom , short p_vto );
extern void
	rfc_set_addreg_l( rfc_set *p_set , int p_regtype , int p_vfrom , int p_vto );
extern void
	rfc_set_addreg_m( rfc_set *p_set , int p_regtype , RFC_INT64 p_vfrom , RFC_INT64 p_vto );
extern void
	rfc_set_addreg_f( rfc_set *p_set , int p_regtype , double p_vfrom , double p_vto );
extern void
	rfc_set_addreg_s( rfc_set *p_set , int p_regtype , const char *p_vfrom , const char *p_vto );
extern void
	rfc_set_addreg_p( rfc_set *p_set , int p_regtype , void *p_vfrom , void *p_vto );

/* add set */
extern void
	rfc_set_addset( rfc_set *p_set , rfc_set *p_value );

/* remove item from set */
extern void
	rfc_set_remove( rfc_set *p_set , const void *p_value );
extern void
	rfc_set_remove_d( rfc_set *p_set , short p_value );
extern void
	rfc_set_remove_l( rfc_set *p_set , int p_value );
extern void
	rfc_set_remove_m( rfc_set *p_set , RFC_INT64 p_value );
extern void
	rfc_set_remove_f( rfc_set *p_set , double p_value );
extern void
	rfc_set_remove_s( rfc_set *p_set , const char *p_value );
extern void
	rfc_set_remove_p( rfc_set *p_set , void *p_value );

/* remove item region from set */
extern void
	rfc_set_removereg( rfc_set *p_set , int p_regtype , const void *p_vfrom , const void *p_vto );
extern void
	rfc_set_removereg_d( rfc_set *p_set , int p_regtype , short p_vfrom , short p_vto );
extern void
	rfc_set_removereg_l( rfc_set *p_set , int p_regtype , int p_vfrom , int p_vto );
extern void
	rfc_set_removereg_m( rfc_set *p_set , int p_regtype , RFC_INT64 p_vfrom , RFC_INT64 p_vto );
extern void
	rfc_set_removereg_f( rfc_set *p_set , int p_regtype , double p_vfrom , double p_vto );
extern void
	rfc_set_removereg_s( rfc_set *p_set , int p_regtype , const char *p_vfrom , const char *p_vto );
extern void
	rfc_set_removereg_p( rfc_set *p_set , int p_regtype , void *p_vfrom , void *p_vto );

/* remove set */
extern void
	rfc_set_removeset( rfc_set *p_set , rfc_set *p_value );

/* get item count */
extern int
	rfc_set_itemcount( rfc_set *p_set );
/* get item by index */
extern const rfc_setitem *
	rfc_set_getitem( rfc_set *p_set , int p_pos );

/* check item in set */
extern short
	rfc_set_isin( rfc_set *p_set , const void *p_value );
extern short
	rfc_set_isin_d( rfc_set *p_set , short p_value );
extern short
	rfc_set_isin_l( rfc_set *p_set , int p_value );
extern short
	rfc_set_isin_m( rfc_set *p_set , RFC_INT64 p_value );
extern short
	rfc_set_isin_f( rfc_set *p_set , double p_value );
extern short
	rfc_set_isin_s( rfc_set *p_set , const char *p_value );
extern short
	rfc_set_isin_p( rfc_set *p_set , void *p_value );

/* check item region in set */
extern short
	rfc_set_isregin( rfc_set *p_set , int p_regtype , const void *p_vfrom , const void *p_vto , short p_partial );
extern short
	rfc_set_isregin_d( rfc_set *p_set , int p_regtype , short p_vfrom , short p_vto , short p_partial );
extern short
	rfc_set_isregin_l( rfc_set *p_set , int p_regtype , int p_vfrom , int p_vto , short p_partial );
extern short
	rfc_set_isregin_m( rfc_set *p_set , int p_regtype , RFC_INT64 p_vfrom , RFC_INT64 p_vto , short p_partial );
extern short
	rfc_set_isregin_f( rfc_set *p_set , int p_regtype , double p_vfrom , double p_vto , short p_partial );
extern short
	rfc_set_isregin_s( rfc_set *p_set , int p_regtype , const char *p_vfrom , const char *p_vto , short p_partial );
extern short
	rfc_set_isregin_p( rfc_set *p_set , int p_regtype , void *p_vfrom , void *p_vto , short p_partial );

/* check set in set */
extern short
	rfc_set_issetin( rfc_set *p_set , rfc_set *p_value , short p_partial );
/* check set is empty */
extern short
	rfc_set_isempty( rfc_set *p_set );

/* enumerate regions */
extern short
	rfc_set_enum( rfc_set *p_set , int *p_regtype , RFC_TYPE *p_from , RFC_TYPE *p_to );
/* end enumeration */
extern void
	rfc_set_endenum( rfc_set *p_set );
