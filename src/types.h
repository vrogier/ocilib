/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
 *
 * Licensed under the Apache License, Version 2.0 (the "License");
 * you may not use this file except in compliance with the License.
 * You may obtain a copy of the License at
 *
 *      http://www.apache.org/licenses/LICENSE-2.0
 *
 * Unless required by applicable law or agreed to in writing, software
 * distributed under the License is distributed on an "AS IS" BASIS,
 * WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
 * See the License for the specific language governing permissions and
 * limitations under the License.
 */

#ifndef OCILIB_TYPES_H_INCLUDED
#define OCILIB_TYPES_H_INCLUDED

#include "ocilibc/types.h"

#include "defs.h"

/* ********************************************************************************************* *
 *                             PRIVATE TYPES
 * ********************************************************************************************* */

/*
* OCI_MemoryBlock : Internal block of Memory.
*
*/

struct OCI_MemoryBlock
{
    int          type; /* type of allocated data */
    unsigned int size; /* allocated memory size*/
};

typedef struct OCI_MemoryBlock OCI_MemoryBlock;

/*
 * OCI_Item : Internal list entry.
 *
 * The library needs to manage internal list of objects in order to be able to
 * free them if the application does not.
 *
 * @note
 * Internal lists are using mutexes for resource locking in multithreaded
 * environments
 *
 */

struct OCI_Item
{
    void            *data; /* pointer to external data */
    struct OCI_Item *next; /* next element in list */
};

typedef struct OCI_Item OCI_Item;

/*
 * OCI_List : Internal list object.
 *
 * The OCI_List object is used to maintain a collection of handles allocated
 * by user programs.
 *
 * Those handles are freed when the collection owner is destroyed.
 * So, we make sure that if OCI_Cleanup() is called, all allocated handles will
 * be destroyed even if the program does not free them.
 *
 */

struct OCI_List
{
    OCI_Item  *head;     /* pointer to first item */
    OCI_Mutex *mutex;    /* mutex handle */
    int        type;     /* type of list item */
    ub4        count;    /* number of elements in list */
};

typedef struct OCI_List OCI_List;

/*
 * Server output object used to retrieve server dbms.output buffers
 *
 */

struct OCI_ServerOutput
{
    OCI_Statement *stmt;           /* pointer to statement object (dbms_output calls) */
    ub1           *arrbuf;         /* array buffer */
    unsigned int   arrsize;        /* array size */
    unsigned int   cursize;        /* number of filled items in the array */
    unsigned int   curpos;         /* current position in the array */
    unsigned int   lnsize;         /* line size */
};

typedef struct OCI_ServerOutput OCI_ServerOutput;

/*
 * Connection trace information
 *
 */

struct OCI_TraceInfo
{
    otext identifier[OCI_SIZE_TRACE_ID+1];
    otext module[OCI_SIZE_TRACE_MODULE+1];
    otext action[OCI_SIZE_TRACE_ACTION+1];
    otext info[OCI_SIZE_TRACE_INFO+1];
    otext operation[OCI_SIZE_TRACE_OPERATION + 1];

};

typedef struct OCI_TraceInfo OCI_TraceInfo;

/* ********************************************************************************************* *
 *                             PUBLIC TYPES
 * ********************************************************************************************* */

struct OCI_Error
{
    void        *source_ptr;        /* pointer to source ptr */
    otext       *location;          /* location (method) */
    otext       *message;           /* error message */
    unsigned int type;              /* OCILIB error type */
    unsigned int source_type;       /* source type */
    int          code;              /* Oracle OCI or OCILIB internal error code */
    unsigned int location_len;      /* length of error message */
    unsigned int message_len;       /* length of error location */
    ub4          row;               /* Error row offset (array DML) */
    boolean      active;            /* to avoid recursive exceptions */
};

/*
 * Mutex object
 *
 * Mutexes have their own error handle to avoid conflict using OCIErrorGet()
 * from different threads
 *
 */

struct OCI_Mutex
{
    OCIThreadMutex *handle;   /* OCI Mutex handle */
    OCIError       *err;      /* OCI Error handle */
};

/*
 * Thread object
 *
 * Threads have their own error handle to avoid conflict using OCIErrorGet()
 *
 */

