/*#######################################################*/
/*#######################################################*/

/* this header - helper interface */
/* version 2.02.0001 */
#define RFC__GENMAP_H

/*#######################################################*/
/*#######################################################*/
/* defined structures */

/* pointer map */
typedef struct _rfc_ptrmap		rfc_ptrmap;
/* string map */
typedef struct _rfc_strmap		rfc_strmap;
/* complex key map */
typedef struct _rfc_map			rfc_map;

/* pointer map entry */
typedef struct _rfc_ptrmapentry	rfc_ptrmapentry;
/* string map entry */
typedef struct _rfc_strmapentry	rfc_strmapentry;
/* complex key map entry */
typedef struct _rfc_mapentry	rfc_mapentry;

/*#######################################################*/
/*#######################################################*/
/* pointer map */

/* array */
struct _rfc_ptrmap
{
	rfc_ptrmapentry *	
					s_p;		/* pointer array */
	int				s_n;		/* pointer count */
	int				s_a;		/* pointers allocated */
	int				s_ref;		/* reference count */
};

/* entry */
struct _rfc_ptrmapentry
{
	void *			s_x;		/* pointer */
	unsigned long	s_y;		/* pointer data */
};

/* array operations */
extern rfc_ptrmap *
	rfc_map_ptrcreate( void );
extern void
	rfc_map_ptralloc( rfc_ptrmap *p_p , int p_a );
extern int
	rfc_map_ptrcheck( rfc_ptrmap *p_p , void *p_d , unsigned long *p_data );
extern int
	rfc_map_ptradd( rfc_ptrmap *p_p , void *p_d , unsigned long p_data );
extern unsigned long
	rfc_map_ptrremove( rfc_ptrmap *p_p , void *p_d );
extern unsigned long
	rfc_map_ptrremovepos( rfc_ptrmap *p_p , int p_pos );
extern void
	rfc_map_ptrclear( rfc_ptrmap *p_p );
extern void
	rfc_map_ptrdrop( rfc_ptrmap *p_p );
extern void
	rfc_map_ptrinit( rfc_ptrmap *p_p );
extern void
	rfc_map_ptrref( rfc_ptrmap *p_p );
extern int
	rfc_map_ptrinsertpos( rfc_ptrmap *p_p , void *ptr );

/* item count */
extern int
	rfc_map_ptrcount( rfc_ptrmap *p_map );
/* get by position */
extern unsigned long
	rfc_map_ptrget( rfc_ptrmap *p_map , int p_pos );
/* set item by position */
extern short
	rfc_map_ptrsetpos( rfc_ptrmap *p_map , int p_pos , unsigned long p_value );
/* add/set item by key */
extern int
	rfc_map_ptrsetkey( rfc_ptrmap *p_map , void *p_d , unsigned long p_data );

/*#######################################################*/
/*#######################################################*/
/* string & pointer map */

/* array */
struct _rfc_strmap
{
	rfc_strmapentry *
					s_p;		/* pointer array */
	int				s_n;		/* pointer count */
	int				s_a;		/* pointers allocated */
	int				s_ref;		/* reference count */
};

/* entry */
struct _rfc_strmapentry
{
	char *			s_x;		/* string */
	void *			s_y;		/* pointer data */
};

extern rfc_strmap *
	rfc_map_strcreate( void );
extern void
	rfc_map_stralloc( rfc_strmap *p_p , int p_a );
extern int
	rfc_map_strcheck( rfc_strmap *p_p , const char *p_d , void **p_data );
extern int
	rfc_map_stradd( rfc_strmap *p_p , const char *p_d , void *p_data );
extern void *
	rfc_map_strremove( rfc_strmap *p_p , const char *p_d );
extern void *
	rfc_map_strremovepos( rfc_strmap *p_p , int p_pos );
extern void
	rfc_map_strclear( rfc_strmap *p_p );
extern void
	rfc_map_strdrop( rfc_strmap *p_p );
