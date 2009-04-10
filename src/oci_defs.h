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

   +----------------------------------------------------------------------+   
   |                          IMPORTANT NOTICE                            |
   +----------------------------------------------------------------------+
   |                                                                      |
   | THIS FILE CONTAINS CONSTANTS AND STRUCTURES DECLARATIONS THAT WERE   |
   | PICKED UP FROM ORACLE PUBLIC HEADER FILES.                           |
   |                                                                      |
   | SO THE CONTENT OF THIS FILE IS UNDER ORACLE COPYRIGHT AND THE        |
   | DECLARATIONS REPRODUCED HERE ARE ORIGINALLY WRITTEN BY ORACLE        | 
   | CORPORATION.                                                         |
   |                                                                      |
   | THE AUTHOR OF OCILIB LIBRARY HAS NOT WRITTEN THE CONTENT OF THIS     |
   | FILE AND HAS PARTIALLY INTEGRATED SOME ORACLE OCI DEFINITONS TO      |
   | ALLOW COMPILATION OF THIS OPEN SOURCE LIBRARY WITHOUT HAVING         |
   | ORACLE PRODUCTS INSTALLED ON DEVELOPMENT ENVIRONMENTS                |
   |                                                                      |
   +----------------------------------------------------------------------+
*/

/* ------------------------------------------------------------------------ *
 * $Id: oci_defs.h, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#ifndef OCILIB_OCI_DEFS_H_INCLUDED 
#define OCILIB_OCI_DEFS_H_INCLUDED 

#include "oci_types.h"

/*--------------------------------------------------------------------------- 
                     PUBLIC TYPES AND CONSTANTS 
  ---------------------------------------------------------------------------*/

/* input data types */

#define SQLT_CHR  1                        /* (ORANET TYPE) character string */
#define SQLT_NUM  2                          /* (ORANET TYPE) oracle numeric */
#define SQLT_INT  3                                 /* (ORANET TYPE) integer */
#define SQLT_FLT  4                   /* (ORANET TYPE) Floating point number */
#define SQLT_STR  5                                /* zero terminated string */
#define SQLT_VNU  6                        /* NUM with preceding length byte */
#define SQLT_PDN  7                  /* (ORANET TYPE) Packed Decimal Numeric */
#define SQLT_LNG  8                                                  /* long */
#define SQLT_VCS  9                             /* Variable character string */
#define SQLT_NON  10                      /* Null/empty PCC Descriptor entry */
#define SQLT_RID  11                                                /* rowid */
#define SQLT_DAT  12                                /* date in oracle format */
#define SQLT_VBI  15                                 /* binary in VCS format */
#define SQLT_BFLOAT 21                                /* Native Binary float*/
#define SQLT_BDOUBLE 22                             /* NAtive binary double */
#define SQLT_BIN  23                                  /* binary data(DTYBIN) */
#define SQLT_LBI  24                                          /* long binary */
#define SQLT_UIN  68                                     /* unsigned integer */
#define SQLT_SLS  91                        /* Display sign leading separate */
#define SQLT_LVC  94                                  /* Longer longs (char) */
#define SQLT_LVB  95                                   /* Longer long binary */ 
#define SQLT_AFC  96                                      /* Ansi fixed char */
#define SQLT_AVC  97                                        /* Ansi Var char */
#define SQLT_IBFLOAT  100                          /* binary float canonical */
#define SQLT_IBDOUBLE 101                         /* binary double canonical */
#define SQLT_CUR  102                                        /* cursor  type */
#define SQLT_RDD  104                                    /* rowid descriptor */
#define SQLT_LAB  105                                          /* label type */
#define SQLT_OSL  106                                        /* oslabel type */

#define SQLT_NTY  108                                   /* named object type */
#define SQLT_REF  110                                            /* ref type */
#define SQLT_CLOB 112                                       /* character lob */
#define SQLT_BLOB 113                                          /* binary lob */
#define SQLT_BFILEE 114                                   /* binary file lob */
#define SQLT_CFILEE 115                                /* character file lob */
#define SQLT_RSET 116                                     /* result set type */
#define SQLT_NCO  122      /* named collection type (varray or nested table) */
#define SQLT_VST  155                                      /* OCIString type */
#define SQLT_ODT  156                                        /* OCIDate type */

/* datetimes and intervals */
#define SQLT_DATE                      184                      /* ANSI Date */
#define SQLT_TIME                      185                           /* TIME */
#define SQLT_TIME_TZ                   186            /* TIME WITH TIME ZONE */
#define SQLT_TIMESTAMP                 187                      /* TIMESTAMP */
#define SQLT_TIMESTAMP_TZ              188       /* TIMESTAMP WITH TIME ZONE */
#define SQLT_INTERVAL_YM               189         /* INTERVAL YEAR TO MONTH */
#define SQLT_INTERVAL_DS               190         /* INTERVAL DAY TO SECOND */
#define SQLT_TIMESTAMP_LTZ             232        /* TIMESTAMP WITH LOCAL TZ */

