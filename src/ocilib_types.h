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
   |               Copyright (c) 2007-2008 Vincent ROGIER                 |
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
 * $Id: ocilib_types.h, v 3.1.0 2008/10/26 07:50 Vince $
 * ------------------------------------------------------------------------ */


#ifndef OCILIB_OCILIB_TYPES_H_INCLUDED 
#define OCILIB_OCILIB_TYPES_H_INCLUDED 

#include "ocilib.h"
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

typedef struct OCI_Item {
   void             *data; /* pointer to external data */
   struct OCI_Item  *next; /* next element in list */
} OCI_Item;

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

typedef struct OCI_List {
    OCI_Item    *head;   /* pointer to first item */
    OCI_Mutex   *mutex;  /* mutex handle */
    ub4          count;  /* number of elements in list */
    int          type;   /* type of list item */
} OCI_List;

/* 
 * Server ouput object used to retreive server dbms.output buffers 
 * 
 */

typedef struct OCI_ServerOutput {
    ub1                *arrbuf;    /* array buffer */
    unsigned int        arrsize;   /* array size */
    unsigned int        cursize;   /* number of filled items in the array */
    unsigned int        curpos;    /* current position in the array */
    unsigned int        lnsize;    /* line size */
    OCI_Statement      *stmt;      /* pointer to statement object (dbms_output calls) */
} OCI_ServerOutput;


/* ************************************************************************ *
 *                             PUBLIC TYPES
 * ************************************************************************ */

/* 
 * Error object
 * 
 */

struct OCI_Error {
    boolean          raise;                 /* Error flag */
    boolean          active;                /* to avoid recursive exceptions */
    OCI_Connection  *con;                   /* pointer to connection object */
    OCI_Statement   *stmt;                  /* pointer to statement object */
    sb4              ocode;                 /* Oracle OCI error code */
    int              icode;                 /* OCILIB internal error code */ 
    mtext            str[OCI_SIZE_BUFFER];  /* error message */
    unsigned int     type;                  /* OCILIB error type */
};

/* 
 * Mutex object 
 *
 * Mutexes have their own error handle to avoid conflict using OCIErrorGet()
 * from differents threads
 *
 */

struct OCI_Mutex {
    OCIThreadMutex  *handle;  /* OCI Mutex handle */
    OCIError        *err;     /* OCI Error handle */
};

/*
 * Thread object
 *
 * Threads have their own error handle to avoid conflict using OCIErrorGet()
 *
 */

struct OCI_Thread {
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

typedef struct OCI_ThreadKey {
    OCIThreadKey    *handle;  /* OCI Thread key handle */
    OCIError        *err;     /* OCI Error handle */
} OCI_ThreadKey;

/* 
 * OCI_Library : Internal OCILIB library encapsulation.
 *
 * It's a static, local and unique object that collects all the global variables
 * needed by the library
 * 
 */

typedef struct OCI_Library {
    OCI_List       *cons;                   /* list of connection objects */
    OCI_List       *pools;                  /* list of pools objects */
    OCIEnv         *env;                    /* OCI environnement handle */
    OCIError       *err;                    /* OCI error handle */
    POCI_ERROR      error_handler;          /* user defined error handler */
    ub1             ver_compile;            /* OCI version used at compile time */
    ub1             ver_runtime;            /* OCI version used at runtime */
    ub1             use_lob_ub8;            /* use 64 bits integers for lobs ? */
    ub1             use_scrollable_cursors; /* use Oracle 9i fetch API */
    ub4             env_mode;               /* default environnement mode */
    boolean         loaded;                 /* OCILIB correclty loaded ? */
    OCI_Error       lib_err;                /* Error used when OCILIB is not loaded */
    OCI_HashTable  *key_map;                /* hash table for mapping name/key */
    OCI_ThreadKey  *key_errs;               /* Thread key to store thread errors */
    unsigned int    nb_hndlp;               /* number of OCI handles allocated */
    unsigned int    nb_descp;               /* number of OCI descriptors allocated */
    unsigned int    nb_objinst;             /* number of OCI objects allocated */
#ifdef OCI_IMPORT_RUNTIME
    LIB_HANDLE      lib_handle;             /* handle of runtime shared library */
#endif
} OCI_Library;

/* 
 * Connection Pool object 
 *
 */

struct OCI_ConnPool {
    OCI_List        *cons;      /* list of connection objects */
#if OCI_VERSION_COMPILE >= OCI_9
    OCICPool        *handle;    /* OCI pool handle */
#else
    void            *handle;    /* fake handle for aligment */
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

struct OCI_Connection {
    mtext              *db;        /* database */
    mtext              *user;      /* user */
    mtext              *pwd;       /* password */
    mtext              *version;   /* server version */
    OCI_List           *stmts;     /* list of statements */
    OCI_List           *trsns;     /* list of transactions */
    OCI_List           *sobjs;     /* list of schema objects */
    OCI_Transaction    *trs;       /* pointer to current transaction object */
    OCI_ConnPool       *pool;      /* pointer to connection pool parent */
    OCI_ServerOutput   *svopt;     /* Pointer to server output object */
    OCIServer          *svr;       /* OCI server  handle */
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
    OCI_XID          xid;       /* global transaction identitifer */
};

/* 
 * Column object
 *
 */

struct OCI_Column {