struct OCI_Thread
{
    OCIThreadHandle *handle;    /* OCI Thread handle */
    OCIThreadId     *id;        /* OCI Thread ID */
    OCIError        *err;       /* OCI Error handle */
    void            *arg;       /* thread routine argument */
    POCI_THREAD      proc;      /* thread routine */
};

/*
 * Thread key object
 *
 * Thread keys have their own error handle to avoid conflict using OCIErrorGet()
 * from different s threads
 *
 */

struct OCI_ThreadKey
{
    OCIThreadKey *handle;     /* OCI Thread key handle */
    OCIError     *err;        /* OCI Error handle */
};

typedef struct OCI_ThreadKey OCI_ThreadKey;

/*
 * OCI_Environment : Internal OCILIB library encapsulation.
 *
 * It's a static, local and unique object that collects all the global variables
 * needed by the library
 *
 */

struct OCI_Environment
{
    OCI_List       *cons;                         /* list of connection objects */
    OCI_List       *pools;                        /* list of pools objects */
    OCI_List       *subs;                         /* list of subscription objects */
    OCI_List       *arrs;                         /* list of arrays objects */
    OCIError       *err;                          /* OCI error handle */
    OCIEnv         *env;                          /* OCI environment handle */
    OCI_Error      *lib_err;                      /* Global error */
    OCI_HashTable  *key_map;                      /* hash table for mapping name/key */
    OCI_ThreadKey  *key_errs;                     /* Thread key to store thread errors */
    OCI_HashTable* sql_funcs;                     /* hash table handle for sql function names */
    OCI_Mutex      *mem_mutex;                    /* mutex for memory counters */
    void           *usrdata;                      /* user data */
    otext          *formats[OCI_FMT_COUNT];       /* string conversion default formats */
    POCI_ERROR      error_handler;                /* user defined error handler */
    POCI_HA_HANDLER ha_handler;                   /* HA event callback*/
#ifdef OCI_IMPORT_RUNTIME
    LIB_HANDLE lib_handle;                        /* handle of runtime shared library */
#endif
    big_uint        mem_bytes_oci;                /* allocated bytes by OCI client */
    big_uint        mem_bytes_lib;                /* allocated bytes by OCILIB */
    unsigned int    version_compile;              /* OCI version used at compile time */
    unsigned int    version_runtime;              /* OCI version used at runtime */
    unsigned int    charset;                      /* charset type */
    unsigned int    nb_hndlp;                     /* number of OCI handles allocated */
    unsigned int    nb_descp;                     /* number of OCI descriptors allocated */
    unsigned int    nb_objinst;                   /* number of OCI objects allocated */
    ub4             env_mode;                     /* default environment mode */
    boolean         loaded;                       /* OCILIB correctly loaded ? */
    boolean         nls_utf8;                     /* is UFT8 enabled for data strings ? */
    boolean         use_lob_ub8;                  /* use 64 bits integers for lobs ? */
    boolean         use_xa;                       /* is xa enabled */
    boolean         use_scrollable_cursors;       /* use Oracle 9i fetch API */
    boolean         use_wide_char_conv;           /* are we on a unix like platform with unicode */
    boolean         warnings_on;                  /* warnings enabled ? */
    boolean         env_vars[OCI_VARS_COUNT];     /* specific environment variables */

};

typedef struct OCI_Environment OCI_Environment;

/*
 * Pool object
 *
 */

struct OCI_Pool
{
    void     *handle;           /* OCI pool handle */
    void     *authp;            /* OCI authentication handle */
    OCIError *err;              /* OCI context handle */
    otext    *name;             /* pool name */
    otext    *db;               /* database */
    otext    *user;             /* user */
    otext    *pwd;              /* password */
    ub4       mode;             /* session mode */
    ub4       min;              /* minimum of objects */
    ub4       max;              /* maximum of objects */
    ub4       incr;             /* increment step of objects */
    ub4       htype;            /* handle type of pool : connection / session */
    ub4       cache_size;       /* statement cache size */
};

/*
 * Connection object
 *
 */

