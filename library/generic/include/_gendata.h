/*#######################################################*/
/*#######################################################*/

/* this header - helper interface */
/* version 2.02.0001 */
#define RFC__GENDATA_H

/*#######################################################*/
/*#######################################################*/
/* defined structures */

/* dynamic indexed array */
typedef struct _rfc_array		rfc_array;
/* dynamic array of not null ints */
typedef struct _rfc_intarray	rfc_intarray;
/* simple data list */
typedef struct _rfc_list		rfc_list;

/* index for array */
typedef struct _rfc_index		rfc_index;

/* sort function */
typedef int
	( *rfc_lst_compare )( void *p_userdata , const RFC_TYPE *p_e1 , const RFC_TYPE *p_e2 );

/*#######################################################*/
/*#######################################################*/
/* general indexed array class */

/* maximum indexes per array */
#define RFC_ARRAY_MAX_INDEX		10
/* align count of items for reallocate */
#define RFC_ARRAY_BLOCK			10

/*#######################################################*/
/*#######################################################*/

/* array data */
struct _rfc_array
{
	void **		s_p;		/* pointer array */
	int			s_n;		/* item count */
	int			s_d;		/* nulls count */
	int			s_a;		/* items allocated */
	int			s_actual;	/* items indexed */
	int			s_i_n;		/* index count */
	rfc_index * s_i[ RFC_ARRAY_MAX_INDEX ];	/* index array */

	int			s_ref;		/* reference count */
};

/* data */
struct _rfc_index
{
	char		s_type;		/* index type (Void, String, Long) */
	int			s_offset;	/* offset in data */
	int			s_size;		/* key size */
	int *		s_data;		/* position by index */
};

/* array functions */
/* construction */
extern rfc_array *		
	rfc_arr_create( int p_indexes );
extern void				
	rfc_arr_destroy( rfc_array *p_arr );
extern void				
	rfc_arr_ref( rfc_array *p_arr );

/* operations */
/* set index to array ('V'oid,'S'tring,'L'ong,'P'ointer to string,'G'eneric pointer) */
extern void
	rfc_arr_index_set( rfc_array *p_arr , int p_index , int p_offset , int p_size , char p_type );
/* add index to array */
extern int
	rfc_arr_index_add( rfc_array *p_arr , int p_offset , int p_size , char p_type );
/* remove index from array */
extern void
	rfc_arr_index_remove( rfc_array *p_arr , int p_index );

/* add data to array */
extern int
	rfc_arr_add( rfc_array *p_arr , void *p_data );
/* set data to array */
extern int
	rfc_arr_set( rfc_array *p_arr , int p_pos , void *p_data );
/* remove by position */
extern void
	rfc_arr_remove( rfc_array *p_arr , int p_pos );
/* remove item from array only */
extern void
	rfc_arr_remove_item( rfc_array *p_arr , int p_pos );
/* remove item by pointer */
extern int
	rfc_arr_remove_data( rfc_array *p_arr , void *p_data );
/* remove item & shift positions */
extern void
	rfc_arr_remove_shift( rfc_array *p_arr , int p_pos );
/* insert item & shift positions */
extern void
	rfc_arr_add_shift( rfc_array *p_arr , int p_pos , void *p_data );

/* clear array without free memory */
extern void
	rfc_arr_remove_all( rfc_array *p_arr );
/* clear array without reallocate memory */
extern void
	rfc_arr_clear( rfc_array *p_arr );
/* clear array with free intenal memory without destroy struct */
extern void
	rfc_arr_init( rfc_array *p_arr );
/* allocate array memory to given count */
extern void
	rfc_arr_allocate( rfc_array *p_arr , int p_count );
/* remove item by position with free memory */
extern void
	rfc_arr_free_pos( rfc_array *p_arr , int p_pos );
/* clear array with free memory */
extern void
	rfc_arr_free_all( rfc_array *p_arr );

/* find data by index */
extern void *
	rfc_arr_find( rfc_array *p_arr , int p_index , const void *p_data );
/* find position by index */
extern int
	rfc_arr_find_id( rfc_array *p_arr , int p_index , const void *p_data );
/* find index position */
extern int
	rfc_arr_find_index( rfc_array *p_arr , int p_index , const void *p_data );
/* find index position range - return match count */
extern int
	rfc_arr_find_range( rfc_array *p_arr , int p_index , const void *p_data , int *p_idx_start , int *p_idx_stop );
/* find index position subkey range - return match count */
extern int
	rfc_arr_find_subrange( rfc_array *p_arr , int p_index , const void *p_data , int *p_idx_start , int *p_idx_stop , int p_size );
/* find first index position for item data >= p_data by sort order */
extern int
	rfc_arr_find_first( rfc_array *p_arr , int p_index , const void *p_data );
/* find last index position for item data <= p_data by sort order */
extern int
	rfc_arr_find_last( rfc_array *p_arr , int p_index , const void *p_data );

/* remove data from indexes */
extern void
	rfc_arr_remove_indexes( rfc_array *p_arr , int p_pos );
/* reappend data to indexes */
extern void
	rfc_arr_reindex( rfc_array *p_arr , int p_pos );

/* return number of non-deleted items */
extern int
	rfc_arr_count( rfc_array *p_arr );
