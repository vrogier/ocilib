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
 * $Id: ocilib_internal.h, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#ifndef OCILIB_OCILIB_INTERNAL_H_INCLUDED 
#define OCILIB_OCILIB_INTERNAL_H_INCLUDED 

#include "ocilib_types.h"
#include "ocilib_checks.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ************************************************************************ *
                         PRIVATE FUNCTIONS PROTOTYPES
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * bind.c
 * ------------------------------------------------------------------------ */

boolean OCI_BindFree
(
    OCI_Bind *bnd
);

/* ------------------------------------------------------------------------ *
 * callback.c
 * ------------------------------------------------------------------------ */

sb4 OCI_ProcInBind
(
    dvoid *ictxp, 
    OCIBind *bindp, 
    ub4 iter, 
    ub4 index,
    dvoid **bufpp, 
    ub4 *alenp,  
    ub1 *piecep, 
    dvoid **indp
);

sb4 OCI_ProcOutBind(
    dvoid *octxp, 
    OCIBind *bindp,
    ub4 iter, 
    ub4 index,
    dvoid **bufpp, 
    ub4 **alenp, 
    ub1 *piecep, 
    void **indp,
    ub2 **rcodep
);

/* ------------------------------------------------------------------------ *
 * collection.c
 * ------------------------------------------------------------------------ */

boolean OCI_CollGetInternalSize
(
    OCI_Coll *coll
);

OCI_Coll * OCI_CollInit
(
    OCI_Connection *con,                           
    OCI_Coll **pcoll,
    void *handle, 
    OCI_TypeInfo *typeinf
);

/* ------------------------------------------------------------------------ *
 * column.c
 * ------------------------------------------------------------------------ */

boolean OCI_ColumnMap
(
    OCI_Column *col, 
    OCI_Statement *stmt
);

boolean OCI_ColumnDescribe
(
    OCI_Column *col, 
    OCI_Connection *con,
    OCI_Statement *stmt,  
    void *handle, 
    int index, 
    int ptype
);

/* ------------------------------------------------------------------------ *
 * connection.c
 * ------------------------------------------------------------------------ */

OCI_Connection * OCI_ConnectionAllocate
(
    OCI_ConnPool *pool,
    const mtext *db, 
    const mtext *user,
    const mtext *pwd, 
    unsigned int mode
);

boolean OCI_ConnectionDeallocate
(
    OCI_Connection *con
);

boolean OCI_ConnectionAttach
(
    OCI_Connection *con
);

boolean OCI_ConnectionLogon
(
    OCI_Connection *con
);

boolean OCI_ConnectionDetach
(
    OCI_Connection *con
);

boolean OCI_ConnectionLogOff
(
    OCI_Connection *con
);

boolean OCI_ConnectionClose
(
    OCI_Connection *con
);

/* ------------------------------------------------------------------------ *
 * connpool.c
 * ------------------------------------------------------------------------ */

boolean OCI_ConnPoolClose
(
    OCI_ConnPool *pool
);

/* ------------------------------------------------------------------------ *
 * date.c
 * ------------------------------------------------------------------------ */

OCI_Date * OCI_DateInit
(
    OCI_Connection *con,  
    OCI_Date **pdate,
    OCIDate *buffer,
    boolean allocate, 
    boolean ansi
);

/* ------------------------------------------------------------------------ *
 * define.c
 * ------------------------------------------------------------------------ */

OCI_Define * OCI_GetDefine
(
    OCI_Resultset *rs, 
    unsigned int index
);

int OCI_GetDefineIndex
(
    OCI_Resultset *rs,
    const mtext *name
);

boolean OCI_DefineGetNumber
(
    OCI_Resultset *rs, 
    unsigned int index,
    void *value, 
    uword type,
    uword size
);

boolean OCI_DefineAlloc
(
    OCI_Define *def
);

boolean OCI_DefineDef
(
    OCI_Define *def
);

void *  OCI_DefineGetData
(
    OCI_Define *def
);

/* ------------------------------------------------------------------------ *
 * element.c
 * ------------------------------------------------------------------------ */

