/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include <threads.h>

/* ********************************************************************************************* *
 *                             PRIVATE TYPES
 * ********************************************************************************************* */

/*
* OCI_MemoryBlock : Internal block of Memory.
*
*/

struct OCI_MemoryBlock
{
    OCI_PointerType type; /* type of allocated data */
    OCI_Size        size;   /* allocated memory size, header included */
};

typedef struct OCI_MemoryBlock OCI_MemoryBlock;

/*
 * OCI_Logger : Internal logging state.
 *
 */

struct OCI_Logger
{
    OCI_LogLevel    level;                          /* current global level */
    OCI_LogCategory categories;                     /* bitmask of OCI_LogCategory */
    FILE           *file;                           /* output stream or NULL for stderr */
    OCI_Boolean     file_owned;                         /* close file on cleanup ? */
    OCI_Boolean     initialized;                        /* TRUE once the sink and lock are ready */
    OCI_Int32       ref_count;                            /* number of environments using the logger
                                                          */
    otext            output_path[OCI_SIZE_BUFFER + 1]; /* cached path as otext */
    POCI_LOG_HANDLER handler;                       /* user supplied log callback */

    /* The logger lock is a file-static mtx_t in log.c rather than a member:
       it must outlive every OCI_Environment and is created once via
       call_once(), so it cannot share this struct's lifetime. */
};

typedef struct OCI_Logger OCI_Logger;

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

#define OCI_HANDLE(HType)       \
        OCI_Environment *env;   \
        HType          *handle; \
        OCI_PointerType htype;  \
        void           *usrdata

#define OCI_DATATYPE(HType) \
        OCI_HANDLE(HType);  \
        ub4 hstate

#define DECLARE_OCI_HANDLE(Name, HType, ...) \
        struct Name                          \
        {                                    \
            OCI_HANDLE(HType);               \
            __VA_ARGS__                      \
        }

#define DECLARE_OCI_DATATYPE(Name, HType, ...) \
        struct Name                            \
        {                                      \
            OCI_DATATYPE(HType);               \
            __VA_ARGS__                        \
        }

/*
 * OCI_List : Internal list object.
 *
 * The OCI_List object is used to maintain a collection of handles allocated
 * by user programs.
 *
 * Those handles are freed when the collection owner is destroyed.
 * So, we make sure that if OCI_EnvironmentFree() is called, all allocated handles will
 * be destroyed even if the program does not free them.
 *
 */

struct OCI_List
{
    OCI_HANDLE(void);
    OCI_Item       *head;      /* pointer to first item */
    mtx_t           mutex;     /* mutex handle */
    OCI_PointerType type;      /* type of list item */
    ub4             count;     /* number of elements in list */
};

typedef struct OCI_List OCI_List;

struct OCI_Handle
{
    OCI_HANDLE(void);
};

typedef struct OCI_Handle OCI_Handle;

/*
 * OCI_DataType : fake dummy structure for casting object with
 * handles for more compact code
 *
 */

struct OCI_DataType
{
    OCI_HANDLE(void);
    ub4 hstate;
};

typedef struct OCI_DataType OCI_DataType;

#if OCI_VERSION_COMPILE >= OCI_9_0
#define OCI_TS_HANDLE OCIDateTime
#define OCI_IV_HANDLE OCIInterval
#else
#define OCI_TS_HANDLE void
#define OCI_IV_HANDLE void
#endif

#if OCI_VERSION_COMPILE >= OCI_23_4
#define OCI_VC_HANDLE OCIVector
#else
#define OCI_VC_HANDLE void
#endif

#if OCI_VERSION_COMPILE >= OCI_21_1
#define OCI_JS_HANDLE OCIJson
#else
#define OCI_JS_HANDLE void
#endif

#if OCI_VERSION_COMPILE >= OCI_21_1
#define OCI_JSN_HANDLE JsonDomNode
#else
#define OCI_JSN_HANDLE void
#endif

/*
 * Server output object used to retrieve server dbms.output buffers
 *
 */

