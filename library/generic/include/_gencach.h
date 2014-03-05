/*#######################################################*/
/*#######################################################*/

/* this header - cache functions */
/* version 2.02.0001 */
#define RFC__GENCACH_H

/*#######################################################*/
/*#######################################################*/
/* defined structures */

typedef struct _rfc_cacheitem	rfc_cacheitem;
typedef struct _rfc_cache		rfc_cache;

typedef void
	( *rfc_cach_delete )( void *p_obj );

/*#######################################################*/
/*#######################################################*/
/* cache */

struct _rfc_cacheitem
{
	union
	{
		int				u_l;			/* integer key value */
		char			*u_s;			/* string key value */
	}				s_key;			/* key field value */

	void			*s_data;		/* stored object */

	rfc_cacheitem	*s_prv;
	rfc_cacheitem	*s_nxt;
};

struct _rfc_cache
{
	rfc_array			s_arr;
	int					s_max_cnt;		/* max items count */
	char				s_key_type;		/* 'S' - string, 'L' - integer */
	rfc_cach_delete		s_fdelete;		/* delete callback */

	short				s_read_flag;

	rfc_cacheitem *		s_head;
	rfc_cacheitem *		s_tail;

	int					s_ref;			/* reference count */
};

/*#######################################################*/
/*#######################################################*/
/* cache functions */

/* create cache */
extern rfc_cache *
	rfc_cache_create( int p_max, char p_type, rfc_cach_delete f_delete );
/* destroy cache */
extern void 
	rfc_cache_destroy( rfc_cache *p_cache );
/* reference cache */
extern void 
	rfc_cache_ref( rfc_cache *p_cache );

/* remove all objects from cache without destroying rfc_cache structure */
extern void 
	rfc_cache_clear( rfc_cache *p_cache );

/* place object into cache, string key */
extern void 
	rfc_cache_add_s( rfc_cache *p_cache, void *p_obj, const char *p_key );
/* place object into cache, integer key */
extern void 
	rfc_cache_add_l( rfc_cache *p_cache, void *p_obj, int p_key );
/* retreive object from cache by string key id */
extern void *
	rfc_cache_get_s( rfc_cache *p_cache, const char *p_key );
/* retreive object from cache by integer key id */
extern void *
	rfc_cache_get_l( rfc_cache *p_cache, int p_key );
/* delete object from cache by string key id */
extern void
	rfc_cache_remove_s( rfc_cache *p_cache, const char *p_key );
/* delete object from cache by integer key id */
extern void
	rfc_cache_remove_l( rfc_cache *p_cache, int p_key );

/*#######################################################*/
/*#######################################################*/