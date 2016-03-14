/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
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

static unsigned int SeekModeValues[] = { OCI_SEEK_SET, OCI_SEEK_END, OCI_SEEK_CUR };
static unsigned int OpenModeValues[] = { OCI_LOB_READONLY, OCI_LOB_READWRITE };
static unsigned int LobTypeValues[]  = { OCI_CLOB, OCI_NCLOB, OCI_BLOB };

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobInit
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OCI_LobInit
(
    OCI_Connection *con,
    OCI_Lob       **plob,
    OCILobLocator  *handle,
    ub4             type
)
{
    OCI_Lob * lob = NULL;
    boolean   res = FALSE;

    OCI_CHECK(NULL == plob, NULL);

    if (!*plob)
    {
        *plob = (OCI_Lob *) OCI_MemAlloc(OCI_IPC_LOB, sizeof(*lob), (size_t) 1, TRUE);
    }

    if (*plob)
    {
        res = TRUE;

        lob = *plob;

        lob->type   = type;
        lob->con    = con;
        lob->handle = handle;
        lob->offset = 1;

        if (!lob->handle || (OCI_OBJECT_ALLOCATED_ARRAY == lob->hstate))
        {
            ub2 csid    = OCI_DEFAULT;
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

                res = OCI_SUCCESSFUL(OCI_DescriptorAlloc((dvoid  *) lob->con->env,
                                                         (dvoid **) (void *) &lob->handle,
                                                         (ub4) OCI_DTYPE_LOB,
                                                         (size_t) 0, (dvoid **) NULL));
            }

            OCI_CALL2
            (
                res, lob->con,

                OCIAttrSet((dvoid *) lob->handle, (ub4) OCI_DTYPE_LOB,
                           (dvoid *) &empty, (ub4) sizeof(empty),
                           (ub4) OCI_ATTR_LOBEMPTY, lob->con->err)
            )

            OCI_CALL2
            (
                res, lob->con,

                OCILobCreateTemporary(lob->con->cxt, lob->con->err,
                                      lob->handle, csid, csfrm, lobtype,
                                      FALSE, OCI_DURATION_SESSION)
            )

        }
        else
        {
            lob->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }
 
    /* check for failure */

    if (!res && lob)
    {
        OCI_LobFree(lob);
        *plob = lob = NULL;
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
    OCI_LIB_CALL_ENTER(OCI_Lob*, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_ENUM_VALUE(con, NULL, type, LobTypeValues, OTEXT("Lob type"))

    call_retval = OCI_LobInit(con, &call_retval, NULL, type);
    call_status = (NULL != call_retval);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobFree
(
    OCI_Lob *lob
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_OBJECT_FETCHED(lob)
    
    call_status = TRUE;

    if (OCI_LobIsTemporary(lob))
    {
        OCI_CALL2
        (
            call_status, lob->con,

            OCILobFreeTemporary(lob->con->cxt, lob->con->err, lob->handle)
        )
    }

    if (OCI_OBJECT_ALLOCATED == lob->hstate)
    {
        OCI_DescriptorFree((dvoid *) lob->handle, (ub4) OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != lob->hstate)
    {
        OCI_FREE(lob)
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(OCI_Lob **, NULL)
    
    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)

    OCI_CHECK_ENUM_VALUE(con, NULL, type, LobTypeValues, OTEXT("Lob type"))

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_LOB, type, sizeof(OCILobLocator *),
                          sizeof(OCI_Lob), OCI_DTYPE_LOB, NULL);

    if (arr)
    {
        call_retval = (OCI_Lob **)arr->tab_obj;
        call_status = TRUE;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobArrayFree
(
    OCI_Lob **lobs
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ARRAY, lobs)

    call_retval = call_status = OCI_ArrayFreeFromHandles((void **)lobs);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LobGetType
(
    OCI_Lob *lob
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)

    call_retval = lob->type;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_ENUM_VALUE(lob->con, NULL, mode, SeekModeValues, OTEXT("Seek Mode"))

    call_status = TRUE;

    size = OCI_LobGetLength(lob);

    switch (mode)
    {
        case OCI_SEEK_CUR:
        {
            if ((offset + lob->offset - 1) <= size) 
            {
                lob->offset += offset;
                call_retval   = TRUE;
            }
            break;
        }
        case OCI_SEEK_SET:
        {
            if (offset <= size) 
            {
                lob->offset = offset + 1;
                call_retval  = TRUE;
            }
            break;
        }
        case OCI_SEEK_END:
        {
            if (offset <= size) 
            {
                lob->offset = size - offset + 1;
                call_retval  = TRUE;
            }
            break;
        }
    }
    
    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetOffset
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_LobGetOffset
(
    OCI_Lob *lob
)
{
    OCI_LIB_CALL_ENTER(big_uint, 0)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob);

    call_retval = lob->offset - 1;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_PTR(OCI_IPC_INT, char_count)
    OCI_CHECK_PTR(OCI_IPC_INT, byte_count)

    call_status = TRUE;

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
                (*byte_count) = (*char_count) * (ub4) UTF8_BYTES_PER_CHAR;
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

        OCI_CALL2
        (
            call_status, lob->con,

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

        OCI_CALL2
        (
            call_status, lob->con,

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

    if (call_status)
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

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_PTR(OCI_IPC_INT, char_count)
    OCI_CHECK_PTR(OCI_IPC_INT, byte_count)

    call_status = TRUE;

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

        OCI_CALL2
        (
            call_status, lob->con,

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

        OCI_CALL2
        (
            call_status, lob->con,

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

    if (call_status)
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

    if (!OCILib.nls_utf8 && OCI_CLOB == lob->type)
    {
        OCI_StringReleaseOracleString((dbtext *) obuf);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)

    call_status = TRUE;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL2
        (
            call_status, lob->con,

            OCILobTrim2(lob->con->cxt, lob->con->err, lob->handle, (ub8) size)
        )
    }
    else

#endif

    {
        OCI_CALL2
        (
            call_status, lob->con,

            OCILobTrim(lob->con->cxt, lob->con->err, lob->handle, (ub4) size)
        )
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob);
    OCI_CHECK_MIN(lob->con, NULL, size, 1);

    call_status = TRUE;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 lob_size = (ub8) size;

        OCI_CALL2
        (
            call_status, lob->con,

            OCILobErase2(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &lob_size, (ub8) (offset + 1))
        )

        size = (big_uint) lob_size;
    }
    else

#endif

    {
        ub4 lob_size = (ub4) size;

        OCI_CALL2
        (
            call_status, lob->con,

            OCILobErase(lob->con->cxt, lob->con->err, lob->handle,  &lob_size, (ub4) offset + 1)
        )

        size = (big_uint) lob_size;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetLength
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_LobGetLength
(
    OCI_Lob *lob
)
{
    OCI_LIB_CALL_ENTER(big_uint, 0)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)

    call_status = TRUE;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 lob_size = 0;

        OCI_CALL2
        (
            call_status, lob->con,

            OCILobGetLength2(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &lob_size)
        )

        call_retval = (big_uint) lob_size;
    }
    else

#endif

    {
        ub4 lob_size = 0;

        OCI_CALL2
        (
            call_status, lob->con,

            OCILobGetLength(lob->con->cxt, lob->con->err, lob->handle, &lob_size)
        )

        call_retval = (big_uint) lob_size;
    }

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(unsigned int, size)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, lob->con,

        OCILobGetChunkSize(lob->con->cxt, lob->con->err, lob->handle, &size)
    )

    call_retval = size;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_PTR(OCI_IPC_LOB, lob_src)

    call_status = TRUE;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {

        OCI_CALL2
        (
            call_status, lob->con,

            OCILobCopy2(lob->con->cxt, lob->con->err, lob->handle,
                        lob_src->handle, (ub8) count,
                        (ub8) (offset_dst + 1),
                        (ub8) (offset_src + 1))
        )
    }
    else

#endif

{
        OCI_CALL2
        (
            call_status, lob->con,

            OCILobCopy(lob->con->cxt, lob->con->err, lob->handle,
                       lob_src->handle, (ub4) count,
                       (ub4) (offset_dst + 1),
                       (ub4) (offset_src + 1))
        )
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_status = TRUE;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL2
        (
            call_status, lob->con,

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
        OCI_CALL2
        (
            call_status, lob->con,

            OCILobLoadFromFile(lob->con->cxt, lob->con->err,
                               lob->handle, file->handle,
                               (ub4) count,
                               (ub4) (offset_dst + 1),
                               (ub4) (offset_src + 1))
        )
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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

	OCI_LIB_CALL_DECL_VAR(boolean, FALSE)

    /* OCILobWriteAppend() seems to cause problems on Oracle client 8.1 and 9.0
    It's an Oracle known bug #886191
    So we use OCI_LobSeek() + OCI_LobWrite() instead */

    if (OCILib.version_runtime < OCI_10_1)
    {
        return OCI_LobSeek(lob, OCI_LobGetLength(lob), OCI_SEEK_SET) &&
               OCI_LobWrite2(lob, buffer, char_count, byte_count);
    }

	OCI_LIB_CALL_CHECK_CTX(OCILib.env_mode)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_PTR(OCI_IPC_INT, char_count)
    OCI_CHECK_PTR(OCI_IPC_INT, byte_count)

    call_status = TRUE;

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

        OCI_CALL2
        (
            call_status, lob->con,

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

        OCI_CALL2
        (
            call_status, lob->con,

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

    if (call_status)
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

    if (!OCILib.nls_utf8 && OCI_CLOB == lob->type)
    {
        OCI_StringReleaseOracleString((dbtext *) obuf);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_PTR(OCI_IPC_LOB, lob_src)

    call_status = TRUE;

    /*
       this might cause an ORA-24805 on Oracle 8.1.x only !
    */

    OCI_CALL2
    (
        call_status, lob->con,

        OCILobAppend(lob->con->cxt, lob->con->err, lob->handle, lob_src->handle)
    )

    if (call_status)
    {
        lob->offset += OCI_LobGetLength(lob);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobIsTemporary
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobIsTemporary
(
    OCI_Lob *lob
)
{
    boolean value = FALSE;
 
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, lob->con,

        OCILobIsTemporary(lob->con->env, lob->con->err, lob->handle, &value)
    )

    call_retval = value;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_ENUM_VALUE(lob->con, NULL, mode, OpenModeValues, OTEXT("Open mode"))

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, lob->con,

        OCILobOpen(lob->con->cxt, lob->con->err, lob->handle, (ub1) mode)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobClose
(
    OCI_Lob *lob
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, lob->con,

        OCILobClose(lob->con->cxt, lob->con->err, lob->handle)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    boolean value = FALSE;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_PTR(OCI_IPC_LOB, lob2)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, lob->con,

        OCILobIsEqual(lob->con->env, lob->handle, lob2->handle, &value)
    )

    call_retval = value;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)
    OCI_CHECK_PTR(OCI_IPC_LOB, lob_src)

    call_status = TRUE;

    if ((OCI_OBJECT_ALLOCATED == lob->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == lob->hstate))
    {
        OCI_CALL2
        (
            call_status, lob->con,

            OCILobLocatorAssign(lob->con->cxt, lob->con->err, lob_src->handle, &lob->handle)
        )
    }
    else
    {
        OCI_CALL2
        (
            call_status, lob->con,

            OCILobAssign(lob->con->env, lob->con->err, lob_src->handle, &lob->handle)
        )
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetMaxSize
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_LobGetMaxSize
(
    OCI_Lob *lob
)
{
    OCI_LIB_CALL_ENTER(big_uint, 0)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)

    call_status = TRUE;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size = 0;

        OCI_CALL2
        (
            call_status, lob->con,

            OCILobGetStorageLimit(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &size)
        )

        call_retval = size;
    }

#endif

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFlush
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobFlush
(
    OCI_Lob *lob
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, lob->con,

        OCILobFlushBuffer(lob->con->cxt, lob->con->err, lob->handle, (ub4) OCI_DEFAULT)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)

    call_status = TRUE;

    if (value)
    {
        OCI_CALL2
        (
            call_status, lob->con,

            OCILobEnableBuffering(lob->con->cxt, lob->con->err, lob->handle)
        )
    }
    else
    {
        OCI_CALL2
        (
            call_status, lob->con,

            OCILobDisableBuffering(lob->con->cxt, lob->con->err, lob->handle)
        )
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
* OCI_LobGetConnection
* --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_LobGetConnection
(
    OCI_Lob *lob
)
{
    OCI_LIB_CALL_ENTER(OCI_Connection*, NULL)

    OCI_CHECK_PTR(OCI_IPC_LOB, lob)

    call_retval = lob->con;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}