struct OCI_Connection
{
    OCIServer        *svr;                      /* OCI server handle */
    OCIError         *err;                      /* OCI context handle */
    OCIEnv           *env;                      /* OCI environment handle */
    OCISession       *ses;                      /* OCI session handle */
    OCISvcCtx        *cxt;                      /* OCI context handle */
    OCI_List         *stmts;                    /* list of statements */
    OCI_List         *trsns;                    /* list of transactions */
    OCI_List         *tinfs;                    /* list of type info objects */
    OCI_Transaction  *trs;                      /* pointer to current transaction object */
    OCI_Pool         *pool;                     /* pointer to parent pool object */
    OCI_ServerOutput *svopt;                    /* Pointer to server output object */
    OCI_TraceInfo   *trace;                     /* trace information */
    otext            *db;                       /* database */
    otext            *user;                     /* user */
    otext            *pwd;                      /* password */
    otext            *ver_str;                  /* string  server version*/
    otext            *sess_tag;                 /* session tag */
    otext            *db_name;                  /* session tag */
    otext            *inst_name;                /* instance name */
    otext            *service_name;             /* server service name */
    otext            *server_name;              /* server name (hostname) */
    otext            *domain_name;              /* server domain name */
    otext            *formats[OCI_FMT_COUNT];   /* string conversion default formats */
    OCI_Timestamp    *inst_startup;             /* instance startup timestamp */
    POCI_TAF_HANDLER  taf_handler;              /* failover call back */
    void             *usrdata;                  /* user data */
    boolean           autocom;                  /* auto commit mode */
    boolean           alloc_handles;            /* do new need to allocate OCI handles ? */
    unsigned int      nb_files;                 /* number of OCI_File opened by the connection */
    unsigned int      mode;                     /* session mode */
    int               cstate;                   /* connection state */
    unsigned int      ver_num;                  /* numeric server version */
};

/*
 * Transaction object
 *
 */

struct OCI_Transaction
{
    OCI_Connection *con;        /* pointer to connection object */
    OCITrans       *htr;        /* OCI transaction handle */
    OCI_XID         xid;        /* global transaction identifier */
    unsigned int    timeout;    /* timeout */
    unsigned int    mode;       /* transaction mode */
    boolean         local;      /* is local transaction ? */
};

/*
 * Column object
 *
 */

struct OCI_Column
{
    /* OCILIB infos */
    OCI_TypeInfo   *typinf;         /* user type descriptor */
    ub4             bufsize;        /* element size */
    ub2             subtype;        /* Ocilib sub type */
    ub2             struct_subtype; /* Ocilib numeric sub type for OCI_GetStruct() :( */
    /* Oracle infos */
    otext          *name;           /* column name */
    ub2             sqlcode;        /* Oracle SQL code */
    ub2             typecode;       /* Oracle type code */
    ub2             libcode;        /* Internal translated Oracle SQL code */
    ub2             size;           /* SQL Size */
    sb2             prec;           /* SQL precision 1 (number prec, leading prec) */
    sb2             prec2;          /* SQL precision 2 (fractional prec) */
    sb1             scale;          /* SQL scale */
    ub1             datatype;       /* internal datatype */
    ub1             nullable;       /* is nullable */
    ub1             charused;       /* is column size expressed in characters */
    ub2             charsize;       /* SQL Size in character */
    ub1             csfrm;          /* charset form */
    ub1             handletype;     /* oracle handle type */
    ub4             props;          /* column properties */
    ub4             collation_id;   /* collation id */
};

/*
 * OCI_Buffer : Internal input/output place holder
 *
 */

struct OCI_Buffer
{
    void        *handle;           /* OCI handle (bind or define) */
    void       **data;             /* data / array of data */
    void        *lens;             /* array of lengths */
    otext       *tmpbuf;           /* temporary buffer */
    OCIInd      *inds;             /* indicators */
    void       **obj_inds;         /* object indicators */
    ub4          count;            /* number of elements in the buffer */
    int          sizelen;          /* size of an element in the lens array */
    unsigned int tmpsize;          /* temporary buffer size */
};

typedef struct OCI_Buffer OCI_Buffer;

/*
 * OCI_Bind object
 *
 */