/* cxcheng: this has been added for backward compatibility -
   it needs to be here because ocidfn.h can get included ahead of sqldef.h */
#define SQLT_FILE SQLT_BFILEE                              /* binary file lob */
#define SQLT_CFILE SQLT_CFILEE
#define SQLT_BFILE SQLT_BFILEE

#define SQLT_PNTY   241               /* pl/sql representation of named types */


/* CHAR/NCHAR/VARCHAR2/NVARCHAR2/CLOB/NCLOB char set "form" information */
#define SQLCS_IMPLICIT 1     /* for CHAR, VARCHAR2, CLOB w/o a specified set */
#define SQLCS_NCHAR    2                  /* for NCHAR, NCHAR VARYING, NCLOB */
#define SQLCS_EXPLICIT 3   /* for CHAR, etc, with "CHARACTER SET ..." syntax */
#define SQLCS_FLEXIBLE 4                 /* for PL/SQL "flexible" parameters */
#define SQLCS_LIT_NULL 5      /* for typecheck of NULL and empty_clob() lits */


/*-----------------------------Handle Types----------------------------------*/
                                           /* handle types range from 1 - 49 */
#define OCI_HTYPE_FIRST          1             /* start value of handle type */
#define OCI_HTYPE_ENV            1                     /* environment handle */
#define OCI_HTYPE_ERROR          2                           /* error handle */
#define OCI_HTYPE_SVCCTX         3                         /* service handle */
#define OCI_HTYPE_STMT           4                       /* statement handle */
#define OCI_HTYPE_BIND           5                            /* bind handle */
#define OCI_HTYPE_DEFINE         6                          /* define handle */
#define OCI_HTYPE_DESCRIBE       7                        /* describe handle */
#define OCI_HTYPE_SERVER         8                          /* server handle */
#define OCI_HTYPE_SESSION        9                  /* authentication handle */
#define OCI_HTYPE_AUTHINFO      OCI_HTYPE_SESSION  /* SessionGet auth handle */
#define OCI_HTYPE_DIRPATH_CTX   14                    /* direct path context */
#define OCI_HTYPE_DIRPATH_COLUMN_ARRAY 15        /* direct path column array */
#define OCI_HTYPE_DIRPATH_STREAM       16              /* direct path stream */
#define OCI_HTYPE_TRANS         10                     /* transaction handle */
#define OCI_HTYPE_CPOOL         26                 /* connection pool handle */

/*-------------------------Descriptor Types----------------------------------*/

#define OCI_DTYPE_LOB 50                                     /* lob  locator */
#define OCI_DTYPE_PARAM 53  /* a parameter descriptor obtained from ocigparm */
#define OCI_DTYPE_FILE 56                                /* File Lob locator */
#define OCI_DTYPE_INTERVAL_YM 62                      /* Interval year month */
#define OCI_DTYPE_INTERVAL_DS 63                      /* Interval day second */
#define OCI_DTYPE_DATE 65                                            /* Date */
#define OCI_DTYPE_TIME 66                                            /* Time */
#define OCI_DTYPE_TIME_TZ 67                           /* Time with timezone */
#define OCI_DTYPE_TIMESTAMP 68                                  /* Timestamp */
#define OCI_DTYPE_TIMESTAMP_TZ 69                 /* Timestamp with timezone */
#define OCI_DTYPE_TIMESTAMP_LTZ 70                /* Timestamp with local tz */
#
/*---------------------------------------------------------------------------*/

/*--------------------------------LOB types ---------------------------------*/
#define OCI_TEMP_BLOB 1                /* LOB type - BLOB ------------------ */
#define OCI_TEMP_CLOB 2                /* LOB type - CLOB ------------------ */
/*---------------------------------------------------------------------------*/

/*--------------------------Attribute Types----------------------------------*/


#define OCI_ATTR_OBJECT   2 /* is the environment initialized in object mode */
#define OCI_ATTR_ENV  5                            /* the environment handle */
#define OCI_ATTR_SERVER 6                               /* the server handle */
#define OCI_ATTR_SESSION 7                        /* the user session handle */
#define OCI_ATTR_TRANS   8                         /* the transaction handle */
#define OCI_ATTR_ROW_COUNT   9                  /* the rows processed so far */
#define OCI_ATTR_PREFETCH_ROWS  11    /* sets the number of rows to prefetch */
#define OCI_ATTR_NESTED_PREFETCH_ROWS 12 /* the prefetch rows of nested table*/
#define OCI_ATTR_PREFETCH_MEMORY 13         /* memory limit for rows fetched */
#define OCI_ATTR_NESTED_PREFETCH_MEMORY 14   /* memory limit for nested rows */
#define OCI_ATTR_PDSCL          16                   /* packed decimal scale */
#define OCI_ATTR_FSPRECISION OCI_ATTR_PDSCL   
                                          /* fs prec for datetime data types */
