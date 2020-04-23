/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2020 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "agent.h"
#include "array.h"
#include "bind.h"
#include "callback.h"
#include "collection.h"
#include "column.h"
#include "connection.h"
#include "date.h"
#include "define.h"
#include "dequeue.h"
#include "dirpath.h"
#include "element.h"
#include "enqueue.h"
#include "error.h"
#include "event.h"
#include "exception.h"
#include "file.h"
#include "format.h"
#include "handle.h"
#include "hash.h"
#include "helpers.h"
#include "interval.h"
#include "iterator.h"
#include "library.h"
#include "list.h"
#include "lob.h"
#include "long.h"
#include "macro.h"
#include "memory.h"
#include "msg.h"
#include "mutex.h"
#include "number.h"
#include "object.h"
#include "pool.h"
#include "queue.h"
#include "ref.h"
#include "resultset.h"
#include "statement.h"
#include "string.h"
#include "subscription.h"
#include "thread.h"
#include "threadkey.h"
#include "timestamp.h"
#include "transaction.h"
#include "typeinfo.h"



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
    unsigned int     length,
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

