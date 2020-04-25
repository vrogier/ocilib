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

static const unsigned int SeekModeValues[] = { OCI_SEEK_SET, OCI_SEEK_END, OCI_SEEK_CUR };
static const unsigned int OpenModeValues[] = { OCI_LOB_READONLY, OCI_LOB_READWRITE };
static const unsigned int LobTypeValues[]  = { OCI_CLOB, OCI_NCLOB, OCI_BLOB };

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
    DECLARE_CTX(TRUE)
    CALL_CONTEXT_FROM_CON(con)

    ALLOC_DATA(OCI_IPC_LOB, lob, 1);

    if (STATUS)
    {
        lob->type   = type;
        lob->con    = con;
        lob->handle = handle;
        lob->offset = 1;

        if (!lob->handle || (OCI_OBJECT_ALLOCATED_ARRAY == lob->hstate))
        {
            const ub2 csid    = OCI_DEFAULT;
            ub1 csfrm   = OCI_DEFAULT;
            ub1 lobtype = 0;
            ub4 empty   = 0;

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

                STATUS = MemoryAllocDescriptor((dvoid  *)lob->con->env, (dvoid **)(void *)&lob->handle, OCI_DTYPE_LOB);
            }

            ATTRIB_SET(OCI_DTYPE_LOB, OCI_ATTR_LOBEMPTY, lob->handle, &empty, sizeof(empty))
            EXEC(OCILobCreateTemporary(lob->con->cxt, lob->con->err, lob->handle, csid, csfrm, lobtype, FALSE, OCI_DURATION_SESSION))
        }
        else
        {
            lob->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }
 
    /* check for failure */

    if (!STATUS && lob)
    {
        LobFree(lob);
        lob = NULL;
    }

    return lob;
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
    CALL_ENTER(OCI_Lob*, NULL)
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CHECK_ENUM_VALUE(con, NULL, type, LobTypeValues, OTEXT("Lob type"))
    CALL_CONTEXT_FROM_CON(con)

    RETVAL = LobInitialize(con, NULL, NULL, type);
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LobFree
 * --------------------------------------------------------------------------------------------- */