#define OCI_ATTR_PDPRC          17                  /* packed decimal format */
#define OCI_ATTR_LFPRECISION OCI_ATTR_PDPRC 
                                          /* fs prec for datetime data types */

#define OCI_ATTR_PARAM_COUNT 18       /* number of column in the select list */
#define OCI_ATTR_ROWID   19                                     /* the rowid */
#define OCI_ATTR_CHARSET  20                      /* the character set value */
#define OCI_ATTR_USERNAME 22                           /* username attribute */
#define OCI_ATTR_PASSWORD 23                           /* password attribute */
#define OCI_ATTR_STMT_TYPE   24                            /* statement type */
#define OCI_ATTR_XID     27           /* XOPEN defined global transaction id */
#define OCI_ATTR_CHARSET_ID 31                           /* Character Set ID */
#define OCI_ATTR_CHARSET_FORM 32                       /* Character Set Form */
#define OCI_ATTR_MAXDATA_SIZE 33       /* Maximumsize of data on the server  */
#define OCI_ATTR_ROWS_RETURNED 42
               /* Number of rows returned in current iter - for Bind handles */

#define OCI_ATTR_LOBEMPTY               45                    /* empty lob ? */

#define OCI_ATTR_NUM_DML_ERRORS         73       /* num of errs in array DML */
#define OCI_ATTR_DML_ROW_OFFSET         74        /* row offset in the array */

#define OCI_ATTR_DATEFORMAT             75     /* default date format string */
#define OCI_ATTR_BUF_ADDR               76                 /* buffer address */
#define OCI_ATTR_BUF_SIZE               77                    /* buffer size */
#define OCI_ATTR_NUM_ROWS               81 /* number of rows in column array */                                 
#define OCI_ATTR_COL_COUNT              82 /* columns of column array
                                                     processed so far.       */
#define OCI_ATTR_NUM_COLS              102              /* number of columns */
#define OCI_ATTR_LIST_COLUMNS          103   /* parameter of the column list */

#define OCI_ATTR_PARAM                 124           /* parameter descriptor */
#define OCI_ATTR_PARSE_ERROR_OFFSET    129             /* Parse Error offset */

#define OCI_ATTR_SERVER_STATUS		   143        /* state of the server hdl */

#define OCI_ATTR_CURRENT_POSITION      164      /* for scrollable result sets*/

#define OCI_ATTR_CONN_NOWAIT           178
#define OCI_ATTR_CONN_BUSY_COUNT       179
#define OCI_ATTR_CONN_OPEN_COUNT       180
#define OCI_ATTR_CONN_TIMEOUT          181
#define OCI_ATTR_STMT_STATE            182
#define OCI_ATTR_CONN_MIN              183
#define OCI_ATTR_CONN_MAX              184
#define OCI_ATTR_CONN_INCR             185
#define OCI_ATTR_ROWS_FETCHED          197      /* rows fetched in last call */

#define OCI_ATTR_TYPECODE              216           /* object or collection */
#define OCI_ATTR_COLLECTION_TYPECODE   217         /* varray or nested table */

#define OCI_ATTR_COLLECTION_ELEMENT    227     /* has a collection attribute */
#define OCI_ATTR_NUM_TYPE_ATTRS        228      /* number of attribute types */
#define OCI_ATTR_LIST_TYPE_ATTRS       229        /* list of type attributes */

#define OCI_ATTR_CLIENT_IDENTIFIER     278       /* value of client id to set*/

#define OCI_ATTR_CHAR_USED             285          /* char length semantics */
#define OCI_ATTR_CHAR_SIZE             286                    /* char length */

#define OCI_ATTR_MODULE                366             /* module for tracing */
#define OCI_ATTR_ACTION                367             /* action for tracing */
#define OCI_ATTR_CLIENT_INFO           368                    /* client info */

#define OCI_ATTR_DRIVER_NAME           424                    /* Driver Name */


/*------- Temporary attribute value for UCS2/UTF16 character set ID -------- */ 

#define OCI_UCS2ID                     1000               /* UCS2 charset ID */
#define OCI_UTF16ID                    1000              /* UTF16 charset ID */

/*-------------------------Credential Types----------------------------------*/