struct OCI_ServerOutput
{
    OCI_Statement *stmt;           /* pointer to statement object (dbms_output calls) */
    ub1           *arrbuf;         /* array buffer */
    OCI_UInt32     arrsize;      /* array size */
    OCI_UInt32     cursize;      /* number of filled items in the array */
    OCI_UInt32     curpos;       /* current position in the array */
    OCI_UInt32     lnsize;       /* line size */
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

/*
 * OCI_LocaleStringInfo : Oracle Localized strings
 *
 */

#define OCI_LSI_COUNT 70

struct OCI_LocaleStringInfo
{
    OCI_UInt32 code;  /* local string code */
    otext     *str;     /* local string content */
};

typedef struct OCI_LocaleStringInfo OCI_LocaleStringInfo;

/* ********************************************************************************************* *
 *                             PUBLIC TYPES
 * ********************************************************************************************* */

struct OCI_Error
{
    OCI_HANDLE(void);
    void           *source_ptr;     /* pointer to source ptr */
    otext          *location;       /* location (method) */
    otext          *message;        /* error message */
    OCI_ErrorType   type;           /* OCILIB error type */
    OCI_PointerType source_type;    /* source type */
    OCI_ErrorCode   code;           /* Oracle OCI or OCILIB internal error code */
    OCI_UInt32      location_len; /* length of error location */
    OCI_UInt32      message_len;  /* length of error message */
    ub4             row;            /* Error row offset (array DML) */
    OCI_Boolean     active;             /* to avoid recursive exceptions */
    OCI_Boolean     warnings_on;        /* snapshot of the raising env's warnings flag --
                                           OCI_Error may outlive its environment */
};

/*
 * OCI_Environment : Internal OCILIB library encapsulation.
 *
 * It's a static, local and unique object that collects all the global variables
 * needed by the library
 *
 */

struct OCI_Environment
{
    OCI_HANDLE(OCIEnv);
    OCI_Symbols    *api;                          /* per-environment OCI dispatch table */
    OCI_List       *cons;                         /* list of connection objects */
    OCI_List       *pools;                        /* list of pools objects */
    OCI_List       *subs;                         /* list of subscription objects */
    OCI_List       *arrs;                         /* list of arrays objects */
    OCIError       *err;                          /* OCI error handle */
    OCI_HashTable * sql_funcs;                    /* hash table handle for sql function names */
    mtx_t           mutex;                        /* per-environment recursive mutex (memory
                                                   * counters, locale cache) */
    otext               *formats[OCI_FMT_COUNT];  /* string conversion default formats */
    OCI_LocaleStringInfo locale_strings[OCI_LSI_COUNT]; /* per-environment locale string cache */
    POCI_ERROR           error_handler;           /* user defined error handler */
    POCI_HA_HANDLER      ha_handler;              /* HA event callback*/
#ifdef OCI_IMPORT_RUNTIME
    LIB_HANDLE lib_handle;                        /* handle of runtime shared library */
#endif
    OCI_UInt64  mem_bytes_lib;                      /* allocated bytes by OCILIB */
    OCI_UInt32  version_compile;                /* OCI version used at compile time */
    OCI_UInt32  version_runtime;                /* OCI version used at runtime */
    OCI_Charset charset;                          /* charset type */
    OCI_UInt32  nb_hndlp;                       /* number of OCI handles allocated */
    OCI_UInt32  nb_descp;                       /* number of OCI descriptors allocated */
    OCI_UInt32  nb_objinst;                     /* number of OCI objects allocated */
    ub4         env_mode;                         /* default environment mode */
    OCI_Boolean loaded;                               /* OCILIB correctly loaded ? */
    OCI_Boolean nls_utf8;                             /* is UFT8 enabled for data strings ? */
    OCI_Boolean use_lob_ub8;                          /* use 64 bits integers for lobs ? */
    OCI_Boolean use_scrollable_cursors;               /* use Oracle 9i fetch API */
    OCI_Boolean use_wide_char_conv;                   /* are we on a unix like platform with unicode
                                                      */
    OCI_Boolean warnings_on;                          /* warnings enabled ? */
    OCI_Boolean env_vars[OCI_VARS_COUNT];             /* specific environment variables */
};

/*
 * Pool object
 *
 */

struct OCI_Pool
{
    OCI_HANDLE(void);
    void           *authp;      /* OCI authentication handle */
    OCIError       *err;        /* OCI context handle */
    otext          *name;       /* pool name */
    otext          *db;         /* database */
    otext          *user;       /* user */
    otext          *pwd;        /* password */
    OCI_SessionMode mode;       /* session mode */
    ub4             min;        /* minimum of objects */
    ub4             max;        /* maximum of objects */
    ub4             incr;       /* increment step of objects */
    ub4             kind;        /* handle type of pool : connection / session */
    ub4             cache_size;  /* statement cache size */
};

/*
 * Connection object
 *
 */

struct OCI_Connection
{
    OCI_HANDLE(OCISvcCtx);
    OCIServer        *svr;                      /* OCI server handle */
    OCIError         *err;                      /* OCI context handle */
    OCISession       *ses;                      /* OCI session handle */
    OCI_List         *stmts;                    /* list of statements */
    OCI_List         *trsns;                    /* list of transactions */
    OCI_List         *tinfs;                    /* list of type info objects */
    OCI_Transaction  *trs;                      /* pointer to current transaction object */
    OCI_Pool         *pool;                     /* pointer to parent pool object */
    OCI_ServerOutput *svopt;                    /* Pointer to server output object */
    OCI_TraceInfo    *trace;                    /* trace information */
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
    OCI_Timestamp    *inst_startup;             /* instance startup timestamp */
    POCI_TAF_HANDLER  taf_handler;              /* failover call back */
    OCI_Boolean       autocom;                      /* auto commit mode */
    OCI_Boolean       alloc_handles;                /* do new need to allocate OCI handles ? */
    OCI_UInt32        nb_files;               /* number of OCI_File opened by the connection */
    OCI_SessionMode   mode;                     /* session mode */
    OCI_Int32         cstate;                         /* connection state */
    OCI_UInt32        ver_num;                /* numeric server version */
};

/*
 * Transaction object
 *
 */

struct OCI_Transaction
{
    OCI_HANDLE(OCITrans);
    OCI_Connection     *con;    /* pointer to connection object */
    OCI_XID             xid;    /* global transaction identifier */
    OCI_UInt32          timeout; /* timeout */
    OCI_TransactionMode mode;   /* transaction mode */
    OCI_Boolean         local;      /* is local transaction ? */
    OCI_Boolean         started;    /* is the transaction currently started ? */
};

/*
 * Column object
 *
 */

struct OCI_Column
{
    /* OCILIB infos */
    OCI_HANDLE(void);
    OCI_TypeInfo   *typinf;         /* user type descriptor */
    OCI_Connection *con;            /* owning connection */
    ub4             bufsize;        /* element size */
    OCI_SubType     subtype;        /* Ocilib sub type */
    OCI_NumericType struct_subtype; /* Ocilib numeric sub type for OCI_GetStruct() :( */
    /* Oracle infos */
    otext              *name;       /* column name */
    ub2                 sqlcode;    /* Oracle SQL code */
    ub2                 typecode;   /* Oracle type code */
    ub2                 libcode;    /* Internal translated Oracle SQL code */
    ub2                 size;       /* SQL Size */
    sb2                 prec;       /* SQL precision 1 (number prec, leading prec) */
    sb2                 prec2;      /* SQL precision 2 (fractional prec) */
    sb1                 scale;      /* SQL scale */
    OCI_SqlType         datatype;   /* internal datatype */
    ub1                 nullable;   /* is nullable */
    ub1                 charused;   /* is column size expressed in characters */
    ub2                 charsize;   /* SQL Size in character */
    OCI_CharsetForm     csfrm;      /* charset form */
    ub1                 handletype; /* oracle handle type */
    OCI_ColumnPropFlags props;      /* column properties */
    ub4                 collation_id; /* collation id */
    ub4                 dimension;  /* dimension */
};

/*
 * OCI_Buffer : Internal input/output place holder
 *
 */

struct OCI_Buffer
{
    void      *handle;             /* OCI handle (bind or define) */
    void     **data;               /* data / array of data */
    void      *lens;               /* array of lengths */
    otext     *tmpbuf;             /* temporary buffer */
    OCIInd    *inds;               /* indicators */
    void     **obj_inds;           /* object indicators */
    ub4        count;              /* number of elements in the buffer */
    OCI_Int32  sizelen;                  /* size of an element in the lens array */
    OCI_UInt32 tmpsize;          /* temporary buffer size */
};

typedef struct OCI_Buffer OCI_Buffer;

/*
 * OCI_Bind object
 *
 */

struct OCI_Bind
{
    OCI_HANDLE(void);
    OCI_Statement         *stmt; /* pointer to statement object */
    void                 **input; /* input values */
    otext                 *name; /* name of the bind */
    OCI_TypeInfo         * typinf; /* for object, collection and ref */
    ub2                   *plrcds; /* PL/SQL tables return codes */
    OCI_Buffer             buffer; /* place holder */
    OCI_Boolean            is_array; /* is it an array bind ? */
    sb4                    size; /* data size */
    ub4                    nbelem; /* PL/SQL tables number of elements */
    ub2                    dynpos; /* index of the bind for dynamic binds */
    ub2                    code; /* SQL datatype code */
    OCI_SqlType            type; /* internal datatype */
    OCI_SubType            subtype; /* internal subtype */
    ub1                    alloc; /* is buffer allocated or mapped to input */
    OCI_CharsetForm        csfrm; /* charset form */
    OCI_BindDirection      direction; /* in, out or in/out bind */
    OCI_BindAllocationMode alloc_mode; /* allocation mode : internal or external */
};

/*
 * OCI_Define : Internal Resultset column data implementation
 *
 */

struct OCI_Define
{
    OCI_HANDLE(void);
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
    OCI_HANDLE(OCIStmt);
    OCI_Statement *stmt;            /* pointer to statement object */
    OCI_HashTable *map;             /* hash table handle for mapping name/index */
    OCI_Define    *defs;            /* array of define objects */
    ub4            nb_defs;         /* number of elements */
    ub4            row_cur;         /* actual position in the array of rows */
    ub4            row_abs;         /* absolute position in the resultset */
    ub4            row_count;       /* number of rows fetched so far */
    ub4            row_fetched;     /* rows fetched by last call (scrollable) */
    ub4            fetch_size;      /* internal array size */
    OCI_Boolean    eof;                 /* end of resultset reached ?  */
    OCI_Boolean    bof;                 /* beginning of resultset reached ?  */
    sword          fetch_status;    /* internal fetch status */
};

/*
 * OCI_BatchErrors : Internal Resultset batch errors
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
 * OCI_ParseBinds : Parse Bind informations
 *
 */

struct OCI_ParseBinds
{
    OCI_UInt32 count;
    otext    **names;
};

typedef struct OCI_ParseBinds OCI_ParseBinds;

/*
 * Statement object
 *
 */

struct OCI_Statement
{
    OCI_DATATYPE(OCIStmt);
    OCI_FetchMode          exec_mode;   /* type of execution */
    OCI_Resultset        **rsts;        /* pointer to resultset list */
    OCI_Statement        **stmts;       /* pointer to implicit statement resultset list */
    OCI_Connection        *con;         /* pointer to connection object */
    otext                 *sql;         /* SQL statement */
    otext                 *sql_id;      /* server statement sql id */
    OCI_Bind             **ubinds;      /* array of user bind objects */
    OCI_Bind             **rbinds;      /* array of register bind objects */
    OCI_HashTable         *map;         /* hash table handle for mapping bind name/index */
    OCI_BatchErrors       *batch;       /* error handling for array DML */
    OCI_ParseBinds        *parse_binds; /* parse bind info */
    ub2                    nb_ubinds;   /* number of used user binds */
    ub2                    nb_rbinds;   /* number of used register binds */
    ub2                    allocated_ubinds; /* number of allocated user binds */
    ub2                    allocated_rbinds; /* number of allocated register binds */
    OCI_Boolean            bind_array;      /* has array binds ? */
    OCI_Boolean            bind_reuse;      /* rebind data allowed ? */
    OCI_BindMode           bind_mode;   /* type of binding */
    OCI_BindAllocationMode bind_alloc_mode; /* type of bind allocation */
    ub4                    fetch_size;  /* fetch array size */
    ub4                    prefetch_size; /* pre-fetch size */
    ub4                    prefetch_mem; /* pre-fetch memory */
    ub4                    piece_size;  /* default piece size for dynamic fetchs */
    OCI_LongMode           long_mode;   /* LONG datatype handling mode */
    ub1                    status;      /* statement status */
    OCI_StatementType      type;        /* type of SQL statement */
    ub4                    nb_iters;    /* current number of iterations for execution */
    ub4                    nb_iters_init; /* initial number of iterations for execution */
    ub4                    nb_stmt;     /* number of implicit statements */
    ub4                    nb_rs;       /* number of resultsets */
    ub2                    cur_rs;      /* index of the current resultset */
    ub2                    dynidx;      /* bind index counter for dynamic exec */
    ub2                    err_pos;     /* error position in sql statement */
};

/*
 * Internal Large object
 *
 */

struct OCI_Lob
{
    OCI_DATATYPE(OCILobLocator);
    OCI_LobType     type;           /* type of lob */
    OCI_Connection *con;            /* pointer to connection object */
    OCI_UInt64      offset;           /* current offset for R/W */
};

/*
 * External Large object
 *
 */

struct OCI_File
{
    OCI_DATATYPE(OCILobLocator);
    OCI_FileType    type;       /* type of file */
    OCI_Connection *con;        /* pointer to connection object */
    otext          *dir;        /* directory name */
    otext          *name;       /* file name */
    OCI_UInt64      offset;       /* current offset for read */
};

/*
 * Long object
 *
 */

struct OCI_Long
{
    OCI_DATATYPE(OCI_Statement);
    OCI_LongType type;          /* type of long */
    OCI_Define  *def;           /* pointer to resultset define object */
    ub4          size;          /* size of the buffer read / written */
    ub4          offset;        /* current offset for R/W */
    ub4          piecesize;     /* size of current fetched piece */
    ub4          maxsize;       /* size to R/W */
    ub1         *buffer;        /* fetched buffer */
};

/*
 * XmlType object
 *
 */

struct OCI_XmlType
{
    OCI_DATATYPE(OCI_Long);
    OCI_Connection *con;        /* pointer to connection object */
};

/*
 * Vector object
 *
 */
struct OCI_Vector
{
    OCI_DATATYPE(OCI_VC_HANDLE);
    OCI_Connection *con;        /* pointer to connection object */
};

/*
* Number object
*
*/

struct OCI_Number
{
    OCI_DATATYPE(OCINumber);
};

/*
 * Date object
 *
 */

struct OCI_Date
{
    OCI_DATATYPE(OCIDate);
    ub4 allocated;              /* is handle allocated ? */
};

/*
 * Timestamp object
 *
 */

struct OCI_Timestamp
{
    OCI_DATATYPE(OCI_TS_HANDLE);
    OCI_TimestampType type;     /* sub type */
};

/*
 * Interval object
 *
 */

struct OCI_Interval
{
    OCI_DATATYPE(OCI_IV_HANDLE);
    OCI_IntervalType type;      /* sub type */
};

/*
 * JSON document object
 *
 */

struct OCI_Json
{
    OCI_DATATYPE(OCI_JS_HANDLE);
    OCI_Connection *con;     /* pointer to connection object */
    JsonDomDoc     *jdoc;      /* cached DOM, populated on first access */
};

/*
 * JSON DOM node object
 *
 */

struct OCI_JsonNode
{
    OCI_DATATYPE(OCI_JSN_HANDLE);
    OCI_Json    *json;       /* parent document; keeps the DOM alive */
    OCI_JsonType type;        /* cached node type */
    otext       *str;         /* cached otext conversion of DOM string/field name */
};

/*
 * Oracle Named type object
 *
 */

struct OCI_Object
{
    OCI_DATATYPE(void);
    OCIObjectLifetime type;           /* object type */
    OCI_Connection   *con;            /* pointer to connection object */
    OCI_TypeInfo     *typinf;         /* pointer to type info object */
    void            **objs;           /* array of OCILIB sub objects */
    otext           **tmpbufs;        /* temporary buffer  per column */
    OCI_UInt32       *tmpsizes;     /* temporary buffer size per column */
    sb2              *tab_ind;        /* indicators for root instance */
    ub2               idx_ind;        /* instance indicator offset / indicator table */
};

/*
 * Oracle Collection Item object
 *
 */

struct OCI_Elem
{
    OCI_DATATYPE(void);
    OCI_UInt32    tmpsize;     /* temporary buffer size */
    void         *obj;           /* OCILIB sub object */
    otext        *tmpbuf;        /* temporary buffer */
    OCI_TypeInfo *typinf;        /* object type information */
    OCIInd       *pind;          /* indicator  pointer */
    uword         alloc;         /* is handle locally allocated */
    ub2           init;          /* underlying object has been initialized ? */
    OCIInd        ind;           /* internal temporary data state indicator */
};

/*
 * Oracle Collection object
 *
 */

struct OCI_Coll
{
    OCI_DATATYPE(OCIColl);