boolean OCI_ElemGetNumber
(
    OCI_Elem *elem, 
    void *value, 
    uword size, 
    uword flag
);

boolean OCI_ElemSetNumber
(
    OCI_Elem  *elem, 
    void *value, 
    uword size, 
    uword flag
);

OCI_Elem * OCI_ElemInit
(
    OCI_Connection *con, 
    OCI_Elem **pelem,
    void *handle, 
    OCIInd *pind, 
    OCI_TypeInfo *typeinf
);

/* ------------------------------------------------------------------------ *
 * error.c
 * ------------------------------------------------------------------------ */

void OCI_ErrorFree
(
    OCI_Error *err
);

void OCI_ErrorReset
(
    OCI_Error *err
);

OCI_Error * OCI_ErrorGet
(
    boolean check
);

OCI_Error * OCI_ErrorCreate
(
    void
);

/* ------------------------------------------------------------------------ *
 * exception.c
 * ------------------------------------------------------------------------ */

OCI_Error * OCI_ExceptionGetError(void);

void OCI_ExceptionRaise
(
    OCI_Error *err
);

void OCI_ExceptionOCI
(
    OCIError *p_err, 
    OCI_Connection *con,
    OCI_Statement *stmt
);

void OCI_ExceptionMemory
(
    int type, 
    size_t nb_bytes, 
    OCI_Connection *con,
    OCI_Statement *stmt
);

void OCI_ExceptionNotAvailable
(
    OCI_Connection *con, 
    int feature
);

void OCI_ExceptionNullPointer
(
    int type
);

void OCI_ExceptionDatatypeNotSupported
(
    OCI_Connection *con, 
    OCI_Statement *stmt, 
    int code
);

void OCI_ExceptionParsingToken
(
    OCI_Connection *con, 
    OCI_Statement *stmt, 
    mtext token
);

void OCI_ExceptionMappingArgument
(
    OCI_Connection *con, 
    OCI_Statement *stmt, 
    int arg
);

void OCI_ExceptionNotInitialized(void);

void OCI_ExceptionLoadingSharedLib(void);

void OCI_ExceptionLoadingSymbols(void);

void OCI_ExceptionNotMultithreaded(void);

void OCI_ExceptionOutOfBounds
(
    OCI_Connection *con, 
    int value
);

void OCI_ExceptionUnfreedData
(
    int type_elem,
    int nb_elem
);

void OCI_ExceptionMaxBind
(
    OCI_Statement *stmt
);

void OCI_ExceptionAttributeNotFound
(
    OCI_Connection *con,
    const mtext *attr
);

void OCI_ExceptionMinimumValue
(
    OCI_Connection *con, 
    OCI_Statement *stmt,
    int min
);

void OCI_ExceptionTypeNotCompatible
(
    OCI_Connection *con
);

void OCI_ExceptionStatementState
(
    OCI_Statement *stmt,
    int state
);

void OCI_ExceptionStatementNotScrollable
(
    OCI_Statement *stmt
);

void OCI_ExceptionBindAlreadyUsed
(
    OCI_Statement *stmt,
    const mtext * bind
);

void OCI_ExceptionBindArraySize
(
    OCI_Statement *stmt, 
    unsigned int maxsize, 
    unsigned int cursize, 
    unsigned int newsize
);

void OCI_ExceptionDirPathColNotFound
(
    OCI_DirPath *dp, 
    const mtext * column,
    const mtext *table
);

void OCI_ExceptionDirPathState
(
    OCI_DirPath *dp, 
    int state
);

/* ------------------------------------------------------------------------ *
 * file.c
 * ------------------------------------------------------------------------ */

OCI_File * OCI_FileInit
(
    OCI_Connection *con,                              
    OCI_File **pfile,
    OCILobLocator *handle, 
    ub4 type
);

boolean OCI_FileGetInfo
(
    OCI_File *file
 );

/* ------------------------------------------------------------------------ *
 * format.c
 * ------------------------------------------------------------------------ */

int OCI_ParseSqlFmt
(
    OCI_Statement *stmt,
    mtext *buffer, 
    const mtext *format,
    va_list *pargs
);

