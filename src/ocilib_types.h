/*
   +----------------------------------------------------------------------+
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+
*/

/* ------------------------------------------------------------------------ *
 * $Id: ocilib_types.h, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */


#ifndef OCILIB_OCILIB_TYPES_H_INCLUDED
#define OCILIB_OCILIB_TYPES_H_INCLUDED

#include "ocilib_defs.h"

/* ************************************************************************ *
 *                             PRIVATE TYPES
 * ************************************************************************ */

/*
 * OCI_Item : Internal list entry.
 *
 * The library needs to manage internal list of objects in order to be able to
 * free them if the application doest not.
 *
 * @note
 * Internal lists are using mutexes for resource locking in multithreaded
 * environments
 *
 */

struct OCI_Item
{
   void             *data; /* pointer to external data */
   struct OCI_Item  *next; /* next element in list */
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
    OCI_Item    *head;   /* pointer to first item */
    OCI_Mutex   *mutex;  /* mutex handle */
    ub4          count;  /* number of elements in list */
    int          type;   /* type of list item */
};

typedef struct OCI_List OCI_List;

/*
 * Server output object used to retrieve server dbms.output buffers
 *
 */

struct OCI_ServerOutput
{
    ub1                *arrbuf;    /* array buffer */
    unsigned int        arrsize;   /* array size */
    unsigned int        cursize;   /* number of filled items in the array */
    unsigned int        curpos;    /* current position in the array */
    unsigned int        lnsize;    /* line size */
    OCI_Statement      *stmt;      /* pointer to statement object (dbms_output calls) */
};

typedef struct OCI_ServerOutput OCI_ServerOutput;

/*
 * Connection trace information
 *
 */

struct OCI_TraceInfo
{
    mtext identifier[OCI_SIZE_TRACE_ID+1];
    mtext module[OCI_SIZE_TRACE_MODULE+1];
    mtext action[OCI_SIZE_TRACE_ACTION+1];
    mtext info[OCI_SIZE_TRACE_INF0+1];
};

typedef struct OCI_TraceInfo OCI_TraceInfo;

/* ************************************************************************ *
 *                             PUBLIC TYPES
 * ************************************************************************ */

/*
 * Error object
 *
 */

