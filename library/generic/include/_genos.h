/*#######################################################*/
/*#######################################################*/

/* this header - OS dependent functions */
/* version 2.02.0001 */
#define RFC__GENOS_H

/*#######################################################*/
/*#######################################################*/
/* defined structures */

/* DLL handle storage */
typedef struct _rfc_dllpool			rfc_dllpool;

typedef struct _rfc_lib_proc		rfc_lib_proc;
typedef struct _rfc_lib_procname	rfc_lib_procname;
typedef struct _rfc_lib_info		rfc_lib_info;
typedef struct _rfc_lib_ref			rfc_lib_ref;

typedef struct _rfc_lock			rfc_lock;

/*#######################################################*/
/*#######################################################*/
/* library pool */

#define RFC_RGB( _r_ , _g_ , _b_ ) \
	( ( long )( ( ( char )( _r_ ) | \
    ( ( short )( _g_ ) << 8 ) ) | \
    ( ( ( long )( char )( _b_ ) ) << 16) ) )

/* data */
struct _rfc_dllpool
{
	rfc_array *		s_lib;		/* library array */
	rfc_array *		s_prc;		/* procedure array */
	rfc_array *		s_ref;		/* reference information */
	char *			s_path;		/* library pool path */
};

struct	_rfc_lib_proc
{
	RFC_PROC	s_proc;
	char		s_state;
};

struct	_rfc_lib_procname
{
	char		s_name[ 1 ];
};

struct _rfc_lib_info
{
	int			s_ext;
	rfc_lib_proc *
				s_procs;
	int			s_procs_n;
	RFC_HND		s_lib;
	char		s_state;
	char *		s_path;
	int			s_count;
	char		s_name[ 1 ];
};

struct _rfc_lib_ref
{
	void *		s_ref;
	int			s_lib;
	int			s_count;
};

struct _rfc_lock
{
	RFC_HND		s_ex_sem;		/* data lock semaphore */
	RFC_HND		s_sh_sem;		/* count lock semaphore */
	int			s_sh_count;		/* count of shared locks */
	int			s_mode;			/* 0=no lock, 1=exclusive mode, 2=shared mode */
};

/*#######################################################*/
/*#######################################################*/
/* module handle */
extern RFC_HND
	rfc_hmodule;

/* functions */
/* construction */
/* create library pool with start in directory <p_start> */
extern rfc_dllpool *
	rfc_dlp_create( const char *p_start );
extern void
	rfc_dlp_destroy( rfc_dllpool *p_dlp );

/* get library pool start path */
extern const char *
	rfc_dlp_start_path( rfc_dllpool *p_dlp );

/* operations */
/* library control */
/* add library information */
extern int
	rfc_dlp_lib_add( rfc_dllpool *p_dlp , const char *p_path , 
		int p_ext , 
		const char *p_name );
/* remove library */
extern int
	rfc_dlp_lib_remove( rfc_dllpool *p_dlp , int p_id );

/* add reference to library */
extern RFC_HND
	rfc_dlp_lib_ref( rfc_dllpool *p_dlp , int p_id , void *p_ref );
/* remove reference from library */
extern void
	rfc_dlp_lib_deref( rfc_dllpool *p_dlp , int p_id , void *p_ref );
/* remove all references from library */
extern void
	rfc_dlp_lib_deref_lib( rfc_dllpool *p_dlp , int p_id );
/* remove references from all libraries */
extern void
	rfc_dlp_lib_deref_ref( rfc_dllpool *p_dlp , void *p_ref );

/* library information */
/* find library by external ID */
extern int
	rfc_dlp_lib_find_ext( rfc_dllpool *p_dlp , int p_ext );
/* find library by name */
extern int
	rfc_dlp_lib_find_name( rfc_dllpool *p_dlp , const char *p_name );

/* get library path by ID */
extern const char *
	rfc_dlp_lib_getpath( rfc_dllpool *p_dlp , int p_id );
/* get full library path by ID (string needs to be destroyed after usage) */
extern rfc_string
	rfc_dlp_lib_getfull( rfc_dllpool *p_dlp , int p_id );
/* get library external ID by ID */
extern int
	rfc_dlp_lib_getext( rfc_dllpool *p_dlp , int p_id );
/* get library code by ID */
extern const char *
	rfc_dlp_lib_getname( rfc_dllpool *p_dlp , int p_id );

/* library procedures */
/* define procedure for library */
extern void
	rfc_dlp_prc_define( rfc_dllpool *p_dlp , const char *p_procname , int p_prc );