#define OCI_CRED_RDBMS      1                  /* database username/password */
#define OCI_CRED_EXT        2             /* externally provided credentials */
#define OCI_CRED_PROXY      3                        /* proxy authentication */

/*------------------------Error Return Values--------------------------------*/

#define OCI_SUCCESS 0                      /* maps to SQL_SUCCESS of SAG CLI */
#define OCI_SUCCESS_WITH_INFO 1             /* maps to SQL_SUCCESS_WITH_INFO */
#define OCI_NO_DATA 100                               /* maps to SQL_NO_DATA */
#define OCI_ERROR -1                                    /* maps to SQL_ERROR */
#define OCI_INVALID_HANDLE -2                  /* maps to SQL_INVALID_HANDLE */
#define OCI_NEED_DATA 99                            /* maps to SQL_NEED_DATA */
#define OCI_STILL_EXECUTING -3123                   /* OCI would block error */

/*---------------- Server Handle Attribute Values ---------------------------*/

#define OCI_SERVER_NOT_CONNECTED	0x0 
#define OCI_SERVER_NORMAL      		0x1 

/*--------------------- User Callback Return Values -------------------------*/
#define OCI_CONTINUE -24200    /* Continue with the body of the OCI function */

/*------------------------Attach Modes---------------------------------------*/

#define OCI_CPOOL            0x0200  /* Attach using server handle from pool */

/*------------------DateTime and Interval check Error codes------------------*/

#define   OCI_DT_INVALID_DAY         0x1                          /* Bad day */
#define   OCI_DT_DAY_BELOW_VALID     0x2      /* Bad DAy Low/high bit (1=low)*/
#define   OCI_DT_INVALID_MONTH       0x4                       /*  Bad MOnth */
#define   OCI_DT_MONTH_BELOW_VALID   0x8   /* Bad MOnth Low/high bit (1=low) */
#define   OCI_DT_INVALID_YEAR        0x10                        /* Bad YeaR */
#define   OCI_DT_YEAR_BELOW_VALID    0x20  /*  Bad YeaR Low/high bit (1=low) */
#define   OCI_DT_INVALID_HOUR        0x40                       /*  Bad HouR */
#define   OCI_DT_HOUR_BELOW_VALID    0x80   /* Bad HouR Low/high bit (1=low) */
#define   OCI_DT_INVALID_MINUTE      0x100                     /* Bad MiNute */
#define   OCI_DT_MINUTE_BELOW_VALID  0x200 /*Bad MiNute Low/high bit (1=low) */
#define   OCI_DT_INVALID_SECOND      0x400                    /*  Bad SeCond */
#define   OCI_DT_SECOND_BELOW_VALID  0x800  /*bad second Low/high bit (1=low)*/
#define   OCI_DT_DAY_MISSING_FROM_1582 0x1000     
                                 /*  Day is one of those "missing" from 1582 */
#define   OCI_DT_YEAR_ZERO           0x2000       /* Year may not equal zero */
#define   OCI_DT_INVALID_TIMEZONE    0x4000                 /*  Bad Timezone */
#define   OCI_DT_INVALID_FORMAT      0x8000         /* Bad date format input */


/*------------------------Scrollable Cursor Fetch Options------------------- 
 * For non-scrollable cursor, the only valid (and default) orientation is 
 * OCI_FETCH_NEXT
 */
#define OCI_FETCH_CURRENT     0x01           /* refetching current position  */
#define OCI_FETCH_NEXT        0x02                               /* next row */
#define OCI_FETCH_FIRST       0x04            /* first row of the result set */
#define OCI_FETCH_LAST        0x08         /* the last row of the result set */
#define OCI_FETCH_PRIOR       0x10   /* the previous row relative to current */
#define OCI_FETCH_ABSOLUTE    0x20             /* absolute offset from first */
#define OCI_FETCH_RELATIVE    0x40             /* offset relative to current */
#define OCI_FETCH_RESERVED_1  0x80                               /* reserved */

/*------------------------Parse mode ----------------------------------------*/

#define OCI_NTV_SYNTAX 1    /* Use what so ever is the native lang of server */


/*------------------------Bind and Define Options----------------------------*/

#define OCI_DATA_AT_EXEC      0x00000002             /* data at execute time */
#define OCI_DYNAMIC_FETCH     0x00000002                /* fetch dynamically */
#define OCI_PIECEWISE         0x00000004          /* piecewise DMLs or fetch */
#define OCI_STMT_SCROLLABLE_READONLY 0x08     /* if result set is scrollable */


/*-----------------------------  Various Modes ------------------------------*/
#define OCI_DEFAULT         0x00000000 
#define OCI_THREADED        0x00000001      /* appl. in threaded environment */
#define OCI_OBJECT          0x00000002  /* application in object environment */
#define OCI_UTF16           0x00004000        /* mode for all UTF16 metadata */