struct OCI_Error
{
    boolean          raise;                  /* Error flag */
    boolean          active;                 /* to avoid recursive exceptions */
    OCI_Connection  *con;                    /* pointer to connection object */
    OCI_Statement   *stmt;                   /* pointer to statement object */
    sb4              ocode;                  /* Oracle OCI error code */
    int              icode;                  /* OCILIB internal error code */
    mtext            str[OCI_SIZE_BUFFER+1]; /* error message */
    unsigned int     type;                   /* OCILIB error type */
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
    OCIThreadMutex  *handle;  /* OCI Mutex handle */
    OCIError        *err;     /* OCI Error handle */
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
 * from differents threads
 *
 */

struct OCI_ThreadKey
{
    OCIThreadKey    *handle;  /* OCI Thread key handle */
    OCIError        *err;     /* OCI Error handle */
};

typedef struct OCI_ThreadKey OCI_ThreadKey;

/*
 * OCI_Library : Internal OCILIB library encapsulation.
 *
 * It's a static, local and unique object that collects all the global variables
 * needed by the library
 *
 */

struct OCI_Library
{
    OCI_List       *cons;                   /* list of connection objects */
    OCI_List       *pools;                  /* list of pools objects */
    OCIEnv         *env;                    /* OCI environment handle */
    OCIError       *err;                    /* OCI error handle */
    POCI_ERROR      error_handler;          /* user defined error handler */
    ub1             ver_compile;            /* OCI version used at compile time */
    ub1             ver_runtime;            /* OCI version used at runtime */
    ub1             use_lob_ub8;            /* use 64 bits integers for lobs ? */
    ub1             use_scrollable_cursors; /* use Oracle 9i fetch API */
    ub4             env_mode;               /* default environment mode */
    boolean         loaded;                 /* OCILIB correctly loaded ? */
    OCI_Error       lib_err;                /* Error used when OCILIB is not loaded */
    OCI_HashTable  *key_map;                /* hash table for mapping name/key */
    OCI_ThreadKey  *key_errs;               /* Thread key to store thread errors */
    unsigned int    nb_hndlp;               /* number of OCI handles allocated */
    unsigned int    nb_descp;               /* number of OCI descriptors allocated */
    unsigned int    nb_objinst;             /* number of OCI objects allocated */
#ifdef OCI_IMPORT_RUNTIME
    LIB_HANDLE      lib_handle;             /* handle of runtime shared library */
#endif
};

typedef struct OCI_Library OCI_Library;

/*
 * Connection Pool object
 *
 */

struct OCI_ConnPool
{
    OCI_List        *cons;      /* list of connection objects */
#if OCI_VERSION_COMPILE >= OCI_9
    OCICPool        *handle;    /* OCI pool handle */
#else
    void            *handle;    /* fake handle for alignment */
#endif
    OCIError        *err;       /* OCI context handle */
    mtext           *name;      /* pool name */
    mtext           *db;        /* database */
    mtext           *user;      /* user */
    mtext           *pwd;       /* password */
    OCI_Mutex       *mutex;     /* mutex handle */
    ub4              mode;      /* session mode */
    ub4              min;       /* minimum of connections */
    ub4              max;       /* maximum of connections */
    ub4              incr;      /* increment step of connections */
    unsigned int     nb_busy;   /* number of busy connections */
    unsigned int     nb_opened; /* number of opened connections */
    unsigned int     timeout;   /* connection idle timeout */
    boolean          nowait;    /* wait for new connection */
};

/*
 * Connection object
 *
 */

struct OCI_Connection
{
    mtext              *db;        /* database */
    mtext              *user;      /* user */
    mtext              *pwd;       /* password */
    mtext              *version;   /* server version */
    OCI_List           *stmts;     /* list of statements */
    OCI_List           *trsns;     /* list of transactions */
    OCI_List           *tinfs;     /* list of type info objects */
    OCI_Transaction    *trs;       /* pointer to current transaction object */
    OCI_ConnPool       *pool;      /* pointer to connection pool parent */
    OCI_ServerOutput   *svopt;     /* Pointer to server output object */
    OCIServer          *svr;       /* OCI server handle */
    OCIError           *err;       /* OCI context handle */
    OCISession         *ses;       /* OCI session handle */
    OCISvcCtx          *cxt;       /* OCI context handle */
    boolean             autocom;   /* auto commit mode */
    unsigned int        nb_files;  /* number of OCI_File opened by the connection */
    unsigned int        mode;      /* session mode */
    int                 cstate;    /* connection state */
    void               *usrdata;   /* user data */
    mtext              *fmt_date;  /* date string format for conversion */
    mtext              *fmt_num;   /* numeric string format for conversion */
    unsigned int        ver_maj;   /* server version major number */
    unsigned int        ver_min;   /* server version minor number */
    unsigned int        ver_rev;   /* server version revision number */
    OCI_TraceInfo      *trace;     /* trace information */
};

/*
 * Transaction object
 *
 */

struct OCI_Transaction
{
    OCI_Connection  *con;       /* pointer to connection object */
    OCITrans        *htr;       /* OCI transaction handle */
    unsigned int     timeout;   /* timeout */
    unsigned int     mode;      /* transaction mode */
    boolean          local;     /* is local transaction ? */
    OCI_XID          xid;       /* global transaction identifier */
};

/*
 * Column object
 *
 */

struct OCI_Column
{
    /* 0racle infos */
    ub2              ocode;     /* Oracle SQL code */
    ub2              tcode;     /* Oracle type code */
    ub2              icode;     /* Internal translated Oracle SQL code */
    ub2              size;      /* SQL Size */
    sb2              prec;      /* SQL precision 1 (number prec, leading prec) */
    sb2              prec2;     /* SQL precision 2 (fractional prec) */
    sb1              scale;     /* SQL scale */
    ub1              type;      /* internal datatype */
    ub1              null;      /* is nullable */
    ub1              charused;  /* is column size expressed in characters */
    mtext           *name;      /* column name */
    ub2              charsize;  /* SQL Size in character */
    ub1              csfrm;     /* charset form */
    ub1              dtype;     /* oracle handle type */

