/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_OCILIB_INTERNAL_H_INCLUDED
#define OCILIB_OCILIB_INTERNAL_H_INCLUDED

#include "ocilib_types.h"
#include "ocilib_checks.h"

#ifdef __cplusplus
extern "C"
{
#endif

/* ********************************************************************************************* *
                         PRIVATE FUNCTIONS PROTOTYPES
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * agent.c
 * --------------------------------------------------------------------------------------------- */

OCI_Agent * OCI_AgentInit
(
    OCI_Connection *con,
    OCI_Agent      *agent,
    OCIAQAgent     *handle,
    const otext    *name,
    const otext    *address
);

/* --------------------------------------------------------------------------------------------- *
 * array.c
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ArrayInit
(
    OCI_Array    *arr,
    OCI_TypeInfo *typinf
);

boolean OCI_ArrayCleanup
(
    OCI_Array *arr
);

OCI_Array * OCI_ArrayCreate
(
    OCI_Connection *con,
    unsigned int    nb_elem,
    unsigned int    elem_type,
    unsigned int    elem_subtype,
    unsigned int    elem_size,
    unsigned int    struct_size,
    unsigned int    handle_type,
    OCI_TypeInfo   *typinf
);

boolean OCI_ArrayClose
(
    OCI_Array *arr
);

boolean OCI_ArrayFreeFromHandles
(
    void ** handles
);

/* --------------------------------------------------------------------------------------------- *
 * bind.c
 * --------------------------------------------------------------------------------------------- */

OCI_Bind* OCI_BindCreate
(
    OCI_Context   *ctx,
    OCI_Statement *stmt,
    void          *data,
    const otext   *name,
    unsigned int   mode,
    ub4            size,
    ub1            type,
    unsigned int   code,
    unsigned int   subtype,
    OCI_TypeInfo  *typinf,
    unsigned int   nbelem
);

boolean OCI_BindFree
(
    OCI_Bind *bnd
);

boolean OCI_BindAllocData
(
    OCI_Bind *bnd
);

/* --------------------------------------------------------------------------------------------- *
 * callback.c
 * --------------------------------------------------------------------------------------------- */

sb4 OCI_ProcInBind
(
    dvoid   *ictxp,
    OCIBind *bindp,
    ub4      iter,
    ub4      index,
    dvoid  **bufpp,
    ub4     *alenp,
    ub1     *piecep,
    dvoid  **indp
);

sb4 OCI_ProcOutBind
(
    dvoid   *octxp,
    OCIBind *bindp,
    ub4      iter,
    ub4      index,
    dvoid  **bufpp,
    ub4    **alenp,
    ub1     *piecep,
    void   **indp,
    ub2    **rcodep
);

ub4 OCI_ProcNotifyChanges
(
    void            *ctx,
    OCISubscription *subscrhp,
    void            *payload,
    ub4              paylen,
    void            *desc,
    ub4              mode
);

ub4 OCI_ProcNotifyMessages
(
    void            *ctx,
    OCISubscription *subscrhp,
    void            *payload,
    ub4              paylen,
    void            *desc,
    ub4              mode
);

sb4 OCI_ProcFailOver
(
    dvoid *svchp,
    dvoid *envhp,
    dvoid *fo_ctx,
    ub4    fo_type,
    ub4    fo_event
);

void OCI_ProcHAEvent
(
    dvoid     *evtctx,
    dvoid     *eventptr
);

/* --------------------------------------------------------------------------------------------- *
 * collection.c
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OCI_CollInit
(
    OCI_Connection *con,
    OCI_Coll       *Coll,
    void           *handle,
    OCI_TypeInfo   *typeinf
);

/* --------------------------------------------------------------------------------------------- *
 * column.c
 * --------------------------------------------------------------------------------------------- */

boolean OCI_ColumnMap
(
    OCI_Column    *col,
    OCI_Statement *stmt
);

boolean OCI_ColumnDescribe
(
    OCI_Column     *col,
    OCI_Connection *con,
    OCI_Statement  *stmt,
    void           *handle,
    int             index,
    int             ptype
);

boolean OCI_ColumnGetAttrInfo
(
    OCI_Column    *col,
    unsigned int   count,
    unsigned int   index,
    size_t        *p_size,
    size_t        *p_align
);

/* --------------------------------------------------------------------------------------------- *
 * connection.c
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_ConnectionCreateInternal
(
    OCI_Pool    *pool,
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int mode,
    const otext *tag
);

boolean OCI_ConnectionClose
(
    OCI_Connection *con
);

/* --------------------------------------------------------------------------------------------- *
 * date.c
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_DateInit
(
    OCI_Connection *con,
    OCI_Date       *date,
    OCIDate        *buffer,
    boolean         allocate,
    boolean         ansi
);

/* --------------------------------------------------------------------------------------------- *
 * define.c
 * --------------------------------------------------------------------------------------------- */

OCI_Define * OCI_GetDefine
(
    OCI_Resultset *rs,
    unsigned int   index
);

int OCI_GetDefineIndex
(
    OCI_Resultset *rs,
    const otext   *name
);

boolean OCI_DefineGetNumber
(
    OCI_Resultset *rs,
    unsigned int   index,
    void          *value,
    uword          type
);

boolean OCI_DefineAlloc
(
    OCI_Define *def
);

boolean OCI_DefineDef
(
    OCI_Define *def,
    ub4         position
);

void * OCI_DefineGetData
(
    OCI_Define *def
);

boolean OCI_DefineIsDataNotNull
(
    OCI_Define *def
);

/* --------------------------------------------------------------------------------------------- *
 * element.c
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * OCI_ElemInit
(
    OCI_Connection *con,
    OCI_Elem       *elem,
    void           *handle,
    OCIInd         *pind,
    OCI_TypeInfo   *typeinf
);

boolean OCI_ElemSetNullIndicator
(
    OCI_Elem *elem,
    OCIInd    value
);

/* --------------------------------------------------------------------------------------------- *
 * error.c
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OCI_ErrorCreate
(
    void
);

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

/* --------------------------------------------------------------------------------------------- *
 * event.c
 * --------------------------------------------------------------------------------------------- */

boolean OCI_EventReset
(
    OCI_Event *event
);

/* --------------------------------------------------------------------------------------------- *
 * exception.c
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OCI_ExceptionGetError
(
    void
);

void OCI_ExceptionRaise
(
    OCI_Error *err
);

void OCI_ExceptionOCI
(
    OCIError       *p_err,
    OCI_Connection *con,
    OCI_Statement  *stmt,
    boolean         warning
);

void OCI_ExceptionMemory
(
    int             type,
    size_t          nb_bytes,
    OCI_Connection *con,
    OCI_Statement  *stmt
);

void OCI_ExceptionNotAvailable
(
    OCI_Connection *con,
    int             feature
);

void OCI_ExceptionNullPointer
(
    int type
);

void OCI_ExceptionDatatypeNotSupported
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    int             code
);

void OCI_ExceptionParsingToken
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    otext           token
);

void OCI_ExceptionMappingArgument
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    int             arg
);

void OCI_ExceptionNotInitialized
(
    void
);

void OCI_ExceptionLoadingSharedLib
(
    void
);

void OCI_ExceptionLoadingSymbols
(
    void
);

void OCI_ExceptionNotMultithreaded
(
    void
);

void OCI_ExceptionOutOfBounds
(
    OCI_Connection *con,
    int             value
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
    const otext    *attr
);

void OCI_ExceptionMinimumValue
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    int             min
);

void OCI_ExceptionTypeNotCompatible
(
    OCI_Connection *con
);

void OCI_ExceptionStatementState
(
    OCI_Statement *stmt,
    int            state
);

void OCI_ExceptionStatementNotScrollable
(
    OCI_Statement *stmt
);

void OCI_ExceptionBindAlreadyUsed
(
    OCI_Statement *stmt,
    const otext   *bind
);

void OCI_ExceptionBindArraySize
(
    OCI_Statement *stmt,
    unsigned int   maxsize,
    unsigned int   cursize,
    unsigned int   newsize
);

void OCI_ExceptionDirPathColNotFound
(
    OCI_DirPath  *dp,
    const otext  *column,
    const otext  *table
);

void OCI_ExceptionDirPathState
(
    OCI_DirPath *dp,
    int          state
);

void OCI_ExceptionOCIEnvironment
(
    void
);

void OCI_ExceptionRebindBadDatatype
(
    OCI_Statement *stmt,
    const otext   *bind
);

void OCI_ExceptionTypeInfoWrongType
(
    OCI_Connection *con,
    const otext    *name
);

void OCI_ExceptionItemNotFound
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    const otext    *name,
    unsigned int    type
);

void OCI_ExceptionArgInvalidValue
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    const otext    *name,
    unsigned int    value
);

void OCI_ExceptionEnvFromXaString
(
    const otext *value
);

void OCI_ExceptionConnFromXaString
(
    const otext *value
);

void OCI_ExceptionExternalBindingNotAllowed
(
    OCI_Statement *stmt,
    const otext   *bind
);

/* --------------------------------------------------------------------------------------------- *
 * file.c
 * --------------------------------------------------------------------------------------------- */

OCI_File * OCI_FileInit
(
    OCI_Connection *con,
    OCI_File       *file,
    OCILobLocator  *handle,
    ub4             type
);

boolean OCI_FileGetInfo
(
    OCI_File *file
);

/* --------------------------------------------------------------------------------------------- *
 * format.c
 * --------------------------------------------------------------------------------------------- */

int OCI_ParseSqlFmt
(
    OCI_Statement *stmt,
    otext         *buffer,
    const otext   *format,
    va_list       *pargs
);

/* --------------------------------------------------------------------------------------------- *
 * hash.c
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_HashCompute
(
    OCI_HashTable *table,
    const otext   *str
);

boolean OCI_HashAdd
(
    OCI_HashTable *table,
    const otext   *key,
    OCI_Variant    value,
    unsigned int   type
);

/* --------------------------------------------------------------------------------------------- *
 * interval.c
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OCI_IntervalInit
(
    OCI_Connection *con,
    OCI_Interval   *itv,
    OCIInterval    *buffer,
    ub4             type
);

/* --------------------------------------------------------------------------------------------- *
 * library.c
 * --------------------------------------------------------------------------------------------- */

void OCI_CallEnter
(
    OCI_Context *ctx
);

void OCI_CallExit
(
    OCI_Context *ctx
);

boolean OCI_KeyMapFree
(
    void
);

unsigned int OCI_ExternalSubTypeToSQLType
(
    unsigned int type,
    unsigned int subtype
);

unsigned int OCI_ExternalSubTypeToHandleType
(
    unsigned int type,
    unsigned int subtype
);

boolean OCI_FreeObjectFromType
(
    void         *obj,
    unsigned int  type
);

boolean OCI_GetStringAttribute
(
    OCI_Connection *con,
    void           *handle,
    unsigned int    type,
    unsigned int    attr,
    otext         **str,
    unsigned int   *size
);

boolean OCI_SetStringAttribute
(
    OCI_Connection *con,
    void           *handle,
    unsigned int    type,
    unsigned int    attr,
    otext         **str,
    const otext    *value
);

/* --------------------------------------------------------------------------------------------- *
 * list.c
 * --------------------------------------------------------------------------------------------- */

OCI_List * OCI_ListCreate
(
    int type
);

boolean OCI_ListFree
(
    OCI_List *list
);

void * OCI_ListAppend
(
    OCI_List *list,
    int       size
);

boolean OCI_ListClear
(
    OCI_List *list
);

typedef void (*POCI_LIST_FOR_EACH)(void *data);

boolean OCI_ListForEach
(
    OCI_List          *list,
    POCI_LIST_FOR_EACH proc
);

typedef void(*POCI_LIST_FOR_EACH_WITH_PARAM)(void *data, void *param);

boolean OCI_ListForEachWithParam
(
    OCI_List          *list,
    void              *param,
    POCI_LIST_FOR_EACH_WITH_PARAM proc
);

boolean OCI_ListRemove
(
    OCI_List *list,
    void     *data
);

boolean OCI_ListExists
(
    OCI_List *list,
    void     *data
);

typedef boolean(*POCI_LIST_FIND)(void *data, void *param);

void * OCI_ListFind
(
    OCI_List        *list,
    POCI_LIST_FIND   proc,
    void            *param
);

/* --------------------------------------------------------------------------------------------- *
 * lob.c
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_LobInit
(
    OCI_Connection *con,
    OCI_Lob        *lob,
    OCILobLocator  *handle,
    ub4             type
);

/* --------------------------------------------------------------------------------------------- *
 * long.c
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OCI_LongInit
(
    OCI_Statement *stmt,
    OCI_Long      *lg,
    OCI_Define    *def,
    unsigned int   type
);

/* --------------------------------------------------------------------------------------------- *
 * memory.c
 * --------------------------------------------------------------------------------------------- */

void * OCI_MemAlloc
(
    int     ptr_type,
    size_t  block_size,
    size_t  block_count,
    boolean zero_fill
);

void * OCI_MemRealloc
(
    void * ptr_mem,
    int    ptr_type,
    size_t block_size,
    size_t block_count,
    boolean zero_fill
);

void OCI_MemFree
(
    void * ptr_mem
);

boolean OCI_HandleAlloc
(
    CONST dvoid *parenth,
    dvoid      **hndlpp,
    ub4          type
);

boolean OCI_HandleFree
(
    dvoid    *hndlp,
    ub4       type
);

boolean OCI_DescriptorAlloc
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type
);

