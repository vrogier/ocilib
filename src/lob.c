/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "lob.h"

#include "array.h"
#include "connection.h"
#include "macros.h"
#include "memory.h"
#include "stringutils.h"

static const OCI_UInt32 SeekModeValues[] =
{
    OCI_SEEK_SET,
    OCI_SEEK_END,
    OCI_SEEK_CUR
};

static const OCI_UInt32 OpenModeValues[] =
{
    OCI_LOB_READONLY,
    OCI_LOB_READWRITE
};

static const OCI_UInt32 LobTypeValues[] =
{
    OCI_CLOB,
    OCI_NCLOB,
    OCI_BLOB
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OcilibLobInitialize
(
    OCI_Connection *con,
    OCI_Lob        *lob,
    OCILobLocator  *handle,
    ub4             type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Lob*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    ALLOC_HANDLE(OCI_IPC_LOB, lob);
    lob->type   = type;
    lob->con    = con;
    lob->handle = handle;
    lob->offset = 1;

    if (NULL == lob->handle || (OCI_OBJECT_ALLOCATED_ARRAY == lob->hstate))
    {
        const ub2 csid    = OCI_DEFAULT;
        ub1       csfrm   = OCI_DEFAULT;
        ub1       lobtype = 0;
        ub4       empty   = 0;

        if (OCI_NCLOB == lob->type)
        {
            csfrm   = SQLCS_NCHAR;
            lobtype = OCI_TEMP_CLOB;
        }
        else if (OCI_CLOB == lob->type)
        {
            csfrm   = SQLCS_IMPLICIT;
            lobtype = OCI_TEMP_CLOB;
        }
        else
        {
            lobtype = OCI_TEMP_BLOB;
        }

        /* allocate handle for non fetched lob (temporary lob) */

        if (OCI_OBJECT_ALLOCATED_ARRAY != lob->hstate)
        {
            lob->hstate = OCI_OBJECT_ALLOCATED;

            CHECK
            (
                OcilibMemoryAllocDescriptor
                (
                    lob->con->env,
                    (dvoid  *)lob->con->env->handle,
                    (dvoid **)(void *)&lob->handle, OCI_DTYPE_LOB
                )
            );
        }

        CHECK_ATTRIB_SET
        (
            OCI_DTYPE_LOB, OCI_ATTR_LOBEMPTY,
            lob->handle, &empty, sizeof(empty),
            lob->con->err
        );

        CHECK_OCI
        (
            lob->con->err,
            OCILobCreateTemporary,
            lob->con->handle, lob->con->err, lob->handle,
            csid, csfrm, lobtype, FALSE, OCI_DURATION_SESSION
        );
    }
    else
    {
        lob->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibLobFree(lob);
            lob = NULL;
        }

        SET_RETVAL(lob);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OcilibLobCreate
(
    OCI_Connection *con,
    OCI_LobType     type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Lob*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    CHECK_ENUM_VALUE(type, LobTypeValues, OTEXT("Lob type"));

    SET_RETVAL(OcilibLobInitialize(con, NULL, NULL, type));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobFree
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_OBJECT_FETCHED(lob);

    if ((OCI_OBJECT_FETCHED_INVALIDATED != lob->hstate) && OcilibLobIsTemporary(lob))
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobFreeTemporary,
            lob->con->handle, lob->con->err,
            lob->handle
        );
    }

    if (OCI_OBJECT_ALLOCATED == lob->hstate)
    {
        OcilibMemoryFreeDescriptor(lob->con->env, (dvoid*)lob->handle, (ub4)OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != lob->hstate)
    {
        OcilibErrorResetSource(NULL, lob);

        FREE(lob);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Lob ** OcilibLobCreateArray
(
    OCI_Connection *con,
    OCI_LobType     type,
    OCI_UInt32      nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Lob**, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_ENUM_VALUE(type, LobTypeValues, OTEXT("Lob type"));

    arr = OcilibArrayCreate(con->env,con, nbelem, OCI_CDT_LOB, type,
                            sizeof(OCILobLocator*), sizeof(OCI_Lob),
                            OCI_DTYPE_LOB, NULL);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Lob**)arr->tab_obj);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobFreeArray
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobFreeArray
(
    OCI_Lob **lobs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, lobs);

    SET_RETVAL(OcilibArrayFreeFromHandles(((OCI_Lob *) lobs[0])->con->env, (void**)lobs));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobGetType
 * --------------------------------------------------------------------------------------------- */

OCI_LobType OcilibLobGetType
(
    OCI_Lob *lob
)
{
    GET_PROP
    (
        /* result */ OCI_LobType, OCI_UNKNOWN,
        /* handle */ OCI_IPC_LOB, lob,
        /* member */ type
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobSeek
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobSeek
(
    OCI_Lob     *lob,
    OCI_UInt64   offset,
    OCI_SeekMode mode
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_ENUM_VALUE(mode, SeekModeValues, OTEXT("Seek Mode"));

    const OCI_UInt64 size = OcilibLobGetLength(lob);

    OCI_Boolean success = FALSE;

    switch (mode)
    {
        case OCI_SEEK_CUR:
        {
            if ((offset + lob->offset - 1) <= size)
            {
                lob->offset += offset;

                success = TRUE;
            }
            break;
        }
        case OCI_SEEK_SET:
        {
            if (offset <= size)
            {
                lob->offset = offset + 1;

                success = TRUE;
            }
            break;
        }
        case OCI_SEEK_END:
        {
            if (offset <= size)
            {
                lob->offset = size - offset + 1;

                success = TRUE;
            }
            break;
        }
    }

    SET_RETVAL(success);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobGetOffset
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibLobGetOffset
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt64, 0,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);

    SET_RETVAL(lob->offset - 1);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobRead
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobRead
(
    OCI_Lob    *lob,
    void       *buffer,
    OCI_UInt32 *char_count,
    OCI_UInt32 *byte_count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    ub1 csfrm = 0;
    ub2 csid  = 0;

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_PTR(OCI_IPC_INT, char_count);
    CHECK_PTR(OCI_IPC_INT, byte_count);

    if (OCI_BLOB != lob->type)
    {
        if (IS_WIDE_CHAR_ENV())
        {
            csid = OCI_UTF16ID;
        }

        if (((*byte_count) == 0) && ((*char_count) > 0))
        {
            if (IS_UTF8_ENV())
            {
                (*byte_count) = (*char_count) * (ub4)OCI_UTF8_BYTES_PER_CHAR;
            }
            else
            {
                (*byte_count) = (*char_count) * (ub4) sizeof(dbtext);
            }
        }
    }

    csfrm = (OCI_NCLOB == lob->type) ? SQLCS_NCHAR : SQLCS_IMPLICIT;

#ifdef OCI_LOB2_API_ENABLED

    if (lob->con->env->use_lob_ub8)
    {
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        CHECK_OCI
        (
            lob->con->err,
            OCILobRead2,
            lob->con->handle, lob->con->err, lob->handle,
            &size_in_out_byte, &size_in_out_char,
            (ub8) lob->offset, buffer,(ub8) (*byte_count),
            (ub1) OCI_ONE_PIECE, (void *) NULL,
            NULL, csid, csfrm
        );

        (*char_count) = (ub4) size_in_out_char;
        (*byte_count) = (ub4) size_in_out_byte;
    }

    else

#endif

    {
        ub4 size_in_out_char_byte = (lob->type == OCI_BLOB) ? *byte_count : *char_count;

        CHECK_OCI
        (
            lob->con->err,
            OCILobRead,
            lob->con->handle, lob->con->err, lob->handle,
            &size_in_out_char_byte, (ub4) lob->offset,
            buffer, (ub4) (*byte_count), (void *) NULL,
            NULL, csid, csfrm
        );

        (*char_count) = (ub4) size_in_out_char_byte;
        (*byte_count) = (ub4) size_in_out_char_byte;
    }

    if (OCI_BLOB != lob->type)
    {
        ub4 ora_byte_count = (ub4) *byte_count;

        if (!lob->con->env->use_lob_ub8 && !IS_UTF8_ENV())
        {
            ora_byte_count *= sizeof(dbtext);
        }

        memset(((char *) buffer) + ora_byte_count, 0, sizeof(dbtext));

#ifndef OCI_LOB2_API_ENABLED

        if (IS_UTF8_ENV())
        {
            (*char_count) = (ub4) StringLength((const char *)buffer, sizeof(char));
        }

#endif

    }

    if (OCI_BLOB == lob->type)
    {
        lob->offset += (OCI_UInt64) (*byte_count);
    }
    else
    {
        lob->offset += (OCI_UInt64) (*char_count);

        if (!IS_UTF8_ENV() && lob->con->env->use_wide_char_conv)
        {
            OcilibStringUTF16ToUTF32(buffer, buffer, (OCI_Int32) (*char_count));
            (*byte_count) = (ub4) (*char_count) * (ub4) sizeof(otext);
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobWrite
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobWrite
(
    OCI_Lob    *lob,
    void       *buffer,
    OCI_UInt32 *char_count,
    OCI_UInt32 *byte_count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    ub1   csfrm = 0;
    ub2   csid  = 0;
    void *obuf  = NULL;

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_PTR(OCI_IPC_INT, char_count);
    CHECK_PTR(OCI_IPC_INT, byte_count);

    if (OCI_BLOB != lob->type)
    {
        if (IS_WIDE_CHAR_ENV())
        {
            csid = OCI_UTF16ID;
        }

        if (((*byte_count) == 0) && ((*char_count) > 0))
        {
            if (IS_UTF8_ENV())
            {
                (*byte_count) = (OCI_UInt32) strlen((const char *) buffer);
            }
            else
            {
                (*byte_count) = (*char_count) * (ub4) sizeof(otext);
            }
        }

        if (((*char_count) == 0) && ((*byte_count) > 0))
        {
            if (IS_UTF8_ENV())
            {

#ifndef OCI_LOB2_API_ENABLED

                (*char_count) = (ub4) StringLength((const char *)buffer, sizeof(char));

#endif

            }
            else
            {
                (*char_count) = (*byte_count) / (ub4) sizeof(otext);
            }
        }

        obuf = OcilibStringGetDBString(lob->con->env, (otext *) buffer, (OCI_Int32 *) byte_count);
    }
    else
    {
        obuf = buffer;
    }

    csfrm = (OCI_NCLOB == lob->type) ? SQLCS_NCHAR : SQLCS_IMPLICIT;

#ifdef OCI_LOB2_API_ENABLED

    if (lob->con->env->use_lob_ub8)
    {
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        CHECK_OCI
        (
            lob->con->err,
            OCILobWrite2,
            lob->con->handle, lob->con->err, lob->handle,
            &size_in_out_byte, &size_in_out_char,
            (ub8) lob->offset, obuf, (ub8) (*byte_count),
            (ub1) OCI_ONE_PIECE, (void *) NULL,
            NULL, csid, csfrm
        );

        (*char_count) = (ub4) size_in_out_char;
        (*byte_count) = (ub4) size_in_out_byte;
    }

    else

#endif

    {
        ub4 size_in_out_char_byte = 0;

        if ((OCI_BLOB == lob->type) || IS_UTF8_ENV())
        {
            size_in_out_char_byte = (*byte_count);
        }
        else
        {
            size_in_out_char_byte = (*char_count);
        }

        CHECK_OCI
        (
            lob->con->err,
            OCILobWrite,
            lob->con->handle, lob->con->err, lob->handle,
            &size_in_out_char_byte, (ub4) lob->offset,
            obuf, (ub4) (*byte_count), (ub1) OCI_ONE_PIECE,
            (void *) NULL, NULL, csid, csfrm
        );

        (*char_count) = (ub4) size_in_out_char_byte;
        (*byte_count) = (ub4) size_in_out_char_byte;

        if ((OCI_CLOB == lob->type) && !IS_UTF8_ENV())
        {
            (*byte_count) *= (ub4) sizeof(otext);
        }
    }

    if (OCI_BLOB == lob->type)
    {
        lob->offset += (OCI_UInt64) (*byte_count);
    }
    else
    {
        lob->offset += (OCI_UInt64) (*char_count);
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (obuf != buffer)
        {
            OcilibStringReleaseDBString(lob->con->env, (dbtext*)obuf);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobTruncate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobTruncate
(
    OCI_Lob   *lob,
    OCI_UInt64 size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);

#ifdef OCI_LOB2_API_ENABLED

    if (lob->con->env->use_lob_ub8)
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobTrim2,
            lob->con->handle, lob->con->err,
            lob->handle, (ub8) size
        );
    }
    else

#endif

    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobTrim,
            lob->con->handle, lob->con->err,
            lob->handle, (ub4) size
        );
    }

    if (lob->offset > size)
    {
        lob->offset = OcilibLobGetLength(lob) + 1;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobErase
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibLobErase
(
    OCI_Lob   *lob,
    OCI_UInt64 offset,
    OCI_UInt64 size
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt64, 0,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_MIN(size, 1);

#ifdef OCI_LOB2_API_ENABLED

    if (lob->con->env->use_lob_ub8)
    {
        ub8 lob_size = (ub8) size;

        CHECK_OCI
        (
            lob->con->err,
            OCILobErase2,
            lob->con->handle, lob->con->err,
            lob->handle, (ub8 *) &lob_size,
            (ub8) (offset + 1)
        );

        size = (OCI_UInt64) lob_size;
    }
    else

#endif

    {
        ub4 lob_size = (ub4) size;

        CHECK_OCI
        (
            lob->con->err,
            OCILobErase,
            lob->con->handle, lob->con->err,
            lob->handle,  &lob_size,
            (ub4) offset + 1
        );

        size = (OCI_UInt64) lob_size;
    }

    SET_RETVAL(size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobGetLength
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibLobGetLength
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt64, 0,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);

    OCI_UInt64 size = 0;

#ifdef OCI_LOB2_API_ENABLED

    if (lob->con->env->use_lob_ub8)
    {
        ub8 lob_size = 0;
        CHECK_OCI
        (
            lob->con->err,
            OCILobGetLength2,
            lob->con->handle, lob->con->err,
            lob->handle, &lob_size
        );

        size = lob_size;
    }
    else

#endif

    {
        ub4 lob_size = 0;

        CHECK_OCI
        (
            lob->con->err,
            OCILobGetLength,
            lob->con->handle, lob->con->err,
            lob->handle, &lob_size
        );

        size = (OCI_UInt64) lob_size;
    }

    SET_RETVAL(size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobGetChunkSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibLobGetChunkSize
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_LOB, lob
    );

    ub4 size = 0;

    CHECK_PTR(OCI_IPC_LOB, lob);

    CHECK_OCI
    (
        lob->con->err,
        OCILobGetChunkSize,
        lob->con->handle, lob->con->err,
        lob->handle, &size
    );

    SET_RETVAL((OCI_UInt32) size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobCopy
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobCopy
(
    OCI_Lob   *lob,
    OCI_Lob   *lob_src,
    OCI_UInt64 offset_dst,
    OCI_UInt64 offset_src,
    OCI_UInt64 count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_PTR(OCI_IPC_LOB, lob_src);

#ifdef OCI_LOB2_API_ENABLED

    if (lob->con->env->use_lob_ub8)
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobCopy2,
            lob->con->handle, lob->con->err, lob->handle,
            lob_src->handle, (ub8) count,
            (ub8) (offset_dst + 1),
            (ub8) (offset_src + 1)
        );
    }
    else

#endif

    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobCopy,
            lob->con->handle, lob->con->err, lob->handle,
            lob_src->handle, (ub4) count,
            (ub4) (offset_dst + 1),
            (ub4) (offset_src + 1)
        );
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobCopyFromFile
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobCopyFromFile
(
    OCI_Lob   *lob,
    OCI_File  *file,
    OCI_UInt64 offset_dst,
    OCI_UInt64 offset_src,
    OCI_UInt64 count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB,  lob);
    CHECK_PTR(OCI_IPC_FILE, file);

#ifdef OCI_LOB2_API_ENABLED

    if (lob->con->env->use_lob_ub8)
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobLoadFromFile2,
            lob->con->handle, lob->con->err,
            lob->handle, file->handle,
            (ub8) count,
            (ub8) (offset_dst + 1),
            (ub8) (offset_src + 1)
        );
    }
    else

#endif

    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobLoadFromFile,
            lob->con->handle, lob->con->err,
            lob->handle, file->handle,
            (ub4) count,
            (ub4) (offset_dst + 1),
            (ub4) (offset_src + 1)
        );
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobAppend
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobAppend
(
    OCI_Lob    *lob,
    void       *buffer,
    OCI_UInt32 *char_count,
    OCI_UInt32 *byte_count
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    ub1   csfrm = 0;
    ub2   csid  = 0;
    void *obuf  = NULL;

    /* OCILobWriteAppend() seems to cause problems on Oracle client 8.1 and 9.0
    It's an Oracle known bug #886191
    So we use OCI_LobSeek() + OCI_LobWrite() instead */

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_PTR(OCI_IPC_INT, char_count);
    CHECK_PTR(OCI_IPC_INT, byte_count);

    if (lob->con->env->version_runtime < OCI_10_1)
    {
        CHECK(OcilibLobSeek(lob, OcilibLobGetLength(lob), OCI_SEEK_SET));
        CHECK(OcilibLobWrite(lob, buffer, char_count, byte_count));
        SET_SUCCESS();
        JUMP_CLEANUP();
    }

    if (OCI_BLOB != lob->type)
    {
        if (IS_WIDE_CHAR_ENV())
        {
            csid = OCI_UTF16ID;
        }

        if (((*byte_count) == 0) && ((*char_count) > 0))
        {
            if (IS_UTF8_ENV())
            {
                (*byte_count) = (OCI_UInt32) strlen((char *) buffer);
            }
            else
            {
                (*byte_count) = (*char_count) * (ub4) sizeof(otext);
            }
        }

        if (((*char_count) == 0) && ((*byte_count) > 0))
        {
            if (IS_UTF8_ENV())
            {

#ifndef OCI_LOB2_API_ENABLED

                (*char_count) = (ub4) StringLength((const char *)buffer, sizeof(char));

#endif

            }
            else
            {
                (*char_count) = (*byte_count) / (ub4) sizeof(otext);
            }
        }

        obuf = OcilibStringGetDBString(lob->con->env, (const otext *) buffer, (OCI_Int32 *) byte_count);
    }
    else
    {
        obuf = buffer;
    }

    csfrm = (OCI_NCLOB == lob->type) ? SQLCS_NCHAR : SQLCS_IMPLICIT;

#ifdef OCI_LOB2_API_ENABLED

    if (lob->con->env->use_lob_ub8)
    {
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        CHECK_OCI
        (
            lob->con->err,
            OCILobWriteAppend2,
            lob->con->handle, lob->con->err, lob->handle,
            &size_in_out_byte, &size_in_out_char,
            obuf, (ub8)  (*byte_count), (ub1) OCI_ONE_PIECE,
            (dvoid *) NULL, NULL, csid, csfrm
        );

        (*char_count) = (ub4) size_in_out_char;
        (*byte_count) = (ub4) size_in_out_byte;
    }

    else

#endif

    {
        ub4 size_in_out_char_byte = 0;

        /* OCILobWriteAppend 'amtp' is in bytes for BLOBs and in characters for
           CLOB/NCLOBs, whatever the client charset is */

        if (OCI_BLOB == lob->type)
        {
            size_in_out_char_byte = (*byte_count);
        }
        else
        {
            size_in_out_char_byte = (*char_count);
        }

        CHECK_OCI
        (
            lob->con->err,
            OCILobWriteAppend,
            lob->con->handle, lob->con->err, lob->handle,
            &size_in_out_char_byte, obuf,  (*byte_count),
            (ub1) OCI_ONE_PIECE, (dvoid *) NULL, NULL, csid, csfrm
        );

        (*char_count) = (ub4) size_in_out_char_byte;
        (*byte_count) = (ub4) size_in_out_char_byte;

        if ((OCI_BLOB != lob->type) && !IS_UTF8_ENV())
        {
            (*byte_count) *= (ub4) sizeof(otext);
        }
    }

    if (OCI_BLOB == lob->type)
    {
        lob->offset += (OCI_UInt64) (*byte_count);
    }
    else
    {
        lob->offset += (OCI_UInt64) (*char_count);
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        if (obuf != buffer)
        {
            OcilibStringReleaseDBString(lob->con->env, (dbtext*)obuf);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobAppendLob
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobAppendLob
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_PTR(OCI_IPC_LOB, lob_src);

    CHECK_OCI
    (
        lob->con->err,
        OCILobAppend,
        lob->con->handle, lob->con->err,
        lob->handle, lob_src->handle
    );

    lob->offset = OcilibLobGetLength(lob);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobIsTemporary
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobIsTemporary
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);

    OCI_Boolean is_temp = FALSE;

    CHECK_OCI
    (
        lob->con->err,
        OCILobIsTemporary,
        lob->con->env->handle, lob->con->err,
        lob->handle, (boolean*) &is_temp
    );

    SET_RETVAL(is_temp);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobOpen
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobOpen
(
    OCI_Lob    *lob,
    OCI_LobMode mode
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_ENUM_VALUE(mode, OpenModeValues, OTEXT("Open mode"));

    CHECK_OCI
    (
        lob->con->err,
        OCILobOpen,
        lob->con->handle, lob->con->err,
        lob->handle, (ub1) mode
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobClose
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobClose
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);

    CHECK_OCI
    (
        lob->con->err,
        OCILobClose,
        lob->con->handle, lob->con->err,
        lob->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobIsEqual
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobIsEqual
(
    OCI_Lob *lob,
    OCI_Lob *lob2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_PTR(OCI_IPC_LOB, lob2);

    OCI_Boolean is_equal = FALSE;

    CHECK_OCI
    (
        lob->con->err,
        OCILobIsEqual,
        lob->con->env->handle, lob->handle,
        lob2->handle, (boolean*) &is_equal
    );

    SET_RETVAL(is_equal);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobAssign
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);
    CHECK_PTR(OCI_IPC_LOB, lob_src);

    if ((OCI_OBJECT_ALLOCATED == lob->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == lob->hstate))
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobLocatorAssign,
            lob->con->handle, lob->con->err,
            lob_src->handle, &lob->handle
        );
    }
    else
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobAssign, lob->con->env->handle,
            lob->con->err, lob_src->handle,
            &lob->handle
        );
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobGetMaxSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibLobGetMaxSize
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt64, 0,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);

    OCI_UInt64 max_size = 0;

#ifdef OCI_LOB2_API_ENABLED

    if (lob->con->env->use_lob_ub8)
    {
        ub8 size = 0;

        CHECK_OCI
        (
            lob->con->err,
            OCILobGetStorageLimit,
            lob->con->handle, lob->con->err,
            lob->handle, (ub8 *) &size
        );

        max_size = size;
    }

#endif

    SET_RETVAL(max_size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobFlush
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobFlush
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);

    CHECK_OCI
    (
        lob->con->err,
        OCILobFlushBuffer,
        lob->con->handle, lob->con->err,
        lob->handle, (ub4) OCI_DEFAULT
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLobEnableBuffering
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobEnableBuffering
(
    OCI_Lob    *lob,
    OCI_Boolean value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);

    if (value)
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobEnableBuffering,
            lob->con->handle, lob->con->err,
            lob->handle
        );
    }
    else
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobDisableBuffering,
            lob->con->handle, lob->con->err,
            lob->handle
        );
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibLobGetConnection
* --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibLobGetConnection
(
    OCI_Lob *lob
)
{
    GET_PROP
    (
        /* result */ OCI_Connection *, NULL,
        /* handle */ OCI_IPC_LOB, lob,
        /* member */ con
    );
}

/* --------------------------------------------------------------------------------------------- *
* OcilibLobIsRemote
* --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLobIsRemote
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    );

    CHECK_PTR(OCI_IPC_LOB, lob);

    OCI_Boolean is_remote = FALSE;

#if OCI_VERSION_COMPILE >= OCI_12_2

    if (OcilibConnectionIsVersionSupported(lob->con, OCI_12_2))
    {
        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_LOB, OCI_ATTR_LOB_REMOTE,
            lob->handle, &is_remote, sizeof(is_remote),
            lob->con->err
        );
    }

#endif

    SET_RETVAL(is_remote);

    EXIT_FUNC();
}