/*------------------------Authentication Modes-------------------------------*/
#define OCI_SYSDBA          0x00000002           /* for SYSDBA authorization */
#define OCI_SYSOPER         0x00000004          /* for SYSOPER authorization */


/*------------------------ Transaction Start Flags --------------------------*/

#define OCI_TRANS_NEW          0x00000001 /* starts a new transaction branch */
#define OCI_TRANS_JOIN         0x00000002    /* join an existing transaction */
#define OCI_TRANS_RESUME       0x00000004         /* resume this transaction */
#define OCI_TRANS_STARTMASK    0x000000ff

#define OCI_TRANS_READONLY     0x00000100   /* starts a readonly transaction */
#define OCI_TRANS_READWRITE    0x00000200 /* starts a read-write transaction */
#define OCI_TRANS_SERIALIZABLE 0x00000400
                                        /* starts a serializable transaction */
#define OCI_TRANS_ISOLMASK     0x0000ff00

#define OCI_TRANS_LOOSE        0x00010000        /* a loosely coupled branch */
#define OCI_TRANS_TIGHT        0x00020000        /* a tightly coupled branch */
#define OCI_TRANS_TYPEMASK     0x000f0000

#define OCI_TRANS_NOMIGRATE    0x00100000      /* non migratable transaction */
#define OCI_TRANS_SEPARABLE    0x00200000  /* separable transaction (8.1.6+) */
#define OCI_TRANS_OTSRESUME    0x00400000      /* OTS resuming a transaction */

/*------------------------ Transaction End Flags ----------------------------*/

#define OCI_TRANS_TWOPHASE      0x01000000           /* use two phase commit */
#define OCI_TRANS_WRITEBATCH    0x00000001  /* force cmt-redo for local txns */
#define OCI_TRANS_WRITEIMMED    0x00000002              /* no force cmt-redo */
#define OCI_TRANS_WRITEWAIT     0x00000004               /* no sync cmt-redo */
#define OCI_TRANS_WRITENOWAIT   0x00000008   /* sync cmt-redo for local txns */

/*------------------------Describe Handle Parameter Attributes --------------*/

#define OCI_ATTR_DATA_SIZE      1                /* maximum size of the data */
#define OCI_ATTR_DATA_TYPE      2     /* the SQL type of the column/argument */
#define OCI_ATTR_DISP_SIZE      3                        /* the display size */
#define OCI_ATTR_NAME           4         /* the name of the column/argument */
#define OCI_ATTR_PRECISION      5                /* precision if number type */
#define OCI_ATTR_SCALE          6                    /* scale if number type */
#define OCI_ATTR_IS_NULL        7                            /* is it null ? */
#define OCI_ATTR_TYPE_NAME      8             /* name of the named data type */
#define OCI_ATTR_SCHEMA_NAME    9                         /* the schema name */
#define OCI_ATTR_SUB_NAME       10      /* type name if package private type */

/*------------------------Other Constants------------------------------------*/

#define OCI_ERROR_MAXMSG_SIZE   1024         /* max size of an error message */
#define OCI_LOBMAXSIZE          MINUB4MAXVAL        /* maximum lob data size */
#define OCI_ROWID_LEN           23 

/*-----------------------Handle Definitions----------------------------------*/
typedef struct OCIEnv           OCIEnv;            /* OCI environment handle */
typedef struct OCIError         OCIError;                /* OCI error handle */
typedef struct OCISvcCtx        OCISvcCtx;             /* OCI service handle */
typedef struct OCIStmt          OCIStmt;             /* OCI statement handle */
typedef struct OCIBind          OCIBind;                  /* OCI bind handle */
typedef struct OCIDefine        OCIDefine;              /* OCI Define handle */
typedef struct OCIDescribe      OCIDescribe;          /* OCI Describe handle */
typedef struct OCIServer        OCIServer;              /* OCI Server handle */
typedef struct OCISession       OCISession;     /* OCI Authentication handle */
typedef struct OCIComplexObject OCIComplexObject;          /* OCI COR handle */
typedef struct OCITrans         OCITrans;          /* OCI Transaction handle */
typedef struct OCISecurity      OCISecurity;          /* OCI Security handle */
typedef struct OCISubscription  OCISubscription;      /* subscription handle */

typedef struct OCICPool         OCICPool;          /* connection pool handle */
typedef struct OCISPool         OCISPool;             /* session pool handle */
typedef struct OCIAuthInfo      OCIAuthInfo;                  /* auth handle */
typedef struct OCIAdmin         OCIAdmin;                    /* admin handle */
typedef struct OCIEvent         OCIEvent;                 /* HA event handle */