boolean OCI_DescriptorArrayAlloc
(
    CONST dvoid *parenth,
    dvoid      **descpp,
    ub4          type,
    ub4          nb_elem
 );

boolean OCI_DescriptorFree
(
    void *descp,
    ub4   type
);

boolean OCI_DescriptorArrayFree
(
    void **descp,
    ub4    type,
    ub4    nb_elem
);

sword OCI_ObjectNew
(
    OCIEnv          *env,
    OCIError        *err,
    CONST OCISvcCtx *svc,
    OCITypeCode      typecode,
    OCIType         *tdo,
    dvoid           *table,
    OCIDuration      duration,
    boolean          value,
    dvoid          **instance
);

sword OCI_OCIObjectFree
(
    OCIEnv   *env,
    OCIError *err,
    dvoid    *instance,
    ub2       flags
);

void * OCI_MemAllocOracleClient
(
    void   *ctxp, 
    size_t  size
);

void * OCI_MemReallocOracleClient
(
    void    *ctxp, 
    void    *memptr, 
    size_t  newsize
);

void OCI_MemFreeOracleClient
(
    void *ctxp,
    void *memptr
);

void OCI_MemUpdateBytes
(
    int type,
    big_int  size
);

/* --------------------------------------------------------------------------------------------- *
 * mutex.c
 * --------------------------------------------------------------------------------------------- */

