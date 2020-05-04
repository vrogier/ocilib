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

#include "lob.h"

#include "array.h"
#include "connection.h"
#include "macros.h"
#include "memory.h"
#include "strings.h"

static const unsigned int SeekModeValues[] =
{
    OCI_SEEK_SET,
    OCI_SEEK_END,
    OCI_SEEK_CUR
};

static const unsigned int OpenModeValues[] =
{
    OCI_LOB_READONLY,
    OCI_LOB_READWRITE
};

static const unsigned int LobTypeValues[] =
{
    OCI_CLOB,
    OCI_NCLOB,
    OCI_BLOB
};

/* --------------------------------------------------------------------------------------------- *
 * LobInit
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * LobInitialize
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
        /* context */ OCI_IPC_LOB, lob
    )

    ALLOC_DATA(OCI_IPC_LOB, lob, 1);

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
                MemoryAllocDescriptor
                (
                    (dvoid  *)lob->con->env,
                    (dvoid **)(void *)&lob->handle, OCI_DTYPE_LOB
                )
            )
        }

        CHECK_ATTRIB_SET
        (
            OCI_DTYPE_LOB, OCI_ATTR_LOBEMPTY,
            lob->handle, &empty, sizeof(empty),
            lob->con->err
        )

        CHECK_OCI
        (
            lob->con->err,
            OCILobCreateTemporary,
            lob->con->cxt, lob->con->err, lob->handle,
            csid, csfrm, lobtype, FALSE, OCI_DURATION_SESSION
        )
    }
    else
    {
        lob->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            LobFree(lob);
            lob = NULL;
        }

        SET_RETVAL(lob)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * LobCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * LobCreate