typedef struct OCIDirPathCtx      OCIDirPathCtx;               /* DP context */
typedef struct OCIDirPathColArray OCIDirPathColArray;     /* DP column array */
typedef struct OCIDirPathStream   OCIDirPathStream;             /* DP stream */

/*--------------------- OCI Thread Object Definitions------------------------*/

/* OCIThread Context */
typedef struct OCIThreadContext OCIThreadContext;

/* OCIThread Mutual Exclusion Lock */
typedef struct OCIThreadMutex   OCIThreadMutex; 

/* OCIThread Key for Thread-Specific Data */
typedef struct OCIThreadKey     OCIThreadKey;  

/* OCIThread Thread ID */
typedef struct OCIThreadId      OCIThreadId;  

/* OCIThread Thread Handle */
typedef struct OCIThreadHandle  OCIThreadHandle;  

/*--------------------- OCI Collection Object Definitions--------------------*/

/* OCIColl - generic collection type */
typedef struct OCIColl OCIColl;

/* OCIArray - varray collection type */
typedef OCIColl OCIArray;

/* OCITable - nested table collection type */
typedef OCIColl OCITable;

/* OCIIter - collection iterator */
typedef struct OCIIter OCIIter;


/*-----------------------Descriptor Definitions------------------------------*/

typedef struct OCISnapshot      OCISnapshot;      /* OCI snapshot descriptor */
typedef struct OCIResult        OCIResult;      /* OCI Result Set Descriptor */
typedef struct OCILobLocator    OCILobLocator; /* OCI Lob Locator descriptor */
typedef struct OCIParam         OCIParam;        /* OCI PARameter descriptor */
typedef struct OCIDateTime      OCIDateTime;      /* OCI DateTime descriptor */
typedef struct OCIInterval      OCIInterval;      /* OCI Interval descriptor */
typedef struct OCIRowid         OCIRowid;            /* OCI ROWID descriptor */


/*----------------------------- OBJECT FREE OPTION --------------------------*/

#define OCI_OBJECTFREE_FORCE      (ub2)0x0001
#define OCI_OBJECTFREE_NONULL     (ub2)0x0002

/*-------------------------- OCINumber --------------------------------------*/

#define OCI_NUMBER_UNSIGNED 0                        /* Unsigned type -- ubX */
#define OCI_NUMBER_SIGNED   2                          /* Signed type -- sbX */
#define OCI_NUMBER_SIZE     22

struct OCINumber
{
  ub1 OCINumberPart[OCI_NUMBER_SIZE];
};

/*-----------------------Objects Definitions---------------------------------*/

typedef struct OCIString        OCIString;
typedef struct OCIRaw           OCIRaw;
typedef struct OCIType          OCIType;
typedef struct OCINumber        OCINumber;
typedef struct OCIRef           OCIRef;   

/*--------------------------- OBJECT INDICATOR ------------------------------*/

typedef sb2 OCIInd;
/*
 * OCIInd -- a variable of this type contains (null) indicator information
 */

#define OCI_IND_NOTNULL (OCIInd)0                                /* not NULL */
#define OCI_IND_NULL (OCIInd)(-1)                                    /* NULL */
#define OCI_IND_BADNULL (OCIInd)(-2)                             /* BAD NULL */
#define OCI_IND_NOTNULLABLE (OCIInd)(-3)                     /* not NULLable */


/*-------------------------Object Ptr Types----------------------------------*/
#define OCI_OTYPE_NAME 1                                      /* object name */
#define OCI_OTYPE_REF  2                                       /* REF to TDO */
#define OCI_OTYPE_PTR  3                                       /* PTR to TDO */

/*------------------------ Lob-specific Definitions -------------------------*/

typedef ub4 OCILobOffset;
typedef ub4 OCILobLength;

/*----------------------------Piece Definitions------------------------------*/

#define OCI_ONE_PIECE 0                                         /* one piece */
#define OCI_FIRST_PIECE 1                                 /* the first piece */
#define OCI_NEXT_PIECE 2                          /* the next of many pieces */
#define OCI_LAST_PIECE 3                                   /* the last piece */


/*----------------------- GET OPTIONS FOR TDO  ------------------------------*/

enum OCITypeGetOpt
{
  OCI_TYPEGET_HEADER,   /* load only the header portion of the TDO           */
  OCI_TYPEGET_ALL       /* load all attribute and method descriptors as well */
};
typedef enum OCITypeGetOpt OCITypeGetOpt;

/*--------------------------- OBJECT PIN OPTION -----------------------------*/