OCI_Mutex * OCI_MutexCreateInternal
(
    void
);

/* --------------------------------------------------------------------------------------------- *
 * number.c
 * --------------------------------------------------------------------------------------------- */

boolean OCI_TranslateNumericValue
(
    OCI_Connection *con,
    void           *in_value,
    uword           in_type,
    void           *out_value,
    uword           out_type
);

OCI_Number * OCI_NumberInit
(
    OCI_Connection  *con,
    OCI_Number      *number,
    OCINumber       *buffer
);

boolean OCI_NumberFromString
(
    OCI_Connection *con,
    void           *out_value,
    uword           type,
    const otext    *in_value,
    const otext   * fmt
);

boolean OCI_NumberToString
(
    OCI_Connection *con,
    void           *number,
    unsigned int    type,
    otext          *out_value,
    int             out_value_size,
    const otext   * fmt
);

/* --------------------------------------------------------------------------------------------- *
 * object.c
 * --------------------------------------------------------------------------------------------- */

void OCI_ObjectGetUserStructSize
(
    OCI_TypeInfo *typinf,
    size_t       *p_size,
    size_t       *p_align
);

OCI_Object * OCI_ObjectInit
(
    OCI_Connection *con,
    OCI_Object     *obj,
    void           *handle,
    OCI_TypeInfo   *typinf,
    OCI_Object     *parent,
    int             index,
    boolean         reset
);