struct OCI_Bind
{
    OCI_Statement *stmt;         /* pointer to statement object */
    void         **input;        /* input values */
    otext         *name;         /* name of the bind */
    OCI_TypeInfo* typinf;        /* for object, collection and ref */
    ub2           *plrcds;       /* PL/SQL tables return codes */
    OCI_Buffer     buffer;       /* place holder */
    boolean        is_array;     /* is it an array bind ? */
    sb4            size;         /* data size */
    ub4            nbelem;       /* PL/SQL tables number of elements */
    ub2            dynpos;       /* index of the bind for dynamic binds */
    ub2            code;         /* SQL datatype code */
    ub1            type;         /* internal datatype */
    ub1            subtype;      /* internal subtype */
    ub1            alloc;        /* is buffer allocated or mapped to input */
    ub1            csfrm;        /* charset form */
    ub1            direction;    /* in, out or in/out bind */
    ub1            alloc_mode;   /* allocation mode : internal or external */
};

/*
 * OCI_Define : Internal Resultset column data implementation
 *
 */

struct OCI_Define
{
    OCI_Resultset *rs;    /* pointer to resultset object */
    void          *obj;   /* current OCILIB object instance */
    OCI_Column     col;   /* column object */
    OCI_Buffer     buf;   /* placeholder */
};

typedef struct OCI_Define OCI_Define;

/*
 * Resultset object
 *
 */

struct OCI_Resultset
{
    OCI_Statement *stmt;            /* pointer to statement object */
    OCI_HashTable *map;             /* hash table handle for mapping name/index */
    OCI_Define    *defs;            /* array of define objects */
    ub4            nb_defs;         /* number of elements */
    ub4            row_cur;         /* actual position in the array of rows */
    ub4            row_abs;         /* absolute position in the resultset */
    ub4            row_count;       /* number of rows fetched so far */
    ub4            row_fetched;     /* rows fetched by last call (scrollable) */
    ub4            fetch_size;      /* internal array size */
    boolean        eof;             /* end of resultset reached ?  */
    boolean        bof;             /* beginning of resultset reached ?  */
    sword          fetch_status;    /* internal fetch status */
};

/*
 * OCI_Define : Internal Resultset column data implementation
 *
 */

struct OCI_BatchErrors
{
    OCI_Error *errs;               /* sub array of OCILIB errors(array DML) */
    ub4        cur;                /* current sub error index (array DML) */
    ub4        count;              /* number of errors (array DML) */
};

typedef struct OCI_BatchErrors OCI_BatchErrors;

/*
 * Statement object
 *
 */

struct OCI_Statement
{
    /* Start OCI_Datatype */
    OCIStmt         *stmt;              /* OCI statement handle */
    ub4              hstate;            /* object variable state */
    /* End OCI_Datatype */
    ub4              exec_mode;         /* type of execution */
    OCI_Resultset  **rsts;              /* pointer to resultset list */
    OCI_Statement  **stmts;             /* pointer to implicit statement resultset list */
    OCI_Connection  *con;               /* pointer to connection object */
    otext           *sql;               /* SQL statement */
    otext           *sql_id;            /* server statement sql id */
    OCI_Bind       **ubinds;            /* array of user bind objects */
    OCI_Bind       **rbinds;            /* array of register bind objects */
    OCI_HashTable   *map;               /* hash table handle for mapping bind name/index */
    OCI_BatchErrors* batch;             /* error handling for array DML */
    ub2              nb_ubinds;         /* number of used user binds */
    ub2              nb_rbinds;         /* number of used register binds */
    ub2              allocated_ubinds;  /* number of allocated user binds */
    ub2              allocated_rbinds;  /* number of allocated register binds */
    boolean          bind_array;        /* has array binds ? */
    boolean          bind_reuse;        /* rebind data allowed ? */
    unsigned int     bind_mode;         /* type of binding */
    unsigned int     bind_alloc_mode;   /* type of bind allocation */
    ub4              fetch_size;        /* fetch array size */
    ub4              prefetch_size;     /* pre-fetch size */
    ub4              prefetch_mem;      /* pre-fetch memory */
    ub4              long_size;         /* default size for LONG columns */
    ub1              long_mode;         /* LONG datatype handling mode */
    ub1              status;            /* statement status */
    ub2              type;              /* type of SQL statement */
    ub4              nb_iters;          /* current number of iterations for execution */
    ub4              nb_iters_init;     /* initial number of iterations for execution */
    ub4              nb_stmt;           /* number of implicit statements */
    ub4              nb_rs;             /* number of resultsets */
    ub2              cur_rs;            /* index of the current resultset */
    ub2              dynidx;            /* bind index counter for dynamic exec */
    ub2              err_pos;           /* error position in sql statement */
};