enum OCIPinOpt
{
  /* 0 = uninitialized */
  OCI_PIN_DEFAULT = 1,                                 /* default pin option */
  OCI_PIN_ANY = 3,                             /* pin any copy of the object */
  OCI_PIN_RECENT = 4,                       /* pin recent copy of the object */
  OCI_PIN_LATEST = 5                        /* pin latest copy of the object */
};
typedef enum OCIPinOpt OCIPinOpt;

/*--------------------------- OBJECT LOCK OPTION ----------------------------*/

enum OCILockOpt
{
  /* 0 = uninitialized */
  OCI_LOCK_NONE = 1,                               /* null (same as no lock) */
  OCI_LOCK_X = 2,                                          /* exclusive lock */
  OCI_LOCK_X_NOWAIT = 3                      /* exclusive lock, do not wait  */
};
typedef enum OCILockOpt OCILockOpt;

/*------------------------- OBJECT MODIFYING OPTION -------------------------*/

enum OCIMarkOpt
{
  /* 0 = uninitialized */
  OCI_MARK_DEFAULT = 1,               /* default (the same as OCI_MARK_NONE) */
  OCI_MARK_NONE = OCI_MARK_DEFAULT,          /* object has not been modified */
  OCI_MARK_UPDATE                                 /* object is to be updated */
};
typedef enum OCIMarkOpt OCIMarkOpt;

/*------------------------------ TYPE CODE ----------------------------------*/

/* The OCITypeCode type is interchangeable with the existing SQLT type
   which is a ub2 */
typedef ub2 OCITypeCode;

/*---------------------------  ORACLE DATE TYPE  ----------------------------*/

struct OCITime
{
  ub1 OCITimeHH;                          /* hours; range is 0 <= hours <=23 */
  ub1 OCITimeMI;                     /* minutes; range is 0 <= minutes <= 59 */
  ub1 OCITimeSS;                     /* seconds; range is 0 <= seconds <= 59 */
};
typedef struct OCITime OCITime;

struct OCIDate
{
  sb2 OCIDateYYYY;         /* gregorian year; range is -4712 <= year <= 9999 */
  ub1 OCIDateMM;                          /* month; range is 1 <= month < 12 */
  ub1 OCIDateDD;                             /* day; range is 1 <= day <= 31 */
  OCITime OCIDateTime;                                               /* time */
};
typedef struct OCIDate OCIDate;

/*-------------------------- OBJECT Duration --------------------------------*/
 
typedef ub2 OCIDuration;
 
#define  OCI_DURATION_BEGIN (OCIDuration)10
                                           /* beginning sequence of duration */
#define  OCI_DURATION_NULL (OCIDuration)(OCI_DURATION_BEGIN-1)
                                                            /* null duration */
#define  OCI_DURATION_DEFAULT (OCIDuration)(OCI_DURATION_BEGIN-2) /* default */
#define  OCI_DURATION_NEXT (OCIDuration)(OCI_DURATION_BEGIN-3)
                                                    /* next special duration */
#define  OCI_DURATION_SESSION (OCIDuration)(OCI_DURATION_BEGIN)
                                                  /* the end of user session */
#define  OCI_DURATION_TRANS (OCIDuration)(OCI_DURATION_BEGIN+1)
                                              /* the end of user transaction */

/*-------------------------Object Ptr Types----------------------------------*/
#define OCI_OTYPE_NAME 1                                      /* object name */

/*--------------------------- OCI Parameter Types ---------------------------*/
#define OCI_PTYPE_UNK                 0                         /* unknown   */
#define OCI_PTYPE_TABLE               1                         /* table     */
#define OCI_PTYPE_VIEW                2                         /* view      */
#define OCI_PTYPE_PROC                3                         /* procedure */
#define OCI_PTYPE_FUNC                4                         /* function  */
#define OCI_PTYPE_PKG                 5                         /* package   */
#define OCI_PTYPE_TYPE                6                 /* user-defined type */
#define OCI_PTYPE_SYN                 7                         /* synonym   */
#define OCI_PTYPE_SEQ                 8                         /* sequence  */
#define OCI_PTYPE_COL                 9                         /* column    */
#define OCI_PTYPE_ARG                 10                        /* argument  */
#define OCI_PTYPE_LIST                11                        /* list      */
#define OCI_PTYPE_TYPE_ATTR           12    /* user-defined type's attribute */
#define OCI_PTYPE_TYPE_COLL           13        /* collection type's element */
#define OCI_PTYPE_TYPE_METHOD         14       /* user-defined type's method */
#define OCI_PTYPE_TYPE_ARG            15   /* user-defined type method's arg */
#define OCI_PTYPE_TYPE_RESULT         16/* user-defined type method's result */
#define OCI_PTYPE_SCHEMA              17                           /* schema */
#define OCI_PTYPE_DATABASE            18                         /* database */
#define OCI_PTYPE_RULE                19                             /* rule */
#define OCI_PTYPE_RULE_SET            20                         /* rule set */
#define OCI_PTYPE_EVALUATION_CONTEXT  21               /* evaluation context */
#define OCI_PTYPE_TABLE_ALIAS         22                      /* table alias */
#define OCI_PTYPE_VARIABLE_TYPE       23                    /* variable type */
#define OCI_PTYPE_NAME_VALUE          24                  /* name value pair */

