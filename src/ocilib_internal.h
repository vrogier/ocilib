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


#ifdef  __cplusplus
}
#endif

#endif    /* OCILIB_OCILIB_TYPES_H_INCLUDED */