/*
 * Internal Large object
 *
 */

struct OCI_Lob
{
    /* Start OCI_Datatype */
    OCILobLocator  *handle;         /* OCI handle */
    ub4             hstate;         /* object variable state */
    /* End OCI_Datatype */
    ub4             type;           /* type of lob */
    OCI_Connection *con;            /* pointer to connection object */
    big_uint        offset;         /* current offset for R/W */
};

/*
 * External Large object
 *
 */

struct OCI_File
{
    /* Start OCI_Datatype */
    OCILobLocator  *handle;     /* OCI handle */
    ub4             hstate;     /* object variable state */
    /* End OCI_Datatype */
    ub4             type;       /* type of file */
    OCI_Connection *con;        /* pointer to connection object */
    otext          *dir;        /* directory name */
    otext          *name;       /* file name */
    big_uint        offset;     /* current offset for read */
};

/*
 * Long object
 *
 */

struct OCI_Long
{
    /* Start OCI_Datatype */
    OCI_Statement *stmt;        /* pointer to statement object */
    ub4            hstate;      /* object variable state */
    /* End OCI_Datatype */
    ub4            type;        /* type of long */
    OCI_Define    *def;         /* pointer to resultset define object */
    ub4            size;        /* size of the buffer read / written */
    ub4            offset;      /* current offset for R/W */
    ub4            piecesize;   /* size of current fetched piece */
    ub4            maxsize;     /* size to R/W */
    ub1           *buffer;      /* fetched buffer */
};

/*
* Number object
*
*/

struct OCI_Number
{
    /* Start OCI_Datatype */
    OCINumber      *handle;     /* OCI handle */
    ub4             hstate;     /* object variable state */
    /* End OCI_Datatype */
    /* padding here in 64bits - cannot avoid it as this struct can be reinterpretated as OCI_Datatype */
    OCI_Connection *con;        /* pointer to connection object */
    OCIError       *err;        /* OCI error handle */
    OCIEnv         *env;        /* OCI environment handle */
};

/*
 * Date object
 *
 */

struct OCI_Date
{
    /* Start OCI_Datatype */
    OCIDate        *handle;     /* OCI handle */
    ub4             hstate;     /* object variable state */
    /* End OCI_Datatype */
    ub4             allocated;  /* is handle allocated ? */
    OCI_Connection *con;        /* pointer to connection object */
    OCIError       *err;        /* OCI context handle */
    OCIEnv         *env;        /* OCI environment handle */
};

/*
 * Timestamp object
 *
 */

struct OCI_Timestamp
{
    /* Start OCI_Datatype */
#if OCI_VERSION_COMPILE >= OCI_9_0
    OCIDateTime    *handle;     /* OCI handle */
#else
    void           *handle;     /* fake handle for alignment */
#endif
    ub4             hstate;     /* object variable state */
    /* End OCI_Datatype */
    ub4             type;       /* sub type */
    OCI_Connection *con;        /* pointer to connection object */
    OCIError       *err;        /* OCI context handle */
    OCIEnv         *env;        /* OCI environment handle */
};

/*
 * Interval object
 *
 */

struct OCI_Interval
{
    /* Start OCI_Datatype */
#if OCI_VERSION_COMPILE >= OCI_9_0
    OCIInterval    *handle;     /* OCI handle */
#else
    void           *handle;     /* fake handle for alignment */
#endif
    ub4             hstate;     /* object variable state */
    /* End OCI_Datatype */
    ub4             type;       /* sub type */
    OCI_Connection *con;        /* pointer to connection object */
    OCIError       *err;        /* OCI context handle */
    OCIEnv         *env;        /* OCI environment handle */
};

/*
 * Oracle Named type object
 *
 */