    /* padding here in 64bits - cannot avoid it as this struct can be reinterpretated as
     * OCI_Datatype */
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
    OCI_HANDLE(OCIIter);
    OCI_Coll   *coll;            /* pointer to connection object */
    OCI_Elem   *elem;            /* item object */
    OCI_Boolean eoc;                 /* end of collection */
    OCI_Boolean boc;                 /* beginning of collection */
    OCI_Boolean dirty;               /* indicate if elem is dirty, e.g. not fetched yet */
};

/*
 * Oracle REF object
 *
 */

struct OCI_Ref
{
    OCI_DATATYPE(OCIRef);
    OCI_Boolean     pinned;          /* is the reference pinned */
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
    OCI_HANDLE(OCIType);
    OCI_Connection  *con;        /* pointer to connection object */
    OCI_Column      *cols;       /* array of column datatype info */
    OCI_TypeInfo    *parent_type; /* parent super type for derived type */
    otext           *name;       /* name of the type info object */
    otext           *schema;     /* owner of the type info object */
    OCI_Int32       *offsets;          /* cached offsets */
    OCI_Size         struct_size;   /* cached structure size */
    OCI_Size         align;        /* cached structure alignment */
    OCI_TypeInfoType type;       /* type of type info handle */
    ub2              typecode;   /* Oracle type code */
    ub2              colcode;    /* Oracle collection code */
    ub2              nb_cols;    /* number of columns */
    ub1              is_final;   /* is is a virtual type that can be inherited ? */
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
    OCI_HANDLE(OCIDirPathCtx);
    OCI_Connection     *con;            /* pointer to connection object */
    OCI_TypeInfo       *typinf;         /* type info about table to load */
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
    OCI_UInt32          res_conv;     /* status of the last conversion */
    OCI_UInt32          res_load;     /* status of the last load */
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
    OCI_EventType     type;            /* event type */
    OCI_UInt32        objname_size;  /* altered object name size */
    OCI_UInt32        rowid_size;    /* altered row id size */
    OCI_UInt32        dbname_size;   /* database name size */
};

/*
 * Oracle Notification object
 *
 */

struct OCI_Subscription
{
    OCI_HANDLE(OCISubscription);
    OCI_Connection      *con;        /* OCILIB connection handle */
    OCIError            *err;        /* OCI error handle  */
    otext               *name;       /* notification name */
    otext               *saved_db;   /* database for reconnection if needed */
    otext               *saved_user; /* user for reconnection if needed */
    otext               *saved_pwd;  /* password for reconnection if needed */
    POCI_NOTIFY          handler;    /* user callback */
    OCI_Event            event;      /* event object for user callback */
    OCI_SubscriptionType type;     /* notification type */
    ub4                  timeout;    /* notification timeout */
};

/*
 * Oracle A/Q Agent
 *
 */

struct OCI_Agent
{
    OCI_DATATYPE(OCIAQAgent);