extern void
	rfc_map_strinit( rfc_strmap *p_p );
extern void
	rfc_map_strref( rfc_strmap *p_p );
extern int
	rfc_map_strinsertpos( rfc_strmap *p_p , const char *ptr );

/* item count */
extern int
	rfc_map_strcount( rfc_strmap *p_map );
/* get by position */
extern void *
	rfc_map_strget( rfc_strmap *p_map , int p_pos );
/* set item by position */
extern short
	rfc_map_strsetpos( rfc_strmap *p_map , int p_pos , void *p_value );
/* add/set item by key */
extern int
	rfc_map_strsetkey( rfc_strmap *p_map , const char *p_d , void *p_data );

/*#######################################################*/
/*#######################################################*/
/* complex key map */

#define RFC_MAP_MAXKEYS			256

/* complex key map entry */
struct _rfc_mapentry
{
	RFC_TYPE		s_value;		/* map data */
	unsigned int	s_size;			/* total element size */
	union
		{
			const char *	u_s;
			int				u_l;
		}			s_key[ RFC_MAP_MAXKEYS ];
									/* map key values */
	/* key data and RFC_EXT_TYPESTRING data if any */
};

/* complex key map */
struct _rfc_map
{
	int				s_type;			/* RFC_EXT_TYPE... value */
	rfc_mapentry **	s_p;			/* map elements */
	int				s_a;			/* element allocated (ptr) */
	int				s_n;			/* element stored (ptr) */
	int				s_axis_n;		/* axis count */
	short *			s_axis_isnum;	/* axis type array */
	int				s_ref;			/* reference count */
};

/* create map */
extern rfc_map * 
	rfc_map_create( int p_type , int p_keys , ... /*short p_isnum1 , ... , short p_isnum<p_keys> */ );
extern rfc_map * 
	rfc_map_create_a( int p_type , int p_keys , const short *p_isnum );
extern rfc_map * 
	rfc_map_create_va( int p_type , int p_keys , va_list p_isnum );
/* destroy map */
extern void
	rfc_map_drop( rfc_map *p_map );
/* reference map */
extern void
	rfc_map_ref( rfc_map *p_map );

/* clear map without free memory */
extern void
	rfc_map_clear( rfc_map *p_map );
/* remove all items with free memory */
extern void
	rfc_map_init( rfc_map *p_map );
/* allocate item memory to given count */
extern void
	rfc_map_allocate( rfc_map *p_map , int p_count );

/* add item to map */
extern int
	rfc_map_add( rfc_map *p_map , const RFC_TYPE *p_value , ... /*{const char *|int} p_keyval1 , ... , {const char *|int} p_keyval<s_keyel_n> */ );
extern int
	rfc_map_add_a( rfc_map *p_map , const RFC_TYPE *p_value , const void **p_key /* {const char *|int}[] */ );
extern int
	rfc_map_add_va( rfc_map *p_map , const RFC_TYPE *p_value , va_list p_key );

/* find item data */
extern int
	rfc_map_check( rfc_map *p_map , RFC_TYPE *p_value , ... /*{const char *|int} p_keyval1 , ... , {const char *|int} p_keyval<s_keyel_n> */ );
extern int
	rfc_map_check_a( rfc_map *p_map , RFC_TYPE *p_value , const void **p_key /* {const char *|int}[] */ );
extern int
	rfc_map_check_va( rfc_map *p_map , RFC_TYPE *p_value , va_list p_key );

/* remove item by position */
extern short
	rfc_map_remove( rfc_map *p_map , int p_pos , RFC_TYPE *p_value );
/* set item by position */
extern short
	rfc_map_set( rfc_map *p_map , int p_pos , RFC_TYPE *p_value );

/* item count */
extern int
	rfc_map_count( rfc_map *p_map );
/* get by position */
extern const rfc_mapentry *
	rfc_map_get( rfc_map *p_map , int p_pos );

/*#######################################################*/
/*#######################################################*/