struct OCI_Object
{
    /* Start OCI_Datatype */
    void               *handle;       /* OCI handle */
    ub4                 hstate;       /* object variable state */
    /* End OCI_Datatype */
    OCIObjectLifetime   type;         /* object type */
    OCI_Connection     *con;          /* pointer to connection object */
    OCI_TypeInfo       *typinf;       /* pointer to type info object */
    void              **objs;         /* array of OCILIB sub objects */
    otext             **tmpbufs;      /* temporary buffer  per column */
    unsigned int       *tmpsizes;     /* temporary buffer size per column */
    sb2                *tab_ind;      /* indicators for root instance */
    ub2                 idx_ind;      /* instance indicator offset / indicator table */
};

/*
 * Oracle Collection Item object
 *
 */

struct OCI_Elem
{
    /* Start OCI_Datatype */
    void           *handle;      /* OCI handle */
    ub4             hstate;      /* object variable state */
    /* End OCI_Datatype */
    unsigned int    tmpsize;     /* temporary buffer size */
    OCI_Connection *con;         /* pointer to connection object */
    void           *obj;         /* OCILIB sub object */
    otext          *tmpbuf;      /* temporary buffer */
    OCI_TypeInfo   *typinf;      /* object type information */
    OCIInd         *pind;        /* indicator  pointer */
    uword           alloc;       /* is handle locally allocated */
    ub2             init;        /* underlying object has been initialized ? */
    OCIInd          ind;         /* internal temporary data state indicator */
};

/*
 * Oracle Collection object
 *
 */

struct OCI_Coll
{
    /* Start OCI_Datatype */
    OCIColl        *handle;      /* OCI handle */
    ub4             hstate;      /* object variable state */
    /* End OCI_Datatype */
    /* padding here in 64bits - cannot avoid it as this struct can be reinterpretated as OCI_Datatype */
    OCI_Connection *con;         /* pointer to connection object */
    OCI_TypeInfo   *typinf;      /* pointer to type info object */
    OCI_Elem       *elem;        /* item object */
};

/*
 * Oracle Iterator object
 *
 */

struct OCI_Iter
{
    OCIIter  *handle;            /* OCI handle */
    OCI_Coll *coll;              /* pointer to connection object */
    OCI_Elem *elem;              /* item object */
    boolean   eoc;               /* end of collection */
    boolean   boc;               /* beginning of collection */
    boolean   dirty;             /* indicate if elem is dirty, e.g. not fetched yet */
};

/*
 * Oracle REF object
 *
 */

struct OCI_Ref
{
    /* Start OCI_Datatype */
    OCIRef         *handle;      /* OCI handle */
    ub4             hstate;      /* object variable state */
    /* End OCI_Datatype */
    boolean         pinned;      /* is the reference pinned */
    OCI_Connection *con;         /* pointer to connection object */
    OCI_TypeInfo   *typinf;      /* pointer to type info object */
    OCI_Object     *obj;         /* Pinned object */
};

/*
 * Type info object
 *
 */

struct OCI_TypeInfo
{
    OCI_Connection *con;         /* pointer to connection object */
    OCI_Column     *cols;        /* array of column datatype info */
    OCI_TypeInfo   *parent_type; /* parent super type for derived type */
    OCIType        *tdo;         /* datatype object type */
    otext          *name;        /* name of the type info object */
    otext          *schema;      /* owner of the type info object */
    int            *offsets;     /* cached offsets */
    size_t          struct_size; /* cached structure size */
    size_t          align;       /* cached structure alignment */
    unsigned int    type;        /* type of type info handle */
    ub2             typecode;    /* Oracle type code */
    ub2             colcode;     /* Oracle collection code */
    ub2             nb_cols;     /* number of columns */
    ub1             is_final;    /* is is a virtual type that can be inherited ? */
};

/*
 * OCI_DirPathColumn : Internal Direct Path column object
 *
 */

struct OCI_DirPathColumn
{
    otext *format;                /* date or numeric format */
    ub1   *data;                  /* array of data */
    ub1   *flags;                 /* array of row flags */
    ub4   *lens;                  /* array of lengths */
    ub4    format_size;           /* size of the column format */
    ub2    type;                  /* column type */
    ub2    sqlcode;               /* sql type */
    ub4    bufsize;               /* buffer size */
    ub2    index;                 /* ref index in the type info columns list */
    ub4    maxsize;               /* input max size */
};

