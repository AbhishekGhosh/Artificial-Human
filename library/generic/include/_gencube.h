/*#######################################################*/
/*#######################################################*/

/* this header - cube functions */
/* version 2.02.0001 */
#define RFC__GENCUBE_H

/*#######################################################*/
/*#######################################################*/
/* defined structures */

typedef struct _rfc_cube		rfc_cube;
typedef struct _rfc_cubeitem	rfc_cubeitem;
typedef struct _rfc_cubekeyhead	rfc_cubekeyhead;
typedef struct _rfc_cubekey		rfc_cubekey;
typedef struct _rfc_cubekeyel	rfc_cubekeyel;

#define RFC_CB_MAXKEYELS		256

/*#######################################################*/
/*#######################################################*/
/* cube */

struct _rfc_cube
{
	/* RFC_EXT_TYPE... value */
	int						s_type;		/* data type */

	rfc_cubeitem *			s_item;		/* item array ordered by key pointer */
	int						s_item_n;	/* item count */
	int						s_item_a;	/* item allocated */

	rfc_ptrmap				s_ext;		/* ext to s_item index */

	rfc_cubekey **			s_key;		/* sorted key array */
	int						s_key_n;	/* keys used */
	int						s_key_a;	/* keys allocated */

	rfc_cubekeyel *			s_keyel;	/* key elements */
	int						s_keyel_n;	/* key element count */

	rfc_cubekey **			s_walk;		/* current walk keys (sorted by key ptr) */
	int						s_walk_el;	/* current walk element */
	int						s_walk_n;	/* current walk elements */
	int						s_walk_enumdist;
										/* key for distribute enumeration */
	int						s_walk_enumkey;
										/* key for item enumeration */
	int						s_walk_enumitem;
										/* item enumeration */

	int						s_ref;		/* reference count */
};

/* cube item struct */
struct _rfc_cubeitem
{
	RFC_TYPE				s_data;		/* RFC_EXT_TYPE... value */
	rfc_cubekey *			s_key;		/* element key */
	void *					s_ext;		/* external ID */
};

/* cube key head data */
struct _rfc_cubekeyhead
{
	int						s_ref;		/* reference count */
	short					s_mark;		/* reference mark */
	short					s_unused;
};

/* full cube key data */
struct _rfc_cubekey
{
	rfc_cubekeyhead			s_head;		/* key head */
	union
		{
			int			u_l;		/* numeric element */
			const char *u_s;		/* string element */
		}					s_els[ RFC_CB_MAXKEYELS ];
										/* key data pointers */
										/* string key data */
};

/* key element */
struct _rfc_cubekeyel
{
	rfc_cubekey **			s_sort;		/* key sort order */
	short					s_isnum;	/* numeric or string key element */
};

/*#######################################################*/
/*#######################################################*/
/* cube functions */

/* create cube */
extern rfc_cube *
	rfc_cb_create( int p_type , int p_keyels , ... /*, short p_isnum<p_keyels> */ );
extern rfc_cube *
	rfc_cb_create_a( int p_type , int p_keyels , const short *p_isnum );
extern rfc_cube *
	rfc_cb_create_va( int p_type , int p_keyels , va_list p_isnum );
/* destroy cube */
extern void
	rfc_cb_destroy( rfc_cube *p_cb );
/* reference cube */
extern void
	rfc_cb_ref( rfc_cube *p_cb );

/* clear cube without free memory */
extern void
	rfc_cb_clear( rfc_cube *p_cb );
/* remove all items with free memory */
extern void
	rfc_cb_init( rfc_cube *p_cb );
/* allocate item memory to given count */
extern void
	rfc_cb_allocate( rfc_cube *p_cb , int p_count );

/* add item to cube */
extern void
	rfc_cb_add( rfc_cube *p_cb , void *p_ext , const void *p_value , ... /*, {const char *|int} p_keyval1 , ... , {const char *|int} p_keyval<s_keyel_n> */ );
extern void
	rfc_cb_add_d( rfc_cube *p_cb , void *p_ext , short p_value , ... /*, {const char *|int} p_keyval1 , ... , {const char *|int} p_keyval<s_keyel_n> */ );