/* return max number of items */
extern int
	rfc_arr_margin( rfc_array *p_arr );
/* return number of indexed items */
extern int
	rfc_arr_actual( rfc_array *p_arr );
/* get item by position */
extern void *
	rfc_arr_get( rfc_array *p_arr , int p_pos );
/* get array position by index position */
extern int
	rfc_arr_get_index( rfc_array *p_arr , int p_index , int p_pos );

/*#######################################################*/
/*#######################################################*/
/* int map */

/* array */
struct _rfc_intarray
{
	int *		s_p;		/* int array */
	int			s_n;		/* item count */
	int			s_d;		/* nulls count */
	int			s_a;		/* items allocated */
	int *		s_idx;		/* position by index */

	int			s_ref;		/* reference count */
};

/* construction */
extern rfc_intarray *
	rfc_arr_int_create( void );
extern void
	rfc_arr_int_drop( rfc_intarray *p_arr );
extern void
	rfc_arr_int_ref( rfc_intarray *p_arr );

/* operations */
/* create index (avoid MULTI-THREAD contention ) */
extern void
	rfc_arr_int_setindex( rfc_intarray *p_arr );
/* drop index */
extern void
	rfc_arr_int_clearindex( rfc_intarray *p_arr );

/* add data to array */
extern int
	rfc_arr_int_add( rfc_intarray *p_arr , int p_data );
/* set data to array */
extern int
	rfc_arr_int_set( rfc_intarray *p_arr , int p_pos , int p_data );
/* remove by position */
extern void
	rfc_arr_int_remove( rfc_intarray *p_arr , int p_pos );
/* remove item by data */
extern int
	rfc_arr_int_remove_data( rfc_intarray *p_arr , int p_data );
/* remove item & shift positions */
extern void
	rfc_arr_int_remove_shift( rfc_intarray *p_arr , int p_pos );
/* insert item & shift positions */
extern void
	rfc_arr_int_add_shift( rfc_intarray *p_arr , int p_pos , int p_data );

/* clear array without free memory */
extern void
	rfc_arr_int_remove_all( rfc_intarray *p_arr );
/* clear array without reallocate memory */
extern void
	rfc_arr_int_clear( rfc_intarray *p_arr );
/* clear array with free intenal memory without destroy struct */
extern void
	rfc_arr_int_init( rfc_intarray *p_arr );
/* allocate array memory to given count */
extern void
	rfc_arr_int_allocate( rfc_intarray *p_arr , int p_count );

/* find position by index */
extern int
	rfc_arr_int_find_id( rfc_intarray *p_arr , int p_data );
/* find index position */
extern int
	rfc_arr_int_find_index( rfc_intarray *p_arr , int p_data );
/* find index position range - return match count */
extern int
	rfc_arr_int_find_range( rfc_intarray *p_arr , int p_data , int *p_idx_start , int *p_idx_stop );
/* find first index position for item data >= p_data by sort order */
extern int
	rfc_arr_int_find_first( rfc_intarray *p_arr , int p_data );
/* find last index position for item data <= p_data by sort order */
extern int
	rfc_arr_int_find_last( rfc_intarray *p_arr , int p_data );

/* return number of non-deleted items */
extern int
	rfc_arr_int_count( rfc_intarray *p_arr );
/* return max number of items */
extern int
	rfc_arr_int_margin( rfc_intarray *p_arr );
/* get item by position */
extern int
	rfc_arr_int_get( rfc_intarray *p_arr , int p_pos );
/* get array position by index position */
extern int
	rfc_arr_int_get_index( rfc_intarray *p_arr , int p_pos );

/*#######################################################*/
/*#######################################################*/
/* simple data list */

struct _rfc_list
{
	int			s_type;		/* RFC_EXT_TYPE... value */
	RFC_TYPE *	s_p;		/* element array */
	int			s_n;		/* item count */
	int			s_a;		/* items allocated */

	int			s_ref;		/* reference count */
};

/* create list */
extern rfc_list *
	rfc_lst_create( int p_type );
/* destroy list */
extern void
	rfc_lst_destroy( rfc_list *p_lst );
/* reference list */
extern void
	rfc_lst_ref( rfc_list *p_lst );

/* clear array without reallocate memory */
extern void
	rfc_lst_clear( rfc_list *p_lst );
/* clear list with free intenal memory without destroy struct */
extern void
	rfc_lst_init( rfc_list *p_lst );
/* allocate list memory to given count */
extern void
	rfc_lst_allocate( rfc_list *p_lst , int p_count );

/* get count */
extern int
	rfc_lst_count( rfc_list *p_lst );
/* append item to end of list - returns index */
extern int
	rfc_lst_add( rfc_list *p_lst , RFC_TYPE *p_data );
/* remove item by position */
extern void
	rfc_lst_remove( rfc_list *p_lst , int p_index );
/* get item by position */
extern RFC_TYPE *
	rfc_lst_get( rfc_list *p_lst , int p_index );
/* set given list item data */
extern short
	rfc_lst_replace( rfc_list *p_lst , int p_index , RFC_TYPE *p_data );
/* sort list elements */
extern short
	rfc_lst_sort( rfc_list *p_lst , void *p_userdata , rfc_lst_compare f_compare );

/*#######################################################*/
/*#######################################################*/
