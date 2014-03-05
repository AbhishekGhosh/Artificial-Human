/*#######################################################*/
/*#######################################################*/
#define RFC__GENMSH_H

/*#######################################################*/
/*#######################################################*/
/* marshaling */

typedef struct _rfc_msh			rfc_msh;
typedef struct _rfc_msh_def		rfc_msh_def;
typedef struct _rfc_msh_itemdef	rfc_msh_itemdef;

typedef short					RFC_MSH_RES;

/*#######################################################*/
/*#######################################################*/

/* item values */
#define RFC_MSH_KIND_VALUE		1
#define RFC_MSH_KIND_VALUELIST	2
#define RFC_MSH_KIND_MSH		3

/* minimal marshaling memory size */
#define RFC_MSH_MINSIZE			504

/* get-functions result value */
#define RFC_MSH_RES_FAIL		0
#define RFC_MSH_RES_OK			1
#define RFC_MSH_RES_NODATA		2

/*#######################################################*/
/*#######################################################*/

/* marshaling item definition */
struct _rfc_msh_itemdef
{
	int				s_kind;		/* value, value list or embedded msh */
	const char *	s_name;		/* item name */
	int				s_type;		/* item type (if value) */
	int				s_offset;	/* offset for current row */
};

/* marshaling definitions */
struct _rfc_msh_def
{
	rfc_list		s_items;	/* item definitions [rfc_msh_itemdef] */
	int				s_nrows;	/* row count */
	int				s_readrow;	/* rows read */
	int				s_offset;	/* offset of next row */
	short			s_dataown;	/* data is allocated (0=referenced) */
};

/* marshaling object */
struct _rfc_msh
{
	rfc_msh_def		s_def;		/* definitions */
	rfc_blob		s_data;		/* data */
};

/*#######################################################*/
/*#######################################################*/

extern rfc_msh *
	rfc_msh_create( short p_bloballoc );
extern void
	rfc_msh_destroy( rfc_msh *p_msh );

/* set RPC data */
extern int
	rfc_msh_item_create( rfc_msh *p_msh , const char *p_name , int p_valtype );
extern rfc_msh_itemdef *
	rfc_msh_item_get( rfc_msh *p_msh , int p_pos );
extern int
	rfc_msh_item_count( rfc_msh *p_msh );

extern short
	rfc_msh_setitem_value( rfc_msh *p_msh , int p_pos , RFC_TYPE *p_val );
extern short
	rfc_msh_setitem_list( rfc_msh *p_msh , int p_pos , rfc_list *p_list );
extern short
	rfc_msh_setitem_rec( rfc_msh *p_msh , int p_pos , rfc_msh *p_rec );

extern short
	rfc_msh_writeblob( rfc_msh *p_msh , rfc_blob *p_blob , int p_head_size );
extern short
	rfc_msh_addrow( rfc_msh *p_msh );

/* get RPC data */
extern RFC_MSH_RES
	rfc_msh_getitem_value( rfc_msh *p_msh , int p_pos , int p_valtype , RFC_TYPE *p_val );
extern RFC_MSH_RES
	rfc_msh_getitem_list( rfc_msh *p_msh , int p_pos , rfc_list *p_list );
extern RFC_MSH_RES
	rfc_msh_getitem_rec( rfc_msh *p_msh , int p_pos , rfc_msh **p_rec );

extern short
	rfc_msh_readblob( rfc_msh *p_msh , rfc_blob *p_blob , int *p_offset );
extern short
	rfc_msh_nextrow( rfc_msh *p_msh );
extern short
	rfc_msh_setown( rfc_msh *p_msh );

extern int
	rfc_msh_findname( rfc_msh *p_msh , const char *p_name );

/*#######################################################*/
/*#######################################################*/
