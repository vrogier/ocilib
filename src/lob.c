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

static const unsigned int SeekModeValues[] = { OCI_SEEK_SET, OCI_SEEK_END, OCI_SEEK_CUR };
static const unsigned int OpenModeValues[] = { OCI_LOB_READONLY, OCI_LOB_READWRITE };
static const unsigned int LobTypeValues[]  = { OCI_CLOB, OCI_NCLOB, OCI_BLOB };

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobInit
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_LobInit
(
    OCI_Connection *con,
    OCI_Lob        *lob,
    OCILobLocator  *handle,
    ub4             type
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_ALLOCATE_DATA(OCI_IPC_LOB, lob, 1);

    if (OCI_STATUS)
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

                OCI_STATUS = OCI_DescriptorAlloc((dvoid  *)lob->con->env, (dvoid **)(void *)&lob->handle, OCI_DTYPE_LOB);
            }

            OCI_SET_ATTRIB(OCI_DTYPE_LOB, OCI_ATTR_LOBEMPTY, lob->handle, &empty, sizeof(empty))
            OCI_EXEC(OCILobCreateTemporary(lob->con->cxt, lob->con->err, lob->handle, csid, csfrm, lobtype, FALSE, OCI_DURATION_SESSION))
        }
        else
        {
            lob->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }
 
    /* check for failure */

    if (!OCI_STATUS && lob)
    {
        OCI_LobFree(lob);
        lob = NULL;
    }

    return lob;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_API OCI_LobCreate