(
    OCI_Connection *con,
    unsigned int    type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Lob*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    CHECK_ENUM_VALUE(type, LobTypeValues, OTEXT("Lob type"))

    SET_RETVAL(LobInitialize(con, NULL, NULL, type))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobFree
 * --------------------------------------------------------------------------------------------- */

boolean LobFree
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_OBJECT_FETCHED(lob)

    if (LobIsTemporary(lob))
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobFreeTemporary,
            lob->con->cxt, lob->con->err,
            lob->handle
        )
    }

    if (OCI_OBJECT_ALLOCATED == lob->hstate)
    {
        MemoryFreeDescriptor((dvoid*)lob->handle, (ub4)OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != lob->hstate)
    {
        FREE(lob)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Lob ** LobCreateArray
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Lob**, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(type, LobTypeValues, OTEXT("Lob type"))

    arr = ArrayCreate(con, nbelem, OCI_CDT_LOB, type,
                      sizeof(OCILobLocator*), sizeof(OCI_Lob),
                      OCI_DTYPE_LOB, NULL);

    CHECK_NULL(arr)

    SET_RETVAL((OCI_Lob**)arr->tab_obj)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean LobFreeArray
(
    OCI_Lob **lobs
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_ARRAY, lobs)

    SET_RETVAL(ArrayFreeFromHandles((void**)lobs))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int LobGetType
(
    OCI_Lob *lob
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_LOB, lob,
        /* member */ type
    )
}

/* --------------------------------------------------------------------------------------------- *
 * LobSeek
 * --------------------------------------------------------------------------------------------- */

boolean LobSeek
(
    OCI_Lob     *lob,
    big_uint     offset,
    unsigned int mode
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_ENUM_VALUE(mode, SeekModeValues, OTEXT("Seek Mode"))

    const big_uint size = LobGetLength(lob);

    boolean success = FALSE;

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

    SET_RETVAL(success)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobGetOffset
 * --------------------------------------------------------------------------------------------- */

big_uint LobGetOffset
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ big_uint, 0,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)

    SET_RETVAL(lob->offset - 1)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobRead2
 * --------------------------------------------------------------------------------------------- */

boolean LobRead2
(
    OCI_Lob      *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    ub1 csfrm = 0;
    ub2 csid = 0;

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_PTR(OCI_IPC_INT, char_count)
    CHECK_PTR(OCI_IPC_INT, byte_count)

    if (OCI_BLOB != lob->type)
    {
        if (OCI_CHAR_WIDE == Env.charset)
        {
            csid = OCI_UTF16ID;
        }

        if (((*byte_count) == 0) && ((*char_count) > 0))
        {
            if (Env.nls_utf8)
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

    if (Env.use_lob_ub8)
    {
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        CHECK_OCI
        (
            lob->con->err,
            OCILobRead2,
            lob->con->cxt, lob->con->err, lob->handle,
            &size_in_out_byte, &size_in_out_char,
            (ub8) lob->offset, buffer,(ub8) (*byte_count),
            (ub1) OCI_ONE_PIECE, (void *) NULL,
            NULL, csid, csfrm
        )

            (*char_count) = (ub4) size_in_out_char;
        (*byte_count)     = (ub4) size_in_out_byte;
    }

    else

#endif

    {
        ub4 size_in_out_char_byte = (lob->type == OCI_BLOB) ? *byte_count : *char_count;

        CHECK_OCI
        (
            lob->con->err,
            OCILobRead,
            lob->con->cxt, lob->con->err, lob->handle,
            &size_in_out_char_byte, (ub4) lob->offset,
            buffer, (ub4) (*byte_count), (void *) NULL,
            NULL, csid, csfrm
        )

            (*char_count) = (ub4) size_in_out_char_byte;
        (*byte_count)     = (ub4) size_in_out_char_byte;
    }

    if (OCI_BLOB != lob->type)
    {
        ub4 ora_byte_count = (ub4) *byte_count;

        if (!Env.use_lob_ub8 && !Env.nls_utf8)
        {
            ora_byte_count *= sizeof(dbtext);
        }

        memset(((char *) buffer) + ora_byte_count, 0, sizeof(dbtext));

#ifndef OCI_LOB2_API_ENABLED

        if (Env.nls_utf8)
        {
            (*char_count) = (ub4) StringLength((const char *)buffer, sizeof(char));
        }

#endif

    }

    if (OCI_BLOB == lob->type)
    {
        lob->offset += (big_uint) (*byte_count);
    }
    else
    {
        lob->offset += (big_uint) (*char_count);

        if (!Env.nls_utf8 && Env.use_wide_char_conv)
        {
            StringUTF16ToUTF32(buffer, buffer, (int) (*char_count));
            (*byte_count) = (ub4) (*char_count) * (ub4) sizeof(otext);
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobRead
 * --------------------------------------------------------------------------------------------- */

unsigned int LobRead
(
    OCI_Lob     *lob,
    void        *buffer,
    unsigned int len
)
{
    unsigned int  char_count = 0;
    unsigned int  byte_count = 0;
    unsigned int *ptr_count  = NULL;

    if (lob)
    {
        if (OCI_BLOB == lob->type)
        {
            byte_count = len;
            ptr_count  = &byte_count;
        }
        else
        {
            char_count = len;
            ptr_count  = &char_count;
        }
    }

    LobRead2(lob, buffer, &char_count, &byte_count);

    return (NULL != ptr_count ? *ptr_count : 0);
}

/* --------------------------------------------------------------------------------------------- *
 * LobWrite
 * --------------------------------------------------------------------------------------------- */

boolean LobWrite2
(
    OCI_Lob      *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    ub1 csfrm = 0;
    ub2   csid = 0;
    void *obuf = NULL;

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_PTR(OCI_IPC_INT, char_count)
    CHECK_PTR(OCI_IPC_INT, byte_count)

    if (OCI_BLOB != lob->type)
    {
        if (OCI_CHAR_WIDE == Env.charset)
        {
            csid = OCI_UTF16ID;
        }

        if (((*byte_count) == 0) && ((*char_count) > 0))
        {
            if (Env.nls_utf8)
            {
                (*byte_count) = (unsigned int) strlen((const char *) buffer);
            }
            else
            {
                (*byte_count) = (*char_count) * (ub4) sizeof(otext);
            }
        }

        if (((*char_count) == 0) && ((*byte_count) > 0))
        {
            if (Env.nls_utf8 )
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

        obuf = StringGetDBString( (otext *) buffer, (int *) byte_count);
    }
    else
    {
        obuf = buffer;
    }

    csfrm = (OCI_NCLOB == lob->type) ? SQLCS_NCHAR : SQLCS_IMPLICIT;

#ifdef OCI_LOB2_API_ENABLED

    if (Env.use_lob_ub8)
    {
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        CHECK_OCI
        (
            lob->con->err,
            OCILobWrite2,
            lob->con->cxt, lob->con->err, lob->handle,
            &size_in_out_byte, &size_in_out_char,
            (ub8) lob->offset, obuf, (ub8) (*byte_count),
            (ub1) OCI_ONE_PIECE, (void *) NULL,
            NULL, csid, csfrm
        )

            (*char_count) = (ub4) size_in_out_char;
        (*byte_count)     = (ub4) size_in_out_byte;
    }

    else

#endif

    {
        ub4 size_in_out_char_byte = 0;

        if ((OCI_BLOB == lob->type) || Env.nls_utf8)
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
            lob->con->cxt, lob->con->err, lob->handle,
            &size_in_out_char_byte, (ub4) lob->offset,
            obuf, (ub4) (*byte_count), (ub1) OCI_ONE_PIECE,
            (void *) NULL, NULL, csid, csfrm
        )

            (*char_count) = (ub4) size_in_out_char_byte;
        (*byte_count)     = (ub4) size_in_out_char_byte;

        if ((OCI_CLOB == lob->type) && !Env.nls_utf8)
        {
            (*byte_count) *= (ub4) sizeof(otext);
        }
    }

    if (OCI_BLOB == lob->type)
    {
        lob->offset += (big_uint) (*byte_count);
    }
    else
    {
        lob->offset += (big_uint) (*char_count);
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (obuf != buffer)
        {
            StringReleaseDBString((dbtext*)obuf);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * LobWrite
 * --------------------------------------------------------------------------------------------- */

unsigned int LobWrite
(
    OCI_Lob     *lob,
    void        *buffer,
    unsigned int len
)
{
    unsigned int  char_count = 0;
    unsigned int  byte_count = 0;
    unsigned int *ptr_count  = NULL;

    if (lob)
    {
        if(OCI_BLOB == lob->type)
        {
            byte_count = len;
            ptr_count  = &byte_count;
        }
        else
        {
            char_count = len;
            ptr_count  = &char_count;
        }
    }

    LobWrite2(lob, buffer, &char_count, &byte_count);

    return (NULL != ptr_count ? *ptr_count : 0);
}

/* --------------------------------------------------------------------------------------------- *
 * LobTruncate
 * --------------------------------------------------------------------------------------------- */

boolean LobTruncate
(
    OCI_Lob *lob,
    big_uint size
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)

#ifdef OCI_LOB2_API_ENABLED

    if (Env.use_lob_ub8)
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobTrim2,
            lob->con->cxt, lob->con->err,
            lob->handle, (ub8) size
        )
    }
    else

#endif

    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobTrim,
            lob->con->cxt, lob->con->err,
            lob->handle, (ub4) size
        )
    }

    if (lob->offset > size)
    {
        lob->offset = LobGetLength(lob) + 1;
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobErase
 * --------------------------------------------------------------------------------------------- */

big_uint LobErase
(
    OCI_Lob *lob,
    big_uint offset,
    big_uint size
)
{
    ENTER_FUNC
    (
        /* returns */ big_uint, 0,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_MIN(size, 1)

#ifdef OCI_LOB2_API_ENABLED

    if (Env.use_lob_ub8)
    {
        ub8 lob_size = (ub8) size;

        CHECK_OCI
        (
            lob->con->err,
            OCILobErase2,
            lob->con->cxt, lob->con->err,
            lob->handle, (ub8 *) &lob_size,
            (ub8) (offset + 1)
        )

        size = (big_uint) lob_size;
    }
    else

#endif

    {
        ub4 lob_size = (ub4) size;

        CHECK_OCI
        (
            lob->con->err,
            OCILobErase,
            lob->con->cxt, lob->con->err,
            lob->handle,  &lob_size,
            (ub4) offset + 1
        )

        size = (big_uint) lob_size;
    }

    SET_RETVAL(size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobGetLength
 * --------------------------------------------------------------------------------------------- */

big_uint LobGetLength
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ big_uint, 0,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)

    big_uint size = 0;

#ifdef OCI_LOB2_API_ENABLED

    if (Env.use_lob_ub8)
    {
        ub8 lob_size = 0;
        CHECK_OCI
        (
            lob->con->err,
            OCILobGetLength2,
            lob->con->cxt, lob->con->err,
            lob->handle, &lob_size
        )

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
            lob->con->cxt, lob->con->err,
            lob->handle, &lob_size
        )

        size = (big_uint) lob_size;
    }

    SET_RETVAL(size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobGetChunkSize
 * --------------------------------------------------------------------------------------------- */

unsigned int LobGetChunkSize
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_LOB, lob
    )

    ub4 size = 0;

    CHECK_PTR(OCI_IPC_LOB, lob)

    CHECK_OCI
    (
        lob->con->err,
        OCILobGetChunkSize,
        lob->con->cxt, lob->con->err,
        lob->handle, &size
    )

    SET_RETVAL((unsigned int) size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobCopy
 * --------------------------------------------------------------------------------------------- */

boolean LobCopy
(
    OCI_Lob *lob,
    OCI_Lob *lob_src,
    big_uint offset_dst,
    big_uint offset_src,
    big_uint count
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_PTR(OCI_IPC_LOB, lob_src)

#ifdef OCI_LOB2_API_ENABLED

    if (Env.use_lob_ub8)
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobCopy2,
            lob->con->cxt, lob->con->err, lob->handle,
            lob_src->handle, (ub8) count,
            (ub8) (offset_dst + 1),
            (ub8) (offset_src + 1)
        )
    }
    else

#endif

    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobCopy,
            lob->con->cxt, lob->con->err, lob->handle,
            lob_src->handle, (ub4) count,
            (ub4) (offset_dst + 1),
            (ub4) (offset_src + 1)
        )
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobCopyFromFile
 * --------------------------------------------------------------------------------------------- */

boolean LobCopyFromFile
(
    OCI_Lob  *lob,
    OCI_File *file,
    big_uint  offset_dst,
    big_uint  offset_src,
    big_uint  count
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB,  lob)
    CHECK_PTR(OCI_IPC_FILE, file)

#ifdef OCI_LOB2_API_ENABLED

    if (Env.use_lob_ub8)
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobLoadFromFile2,
            lob->con->cxt, lob->con->err,
            lob->handle, file->handle,
            (ub8) count,
            (ub8) (offset_dst + 1),
            (ub8) (offset_src + 1)
        )
    }
    else

#endif

    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobLoadFromFile,
            lob->con->cxt, lob->con->err,
            lob->handle, file->handle,
            (ub4) count,
            (ub4) (offset_dst + 1),
            (ub4) (offset_src + 1)
        )
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobAppend2
 * --------------------------------------------------------------------------------------------- */

boolean LobAppend2
(
    OCI_Lob      *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    ub1 csfrm = 0;
    ub2   csid = 0;
    void *obuf = NULL;

    /* OCILobWriteAppend() seems to cause problems on Oracle client 8.1 and 9.0
    It's an Oracle known bug #886191
    So we use OCI_LobSeek() + OCI_LobWrite() instead */

    if (Env.version_runtime < OCI_10_1)
    {
        CHECK(LobSeek(lob, LobGetLength(lob), OCI_SEEK_SET))
        CHECK(LobWrite2(lob, buffer, char_count, byte_count))
        SET_SUCCESS()
        JUMP_CLEANUP()
    }

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_PTR(OCI_IPC_INT, char_count)
    CHECK_PTR(OCI_IPC_INT, byte_count)

    if (OCI_BLOB != lob->type)
    {
        if (OCI_CHAR_WIDE == Env.charset)
        {
            csid = OCI_UTF16ID;
        }

        if (((*byte_count) == 0) && ((*char_count) > 0))
        {
            if (Env.nls_utf8)
            {
                (*byte_count) = (unsigned int) strlen( (char *) buffer);
            }
            else
            {
                (*byte_count) = (*char_count) * (ub4) sizeof(otext);
            }
        }

        if (((*char_count) == 0) && ((*byte_count) > 0))
        {
            if (Env.nls_utf8)
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

        obuf = StringGetDBString((const otext *) buffer, (int *) byte_count);
    }
    else
    {
        obuf = buffer;
    }

    csfrm = (OCI_NCLOB == lob->type) ? SQLCS_NCHAR : SQLCS_IMPLICIT;

#ifdef OCI_LOB2_API_ENABLED

    if (Env.use_lob_ub8)
    {
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        CHECK_OCI
        (
            lob->con->err,
            OCILobWriteAppend2,
            lob->con->cxt, lob->con->err, lob->handle,
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

        if ((OCI_BLOB == lob->type) || !Env.nls_utf8)
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
            lob->con->cxt, lob->con->err, lob->handle,
            &size_in_out_char_byte, obuf,  (*byte_count),
            (ub1) OCI_ONE_PIECE, (dvoid *) NULL, NULL, csid, csfrm
        );

        (*char_count) = (ub4) size_in_out_char_byte;
        (*byte_count) = (ub4) size_in_out_char_byte;

        if ((OCI_CLOB == lob->type) && !Env.nls_utf8)
        {
            (*byte_count) *= (ub4) sizeof(otext);
        }
    }

    if (OCI_BLOB == lob->type)
    {
        lob->offset += (big_uint) (*byte_count);
    }
    else
    {
        lob->offset += (big_uint) (*char_count);
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        if (obuf != buffer)
        {
            StringReleaseDBString((dbtext*)obuf);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * LobAppend
 * --------------------------------------------------------------------------------------------- */

unsigned int LobAppend
(
    OCI_Lob     *lob,
    void        *buffer,
    unsigned int len
)
{
    unsigned int  char_count = 0;
    unsigned int  byte_count = 0;
    unsigned int *ptr_count  = NULL;

    if (NULL != lob)
    {
        if(OCI_BLOB == lob->type)
        {
            byte_count = len;
            ptr_count  = &byte_count;
        }
        else
        {
            char_count = len;
            ptr_count  = &char_count;
        }
    }

    LobAppend2(lob, buffer, &char_count, &byte_count);

    return (NULL != ptr_count ? *ptr_count : 0);
}

/* --------------------------------------------------------------------------------------------- *
 * LobAppendLob
 * --------------------------------------------------------------------------------------------- */

boolean LobAppendLob
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_PTR(OCI_IPC_LOB, lob_src)

    CHECK_OCI
    (
        lob->con->err,
        OCILobAppend,
        lob->con->cxt, lob->con->err,
        lob->handle, lob_src->handle
    )

    lob->offset = LobGetLength(lob);

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobIsTemporary
 * --------------------------------------------------------------------------------------------- */

boolean LobIsTemporary
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)

    boolean is_temp = FALSE;

    CHECK_OCI
    (
        lob->con->err,
        OCILobIsTemporary,
        lob->con->env, lob->con->err,
        lob->handle, &is_temp
    )

    SET_RETVAL(is_temp)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobOpen
 * --------------------------------------------------------------------------------------------- */

boolean LobOpen
(
    OCI_Lob     *lob,
    unsigned int mode
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_ENUM_VALUE(mode, OpenModeValues, OTEXT("Open mode"))

    CHECK_OCI
    (
        lob->con->err,
        OCILobOpen,
        lob->con->cxt, lob->con->err,
        lob->handle, (ub1) mode
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobClose
 * --------------------------------------------------------------------------------------------- */

boolean LobClose
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)

    CHECK_OCI
    (
        lob->con->err,
        OCILobClose,
        lob->con->cxt, lob->con->err,
        lob->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobIsEqual
 * --------------------------------------------------------------------------------------------- */

boolean LobIsEqual
(
    OCI_Lob *lob,
    OCI_Lob *lob2
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_PTR(OCI_IPC_LOB, lob2)

    boolean is_equal = FALSE;

    CHECK_OCI
    (
        lob->con->err,
        OCILobIsEqual,
        lob->con->env, lob->handle,
        lob2->handle, &is_equal
    )

    SET_RETVAL(is_equal)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobAssign
 * --------------------------------------------------------------------------------------------- */

boolean LobAssign
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)
    CHECK_PTR(OCI_IPC_LOB, lob_src)

    if ((OCI_OBJECT_ALLOCATED == lob->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == lob->hstate))
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobLocatorAssign,
            lob->con->cxt, lob->con->err,
            lob_src->handle, &lob->handle
        )
    }
    else
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobAssign, lob->con->env,
            lob->con->err, lob_src->handle,
            &lob->handle
        )
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobGetMaxSize
 * --------------------------------------------------------------------------------------------- */

big_uint LobGetMaxSize
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ big_uint, 0,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)

    big_uint max_size = 0;

#ifdef OCI_LOB2_API_ENABLED

    if (Env.use_lob_ub8)
    {
        ub8 size = 0;

        CHECK_OCI
        (
            lob->con->err,
            OCILobGetStorageLimit,
            lob->con->cxt, lob->con->err,
            lob->handle, (ub8 *) &size
        )

        max_size = size;
    }

#endif

    SET_RETVAL(max_size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobFlush
 * --------------------------------------------------------------------------------------------- */

boolean LobFlush
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)

    CHECK_OCI
    (
        lob->con->err,
        OCILobFlushBuffer,
        lob->con->cxt, lob->con->err,
        lob->handle, (ub4) OCI_DEFAULT
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * LobEnableBuffering
 * --------------------------------------------------------------------------------------------- */

boolean LobEnableBuffering
(
    OCI_Lob *lob,
    boolean  value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)

    if (value)
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobEnableBuffering,
            lob->con->cxt, lob->con->err,
            lob->handle
        )
    }
    else
    {
        CHECK_OCI
        (
            lob->con->err,
            OCILobDisableBuffering,
            lob->con->cxt, lob->con->err,
            lob->handle
        )
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* LobGetConnection
* --------------------------------------------------------------------------------------------- */

OCI_Connection * LobGetConnection
(
    OCI_Lob *lob
)
{
    GET_PROP
    (
        /* result */ OCI_Connection *, NULL,
        /* handle */ OCI_IPC_LOB, lob,
        /* member */ con
    )
}

/* --------------------------------------------------------------------------------------------- *
* LobIsRemote
* --------------------------------------------------------------------------------------------- */

boolean LobIsRemote
(
    OCI_Lob *lob
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_LOB, lob
    )

    CHECK_PTR(OCI_IPC_LOB, lob)

    boolean is_remote = FALSE;

#if OCI_VERSION_COMPILE >= OCI_12_2

    if (ConnectionIsVersionSupported(lob->con, OCI_12_2))
    {
        CHECK_ATTRIB_GET
        (
            OCI_DTYPE_LOB, OCI_ATTR_LOB_REMOTE,
            lob->handle, &is_remote, sizeof(is_remote),
            lob->con->err
        )
    }

#endif

    SET_RETVAL(is_remote)

    EXIT_FUNC()
}