    /* padding here in 64bits - cannot avoid it as this struct can be reinterpretated as
     * OCI_Datatype */
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
    OCI_HANDLE(OCIAQMsgProperties);
    OCI_TypeInfo *typinf;              /* pointer to type info object */
    void         *payload;             /* message payload (object handle or raw handle) */
    OCIRaw       *id;                  /* message identifier */
    OCI_Date     *date;                /* enqueue date */
    otext        *correlation;         /* correlation string */
    otext        *except_queue;        /* exception queue name */
    OCI_Agent    *sender;              /* sender */
    OCI_Object   *obj;                 /* OCILIB object handle for object payloads */
    OCIInd        ind;                 /* message payload indicator pointer */
};

/*
 * Oracle A/Q enqueue
 *
 */

struct OCI_Enqueue
{
    OCI_HANDLE(OCIAQEnqOptions);
    OCI_TypeInfo *typinf;            /* pointer to type info object */
    otext        *name;              /* queue name */
};

/*
 * Oracle A/Q Dequeue
 *
 */

struct OCI_Dequeue
{
    OCI_HANDLE(OCIAQDeqOptions);
    OCI_TypeInfo    *typinf;             /* pointer to type info object */
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
 * Oracle SODA database
 *
 * There is no corresponding OCI handle: all SODA calls operate on the
 * service context of the owning OCI_Connection.
 */

struct OCI_SodaDb
{
    OCI_HANDLE(void);
    OCI_Connection *con;             /* parent connection (not owned) */
};

/*
 * Oracle SODA collection
 *
 */

struct OCI_SodaColl
{
    OCI_HANDLE(OCISodaColl);
    OCI_SodaDb *sdb;                 /* parent SODA database (not owned) */
    ub1         content_sql_type;    /* OCI_ATTR_SODA_CTNT_SQL_TYPE */
    ub1         content_format;      /* OCI_ATTR_SODA_CTNT_FORMAT */
    OCI_Boolean binary_content;      /* TRUE when content is binary */
    otext      *name;                /* collection name */
    otext      *descriptor;          /* collection metadata descriptor */
};

/*
 * Oracle SODA document
 *
 */

struct OCI_SodaDoc
{
    OCI_HANDLE(OCISodaDoc);
    OCI_SodaDb *sdb;                  /* parent SODA database (not owned) */
    void       *content;              /* cached document content */
    otext      *content_text;         /* cached content converted to otext */
    otext      *key;                  /* document key */
    otext      *version;              /* document version */
    otext      *created_on;           /* creation timestamp */
    otext      *last_modified;        /* last modification timestamp */
    otext      *media_type;           /* document media type */
    ub4         content_len;          /* cached document content byte length */
    ub4         content_text_size;    /* size of content_text buffer */
    OCI_Boolean content_is_json;      /* TRUE when content is an OCIJson descriptor */
    OCI_Json   *json;                 /* cached JSON wrapper for descriptor content */
};

/*
 * Oracle SODA operation options
 *
 */

struct OCI_SodaOptions
{
    OCI_HANDLE(OCISodaOperationOptions);
    OCI_SodaDb *sdb;                  /* parent SODA database (not owned) */
    otext     **keys;                 /* cached keys for multi-key operations */
    OCI_UInt32  key_count;          /* number of cached keys */
};

/*
 * Oracle SODA collection cursor
 *
 */

struct OCI_SodaCollCursor
{
    OCI_HANDLE(OCISodaCollCursor);
    OCI_SodaDb *sdb;                  /* parent SODA database (not owned) */
};

/*
 * Oracle SODA document cursor
 *
 */

struct OCI_SodaDocCursor
{
    OCI_HANDLE(OCISodaDocCursor);
    OCI_SodaColl *coll;               /* parent SODA collection (not owned) */
    OCISodaDoc  **docs;               /* materialized documents (multi-key emulation) */
    OCI_UInt32    doc_count;        /* number of materialized documents */
    OCI_UInt32    doc_pos;          /* current position in materialized documents */
};

/*
 * OCILIB array
 *
 */

struct OCI_Array
{
    OCI_Environment *env;            /* owning environment */
    OCI_Connection  *con;            /* Connection object for type dependent on connections */
    void           **tab_obj;       /* array of pointers to OCILIB objects */
    void            *mem_handle;    /* array OCI handles */
    void            *mem_struct;    /* array of OCILIB objects structures */
    OCI_UInt32       elem_type;   /* array element type */
    OCI_UInt32       elem_subtype; /* array element subtype */
    OCI_UInt32       elem_size;   /* array element handle size */
    OCI_UInt32       nb_elem;     /* array size of number of elements */
    OCI_UInt32       struct_size; /* array element size */
    OCI_UInt32       handle_type; /* array element OCI handle type */
};

typedef struct OCI_Array OCI_Array;

/*
 * Hash table object
 *
 */

struct OCI_HashTable
{
    OCI_HANDLE(void);
    OCI_HashEntry **items;         /* array of slots */
    OCI_UInt32      size;        /* size of the slots array */
    OCI_UInt32      count;       /* number of used slots */
    OCI_HashType    type;          /* type of data */
};

/*
 * OCI_SQLCmdInfo : Oracle SQL commands code and verbs
 *
 */

struct OCI_SQLCmdInfo
{
    OCI_SQLCommand code;  /* SQL command code */
    const otext   *verb; /* SQL command verb */
};

typedef struct OCI_SQLCmdInfo OCI_SQLCmdInfo;

/* OCI context */

struct OCI_Context
{
    void           * source_ptr;
    const char     * location;
    OCI_PointerType  source_type;
    OCI_Environment *env;
};

typedef struct OCI_Context OCI_Context;

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
    unsigned char unknown_fields[6 * sizeof(OCI_Int32) + 2 * sizeof(void*)];
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
