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

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE VARIABLES
 * ********************************************************************************************* */

static const unsigned int LongTypeValues[] = { OCI_CLONG, OCI_BLONG };

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongInit
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OCI_LongInit
(
    OCI_Statement *stmt,
    OCI_Long      *lg,
    OCI_Define    *def,
    unsigned int   type
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_STMT(stmt)

    OCI_ALLOCATE_DATA(OCI_IPC_LONG, lg, 1);

    if (OCI_STATUS)
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

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Long * OCI_API OCI_LongCreate
(
    OCI_Statement *stmt,
    unsigned int   type
)
{
    OCI_CALL_ENTER(OCI_Long*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    OCI_CALL_CHECK_ENUM_VALUE(stmt->con, stmt, type, LongTypeValues, OTEXT("Long Type"))
    OCI_CALL_CONTEXT_SET_FROM_STMT(stmt)

    OCI_RETVAL = OCI_LongInit(stmt, NULL, NULL, type);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LongFree
(
    OCI_Long *lg
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LONG, lg)
    OCI_CALL_CHECK_OBJECT_FETCHED(lg)
    OCI_CALL_CONTEXT_SET_FROM_STMT(lg->stmt)

    OCI_FREE(lg->buffer)
    OCI_FREE(lg)

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LongGetType
(
    OCI_Long *lg
)
{
    OCI_GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_LONG, lg, type, lg->stmt->con, lg->stmt, lg->stmt->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongRead
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LongRead
(
    OCI_Long    *lg,
    void        *buffer,
    unsigned int len
)
{
    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_LONG, lg)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, buffer)
    OCI_CALL_CONTEXT_SET_FROM_STMT(lg->stmt)

    OCI_STATUS = TRUE;
    OCI_RETVAL = len;

    /* lg->size and lg offset are still expressed in db text units even
       if the buffer had already been expanded to otext *
    */

    if (OCI_CLONG == lg->type)
    {
        OCI_RETVAL *= (unsigned int) sizeof(dbtext);
    }

    /* check buffer size to read */

    if ((OCI_RETVAL + lg->offset) > lg->size)
    {
        OCI_RETVAL = lg->size - lg->offset;
    }

    /* copy buffer */

    memcpy(buffer, lg->buffer + (size_t) lg->offset, (size_t) (OCI_RETVAL));

    lg->offset += OCI_RETVAL;

    if (OCI_CLONG == lg->type)
    {
        ((otext *)buffer)[OCI_RETVAL] = 0;

        OCI_RETVAL /= (unsigned int) sizeof(otext);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongWrite
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LongWrite
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

    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, buffer)
    OCI_CALL_CHECK_PTR(OCI_IPC_LONG, lg)
    OCI_CALL_CONTEXT_SET_FROM_STMT(lg->stmt)

    if (OCI_CLONG == lg->type)
    {
        len *= (unsigned int) sizeof(otext);
        obuf = OCI_StringGetOracleString((const otext *) buffer, (int *) &len);
    }
    else
    {
        obuf = buffer;
    }

    /* get piece info */

    OCI_EXEC(OCIStmtGetPieceInfo(lg->stmt->stmt, lg->stmt->con->err, &handle, &type, &in_out, &iter, &dx, &piece))

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

    OCI_EXEC(OCIStmtSetPieceInfo(handle, type, lg->stmt->con->err, (dvoid *) obuf, &count,  piece, (dvoid *) NULL, (ub2 *) NULL))

    /* perform write call */

    if (OCI_STATUS)
    {
        code = OCIStmtExecute(lg->stmt->con->cxt, lg->stmt->stmt,
                              lg->stmt->con->err, (ub4) 1, (ub4) 0,
                              (OCISnapshot *) NULL, (OCISnapshot *) NULL,
                              (ub4) 0);
    }

    if (OCI_FAILURE(code) && (OCI_NEED_DATA != code))
    {
        OCI_STATUS = (OCI_SUCCESS_WITH_INFO == code);

        OCI_ExceptionOCI(lg->stmt->con->err, lg->stmt->con, lg->stmt, OCI_STATUS);
    }

    if (OCI_CLONG == lg->type)
    {
        OCI_StringReleaseOracleString((dbtext *) obuf);
    }

    /* update size */

    if (OCI_STATUS)
    {
        lg->size += count;

        /* at this point, count is expressed in db text bytes for character LONGs
         **/

        if (OCI_CLONG == lg->type)
        {
            count /= (unsigned int) sizeof(dbtext);
        }

    }

    OCI_RETVAL = count;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongGetSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LongGetSize
(
    OCI_Long *lg
)
{
    OCI_CALL_ENTER(unsigned int, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_LONG, lg)
    OCI_CALL_CONTEXT_SET_FROM_STMT(lg->stmt)

    OCI_RETVAL = lg->size;

    if (OCI_CLONG == lg->type)
    {
        OCI_RETVAL /= (unsigned int) sizeof(dbtext);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LongGetBuffer
 * --------------------------------------------------------------------------------------------- */

void * OCI_API OCI_LongGetBuffer
(
    OCI_Long *lg
)
{
    OCI_GET_PROP(void *, NULL, OCI_IPC_LONG, lg, buffer, lg->stmt->con, lg->stmt, lg->stmt->con->err)
}
