/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2021 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "long.h"

#include "macros.h"
#include "memory.h"
#include "stringutils.h"

static const unsigned int LongTypeValues[] =
{
    OCI_CLONG,
    OCI_BLONG
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibLongInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OcilibLongInitialize
(
    OCI_Statement *stmt,
    OCI_Long      *lg,
    OCI_Define    *def,
    unsigned int   type
)
{
    ENTER_FUNC_NO_CONTEXT
    (
        /* returns */ OCI_Long*, lg
    )

    ALLOC_DATA(OCI_IPC_LONG, lg, 1);

    lg->size    = 0;
    lg->maxsize = 0;
    lg->stmt    = stmt;
    lg->def     = def;
    lg->type    = type;
    lg->offset  = 0;

    if (def)
    {
        lg->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }
    else if (OCI_OBJECT_ALLOCATED_ARRAY != lg->hstate)
    {
        lg->hstate = OCI_OBJECT_ALLOCATED;
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibLongFree(lg);
            lg = NULL;
        }

        SET_RETVAL(lg)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLongCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OcilibLongCreate
(
    OCI_Statement *stmt,
    unsigned int   type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Long*, NULL,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_ENUM_VALUE(type, LongTypeValues, OTEXT("Long Type"))

    SET_RETVAL(OcilibLongInitialize(stmt, NULL, NULL, type))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLongFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibLongFree
(
    OCI_Long *lg
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LONG, lg
    )

    CHECK_PTR(OCI_IPC_LONG, lg)
    CHECK_OBJECT_FETCHED(lg)

    OcilibErrorResetSource(NULL, lg);

    FREE(lg->buffer)
    FREE(lg)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLongGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibLongGetType
(
    OCI_Long *lg
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_LONG, lg,
        /* member */ type
    )
}

/* --------------------------------------------------------------------------------------------- *
 * LongRead
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibLongRead
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_LONG, lg
    )

    CHECK_PTR(OCI_IPC_LONG, lg)
    CHECK_PTR(OCI_IPC_VOID, buffer)

    /* lg->size and lg offset are still expressed in db text units even
       if the buffer had already been expanded to otext *
    */

    if (OCI_CLONG == lg->type)
    {
        len *= (unsigned int) sizeof(dbtext);
    }

    /* check buffer size to read */

    if ((len + lg->offset) > lg->size)
    {
        len = lg->size - lg->offset;
    }

    /* copy buffer */

    memcpy(buffer, lg->buffer + (size_t) lg->offset, (size_t) (len));

    lg->offset += len;

    if (OCI_CLONG == lg->type)
    {
        ((otext *)buffer)[len] = 0;

        len /= (unsigned int) sizeof(otext);
    }

    SET_RETVAL(len)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLongWrite
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibLongWrite
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_LONG, lg
    )

    sword code = OCI_SUCCESS;
    void *obuf   = NULL;
    void *handle = NULL;
    ub1   in_out = OCI_PARAM_IN;
    ub1   piece  = OCI_ONE_PIECE;
    ub4   type   = 0;
    ub4   iter   = 0;
    ub4   dx     = 0;
    ub4   count  = 0;

    CHECK_PTR(OCI_IPC_VOID, buffer)
    CHECK_PTR(OCI_IPC_LONG, lg)

    if (OCI_CLONG == lg->type)
    {
        len *= (unsigned int) sizeof(otext);
        obuf = OcilibStringGetDBString((const otext *) buffer, (int *) &len);
    }
    else
    {
        obuf = buffer;
    }

    /* get piece info */

    CHECK_OCI
    (
        lg->stmt->con->err,
        OCIStmtGetPieceInfo,
        lg->stmt->stmt, lg->stmt->con->err,
        &handle, &type, &in_out, &iter, &dx, &piece
    )

    /* set up piece type */

    if (len > 0)
    {
        piece = (ub1) ((lg->size > 0) ? OCI_NEXT_PIECE : OCI_FIRST_PIECE);
    }
    else
    {
        piece = (ub1) OCI_LAST_PIECE;
    }

    /* correct size to write for last piece flag */

    if ((lg->size + len) >= lg->maxsize)
    {
        piece = OCI_LAST_PIECE;
        count = lg->maxsize - lg->size;
    }
    else
    {
        count = len;
    }

    /* set up info for writing */

    CHECK_OCI
    (
        lg->stmt->con->err,
        OCIStmtSetPieceInfo,
        handle, type, lg->stmt->con->err,
        (dvoid *) obuf, &count,  piece,
        (dvoid *) NULL, (ub2 *) NULL
    )

    /* perform write call */

    code = OCIStmtExecute(lg->stmt->con->cxt, lg->stmt->stmt,
                          lg->stmt->con->err, (ub4) 1, (ub4) 0,
                          (OCISnapshot *) NULL, (OCISnapshot *) NULL,
                          (ub4) 0);

    if (OCI_FAILURE(code) && (OCI_NEED_DATA != code))
    {
        THROW(OcilibExceptionOCI, lg->stmt->con->err, code)
    }

    /* update size */

    lg->size += count;

    /* at this point, count is expressed in db text bytes for character LONGs
     **/

    if (OCI_CLONG == lg->type)
    {
        count /= (unsigned int) sizeof(dbtext);
    }

    SET_RETVAL(count)

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != lg && OCI_CLONG == lg->type)
        {
            OcilibStringReleaseDBString((dbtext*)obuf);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLongGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibLongGetSize
(
    OCI_Long *lg
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_LONG, lg
    )

    CHECK_PTR(OCI_IPC_LONG, lg)

    unsigned int size = lg->size;

    if (OCI_CLONG == lg->type)
    {
        size /= (unsigned int) sizeof(dbtext);
    }

    SET_RETVAL(size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLongGetBuffer
 * --------------------------------------------------------------------------------------------- */

void * OcilibLongGetBuffer
(
    OCI_Long *lg
)
{
    GET_PROP
    (
        /* result */ void *, NULL,
        /* handle */ OCI_IPC_LONG, lg,
        /* member */ buffer
    )
}