boolean LobFree
(
    OCI_Lob *lob
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_OBJECT_FETCHED(lob)
    CALL_CONTEXT_FROM_CON(lob->con)

    if (LobIsTemporary(lob))
    {
        EXEC(OCILobFreeTemporary(lob->con->cxt, lob->con->err, lob->handle))
    }

    if (OCI_OBJECT_ALLOCATED == lob->hstate)
    {
        MemoryFreeDescriptor((dvoid *) lob->handle, (ub4) OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != lob->hstate)
    {
        FREE(lob)
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    OCI_Array *arr = NULL;

    CALL_ENTER(OCI_Lob **, NULL)
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CHECK_ENUM_VALUE(con, NULL, type, LobTypeValues, OTEXT("Lob type"))
    CALL_CONTEXT_FROM_CON(con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_LOB, type, sizeof(OCILobLocator *), sizeof(OCI_Lob), OCI_DTYPE_LOB, NULL);
    STATUS = (NULL != arr);

    if (STATUS)
    {
        RETVAL = (OCI_Lob **)arr->tab_obj;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LobFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean LobFreeArray
(
    OCI_Lob **lobs
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ARRAY, lobs)

    RETVAL = STATUS = ArrayFreeFromHandles((void **)lobs);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LobGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int LobGetType
(
    OCI_Lob *lob
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_LOB, lob, type, lob->con, NULL, lob->con->err)
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
    big_uint size = 0;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_ENUM_VALUE(lob->con, NULL, mode, SeekModeValues, OTEXT("Seek Mode"))
    CALL_CONTEXT_FROM_CON(lob->con)

    size = LobGetLength(lob);

    switch (mode)
    {
        case OCI_SEEK_CUR:
        {
            if ((offset + lob->offset - 1) <= size) 
            {
                lob->offset += offset;
                RETVAL   = TRUE;
            }
            break;
        }
        case OCI_SEEK_SET:
        {
            if (offset <= size) 
            {
                lob->offset = offset + 1;
                RETVAL  = TRUE;
            }
            break;
        }
        case OCI_SEEK_END:
        {
            if (offset <= size) 
            {
                lob->offset = size - offset + 1;
                RETVAL  = TRUE;
            }
            break;
        }
    }
    
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LobGetOffset
 * --------------------------------------------------------------------------------------------- */

big_uint LobGetOffset
(
    OCI_Lob *lob
)
{
    CALL_ENTER(big_uint, 0)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob);
    CALL_CONTEXT_FROM_CON(lob->con)

    RETVAL = lob->offset - 1;
    STATUS = TRUE;

    CALL_EXIT()
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
    ub1 csfrm   = 0;
    ub2 csid    = 0;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_PTR(OCI_IPC_INT, char_count)
    CALL_CHECK_PTR(OCI_IPC_INT, byte_count)
    CALL_CONTEXT_FROM_CON(lob->con)

    if (OCI_BLOB != lob->type)
    {
        if (OCI_CHAR_WIDE == OCILib.charset)
        {
            csid = OCI_UTF16ID;
        }

        if (((*byte_count) == 0) && ((*char_count) > 0))
        {
            if (OCILib.nls_utf8)
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

    if (OCILib.use_lob_ub8)
    {        
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        EXEC
        (
            OCILobRead2(lob->con->cxt, lob->con->err, lob->handle,
                        &size_in_out_byte, &size_in_out_char,
                        (ub8) lob->offset, buffer,(ub8) (*byte_count),
                        (ub1) OCI_ONE_PIECE, (void *) NULL,
                        NULL, csid, csfrm)
        )

        (*char_count) = (ub4) size_in_out_char;
        (*byte_count) = (ub4) size_in_out_byte;
    }

    else

#endif

    {
        ub4 size_in_out_char_byte = (lob->type == OCI_BLOB) ? *byte_count : *char_count;

        EXEC
        (
            OCILobRead(lob->con->cxt, lob->con->err, lob->handle,
                       &size_in_out_char_byte, (ub4) lob->offset,
                       buffer, (ub4) (*byte_count), (void *) NULL,
                       NULL, csid, csfrm)
        )

        (*char_count) = (ub4) size_in_out_char_byte;
        (*byte_count) = (ub4) size_in_out_char_byte;
    }

    if (OCI_BLOB != lob->type)
    {
        ub4 ora_byte_count = (ub4) *byte_count;

        if (!OCILib.use_lob_ub8 && !OCILib.nls_utf8)
        {
            ora_byte_count *= sizeof(dbtext);
        }

        memset(((char *) buffer) + ora_byte_count, 0, sizeof(dbtext));

    #ifndef OCI_LOB2_API_ENABLED

        if (OCILib.nls_utf8)
        {
            (*char_count) = (ub4) StringLength((const char *)buffer, sizeof(char));
        }

    #endif

    }

    if (STATUS)
    {
        if (OCI_BLOB == lob->type)
        {
            lob->offset += (big_uint) (*byte_count);
        }
        else
        {
            lob->offset += (big_uint) (*char_count);

            if (!OCILib.nls_utf8 && OCILib.use_wide_char_conv)
            {
                StringUTF16ToUTF32(buffer, buffer, (int) (*char_count));
                (*byte_count) = (ub4) (*char_count) * (ub4) sizeof(otext);
            }
        }
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    unsigned int char_count = 0;
    unsigned int byte_count = 0;
    unsigned int *ptr_count = NULL;

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
    ub1     csfrm = 0;
    ub2     csid  = 0;
    void   *obuf  = NULL;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_PTR(OCI_IPC_INT, char_count)
    CALL_CHECK_PTR(OCI_IPC_INT, byte_count)
    CALL_CONTEXT_FROM_CON(lob->con)

    if (OCI_BLOB != lob->type)
    {
        if (OCI_CHAR_WIDE == OCILib.charset)
        {
            csid = OCI_UTF16ID;
        }

        if (((*byte_count) == 0) && ((*char_count) > 0))
        {
            if (OCILib.nls_utf8)
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
            if (OCILib.nls_utf8 )
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

    if (OCILib.use_lob_ub8)
    {
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        EXEC
        (
            OCILobWrite2(lob->con->cxt, lob->con->err, lob->handle,
                         &size_in_out_byte, &size_in_out_char,
                         (ub8) lob->offset, obuf, (ub8) (*byte_count),
                         (ub1) OCI_ONE_PIECE, (void *) NULL,
                         NULL, csid, csfrm)
        )

        (*char_count) = (ub4) size_in_out_char;
        (*byte_count) = (ub4) size_in_out_byte;
    }

    else

#endif

    {
        ub4 size_in_out_char_byte = 0;

        if ((OCI_BLOB == lob->type) || OCILib.nls_utf8)
        {
            size_in_out_char_byte = (*byte_count);
        }
        else
        {
            size_in_out_char_byte = (*char_count);
        }

        EXEC
        (
            OCILobWrite(lob->con->cxt, lob->con->err, lob->handle,
                        &size_in_out_char_byte, (ub4) lob->offset,
                        obuf, (ub4) (*byte_count), (ub1) OCI_ONE_PIECE,
                        (void *) NULL, NULL, csid, csfrm)
        )

        (*char_count) = (ub4) size_in_out_char_byte;
        (*byte_count) = (ub4) size_in_out_char_byte;

        if ((OCI_CLOB == lob->type) && !OCILib.nls_utf8)
        {
             (*byte_count) *= (ub4) sizeof(otext);
        }
    }

    if (STATUS)
    {
        if (OCI_BLOB == lob->type)
        {
            lob->offset += (big_uint) (*byte_count);
        }
        else
        {
            lob->offset += (big_uint) (*char_count);
        }
    }

    if (obuf != buffer)
    {
        StringReleaseDBString((dbtext *) obuf);
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    unsigned int char_count = 0;
    unsigned int byte_count = 0;
    unsigned int *ptr_count = NULL;

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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CONTEXT_FROM_CON(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        EXEC(OCILobTrim2(lob->con->cxt, lob->con->err, lob->handle, (ub8) size))
    }
    else

#endif

    {
        EXEC(OCILobTrim(lob->con->cxt, lob->con->err, lob->handle, (ub4) size))
    }

    if (STATUS)
    {
        if (lob->offset > size)
        {
            lob->offset = LobGetLength(lob) + 1;
        }
    }

    RETVAL = STATUS;


    CALL_EXIT()
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
    CALL_ENTER(big_uint, 0)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob);
    CALL_CHECK_MIN(lob->con, NULL, size, 1);
    CALL_CONTEXT_FROM_CON(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 lob_size = (ub8) size;

        EXEC(OCILobErase2(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &lob_size, (ub8) (offset + 1)))

        size = (big_uint) lob_size;
    }
    else

#endif

    {
        ub4 lob_size = (ub4) size;

        EXEC(OCILobErase(lob->con->cxt, lob->con->err, lob->handle,  &lob_size, (ub4) offset + 1))

        size = (big_uint) lob_size;
    }

    if (OCI_SUCCESS)
    {
        RETVAL = size;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LobGetLength
 * --------------------------------------------------------------------------------------------- */

big_uint LobGetLength
(
    OCI_Lob *lob
)
{
    CALL_ENTER(big_uint, 0)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CONTEXT_FROM_CON(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 lob_size = 0;

        EXEC(OCILobGetLength2(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &lob_size))

        RETVAL = (big_uint) lob_size;
    }
    else

#endif

    {
        ub4 lob_size = 0;

        EXEC(OCILobGetLength(lob->con->cxt, lob->con->err, lob->handle, &lob_size))

        RETVAL = (big_uint) lob_size;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LobGetChunkSize
 * --------------------------------------------------------------------------------------------- */

unsigned int LobGetChunkSize
(
    OCI_Lob *lob
)
{
    ub4 size = 0;

    CALL_ENTER(unsigned int, size)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CONTEXT_FROM_CON(lob->con)

    EXEC(OCILobGetChunkSize(lob->con->cxt, lob->con->err, lob->handle, &size))

    RETVAL = (unsigned int) size;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob_src)
    CALL_CONTEXT_FROM_CON(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        EXEC
        (
            OCILobCopy2(lob->con->cxt, lob->con->err, lob->handle,
                        lob_src->handle, (ub8) count,
                        (ub8) (offset_dst + 1),
                        (ub8) (offset_src + 1))
        )
    }
    else

#endif

{
        EXEC
        (
            OCILobCopy(lob->con->cxt, lob->con->err, lob->handle,
                       lob_src->handle, (ub4) count,
                       (ub4) (offset_dst + 1),
                       (ub4) (offset_src + 1))
        )
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CONTEXT_FROM_CON(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        EXEC
        (
            OCILobLoadFromFile2(lob->con->cxt, lob->con->err,
                                lob->handle, file->handle,
                                (ub8) count,
                                (ub8) (offset_dst + 1),
                                (ub8) (offset_src + 1))
        )
    }
    else

#endif

    {
        EXEC
        (
            OCILobLoadFromFile(lob->con->cxt, lob->con->err,
                               lob->handle, file->handle,
                               (ub4) count,
                               (ub4) (offset_dst + 1),
                               (ub4) (offset_src + 1))
        )
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    ub1     csfrm = 0;
    ub2     csid  = 0;
    void   *obuf  = NULL;

    DECLARE_VARS(boolean, FALSE, TRUE)

    /* OCILobWriteAppend() seems to cause problems on Oracle client 8.1 and 9.0
    It's an Oracle known bug #886191
    So we use OCI_LobSeek() + OCI_LobWrite() instead */

    if (OCILib.version_runtime < OCI_10_1)
    {
        return LobSeek(lob, LobGetLength(lob), OCI_SEEK_SET) &&
               LobWrite2(lob, buffer, char_count, byte_count);
    }

    CTX_ENTER(OCILib.env_mode)

    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_PTR(OCI_IPC_INT, char_count)
    CALL_CHECK_PTR(OCI_IPC_INT, byte_count)
    CALL_CONTEXT_FROM_CON(lob->con)

    if (OCI_BLOB != lob->type)
    {
        if (OCI_CHAR_WIDE == OCILib.charset)
        {
            csid = OCI_UTF16ID;
        }

        if (((*byte_count) == 0) && ((*char_count) > 0))
        {
            if (OCILib.nls_utf8)
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
            if (OCILib.nls_utf8)
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

    if (OCILib.use_lob_ub8)
    {
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        EXEC
        (
            OCILobWriteAppend2(lob->con->cxt, lob->con->err, lob->handle,
                               &size_in_out_byte, &size_in_out_char,
                               obuf, (ub8)  (*byte_count), (ub1) OCI_ONE_PIECE,
                               (dvoid *) NULL, NULL, csid, csfrm)
        )

        (*char_count) = (ub4) size_in_out_char;
        (*byte_count) = (ub4) size_in_out_byte;
    }

    else

#endif

    {
        ub4 size_in_out_char_byte = 0;

        if ((OCI_BLOB == lob->type) || !OCILib.nls_utf8)
        {
            size_in_out_char_byte = (*byte_count);
        }
        else
        {
            size_in_out_char_byte = (*char_count);
        }

        EXEC
        (
            OCILobWriteAppend(lob->con->cxt, lob->con->err, lob->handle,
                              &size_in_out_char_byte, obuf,  (*byte_count),
                              (ub1) OCI_ONE_PIECE, (dvoid *) NULL, NULL, csid, csfrm)
        )

        (*char_count) = (ub4) size_in_out_char_byte;
        (*byte_count) = (ub4) size_in_out_char_byte;

        if ((OCI_CLOB == lob->type) && !OCILib.nls_utf8)
        {
             (*byte_count) *= (ub4) sizeof(otext);
        }
    }

    if (STATUS)
    {
        if (OCI_BLOB == lob->type)
        {
            lob->offset += (big_uint) (*byte_count);
        }
        else
        {
            lob->offset += (big_uint) (*char_count);
        }
    }

    if (obuf != buffer)
    {
        StringReleaseDBString((dbtext *) obuf);
    }

    RETVAL = STATUS;

    CALL_EXIT()
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
    unsigned int char_count = 0;
    unsigned int byte_count = 0;
    unsigned int *ptr_count = NULL;

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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob_src)
    CALL_CONTEXT_FROM_CON(lob->con)

    EXEC(OCILobAppend(lob->con->cxt, lob->con->err, lob->handle, lob_src->handle))

    if (STATUS)
    {
        lob->offset += LobGetLength(lob);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LobIsTemporary
 * --------------------------------------------------------------------------------------------- */

boolean LobIsTemporary
(
    OCI_Lob *lob
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CONTEXT_FROM_CON(lob->con)

    EXEC(OCILobIsTemporary(lob->con->env, lob->con->err, lob->handle, &RETVAL))

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_ENUM_VALUE(lob->con, NULL, mode, OpenModeValues, OTEXT("Open mode"))
    CALL_CONTEXT_FROM_CON(lob->con)

    EXEC(OCILobOpen(lob->con->cxt, lob->con->err, lob->handle, (ub1) mode))

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LobClose
 * --------------------------------------------------------------------------------------------- */

boolean LobClose
(
    OCI_Lob *lob
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CONTEXT_FROM_CON(lob->con)

    EXEC(OCILobClose(lob->con->cxt, lob->con->err, lob->handle))

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob2)
    CALL_CONTEXT_FROM_CON(lob->con)

    EXEC(OCILobIsEqual(lob->con->env, lob->handle, lob2->handle, &RETVAL))

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob_src)
    CALL_CONTEXT_FROM_CON(lob->con)

    if ((OCI_OBJECT_ALLOCATED == lob->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == lob->hstate))
    {
        EXEC(OCILobLocatorAssign(lob->con->cxt, lob->con->err, lob_src->handle, &lob->handle))
    }
    else
    {
        EXEC(OCILobAssign(lob->con->env, lob->con->err, lob_src->handle, &lob->handle))
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LobGetMaxSize
 * --------------------------------------------------------------------------------------------- */

big_uint LobGetMaxSize
(
    OCI_Lob *lob
)
{
    CALL_ENTER(big_uint, 0)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CONTEXT_FROM_CON(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size = 0;

        EXEC(OCILobGetStorageLimit(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &size))

        RETVAL = size;
    }

#endif

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * LobFlush
 * --------------------------------------------------------------------------------------------- */

boolean LobFlush
(
    OCI_Lob *lob
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CONTEXT_FROM_CON(lob->con)

    EXEC(OCILobFlushBuffer(lob->con->cxt, lob->con->err, lob->handle, (ub4) OCI_DEFAULT))

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CONTEXT_FROM_CON(lob->con)

    if (value)
    {
        EXEC(OCILobEnableBuffering(lob->con->cxt, lob->con->err, lob->handle))
    }
    else
    {
        EXEC(OCILobDisableBuffering(lob->con->cxt, lob->con->err, lob->handle))
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
* LobGetConnection
* --------------------------------------------------------------------------------------------- */

OCI_Connection * LobGetConnection
(
    OCI_Lob *lob
)
{
    GET_PROP(OCI_Connection *, NULL, OCI_IPC_LOB, lob, con, lob->con, NULL, lob->con->err)
}

/* --------------------------------------------------------------------------------------------- *
* LobIsRemote
* --------------------------------------------------------------------------------------------- */

boolean LobIsRemote
(
    OCI_Lob *lob
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    CALL_CONTEXT_FROM_CON(lob->con)

#if OCI_VERSION_COMPILE >= OCI_12_2

    if (ConnectionIsVersionSupported(lob->con, OCI_12_2))
    {
        ATTRIB_GET(OCI_DTYPE_LOB, OCI_ATTR_LOB_REMOTE, lob->handle, &RETVAL, sizeof(RETVAL))
    }

#endif

    CALL_EXIT()
}
