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

#include "long.h"

#include "macros.h"
#include "memory.h"
#include "strings.h"

static const unsigned int LongTypeValues[] = { OCI_CLONG, OCI_BLONG };

/* --------------------------------------------------------------------------------------------- *
 * LongInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Long * LongInitialize
(
    OCI_Statement *stmt,
    OCI_Long      *lg,
    OCI_Define    *def,
    unsigned int   type
)
{
    DECLARE_CTX(TRUE)
    CTX_SET_FROM_STMT(stmt)

    ALLOC_DATA(OCI_IPC_LONG, lg, 1);

    if (STATUS)
    {
        lg->size        = 0;
        lg->maxsize     = 0;
        lg->stmt        = stmt;
        lg->def         = def;
        lg->type        = type;
        lg->offset      = 0;

        if (def)
        {
            lg->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
        else if (OCI_OBJECT_ALLOCATED_ARRAY != lg->hstate)
        {
            lg->hstate = OCI_OBJECT_ALLOCATED;
        }
    }

    return lg;
}

/* --------------------------------------------------------------------------------------------- *
 * LongCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Long * LongCreate
(
    OCI_Statement *stmt,
    unsigned int   type
)
{
    CALL_ENTER(OCI_Long*, NULL)
    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_ENUM_VALUE(stmt->con, stmt, type, LongTypeValues, OTEXT("Long Type"))
    CTX_SET_FROM_STMT(stmt)

    RETVAL = LongInitialize(stmt, NULL, NULL, type);
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LongFree
 * --------------------------------------------------------------------------------------------- */

boolean LongFree
(
    OCI_Long *lg
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_LONG, lg)
    CHECK_OBJECT_FETCHED(lg)
    CTX_SET_FROM_STMT(lg->stmt)

    FREE(lg->buffer)
    FREE(lg)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LongGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int LongGetType
(
    OCI_Long *lg
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_LONG, lg, type, lg->stmt->con, lg->stmt, lg->stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * LongRead
 * --------------------------------------------------------------------------------------------- */

unsigned int LongRead
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
)
{
    CALL_ENTER(unsigned int, 0)
    CHECK_PTR(OCI_IPC_LONG, lg)
    CHECK_PTR(OCI_IPC_VOID, buffer)
    CTX_SET_FROM_STMT(lg->stmt)

    STATUS = TRUE;
    RETVAL = len;

    /* lg->size and lg offset are still expressed in db text units even
       if the buffer had already been expanded to otext *
    */

    if (OCI_CLONG == lg->type)
    {
        RETVAL *= (unsigned int) sizeof(dbtext);
    }

    /* check buffer size to read */

    if ((RETVAL + lg->offset) > lg->size)
    {
        RETVAL = lg->size - lg->offset;
    }

    /* copy buffer */

    memcpy(buffer, lg->buffer + (size_t) lg->offset, (size_t) (RETVAL));

    lg->offset += RETVAL;

    if (OCI_CLONG == lg->type)
    {
        ((otext *)buffer)[RETVAL] = 0;

        RETVAL /= (unsigned int) sizeof(otext);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LongWrite
 * --------------------------------------------------------------------------------------------- */

unsigned int LongWrite
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
)
{
    sword code   = OCI_SUCCESS;
    void *obuf   = NULL;
    void *handle = NULL;
    ub1 in_out   = OCI_PARAM_IN;
    ub1 piece    = OCI_ONE_PIECE;
    ub4 type     = 0;
    ub4 iter     = 0;
    ub4 dx       = 0;
    ub4 count    = 0;

    CALL_ENTER(unsigned int, 0)
    CHECK_PTR(OCI_IPC_VOID, buffer)
    CHECK_PTR(OCI_IPC_LONG, lg)
    CTX_SET_FROM_STMT(lg->stmt)

    if (OCI_CLONG == lg->type)
    {
        len *= (unsigned int) sizeof(otext);
        obuf = StringGetDBString((const otext *) buffer, (int *) &len);
    }
    else
    {
        obuf = buffer;
    }

    /* get piece info */

    EXEC(OCIStmtGetPieceInfo(lg->stmt->stmt, lg->stmt->con->err, &handle, &type, &in_out, &iter, &dx, &piece))

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

    EXEC(OCIStmtSetPieceInfo(handle, type, lg->stmt->con->err, (dvoid *) obuf, &count,  piece, (dvoid *) NULL, (ub2 *) NULL))

    /* perform write call */

    if (STATUS)
    {
        code = OCIStmtExecute(lg->stmt->con->cxt, lg->stmt->stmt,
                              lg->stmt->con->err, (ub4) 1, (ub4) 0,
                              (OCISnapshot *) NULL, (OCISnapshot *) NULL,
                              (ub4) 0);
    }

    if (OCI_FAILURE(code) && (OCI_NEED_DATA != code))
    {
        STATUS = (OCI_SUCCESS_WITH_INFO == code);

        ExceptionOCI(lg->stmt->con->err, lg->stmt->con, lg->stmt, STATUS);
    }

    if (OCI_CLONG == lg->type)
    {
        StringReleaseDBString((dbtext *) obuf);
    }

    /* update size */

    if (STATUS)
    {
        lg->size += count;

        /* at this point, count is expressed in db text bytes for character LONGs
         **/

        if (OCI_CLONG == lg->type)
        {
            count /= (unsigned int) sizeof(dbtext);
        }

    }

    RETVAL = count;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LongGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int LongGetSize
(
    OCI_Long *lg
)
{
    CALL_ENTER(unsigned int, 0)
    CHECK_PTR(OCI_IPC_LONG, lg)
    CTX_SET_FROM_STMT(lg->stmt)

    RETVAL = lg->size;

    if (OCI_CLONG == lg->type)
    {
        RETVAL /= (unsigned int) sizeof(dbtext);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LongGetBuffer
 * --------------------------------------------------------------------------------------------- */

void * LongGetBuffer
(
    OCI_Long *lg
)
{
    GET_PROP(void *, NULL, OCI_IPC_LONG, lg, buffer, lg->stmt->con, lg->stmt, lg->stmt->con->err)
}