    /* OCILIB infos */
    ub4              bufsize;   /* element size */
    OCI_TypeInfo    *typinf;    /* user type descriptor */
    ub4              subtype;   /* object type */
};

/*
 * OCI_Buffer : Internal input/output buffer
 *
 */

struct OCI_Buffer
{
    void            *handle;   /* OCI handle (bind or define) */
    void           **data;     /* data / array of data */
    void            *inds;     /* array of indicators */
    void            *lens;     /* array of lengths */
    dtext           *temp;     /* temporary buffer for string conversion */
    ub4              count;    /* number of elements in the buffer */
    int              sizelen;  /* size of an element in the lens array */
};

typedef struct OCI_Buffer OCI_Buffer;

/*
 * OCI_Bind object
 *
 */

struct OCI_Bind
{
    OCI_Statement   *stmt;      /* pointer to statement object */
    void           **input;     /* input values */
    mtext           *name;      /* name of the bind */
    sb4              size;      /* data size */
    OCI_Buffer       buf;       /* place holder */
    ub2              dynpos;    /* index of the bind for dynamic binds */
    ub2             *plrcds;    /* PL/SQL tables return codes */
    ub4              nbelem;    /* PL/SQL tables number of elements */
    OCI_TypeInfo    *typinf;    /* for object, collection and ref */
    ub1              type;      /* internal datatype */
    ub1              subtype;   /* internal subtype */
    ub2              code;      /* SQL datatype code */
    ub1              alloc;     /* is buffer allocated or mapped to input */
};

/*
 * OCI_Define : Internal Resultset column data implementation
 *
 */

struct OCI_Define
{
    OCI_Resultset   *rs;           /* pointer to resultset object */
    void            *obj;          /* current OCILIB object instance */
    OCI_Column       col;          /* column object */
    OCI_Buffer       buf;          /* placeholder */
};

typedef struct OCI_Define OCI_Define;

/*
 * Resultset object
 *
 */

struct OCI_Resultset
{
    OCI_Statement   *stmt;          /* pointer to statement object */
    OCI_HashTable   *map;           /* hash table handle for mapping name/index */
    OCI_Define      *defs;          /* array of define objects */
    ub4              nb_defs;       /* number of elements */
    ub4              row_cur;       /* actual position in the array of rows */
    ub4              row_abs;       /* absolute position in the resultset */
    ub4              row_count;     /* number of rows fetched so far */
    ub4              row_fetched;   /* rows fetched by last call (scrollable) */
    boolean          eof;           /* end of resultset reached ?  */
    boolean          bof;           /* beginning of resultset reached ?  */
    ub4              fetch_size;    /* internal array size */
    sword            fetch_status;  /* internal fetch status */

};

/*
 * Statement object
 *
 */

struct OCI_Statement
{
    OCIStmt         *stmt;          /* OCI statement handle */
    ub4              hstate;        /* object variable state */
    OCI_Resultset  **rsts;          /* pointer to resultset list */
    OCI_Connection  *con;           /* pointer to connection object */
    mtext           *sql;           /* SQL statement */
    OCI_Bind       **ubinds;        /* array of user bind objects */
    OCI_Bind       **rbinds;        /* array of register bind objects */
    OCI_HashTable   *map;           /* hash table handle for mapping bind name/index */
    ub2              nb_ubinds;     /* number of elements in the bind array */
    ub2              nb_rbinds;     /* number of output binds */
    boolean          bind_reuse;    /* rebind data allowed ? */
    unsigned int     bind_mode;     /* type of binding */
    ub4              exec_mode;     /* type of execution */
    ub4              fetch_size;    /* fetch array size */
    ub4              prefetch_size; /* pre-fetch size */
    ub4              prefetch_mem;  /* pre-fetch memory */
    ub4              long_size;     /* default size for LONG columns */
    ub1              long_mode;     /* LONG datatype handling mode */
    ub1              status;        /* statement status */
    ub2              type;          /* type of SQL statement */
    ub4              nb_iters;      /* number of iterations for execution */
    ub4              nb_rs;         /* number of resultsets */
    ub2              cur_rs;        /* index of the current resultset */
    ub2              dynidx;        /* bind index counter for dynamic exec */
    ub2              err_pos;       /* error position in sql statement */
    ub2              bind_array;    /* has array binds ? */
};

/*
 * Internal Large object
 *
 */

struct OCI_Lob
{
    OCILobLocator   *handle;    /* OCI handle */
    ub4              hstate;    /* object variable state */
    OCI_Connection  *con;       /* pointer to connection object */
    ub4              type;      /* type of lob */
    big_uint         offset;    /* current offset for R/W */
};

/*
 * External Large object
 *
 */

struct OCI_File
{
    OCILobLocator   *handle;    /* OCI handle */
    ub4              hstate;    /* object variable state */
    OCI_Connection  *con;       /* pointer to connection object */
    mtext           *dir;       /* directory name */
    mtext           *name;      /* file name */
    ub4              type;      /* type of file */
    big_uint         offset;    /* current offset for read */
};

/*
 * Long object
 *
 */

struct OCI_Long
{
    OCI_Statement   *stmt;      /* pointer to statement object */
    ub4              hstate;    /* object variable state */
    OCI_Define      *def;       /* pointer to resultset define object */
    ub4              size;      /* size of the buffer read / written */
    unsigned int     type;      /* type of long */
    ub4              offset;    /* current offset for R/W */
    ub4              piecesize; /* size of current fetched piece */
    ub4              maxsize;   /* current offset for R/W */
    ub1             *buffer;    /* fetched buffer */
};

/*
 * Date object
 *
 */

struct OCI_Date
{
    OCIDate         *handle;    /* OCI handle */
    ub4              hstate;    /* object variable state */
    OCI_Connection  *con;       /* pointer to connection object */
    OCIError        *err;       /* OCI context handle */
    ub4              allocated; /* is handle allocated ? */
};

/*
 * Timestamp object
 *
 */

struct OCI_Timestamp
{
#if OCI_VERSION_COMPILE >= OCI_9
    OCIDateTime     *handle;    /* OCI handle */
#else
    void            *handle;    /* fake handle for alignment */
#endif
    ub4              hstate;    /* object variable state */
    OCI_Connection  *con;       /* pointer to connection object */
    OCIError        *err;       /* OCI context handle */
    ub4              type;      /* sub type */
};

/*
 * Interval object
 *
 */

struct OCI_Interval
{
#if OCI_VERSION_COMPILE >= OCI_9
    OCIInterval     *handle;    /* OCI handle */
#else
    void            *handle;    /* fake handle for alignment */
#endif
    ub4              hstate;    /* object variable state */
    OCI_Connection  *con;       /* pointer to connection object */
    OCIError        *err;       /* OCI context handle */
    ub4              type;      /* sub type */
};

/*
 * Oracle Named type object
 *
 */

struct OCI_Object
{
    void             *handle;   /* OCI handle */
    ub4               hstate;   /* object variable state */
    OCI_Connection   *con;      /* pointer to connection object */
    OCI_TypeInfo     *typinf;   /* pointer to type info object */
    void            **objs;     /* array of OCILIB sub objects */
    void             *buf;      /* buffer to store converted out string attribute */
    int               buflen;   /* buffer length */
    sb2              *tab_ind;  /* indicators for root instance */
    ub2               idx_ind;  /* instance indicator offset / indicator table */
    OCIObjectLifetime type;     /* object type */
};

/*
 * Oracle Collection Item object
 *
 */

struct OCI_Elem
{
    void            *handle;   /* OCI handle */
    ub4              hstate;   /* object variable state */
    OCI_Connection  *con;      /* pointer to connection object */
    void            *obj;      /* OCILIB sub object */
    void            *buf;      /* buffer to store converted out string attribute */
    int              buflen;   /* buffer length */
    boolean          init;     /* underlying object has been initialized ? */
    OCI_TypeInfo    *typinf;   /* object type information */
    OCIInd          *ind;      /* data state indicator */
};

/*
 * Oracle Collection object
 *
 */

struct OCI_Coll
{
    OCIColl           *handle;   /* OCI handle */
    ub4                hstate;   /* object variable state */
    OCI_Connection    *con;      /* pointer to connection object */
    OCI_TypeInfo      *typinf;   /* pointer to type info object */
    OCI_Elem          *elem;     /* item object */
    sb4                size;     /* collection size */
};

/*
 * Oracle Iterator object
 *
 */

struct OCI_Iter
{
    OCIIter           *handle;   /* OCI handle */
    OCI_Coll          *coll;     /* pointer to connection object */
    OCI_Elem          *elem;     /* item object */
    boolean            eoc;      /* end of collection */
    boolean            boc;      /* beginning of collection */
};

/*
 * Oracle REF object
 *
 */

struct OCI_Ref
{
    OCIRef            *handle;   /* OCI handle */
    ub4                hstate;   /* object variable state */
    OCI_Connection    *con;      /* pointer to connection object */
    OCI_TypeInfo      *typinf;   /* pointer to type info object */
    OCI_Object        *obj;      /* Pinned object */
    boolean            pinned;   /* is the reference pinned */
};

/*
 * Type info object
 *
 */

struct OCI_TypeInfo
{
    OCI_Connection  *con;        /* pointer to connection object */
    mtext           *name;       /* name of the type info object */
    mtext           *schema;     /* owner of the type info object */
    unsigned int     type;       /* type of type info handle */
    OCIType         *tdo;        /* datatype object type */
    ub2              tcode;      /* Oracle type code */
    ub2              ccode;      /* Oracle collection code */
    OCI_Column      *cols;       /* array of column datatype info */
    ub2              nb_cols;    /* number of columns */
    ub2              refcount;   /* reference counter */
};

/*
 * OCI_DirPathColumn : Internal Direct Path column object
 *
 */

struct OCI_DirPathColumn
{
    ub4              format_size; /* size of the column format */
    mtext           *format;      /* date or numeric format */
    ub2              maxsize;     /* input max size */
    ub2              type;        /* column type */
    ub2              sqlcode;     /* sql type */
    ub1             *data;        /* array of data */
    ub1             *flags;       /* array of row flags */
    ub4             *lens;        /* array of lengths */
    ub2              bufsize;     /* buffer size */
    ub2              index;       /* ref index in the type info columns list */
};

typedef struct OCI_DirPathColumn OCI_DirPathColumn;

/*
 * Oracle Direct Path column object
 *
 */

struct OCI_DirPath
{
    OCI_Connection      *con;       /* pointer to connection object */
    OCI_TypeInfo        *typinf;    /* type info about table to load */
    OCIDirPathCtx       *ctx;       /* OCI DP context handle */
    OCIDirPathColArray  *arr;       /* OCI DP column array handle */
    OCIDirPathStream    *strm;      /* OCI DP stream handle */
    OCI_DirPathColumn   *cols;      /* array of column info */
    ub4                  nb_cols;   /* number of columns to load */
    ub4                  nb_rows;   /* maximum number of row to load per stream */
    ub4                  nb_cur;    /* current number of row to load per stream */
    ub4                  nb_loaded; /* number of row loaded so far */
    ub4                  nb_prcsd;  /* number of row processed at last call */
    sb2                  err_col;   /* index of the column not processed at last call */
    sb2                  err_row;   /* index of the row not processed at last call */
    ub4                  status;    /* internal status */
};

/*
 * Hash table object
 *
 */

struct OCI_HashTable
{
    OCI_HashEntry    **items;     /* array of slots */
    unsigned int       size;      /* size of the slots array */
    unsigned int       count;     /* number of used slots */
    unsigned int       type;      /* type of data */
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


/* static and unique OCI_Library object */

extern OCI_Library OCILib;

#endif /* OCILIB_OCILIB_TYPES_H_INCLUDED */