/* --------------------------------------------------------------------------------------------- *
 * pool.c
 * --------------------------------------------------------------------------------------------- */

boolean OCI_PoolClose
(
    OCI_Pool *pool
);

/* --------------------------------------------------------------------------------------------- *
 * ref.c
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OCI_RefInit
(
    OCI_Connection *con,
    OCI_TypeInfo   *typeinf,
    OCI_Ref        *ref,
    void           *handle
);

boolean OCI_RefPin
(
    OCI_Ref *ref
);

boolean OCI_RefUnpin
(
    OCI_Ref *ref
);

/* --------------------------------------------------------------------------------------------- *
 * resultset.c
 * --------------------------------------------------------------------------------------------- */

OCI_Resultset * OCI_ResultsetCreate
(
    OCI_Statement *stmt,
    int            size
);

boolean OCI_ResultsetInit
(
    OCI_Resultset *rs
);

boolean OCI_ResultsetFree
(
    OCI_Resultset *rs
);

/* --------------------------------------------------------------------------------------------- *
 * statement.c
 * --------------------------------------------------------------------------------------------- */

int OCI_BindGetInternalIndex
(
    OCI_Statement *stmt,
    const otext   *name
);

boolean OCI_FetchIntoUserVariables
(
    OCI_Statement *stmt,
    va_list        args
);

boolean OCI_StatementClose
(
    OCI_Statement *stmt
);

OCI_Statement * OCI_StatementInit
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    OCIStmt        *handle,
    boolean         is_desc,
    const otext    *sql
);

boolean OCI_API OCI_PrepareInternal
(
    OCI_Statement *stmt,
    const otext   *sql
);

boolean OCI_API OCI_ExecuteInternal
(
    OCI_Statement *stmt,
    ub4            mode
);

/* --------------------------------------------------------------------------------------------- *
 * string.c
 * --------------------------------------------------------------------------------------------- */