/* ------------------------------------------------------------------------ *
 * hash.c
 * ------------------------------------------------------------------------ */

unsigned int OCI_HashCompute
(
    OCI_HashTable *table, 
    const mtext *str
);

boolean OCI_HashAdd
(
    OCI_HashTable *table, 
    const mtext *key, 
    OCI_Variant value, 
    unsigned int type
);

/* ------------------------------------------------------------------------ *
 * interval.c
 * ------------------------------------------------------------------------ */

OCI_Interval  * OCI_IntervalInit
(
    OCI_Connection *con,                              
    OCI_Interval **pitv,
    OCIInterval *buffer, 
    ub4 type
);

/* ------------------------------------------------------------------------ *
 * library.c
 * ------------------------------------------------------------------------ */

void OCI_SetLastError
(
    OCI_Error err
 );

void OCI_SetStatus
(
    boolean res
 );

boolean OCI_KeyMapFree(void);

/* ------------------------------------------------------------------------ *
 * list.c
 * ------------------------------------------------------------------------ */

OCI_List * OCI_ListCreate
(
    int type
);

boolean OCI_ListFree
(
    OCI_List *list
);

OCI_Item * OCI_ListCreateItem
(
    int type,
    int size
);

OCI_Item * OCI_ListAppend
(
    OCI_List *list,
    int size
);

boolean OCI_ListClear
(
    OCI_List *list
);

boolean OCI_ListForEach
(
    OCI_List *list,
    boolean (*proc)(void *)
);

boolean OCI_ListRemove
(
    OCI_List *list, 
    void *data
);

/* ------------------------------------------------------------------------ *
 * lob.c
 * ------------------------------------------------------------------------ */

OCI_Lob * OCI_LobInit
(
    OCI_Connection *con,
    OCI_Lob **plob,
    OCILobLocator *handle,
    ub4 type
);

/* ------------------------------------------------------------------------ *
 * long.c
 * ------------------------------------------------------------------------ */

OCI_Long * OCI_LongInit
(
    OCI_Statement *stmt,
    OCI_Long **plg,
    OCI_Define *def, 
    unsigned int type
);

/* ------------------------------------------------------------------------ *
 * memory.c
 * ------------------------------------------------------------------------ */

void * OCI_MemAlloc
(
    int ptr_type, 
    int block_size, 
    int block_count,
    boolean zero_fill
);

void * OCI_MemRealloc
(
    void * ptr_mem, 
    int ptr_type, 
    int block_size, 
    int block_count
);

void OCI_MemFree
(
    void * ptr_mem
);

sword OCI_HandleAlloc
(
    CONST dvoid *parenth, 
    dvoid **hndlpp, 
    CONST ub4 type, 
    CONST size_t xtramem_sz,
    dvoid **usrmempp
);

sword OCI_HandleFree
(
    dvoid *hndlp, 
    CONST ub4 type
);

sword OCI_DescriptorAlloc
(
    CONST dvoid *parenth, 
    dvoid **descpp,
    CONST ub4 type, 
    CONST size_t xtramem_sz,
    dvoid **usrmempp
);

sword OCI_DescriptorFree
(
    void *descp, CONST ub4 type
);

sword OCI_ObjectNew
(
    OCIEnv *env,
    OCIError *err,
    CONST OCISvcCtx *svc,
    OCITypeCode typecode, 
    OCIType *tdo,
    dvoid *table, 
    OCIDuration duration, 
    boolean value, 
    dvoid **instance
);

sword OCI_OCIObjectFree
(
    OCIEnv *env,
    OCIError *err, 
    dvoid *instance,
    ub2 flags
);


/* ------------------------------------------------------------------------ *
 * mutex.c
 * ------------------------------------------------------------------------ */

OCI_Mutex * OCI_MutexCreateInternal(void);

/* ------------------------------------------------------------------------ *
 * number.c
 * ------------------------------------------------------------------------ */

boolean OCI_NumberGet
(
    OCI_Connection *con,
    OCINumber *data,
    void *value,
    uword size,
    uword flag)