typedef struct OCI_DirPathColumn OCI_DirPathColumn;

/*
 * Oracle Direct Path column object
 *
 */

struct OCI_DirPath
{
    OCI_Connection     *con;            /* pointer to connection object */
    OCI_TypeInfo       *typinf;         /* type info about table to load */
    OCIDirPathCtx      *ctx;            /* OCI DP context handle */
    OCIDirPathColArray *arr;            /* OCI DP column array handle */
    OCIDirPathStream   *strm;           /* OCI DP stream handle */
    OCI_DirPathColumn  *cols;           /* array of column info */
    ub4                *err_rows;       /* array of err rows index */
    ub2                *err_cols;       /* array of err col index */
    ub4                 nb_processed;   /* number of row processed at last call */
    ub4                 nb_loaded;      /* number of row loaded so far */
    ub4                 nb_converted;   /* number of row converted so far */
    ub4                 nb_entries;     /* number of rows currently set */
    ub4                 load_offset;    /* absolute offset of the last converted row  */
    ub4                 status;         /* internal status */
    ub4                 nb_cur;         /* current number of row to load per stream */
    ub2                 nb_cols;        /* number of columns to load */
    ub2                 nb_rows;        /* maximum number of row to load per stream */
    ub2                 cvt_mode;       /* conversion mode */
    ub2                 idx_err_col;    /* index of current erred row */
    ub4                 idx_err_row;    /* index of current erred column */
    ub4                 nb_err;         /* number of conversion errors since the last load */
    unsigned int        res_conv;       /* status of the last conversion */
    unsigned int        res_load;       /* status of the last load */
};

/*
 * Oracle Event object
 *
 */

struct OCI_Event
{
    OCI_Subscription *sub;             /* OCILIB subscription handle */
    otext            *objname;         /* altered object name */
    otext            *rowid;           /* altered row id */
    otext            *dbname;          /* database name */
    ub4               op;              /* event object operation */
    unsigned int      type;            /* event type */
    unsigned int      objname_size;    /* altered object name size */
    unsigned int      rowid_size;      /* altered row id size */
    unsigned int      dbname_size;     /* database name size */
};

/*
 * Oracle Notification object
 *
 */

struct OCI_Subscription
{
    OCI_Connection  *con;            /* OCILIB connection handle */
    OCISubscription *subhp;          /* OCI subscription handle */
    OCIEnv          *env;            /* OCI environment handle */
    OCIError        *err;            /* OCI error handle  */
    otext           *name;           /* notification name */
    otext           *saved_db;       /* database for reconnection if needed */
    otext           *saved_user;     /* user for reconnection if needed */
    otext           *saved_pwd;      /* password for reconnection if needed */
    POCI_NOTIFY      handler;        /* user callback */
    OCI_Event        event;          /* event object for user callback */
    unsigned int     type;           /* notification type */
    ub4              timeout;        /* notification timeout */
    ub4              port;           /* port to use  */
};

/*
 * Oracle A/Q Agent
 *
 */

struct OCI_Agent
{
    /* Start OCI_Datatype */
    OCIAQAgent     *handle;        /* OCI agent handle */
    ub4             hstate;        /* object variable state */
    /* End OCI_Datatype */
    /* padding here in 64bits - cannot avoid it as this struct can be reinterpretated as OCI_Datatype */
    OCI_Connection *con;           /* OCILIB connection handle */
    otext          *address;       /* agent address */
    otext          *name;          /* agent name */
};

/*
 * Oracle A/Q message
 *
 */

struct OCI_Msg
{
    OCI_TypeInfo       *typinf;        /* pointer to type info object */
    OCIAQMsgProperties *proph;         /* OCI message properties handle */
    void               *payload;       /* message payload (object handle or raw handle) */
    OCIRaw             *id;            /* message identifier */
    OCI_Date           *date;          /* enqueue date */
    otext              *correlation;   /* correlation string */
    otext              *except_queue;  /* exception queue name */
    OCI_Agent          *sender;        /* sender */
    OCI_Object         *obj;           /* OCILIB object handle for object payloads */
    OCIInd              ind;           /* message payload indicator pointer */
};