size_t OCI_StringLength
(
    void const  *ptr,
    size_t size_elem
);

unsigned int OCI_StringBinaryToString
(
    const unsigned char *binary,
    unsigned int         binary_size,
    otext               *buffer
);

boolean OCI_StringRequestBuffer
(
    otext         **buffer,
    unsigned int   *buffer_size,
    unsigned int    request_size
);

void OCI_StringTranslate
(
    void  *src,
    void  *dst,
    int    len,
    size_t size_char_in,
    size_t size_char_out
);

#define OCI_StringAnsiToNative(s, d, l)    \
    OCI_StringTranslate( (void *) (s), (void *) (d), l, sizeof(char),   sizeof(otext) )

#define OCI_StringNativeToAnsi(s, d, l)     \
    OCI_StringTranslate( (void *) (s), (void *) (d), l, sizeof(otext),  sizeof(char)  )

#define OCI_StringUTF16ToUTF32(s, d, l)     \
    OCI_StringTranslate( (void *) (s), (void *) (d), l, sizeof(short),  sizeof(int)   )

#define OCI_StringUTF32ToUTF16(s, d, l)    \
    OCI_StringTranslate( (void *) (s), (void *) (d), l, sizeof(int),    sizeof(short) )

#define OCI_StringOracleToNative(s, d, l)  \
    OCI_StringTranslate( (void *) (s), (void *) (d), l, sizeof(dbtext), sizeof(otext) )

#define OCI_StringRawCopy(s, d, l)          \
    OCI_StringTranslate( (void *) (s), (void *) (d), l, sizeof(otext),  sizeof(otext) )

dbtext * OCI_StringGetOracleString
(
    const otext  *src,
    int          *len
);

void OCI_StringReleaseOracleString
(
    dbtext *str
);

int OCI_StringCopyOracleStringToNativeString
(
    const dbtext  *src,
    otext         *dst,
    int            len
);

otext * OCI_StringDuplicateFromOracleString
(
    const dbtext  *src,
    int            len
);

otext * OCI_StringFromStringPtr
(
    OCIEnv           *env,
    OCIString        *str,
    otext          **buffer,
    unsigned int     *buffer_size
);

boolean OCI_StringToStringPtr
(
    OCIEnv      *env,
    OCIString  **str,
    OCIError    *err,
    const otext *value
);

boolean OCI_StringFreeStringPtr
(
    OCIEnv      *env,
    OCIString  **str,
    OCIError    *err
);

unsigned int OCI_StringGetFromType
(
    OCI_Connection   *con,
    OCI_Column       *col,
    void             *data,
    unsigned int      data_size,
    otext            *buffer,
    unsigned int      buffer_size,
    boolean           quote
);

unsigned int OCI_StringAddToBuffer
(
    otext           *buffer,
    unsigned int     offset,
    const otext     *str,
    boolean          check_quote
);

unsigned int OCI_StringGetTypeName
(
    const otext  *source,
    otext        *dest,
    unsigned int  length
);

unsigned int OCI_StringGetFullTypeName
(
    const otext  *schema,
    const otext  *package,
    const otext  *type,
    const otext  *link,
    otext        *name,
    unsigned int  length
);

/* --------------------------------------------------------------------------------------------- *
 * subscription.c
 * --------------------------------------------------------------------------------------------- */

boolean OCI_SubscriptionClose
(
    OCI_Subscription *sub
);

/* --------------------------------------------------------------------------------------------- *
 * thread.c
 * --------------------------------------------------------------------------------------------- */

void OCI_ThreadProc
(
    dvoid *arg
);

/* --------------------------------------------------------------------------------------------- *
 * threadkey.c
 * --------------------------------------------------------------------------------------------- */

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
    void          *value
);

boolean OCI_ThreadKeyGet
(
    OCI_ThreadKey* key,
    void         **value
);

/* --------------------------------------------------------------------------------------------- *
 * timestamp.c
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_TimestampInit
(
    OCI_Connection *con,
    OCI_Timestamp  *tmsp,
    OCIDateTime    *buffer,
    ub4             type
);

/* --------------------------------------------------------------------------------------------- *
 * transaction.c
 * --------------------------------------------------------------------------------------------- */

boolean OCI_TransactionClose
(
    OCI_Transaction * trans
);

/* --------------------------------------------------------------------------------------------- *
 * typeinf.c
 * --------------------------------------------------------------------------------------------- */

boolean OCI_TypeInfoClose
(
    OCI_TypeInfo *typeinf
);

#ifdef  __cplusplus
}
#endif

#endif    /* OCILIB_OCILIB_TYPES_H_INCLUDED */