;

boolean OCI_NumberSet
(
    OCI_Connection *con, 
    OCINumber *data,
    void *value, 
    uword size, 
    uword flag
);

boolean OCI_NumberConvertStr
(
    OCI_Connection *con, 
    OCINumber *num, 
    const dtext *str, 
    int str_size, 
    const mtext* fmt, 
    ub4 fmt_size
);

boolean OCI_NumberGetFromStr
    (
    OCI_Connection *con, 
    void *value, 
    uword size,
    uword type,
    const dtext *str,
    int str_size, 
    const mtext* fmt, 
    ub4 fmt_size
);

/* ------------------------------------------------------------------------ *
 * object.c
 * ------------------------------------------------------------------------ */

void OCI_ObjectReset
(
    OCI_Object *obj
);

boolean OCI_ObjectGetAttr
(
    OCI_Object *obj, 
    const mtext *attr, 
    void ** p_value, 
    OCIInd* p_ind, 
    OCIType **p_tdo
);

boolean OCI_ObjectSetAttr
(
    OCI_Object *obj, 
    const mtext *attr, 
    void * value, 
    OCIInd ind
);

boolean OCI_ObjectSetNumber
(
    OCI_Object *obj, 
    const mtext *attr, 
    void *value,
    uword size, uword flag
);

boolean OCI_ObjectGetNumber
(
    OCI_Object *obj, 
    const mtext *attr, 
    void *value,
    uword size, uword flag
);

int OCI_ObjectGetIndex
(
    OCI_Object *obj,
    const mtext *attr,
    int type
);

ub2 OCI_GetIndTabIndex
(
    OCI_TypeInfo *typeinf, 
    int index
);

OCI_Object * OCI_ObjectInit
(
    OCI_Connection *con,
    OCI_Object **pobj,
    void *handle, 
    OCI_TypeInfo *typeinf,
    sb2 *tab_ind,
    int index,
    boolean reset
);

/* ------------------------------------------------------------------------ *
 * ref.c
 * ------------------------------------------------------------------------ */

OCI_Ref * OCI_RefInit
(
    OCI_Connection *con,
    OCI_TypeInfo *typeinf, 
    OCI_Ref **pref, 
    void *handle
);

boolean OCI_RefPin
(
    OCI_Ref *ref
);

boolean OCI_RefUnpin
(
    OCI_Ref *ref
);

/* ------------------------------------------------------------------------ *
 * resultset.c
 * ------------------------------------------------------------------------ */

OCI_Resultset * OCI_ResultsetCreate
(
    OCI_Statement *stmt, 
    int size
);

boolean OCI_ResultsetFree
(
    OCI_Resultset *rs
);

boolean OCI_FetchPieces
(
    OCI_Resultset *rs
);

boolean OCI_FetchData
(
    OCI_Resultset *rs,
    int mode, 
    int offset,
    boolean *err
);

boolean OCI_FetchCustom
(
    OCI_Resultset *rs,
    int mode, 
    int offset,
    boolean *err
);

#ifdef OCI_CHECK_DATASTRINGS 

boolean OCI_ResultsetExpandStrings
(
    OCI_Resultset *rs
);

#endif  

/* ------------------------------------------------------------------------ *
 * statement.c
 * ------------------------------------------------------------------------ */

boolean OCI_BindFreeAll
(
    OCI_Statement *stmt
);

boolean OCI_BindCheck
(
    OCI_Statement *stmt
);

boolean OCI_BindReset
(
    OCI_Statement *stmt
);

boolean OCI_BindData
(
    OCI_Statement *stmt, 
    void *data, 
    ub4 size,
    const mtext *name, 
    ub1 type, 
    unsigned int code, 
    unsigned int mode,
    unsigned int subtype,
    OCI_TypeInfo *typinf,
    unsigned int nbelem
);

int OCI_BindGetIndex
(
    OCI_Statement *stmt, 
    const mtext *name
);

boolean OCI_FetchIntoUserVariables
(
    OCI_Statement *stmt, 
    va_list args
);