/*------------------------------ TYPE CODE ----------------------------------*/

#define OCI_TYPECODE_VARRAY      247         /* SQL VARRAY  OTS PAGED VARRAY */
#define OCI_TYPECODE_TABLE       248              /* SQL TABLE  OTS MULTISET */

/*------------------------Piece Information----------------------------------*/
#define OCI_PARAM_IN 0x01                                    /* in parameter */
#define OCI_PARAM_OUT 0x02                                  /* out parameter */

/*----------------------- OBJECT PROPERTY ID -------------------------------*/

typedef ub1 OCIObjectPropId;
#define OCI_OBJECTPROP_LIFETIME 1       /* persistent or transient or value */
#define OCI_OBJECTPROP_SCHEMA 2   /* schema name of table containing object */
#define OCI_OBJECTPROP_TABLE 3     /* table name of table containing object */
#define OCI_OBJECTPROP_PIN_DURATION 4             /* pin duartion of object */
#define OCI_OBJECTPROP_ALLOC_DURATION 5         /* alloc duartion of object */
#define OCI_OBJECTPROP_LOCK 6                      /* lock status of object */
#define OCI_OBJECTPROP_MARKSTATUS 7                /* mark status of object */
#define OCI_OBJECTPROP_VIEW 8            /* is object a view object or not? */

/*----------------------- OBJECT LIFETIME ----------------------------------*/

enum OCIObjectLifetime
{
   /* 0 = uninitialized */
   OCI_OBJECT_PERSISTENT = 1,                          /* persistent object */
   OCI_OBJECT_TRANSIENT,                                /* transient object */
   OCI_OBJECT_VALUE                                         /* value object */
};
typedef enum OCIObjectLifetime OCIObjectLifetime;

/*----------------------- OBJECT MARK STATUS -------------------------------*/

typedef uword OCIObjectMarkStatus;
#define OCI_OBJECT_NEW     0x0001                             /* new object */
#define OCI_OBJECT_DELETED 0x0002                  /* object marked deleted */
#define OCI_OBJECT_UPDATED 0x0004                  /* object marked updated */

/* macros to test the object mark status */ 
#define OCI_OBJECT_IS_UPDATED(flag) bit((flag), OCI_OBJECT_UPDATED)
#define OCI_OBJECT_IS_DELETED(flag) bit((flag), OCI_OBJECT_DELETED)
#define OCI_OBJECT_IS_NEW(flag) bit((flag), OCI_OBJECT_NEW)
#define OCI_OBJECT_IS_DIRTY(flag) \
  bit((flag), OCI_OBJECT_UPDATED|OCI_OBJECT_NEW|OCI_OBJECT_DELETED)

/*----- values for cflg argument to OCIDirpathColArrayEntrySet --------------*/

#define OCI_DIRPATH_COL_COMPLETE 0                /* column data is complete */
#define OCI_DIRPATH_COL_NULL     1                         /* column is null */
#define OCI_DIRPATH_COL_PARTIAL  2                 /* column data is partial */
#define OCI_DIRPATH_COL_ERROR    3               /* column error, ignore row */

/*----- values for action parameter to OCIDirPathDataSave -------------------*/
#define OCI_DIRPATH_DATASAVE_SAVEONLY 0              /* data save point only */

/*------------- Supported Values for Direct Path Date cache -----------------*/
#define OCI_ATTR_DIRPATH_DCACHE_NUM         303        /* date cache entries */
#define OCI_ATTR_DIRPATH_DCACHE_SIZE        304          /* date cache limit */
#define OCI_ATTR_DIRPATH_DCACHE_MISSES      305         /* date cache misses */
#define OCI_ATTR_DIRPATH_DCACHE_HITS        306           /* date cache hits */
#define OCI_ATTR_DIRPATH_DCACHE_DISABLE     307 /* on set: disable datecache */

/*------------- Supported Values for Direct Path Stream Version -------------*/

#define OCI_ATTR_DIRPATH_NOLOG          79               /* nologging option */
#define OCI_ATTR_DIRPATH_PARALLEL       80     /* parallel (temp seg) option */


#endif /* OCILIB_OCI_DEFS_H_INCLUDED */