/* load proc in library - with reference control */
extern RFC_PROC
	rfc_dlp_prc_load( rfc_dllpool *p_dlp , int p_lib , void *p_ref , int p_prc );
/* find proc in library - without reference control */
extern RFC_PROC
	rfc_dlp_prc_find( rfc_dllpool *p_dlp , int p_lib , int p_prc );

/* library resources */

/* accelerator resource */
extern RFC_HND
	rfc_res_accel( RFC_HND p_inst , const char *p_name );
extern RFC_HND
	rfc_dlp_res_accel( rfc_dllpool *p_dlp , int p_lib , const char *p_name );
/* image resource (IMAGE_BITMAP, IMAGE_CURSOR, IMAGE_ICON) */
extern RFC_HND
	rfc_res_image( RFC_HND p_inst , 
		const char *p_name , 
		unsigned p_type ,
		int p_cx ,
		int p_cy ,
		unsigned p_load );
extern RFC_HND
	rfc_dlp_res_image( rfc_dllpool *p_dlp , int p_lib , 
		const char *p_name , 
		unsigned p_type ,
		int p_cx ,
		int p_cy ,
		unsigned p_load );
/* menu resource */
extern RFC_HND
	rfc_res_menu( RFC_HND p_inst , const char *p_name );
extern RFC_HND
	rfc_dlp_res_menu( rfc_dllpool *p_dlp , int p_lib , const char *p_name );
/* string resource */
extern const char *
	rfc_res_string( RFC_HND p_inst , int p_id );
extern const char *
	rfc_dlp_res_string( rfc_dllpool *p_dlp , int p_lib , int p_id );
/* dialog resource */
extern RFC_HND
	rfc_res_dialog( RFC_HND p_inst , const char *p_name );
extern RFC_HND
	rfc_dlp_res_dialog( rfc_dllpool *p_dlp , int p_lib , const char *p_name );
/* generic resource */
extern RFC_HND
	rfc_res_custom( RFC_HND p_inst , const char *p_name , const char *p_type );

/* generic window operations */
extern void
	rfc_wnd_enable( RFC_HND p_wnd , short p_enable );
extern short
	rfc_wnd_isenabled( RFC_HND p_wnd );

extern void
	rfc_wnd_show( RFC_HND p_wnd , short p_show );
extern short
	rfc_wnd_isshow( RFC_HND p_wnd );

extern void
	rfc_wnd_focus( RFC_HND p_wnd );

/*#######################################################*/
/*#######################################################*/
/* semaphore & event & mutex operations */

/* semaphore */
extern int			rfc_hnd_semlock( RFC_HND p_hnd );
extern void			rfc_hnd_semunlock( RFC_HND p_hnd );
extern RFC_HND		rfc_hnd_semcreate( void );
extern void			rfc_hnd_semdestroy( RFC_HND p_hnd );

/* event */
extern RFC_HND		rfc_hnd_evcreate( void );
extern void			rfc_hnd_evdestroy( RFC_HND p_hnd );
extern void			rfc_hnd_evsignal( RFC_HND p_hnd );
extern void			rfc_hnd_evreset( RFC_HND p_hnd );

/* interruptable semaphore */
typedef struct { RFC_HND sem; RFC_HND ie; } RFC_SEM;
extern int			rfc_int_semlock( RFC_SEM p_sem );
extern void			rfc_int_semunlock( RFC_SEM p_sem );
extern RFC_SEM 		rfc_int_semcreate( void );
extern void			rfc_int_semstop( RFC_SEM p_sem );
extern void			rfc_int_semdestroy( RFC_SEM p_sem );

/* interruptable resource pool */
typedef struct { RFC_HND sem; RFC_HND ie; } RFC_RPL;
extern RFC_RPL 		rfc_int_poolcreate( int count /* -1 if not limited */ );
extern int			rfc_int_poolget( RFC_RPL pool );
extern int			rfc_int_poolput( RFC_RPL pool );
extern void			rfc_int_poolstop( RFC_RPL p_pool );
extern void			rfc_int_pooldestroy( RFC_RPL pool );

/* p_timeout_ms<0 - INFINITE, p_timeout_ms=0 - check/no wait, p_timeout_ms>0 - wait timeout */
extern short		rfc_hnd_waitevent( RFC_HND p_hnd , int p_timeout_ms );