boolean OCI_StatementReset
(
    OCI_Statement *stmt
);

boolean OCI_StatementClose
(
    OCI_Statement *stmt
);

OCI_Statement * OCI_StatementInit
(
    OCI_Connection *con,                                  
    OCI_Statement **pstmt,
    OCIStmt *handle,  
    OCI_Define *def
);

/* ------------------------------------------------------------------------ *
 * string.c
 * ------------------------------------------------------------------------ */

int OCI_StringCopy4to2bytes
(
    const unsigned int* src,
    int src_size, 
    unsigned short* dst, 
    int dst_size
);

int OCI_StringCopy2to4bytes
(
    const unsigned short* src,
    int src_size, 
    unsigned int* dst,
    int dst_size
);


void * OCI_GetInputString
(
    void *src, 
    int *size, 
    int size_char_in, 
    int size_char_out
);

void OCI_GetOutputString
(
    void *src, 
    void *dest,
    int *size,
    int size_char_in, 
    int size_char_out
);

void OCI_MoveString
(
    void *src,
    void *dst,
    int char_count, 
    int size_char_in,
    int size_char_out
);
        
void OCI_ConvertString
(
    void *str,
    int char_count, 
    int size_char_in,
    int size_char_out
);

void OCI_CopyString
(
    void *src, 
    void *dest,
    int *size,
    int size_char_in,
    int size_char_out
);

void OCI_ReleaseMetaString
(
    void *ptr
);

void OCI_ReleaseDataString
(
    void *ptr
);

int OCI_StringLength
(
    void *ptr,
    int size_elem
);


#define OCI_GetInputMetaString(s, n)     OCI_GetInputString((void *) s, n,     \
                                                             sizeof(mtext),    \
                                                             sizeof(omtext))

#define OCI_GetOutputMetaString(s, d, n) OCI_GetOutputString((void *) s, d, n, \
                                                             sizeof(omtext),   \
                                                             sizeof(mtext)) 

#define OCI_GetInputDataString(s, n)     OCI_GetInputString((void *) s, n,     \
                                                             sizeof(dtext),    \
                                                             sizeof(odtext))

#define OCI_GetOutputDataString(s, d, n) OCI_GetOutputString((void *) s, d, n, \
                                                              sizeof(odtext),  \
                                                              sizeof(dtext))

void * OCI_StringFromStringPtr
(
    OCIString *str, 
    void ** buf, 
    int *buflen
);

boolean OCI_StringToStringPtr
(
    OCIString **str, 
    OCIError *err, 
    void *value, 
    void **buf, 
    int *buflen
);

/* ------------------------------------------------------------------------ *
 * thread.c
 * ------------------------------------------------------------------------ */

void OCI_ThreadProc
(
    dvoid *arg
);

/* ------------------------------------------------------------------------ *
 * threadkey.c
 * ------------------------------------------------------------------------ */

OCI_ThreadKey * OCI_ThreadKeyCreateInternal
(
    POCI_THREADKEYDEST destfunc
);

boolean OCI_ThreadKeyFree
(
    OCI_ThreadKey *key
);

boolean OCI_ThreadKeySet
(
    OCI_ThreadKey *key,
    void *value
);

boolean OCI_ThreadKeyGet
(
    OCI_ThreadKey* key, 
    void **value
);

/* ------------------------------------------------------------------------ *
 * timestamp.c
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_TimestampInit
(
    OCI_Connection *con, 
    OCI_Timestamp **ptmsp,
    OCIDateTime *buffer, 
    ub4 type
);

/* ------------------------------------------------------------------------ *
 * transaction.c
 * ------------------------------------------------------------------------ */

boolean OCI_TransactionClose
(
    OCI_Transaction * trans
);

/* ------------------------------------------------------------------------ *
 * typeinf.c
 * ------------------------------------------------------------------------ */

boolean OCI_TypeInfoClose
(
    OCI_TypeInfo *typeinf
);


#ifdef  __cplusplus
}
#endif


#endif    /* OCILIB_COMMON_FUNCTIONS_H_INCLUDED */