/*
 * Oracle A/Q enqueue
 *
 */

struct OCI_Enqueue
{
    OCI_TypeInfo    *typinf;         /* pointer to type info object */
    OCIAQEnqOptions *opth;           /* OCI enqueue options handle */
    otext           *name;           /* queue name */
};

/*
 * Oracle A/Q Dequeue
 *
 */

struct OCI_Dequeue
{
    OCI_TypeInfo    *typinf;             /* pointer to type info object */
    OCIAQDeqOptions *opth;               /* OCI dequeue options handle */
    otext           *name;               /* queue name */
    otext           *pattern;            /* queue name */
    otext           *consumer;           /* consumer name */
    OCI_Msg         *msg;                /* message retrieved from queue */
    OCIAQAgent     **agent_list;         /* array of agents objects */
    OCI_Agent       *agent;              /* pointer to agent object for listen call */
    OCISubscription *subhp;              /* AQ subscription for async dequeueing */
    POCI_NOTIFY_AQ   callback;           /* user callback */
    ub4              agent_count;        /* number of agents objects */
};

/*
 * OCILIB array
 *
 */

struct OCI_Array
{
    OCIError       *err;            /* OCI context handle */
    OCIEnv         *env;            /* OCI environment handle */
    OCI_Connection *con;            /* Connection object for type dependent on connections */
    void          **tab_obj;        /* array of pointers to OCILIB objects */
    void           *mem_handle;     /* array OCI handles */
    void           *mem_struct;     /* array of OCILIB objects structures */
    unsigned int    elem_type;      /* array element type */
    unsigned int    elem_subtype;   /* array element subtype */
    unsigned int    elem_size;      /* array element handle size */
    unsigned int    nb_elem;        /* array size of number of elements */
    unsigned int    struct_size;    /* array element size */
    unsigned int    handle_type;    /* array element OCI handle type */
};

typedef struct OCI_Array OCI_Array;

/*
 * Hash table object
 *
 */

struct OCI_HashTable
{
    OCI_HashEntry **items;        /* array of slots */
    unsigned int    size;         /* size of the slots array */
    unsigned int    count;        /* number of used slots */
    unsigned int    type;         /* type of data */
};

/*
 * OCI_Datatype : fake dummy structure for casting object with
 * handles for more compact code
 *
 */

struct OCI_Datatype
{
    void *handle;   /* OCI handle */
    ub4   hstate;   /* object variable state */
};

typedef struct OCI_Datatype OCI_Datatype;

/*
 * OCI_SQLCmdInfo : Oracle SQL commands code and verbs
 *
 */

struct OCI_SQLCmdInfo
{
    unsigned int code;  /* SQL command code */
    const otext *verb;  /* SQL command verb */
};

typedef struct OCI_SQLCmdInfo OCI_SQLCmdInfo;

/* OCI context */

struct OCI_Context
{
    void       * source_ptr;
    const char * location;
    unsigned int source_type;
};

typedef struct OCI_Context OCI_Context;

/* static and unique OCI_Environment object */

extern OCI_Environment      Env;
extern const OCI_SQLCmdInfo SQLCmds[];

/* Start of Experimental section containing some Oracle opaque structure definitions

   These partial structures are "guessed" from memory analysis in order to
   find workarounds to bugs that Oracle does not / refuses to fix.

   These structures are not used in OCILIB unless specific environment variables are set
*/

/* The following structures contain definitions for a structure matching an Oracle Parameter
   retrieved from a statement handle when describing columns from resultsets.
   They were added in order to implement a workaround for the unfixed Oracle Bug 9838993
*/

struct OCIParamStructColumnInfo
{
    unsigned char unknown_fields[6 * sizeof(int) + 2 * sizeof(void*)];
    unsigned char attributes[sizeof(void *)];
    char         *name;
};

typedef struct OCIParamStructColumnInfo OCIParamStructColumnInfo;

struct OCIParamStruct
{
    unsigned char unknown_fields[3 * sizeof(void*)];

    OCIParamStructColumnInfo *column_info;
};

typedef struct OCIParamStruct OCIParamStruct;

/* End of Experimental section */

#endif /* OCILIB_TYPES_H_INCLUDED */