/* mutex */
extern RFC_HND		rfc_hnd_mutcreate( void );
extern void			rfc_hnd_mutdestroy( RFC_HND p_hnd );
extern void			rfc_hnd_mutlock( RFC_HND p_hnd );
extern void			rfc_hnd_mutunlock( RFC_HND p_hnd );

/* rfc_lock */
extern rfc_lock *	rfc_lock_create( void );
extern void			rfc_lock_destroy( rfc_lock *p_lock );
extern void			rfc_lock_shared( rfc_lock *p_lock );
extern void			rfc_lock_exclusive( rfc_lock *p_lock );
extern void			rfc_lock_release( rfc_lock *p_lock );
extern void			rfc_lock_escalate( rfc_lock *p_lock );

/*#######################################################*/
/*#######################################################*/
/* threads */

extern int			rfc_thr_waitexit( RFC_THREAD *p_hnd );
extern void			rfc_thr_closehandle( RFC_THREAD *p_hnd );
extern int			rfc_thr_process( RFC_THREAD *p_thread , void *p_arg , RFC_THRFUNC p_action );
extern void			rfc_thr_threxit( int p_exitcode );
extern void			rfc_thr_sleep( int p_sec );

/* call stack structures */
typedef struct _rfc_threadstack
{
	rfc_list levels;
	int extraLevels; /* call stack items added to levels, but ignored  in reporting */
	short brokenStack; /* 1 if stack filled partially because of error while getting stack */
} rfc_threadstack;

typedef struct _rfc_threadstacklevel
{
	const char *moduleName;
	const char *className;
	const char *functionName;
	int line;
	const char *message;
} rfc_threadstacklevel;

extern void rfc_thr_initstackhandle();
extern void rfc_thr_exitstackhandle();
extern rfc_threadstack *rfc_thr_stackget( int skipLevels );
extern rfc_threadstack *rfc_thr_stackgetforthread( RFC_HND thread , int skipLevels );

extern int			rfc_thr_stackfulldepth( rfc_threadstack *stack );
extern int			rfc_thr_stackdepth( rfc_threadstack *stack );
extern rfc_threadstacklevel *rfc_thr_stacklevel( rfc_threadstack *stack , int level );
extern void			rfc_thr_stackfree( rfc_threadstack *stack );
extern short		rfc_thr_stacknamebyaddr( void *p_addr , char *p_class , char *p_func );
extern int			rfc_thr_increment( int *pv );

/*#######################################################*/
/*#######################################################*/
/* library management */

extern RFC_HND		rfc_lib_load( const char *p_lib );
extern RFC_PROC		rfc_lib_getfunc( RFC_HND p_lib , const char *p_func );
extern void			rfc_lib_unload( RFC_HND p_lib );

/*#######################################################*/
/*#######################################################*/
/* environment */

extern void			rfc_env_gethostname( char *p_name , int p_size );

/*#######################################################*/
/*#######################################################*/
/* high precision time measurement */

extern void			rfc_hpt_startadjustment();
extern void			rfc_hpt_stopadjustment();
extern void			rfc_hpt_setpoint( RFC_INT64 *pv );
extern RFC_INT64	rfc_hpt_timepassed( RFC_INT64 *pv );
extern RFC_INT64	rfc_hpt_ticks2ms( RFC_INT64 ticks );
extern RFC_INT64	rfc_hpt_ms2ticks( RFC_INT64 ms );

/*#######################################################*/
/*#######################################################*/

typedef struct {
	/* false in first call */
	short initialized;

	/* current counter values */
	RFC_INT64 idle;
	RFC_INT64 user;
	RFC_INT64 kernel;
	int clocks;
	/* previous counter values */
	RFC_INT64 pidle;
	RFC_INT64 puser;
	RFC_INT64 pkernel;
	int pclocks;
	/* anchor counter values to calculate CPU load */
	RFC_INT64 aidle; 
	RFC_INT64 auser;
	RFC_INT64 akernel;
	int aclocks;
} CPULOADINFO;

extern float		rfc_sys_getcpuload( CPULOADINFO *info );

/*#######################################################*/
/*#######################################################*/

typedef struct {
	RFC_INT64	timeCreated;	/* number of 100-nanosecond intervals since January 1, 1601 */
	RFC_INT64	timeUpdated;	/* number of 100-nanosecond intervals since January 1, 1601 */
	RFC_INT64	size;			/* number of bytes */
} RFC_FILEINFO;

extern short		rfc_sys_getfileinfo( const char *path , RFC_FILEINFO *info );

/*#######################################################*/
/*#######################################################*/