(
    OCI_Connection *con,
    unsigned int    type
)
{
    OCI_CALL_ENTER(OCI_Lob*, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_ENUM_VALUE(con, NULL, type, LobTypeValues, OTEXT("Lob type"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_RETVAL = OCI_LobInit(con, NULL, NULL, type);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobFree
(
    OCI_Lob *lob
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_OBJECT_FETCHED(lob)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    if (OCI_LobIsTemporary(lob))
    {
        OCI_EXEC(OCILobFreeTemporary(lob->con->cxt, lob->con->err, lob->handle))
    }

    if (OCI_OBJECT_ALLOCATED == lob->hstate)
    {
        OCI_DescriptorFree((dvoid *) lob->handle, (ub4) OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != lob->hstate)
    {
        OCI_FREE(lob)
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Lob ** OCI_API OCI_LobArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_CALL_ENTER(OCI_Lob **, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CALL_CHECK_ENUM_VALUE(con, NULL, type, LobTypeValues, OTEXT("Lob type"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_LOB, type, sizeof(OCILobLocator *), sizeof(OCI_Lob), OCI_DTYPE_LOB, NULL);
    OCI_STATUS = (NULL != arr);

    if (OCI_STATUS)
    {
        OCI_RETVAL = (OCI_Lob **)arr->tab_obj;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobArrayFree
(
    OCI_Lob **lobs
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ARRAY, lobs)

    OCI_RETVAL = OCI_STATUS = OCI_ArrayFreeFromHandles((void **)lobs);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LobGetType
(
    OCI_Lob *lob
)
{
    OCI_GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_LOB, lob, type, lob->con, NULL, lob->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobSeek
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobSeek
(
    OCI_Lob     *lob,
    big_uint     offset,
    unsigned int mode
)
{
    big_uint size = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_ENUM_VALUE(lob->con, NULL, mode, SeekModeValues, OTEXT("Seek Mode"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    size = OCI_LobGetLength(lob);

    switch (mode)
    {
        case OCI_SEEK_CUR:
        {
            if ((offset + lob->offset - 1) <= size) 
            {
                lob->offset += offset;
                OCI_RETVAL   = TRUE;
            }
            break;
        }
        case OCI_SEEK_SET:
        {
            if (offset <= size) 
            {
                lob->offset = offset + 1;
                OCI_RETVAL  = TRUE;
            }
            break;
        }
        case OCI_SEEK_END:
        {
            if (offset <= size) 
            {
                lob->offset = size - offset + 1;
                OCI_RETVAL  = TRUE;
            }
            break;
        }
    }
    
    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetOffset
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_LobGetOffset
(
    OCI_Lob *lob
)
{
    OCI_CALL_ENTER(big_uint, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob);
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    OCI_RETVAL = lob->offset - 1;
    OCI_STATUS = TRUE;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobRead2
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobRead2
(
    OCI_Lob      *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
)
{
    ub1 csfrm   = 0;
    ub2 csid    = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, char_count)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, byte_count)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

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

        OCI_EXEC
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

        OCI_EXEC
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
            (*char_count) = (ub4) OCI_StringLength((const char *)buffer, sizeof(char));
        }

    #endif

    }

    if (OCI_STATUS)
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
                OCI_StringUTF16ToUTF32(buffer, buffer, (int) (*char_count));
                (*byte_count) = (ub4) (*char_count) * (ub4) sizeof(otext);
            }
        }
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobRead
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LobRead
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

    OCI_LobRead2(lob, buffer, &char_count, &byte_count);

    return (NULL != ptr_count ? *ptr_count : 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobWrite
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobWrite2
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

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, char_count)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, byte_count)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

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

                (*char_count) = (ub4) OCI_StringLength((const char *)buffer, sizeof(char));

        #endif

            }
            else
            {
                (*char_count) = (*byte_count) / (ub4) sizeof(otext);
            }
        }

        obuf = OCI_StringGetOracleString( (otext *) buffer, (int *) byte_count);
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

        OCI_EXEC
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

        OCI_EXEC
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

    if (OCI_STATUS)
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
        OCI_StringReleaseOracleString((dbtext *) obuf);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobWrite
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LobWrite
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

    OCI_LobWrite2(lob, buffer, &char_count, &byte_count);

    return (NULL != ptr_count ? *ptr_count : 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobTruncate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobTruncate
(
    OCI_Lob *lob,
    big_uint size
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_EXEC(OCILobTrim2(lob->con->cxt, lob->con->err, lob->handle, (ub8) size))
    }
    else

#endif

    {
        OCI_EXEC(OCILobTrim(lob->con->cxt, lob->con->err, lob->handle, (ub4) size))
    }

    if (OCI_STATUS)
    {
        if (lob->offset > size)
        {
            lob->offset = OCI_LobGetLength(lob) + 1;
        }
    }

    OCI_RETVAL = OCI_STATUS;


    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobErase
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_LobErase
(
    OCI_Lob *lob,
    big_uint offset,
    big_uint size
)
{
    OCI_CALL_ENTER(big_uint, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob);
    OCI_CALL_CHECK_MIN(lob->con, NULL, size, 1);
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 lob_size = (ub8) size;

        OCI_EXEC(OCILobErase2(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &lob_size, (ub8) (offset + 1)))

        size = (big_uint) lob_size;
    }
    else

#endif

    {
        ub4 lob_size = (ub4) size;

        OCI_EXEC(OCILobErase(lob->con->cxt, lob->con->err, lob->handle,  &lob_size, (ub4) offset + 1))

        size = (big_uint) lob_size;
    }

    if (OCI_SUCCESS)
    {
        OCI_RETVAL = size;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetLength
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_LobGetLength
(
    OCI_Lob *lob
)
{
    OCI_CALL_ENTER(big_uint, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 lob_size = 0;

        OCI_EXEC(OCILobGetLength2(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &lob_size))

        OCI_RETVAL = (big_uint) lob_size;
    }
    else

#endif

    {
        ub4 lob_size = 0;

        OCI_EXEC(OCILobGetLength(lob->con->cxt, lob->con->err, lob->handle, &lob_size))

        OCI_RETVAL = (big_uint) lob_size;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetChunkSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LobGetChunkSize
(
    OCI_Lob *lob
)
{
    ub4 size = 0;

    OCI_CALL_ENTER(unsigned int, size)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    OCI_EXEC(OCILobGetChunkSize(lob->con->cxt, lob->con->err, lob->handle, &size))

    OCI_RETVAL = (unsigned int) size;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobCopy
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobCopy
(
    OCI_Lob *lob,
    OCI_Lob *lob_src,
    big_uint offset_dst,
    big_uint offset_src,
    big_uint count
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob_src)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_EXEC
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
        OCI_EXEC
        (
            OCILobCopy(lob->con->cxt, lob->con->err, lob->handle,
                       lob_src->handle, (ub4) count,
                       (ub4) (offset_dst + 1),
                       (ub4) (offset_src + 1))
        )
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobCopyFromFile
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobCopyFromFile
(
    OCI_Lob  *lob,
    OCI_File *file,
    big_uint  offset_dst,
    big_uint  offset_src,
    big_uint  count
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_PTR(OCI_IPC_FILE, file)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_EXEC
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
        OCI_EXEC
        (
            OCILobLoadFromFile(lob->con->cxt, lob->con->err,
                               lob->handle, file->handle,
                               (ub4) count,
                               (ub4) (offset_dst + 1),
                               (ub4) (offset_src + 1))
        )
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobAppend2
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobAppend2
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

    OCI_CALL_DECLARE_VARIABLES(boolean, FALSE, TRUE)

    /* OCILobWriteAppend() seems to cause problems on Oracle client 8.1 and 9.0
    It's an Oracle known bug #886191
    So we use OCI_LobSeek() + OCI_LobWrite() instead */

    if (OCILib.version_runtime < OCI_10_1)
    {
        return OCI_LobSeek(lob, OCI_LobGetLength(lob), OCI_SEEK_SET) &&
               OCI_LobWrite2(lob, buffer, char_count, byte_count);
    }

    OCI_CALL_CONTEXT_ENTER(OCILib.env_mode)

    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, char_count)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, byte_count)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

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

                (*char_count) = (ub4) OCI_StringLength((const char *)buffer, sizeof(char));

            #endif

            }
            else
            {
                (*char_count) = (*byte_count) / (ub4) sizeof(otext);
            }
        }

        obuf = OCI_StringGetOracleString((const otext *) buffer, (int *) byte_count);
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

        OCI_EXEC
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

        OCI_EXEC
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

    if (OCI_STATUS)
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
        OCI_StringReleaseOracleString((dbtext *) obuf);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobAppend
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LobAppend
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

    OCI_LobAppend2(lob, buffer, &char_count, &byte_count);

    return (NULL != ptr_count ? *ptr_count : 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobAppendLob
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobAppendLob
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob_src)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    OCI_EXEC(OCILobAppend(lob->con->cxt, lob->con->err, lob->handle, lob_src->handle))

    if (OCI_STATUS)
    {
        lob->offset += OCI_LobGetLength(lob);
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobIsTemporary
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobIsTemporary
(
    OCI_Lob *lob
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    OCI_EXEC(OCILobIsTemporary(lob->con->env, lob->con->err, lob->handle, &OCI_RETVAL))

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobOpen
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobOpen
(
    OCI_Lob     *lob,
    unsigned int mode
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_ENUM_VALUE(lob->con, NULL, mode, OpenModeValues, OTEXT("Open mode"))
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    OCI_EXEC(OCILobOpen(lob->con->cxt, lob->con->err, lob->handle, (ub1) mode))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobClose
(
    OCI_Lob *lob
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    OCI_EXEC(OCILobClose(lob->con->cxt, lob->con->err, lob->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobIsEqual
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobIsEqual
(
    OCI_Lob *lob,
    OCI_Lob *lob2
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob2)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    OCI_EXEC(OCILobIsEqual(lob->con->env, lob->handle, lob2->handle, &OCI_RETVAL))

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobAssign
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobAssign
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob_src)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    if ((OCI_OBJECT_ALLOCATED == lob->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == lob->hstate))
    {
        OCI_EXEC(OCILobLocatorAssign(lob->con->cxt, lob->con->err, lob_src->handle, &lob->handle))
    }
    else
    {
        OCI_EXEC(OCILobAssign(lob->con->env, lob->con->err, lob_src->handle, &lob->handle))
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetMaxSize
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_LobGetMaxSize
(
    OCI_Lob *lob
)
{
    OCI_CALL_ENTER(big_uint, 0)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size = 0;

        OCI_EXEC(OCILobGetStorageLimit(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &size))

        OCI_RETVAL = size;
    }

#endif

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFlush
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobFlush
(
    OCI_Lob *lob
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    OCI_EXEC(OCILobFlushBuffer(lob->con->cxt, lob->con->err, lob->handle, (ub4) OCI_DEFAULT))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobEnableBuffering
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobEnableBuffering
(
    OCI_Lob *lob,
    boolean  value
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CALL_CONTEXT_SET_FROM_CONN(lob->con)

    if (value)
    {
        OCI_EXEC(OCILobEnableBuffering(lob->con->cxt, lob->con->err, lob->handle))
    }
    else
    {
        OCI_EXEC(OCILobDisableBuffering(lob->con->cxt, lob->con->err, lob->handle))
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
* OCI_LobGetConnection
* --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_LobGetConnection
(
    OCI_Lob *lob
)
{
    OCI_GET_PROP(OCI_Connection *, NULL, OCI_IPC_LOB, lob, con, lob->con, NULL, lob->con->err)
}