    /* 0racle infos */ 
    ub2              ocode;     /* Oracle SQL code */
    ub2              tcode;     /* Oracle type code */
    ub2              icode;     /* Internal translated Oracle SQL code */
    ub2              size;      /* SQL Size */
    sb2              prec;      /* SQL precision 1 (number prec, leading prec) */
    sb2              prec2;     /* SQL precision 2 (fractionnal prec) */
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
    OCI_Schema      *nty;       /* user type descriptor */ 
    ub4              subtype;   /* object type */
};

/* 
 * OCI_Buffer : Internal input/output buffer
 *
 */

typedef struct OCI_Buffer {
    void            *handle;   /* OCI handle (bind or define) */
    void           **data;     /* data / array of data */
    void            *inds;     /* array of indicators */
    void            *lens;     /* array of lengths */
    dtext           *temp;     /* temporary buffer for string conversion */
    ub4              count;    /* number of elements in the buffer */
    int              sizelen;  /* size of an element in the lens array */
} OCI_Buffer;

/* 
 * OCI_Bind object
 *
 */

struct OCI_Bind {
    OCI_Statement   *stmt;      /* pointer to statement object */
    void           **input;     /* input values */
    mtext           *name;      /* name of the bind */
    ub1              type;      /* internal datatype */
    ub1              subtype;   /* internal subtype */
    ub1              alloc;     /* is buffer allocated or mapped to input */
    sb4              size;      /* data size */
    OCI_Buffer       buf;       /* place holder */
    ub2              code;      /* SQL datatype code */
    ub2              dynpos;    /* index of the bind for dynamic binds */
    ub2             *plrcds;    /* PL/SQL tables return codes */
    ub4              nbelem;    /* PL/SQL tables nb elements */
    OCI_Schema      *nty;       /* for registerr of object, collection and ref */
};

/*
 * OCI_Define : Internal Resultset column data implementation
 *
 */

typedef struct OCI_Define {
    OCI_Resultset   *rs;           /* pointer to resultset object */
    void            *obj;          /* current OCILIB object instance */
    OCI_Column       col;          /* column object */
    OCI_Buffer       buf;          /* placeholder */
} OCI_Define;

/* 
 * Resultset object
 *
 */

struct OCI_Resultset {
    OCI_Statement   *stmt;          /* pointer to statement object */
    OCI_HashTable   *map;           /* hash table handle for mapping name/index */
    OCI_Define      *defs;          /* array of define objects */
    ub4              nb_defs;       /* number of elements */
    ub4              row_cur;       /* actual position in the array of rows */
    ub4              row_abs;       /* absolute position in the resulset */
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

struct OCI_Statement {
    OCIStmt         *stmt;          /* OCI statement handle */
    ub4              hstate;        /* object variable state */
    OCI_Resultset  **rsts;          /* pointer to resultset list */
    OCI_Connection  *con;           /* pointer to connection object */
    mtext           *sql;           /* SQL statement */
    OCI_Bind       **ubinds;        /* array of user bind objects */
    OCI_Bind       **rbinds;        /* array of register bind objects */
    OCI_HashTable   *map;           /* hash table handle for mapping bind name/index */
    ub2              nb_ubinds;     /* number of elememts in the bind array */
    ub2              nb_rbinds;     /* number of output binds */
    unsigned int     bind_mode;     /* type of binding */
    ub4              exec_mode;     /* type of execution */
    ub4              fetch_size;    /* fetch array size */
    ub4              prefetch_size; /* prefetch size */
    ub4              prefetch_mem;  /* prefetch memory */
    ub4              long_size;     /* default size for LONG columns */
    ub1              long_mode;     /* LONG datatype handling mode */
    ub1              status;        /* statement status */
    ub2              type;          /* type of SQL statement */
    ub4              nb_iters;      /* number of iterations for execution */
    ub4              nb_rs;         /* number of resulsets */
    ub2              cur_rs;        /* index of the current resulset */
    ub2              dynidx;        /* bind index counter for dynamic exec */
    ub2              err_pos;       /* error position in sql statement */
};

/*
 * Internal Large object 
 *
 */

struct OCI_Lob {
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

struct OCI_File {
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

struct OCI_Long {
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

struct OCI_Date {
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

struct OCI_Timestamp {
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

struct OCI_Interval {
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

struct OCI_Object { 
    void             *handle;   /* OCI handle */
    ub4               hstate;   /* object variable state */
    OCI_Connection   *con;      /* pointer to connection object */
    OCI_Schema       *nty;      /* pointer to schema object type */
    void            **objs;     /* array of OCILIB sub objects */
    void             *buf;      /* buffer to store converted out string attribute */ 
    int               buflen;   /* buffer len */
    OCIObjectLifetime type;     /* object type */
};

/*
 * Oracle Collection Item object 
 *
 */

struct OCI_Elem { 
    void            *handle;    /* OCI handle */
    ub4              hstate;    /* object variable state */
    OCI_Connection  *con;       /* pointer to connection object */
    OCIInd          *ind;       /* data state indicator */
    void            *obj;       /* OCILIB sub object */
    OCI_Column      *col;       /* datatype infos */
    void            *buf;      /* buffer to store converted out string attribute */ 
    int              buflen;   /* buffer len */
};

/*
 * Oracle Collection object 
 *
 */

struct OCI_Coll { 
    OCIColl           *handle;   /* OCI handle */
    ub4                hstate;   /* object variable state */
    OCI_Connection    *con;      /* pointer to connection object */
    OCI_Schema        *nty;      /* pointer to schema object type */
    OCI_Elem          *elem;     /* item object */
    sb4                size;     /* collection size */               
};

/*
 * Oracle Iterator object 
 *
 */

struct OCI_Iter { 
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

struct OCI_Ref { 
    OCIRef            *handle;   /* OCI handle */
    ub4                hstate;   /* object variable state */
    OCI_Connection    *con;      /* pointer to connection object */
    OCI_Schema        *nty;      /* pointer to schema object type */
    OCI_Object        *obj;      /* Pinned object */
    boolean            pinned;   /* is the reference pinned */
};

/* 
 * Schema object 
 *
 */

struct OCI_Schema {
    OCI_Connection  *con;        /* pointer to connection object */
    mtext           *name;       /* name of the schema object */
    unsigned int     type;       /* type of schema handle */
    OCIType         *tdo;        /* datatype object type */
    ub2              tcode;      /* Oracle type code */
    ub2              ccode;      /* Oracle collection code */
    OCI_Column      *cols;       /* array of column datatype infos */
    ub2              nb_cols;    /* number of columns */
    ub2              refcount;   /* reference counter */
};

/* 
 * Hash table object 
 *
 */

struct OCI_HashTable {
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

typedef struct OCI_Datatype
{
    void *handle;   /* OCI handle */
    ub4   hstate;   /* object variable state */
} OCI_Datatype;

/* static and unique OCI_Library object */

extern OCI_Library OCILib;

#endif /* OCILIB_OCILIB_TYPES_H_INCLUDED */