extern void
	rfc_cb_add_l( rfc_cube *p_cb , void *p_ext , int p_value , ... /*, {const char *|int} p_keyval1 , ... , {const char *|int} p_keyval<s_keyel_n> */ );
extern void
	rfc_cb_add_m( rfc_cube *p_cb , void *p_ext , RFC_INT64 p_value , ... /*, {const char *|int} p_keyval1 , ... , {const char *|int} p_keyval<s_keyel_n> */ );
extern void
	rfc_cb_add_f( rfc_cube *p_cb , void *p_ext , double p_value , ... /*, {const char *|int} p_keyval1 , ... , {const char *|int} p_keyval<s_keyel_n> */ );
extern void
	rfc_cb_add_s( rfc_cube *p_cb , void *p_ext , const char *p_value , ... /*, {const char *|int} p_keyval1 , ... , {const char *|int} p_keyval<s_keyel_n> */ );
extern void
	rfc_cb_add_p( rfc_cube *p_cb , void *p_ext , void *p_value , ... /*, {const char *|int} p_keyval1 , ... , {const char *|int} p_keyval<s_keyel_n> */ );

extern void
	rfc_cb_add_a( rfc_cube *p_cb , void *p_ext , const void *p_value , const void **p_keyval );
extern void
	rfc_cb_add_va( rfc_cube *p_cb , void *p_ext , const void *p_value , va_list p_va );

/* remove item from cube */
extern void
	rfc_cb_remove( rfc_cube *p_cb , void *p_ext );

/* get item count */
extern int
	rfc_cb_itemcount( rfc_cube *p_cb );
/* get item by index */
extern const rfc_cubeitem *
	rfc_cb_getitem( rfc_cube *p_cb , int p_pos );

/* walk from current to key element - return distribute count */
extern int
	rfc_cb_walk( rfc_cube *p_cb , int p_el );
/* from current filter walk - return key count */
extern int
	rfc_cb_walkto_l( rfc_cube *p_cb , int p_keyval );
extern int
	rfc_cb_walkto_s( rfc_cube *p_cb , const char *p_keyval );

/* get current walk distribution next key element value - numeric */
extern short
	rfc_cb_distenum_l( rfc_cube *p_cb , int *p_vptr );
/* get current walk distribution next key element value - string */
extern short
	rfc_cb_distenum_s( rfc_cube *p_cb , const char **p_vptr );
/* from current walk get distribute count by given axis */
extern int
	rfc_cb_distcount( rfc_cube *p_cb , int p_el );
/* end enum distribution */
extern void
	rfc_cb_enddist( rfc_cube *p_cb );

/* from current walk get key count */
extern int
	rfc_cb_walkkeycount( rfc_cube *p_cb );
/* get current walk key element */
extern const rfc_cubekey *
	rfc_cb_walkkey( rfc_cube *p_cb , int p_index );
/* get key items */
extern int
	rfc_cb_keyitems( rfc_cube *p_cb , const rfc_cubekey *p_key , int *p_from , int *p_to );

/* walk from current with filter by given axis - return key count */
extern int
	rfc_cb_fwalk_l( rfc_cube *p_cb , int p_keyel , int p_keyval );
extern int
	rfc_cb_fwalk_s( rfc_cube *p_cb , int p_keyel , const char *p_keyval );

/* walk from root with filter by all axes */
extern int
	rfc_cb_mfwalk( rfc_cube *p_cb , ... /*{int|const char *} p_keyval1 , ... , int p_el<n> , {int|const char *} p_keyval<n> , -1 */ );
extern int
	rfc_cb_mfwalk_a( rfc_cube *p_cb , int *p_el , const void **p_keyval );
extern int
	rfc_cb_mfwalk_va( rfc_cube *p_cb , va_list p_keyval );

/* scan current walk subtree items - returns NULL on end */
extern const rfc_cubeitem *
	rfc_cb_scan( rfc_cube *p_cb );
/* restart scan */
extern void
	rfc_cb_endscan( rfc_cube *p_cb );

/* cancel walk */
extern void
	rfc_cb_endwalk( rfc_cube *p_cb );
