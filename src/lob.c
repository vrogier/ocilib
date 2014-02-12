/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2014 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: lob.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

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
    boolean res   = TRUE;

    OCI_CHECK(plob == NULL, NULL);

    if (!*plob)
    {
        *plob = (OCI_Lob *) OCI_MemAlloc(OCI_IPC_LOB, sizeof(*lob), (size_t) 1, TRUE);
    }

    if (*plob)
    {
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
    else
    {
        res = FALSE;
    }

    /* check for failure */

    if (!res)
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
    OCI_Lob *lob = NULL;

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    lob = OCI_LobInit(con, &lob, NULL, type);

    OCI_RESULT(lob != NULL);

    return lob;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobFree
(
    OCI_Lob *lob
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    OCI_CHECK_OBJECT_FETCHED(lob, FALSE);

    if (OCI_LobIsTemporary(lob))
    {
        OCI_CALL2
        (
            res, lob->con,

            OCILobFreeTemporary(lob->con->cxt, lob->con->err, lob->handle)
        )
    }

    if (OCI_OBJECT_ALLOCATED == lob->hstate)
    {
        OCI_DescriptorFree((dvoid *) lob->handle, (ub4) OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != lob->hstate)
    {
        OCI_FREE(lob);
    }

    OCI_RESULT(res);

    return res;
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
    OCI_Lob **lobs = NULL;

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_LOB, type, sizeof(OCILobLocator *),
                          sizeof(OCI_Lob), OCI_DTYPE_LOB, NULL);

    if (arr)
    {
        lobs = (OCI_Lob **) arr->tab_obj;
    }

    return lobs;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobArrayFree
(
    OCI_Lob **lobs
)
{
    return OCI_ArrayFreeFromHandles((void **) lobs);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LobGetType
(
    OCI_Lob *lob
)
{
    OCI_CHECK_PTR(OCI_IPC_LOB, lob, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return lob->type;
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
    boolean res   = TRUE;
    big_uint size = 0;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    size = OCI_LobGetLength(lob);

    if ((offset + lob->offset - 1) > size)
    {
        res = FALSE;
    }

    if (res)
    {
        switch (mode)
        {
            case OCI_SEEK_CUR:
                lob->offset += offset;
                break;
            case OCI_SEEK_SET:
                lob->offset = offset + 1;
                break;
            case OCI_SEEK_END:
                lob->offset = size - offset + 1;
                break;
            default:
                res = FALSE;
        }
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetOffset
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_LobGetOffset
(
    OCI_Lob *lob
)
{
    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);

    OCI_RESULT(TRUE);

    return lob->offset - 1;
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
    boolean res = TRUE;
    ub1 csfrm   = 0;
    ub2 csid    = 0;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, char_count, FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, byte_count, FALSE);

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

    csfrm = (lob->type == OCI_NCLOB) ? SQLCS_NCHAR : SQLCS_IMPLICIT;

    OCI_CHECK_MIN(lob->con, NULL, (*byte_count), 1, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {        
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        OCI_CALL2
        (
            res, lob->con,

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
        ub4 size_in_out_char_byte = 0;

        if (lob->type == OCI_BLOB)
        {
            size_in_out_char_byte = (*byte_count);
        }
        else
        {
            size_in_out_char_byte = (*char_count);
        }

        OCI_CALL2
        (
            res, lob->con,

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
            (*char_count) = OCI_StringUTF8Length((const char *) buffer);
        }

    #endif

    }

    if (res)
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

    OCI_RESULT(res);

    return res;
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

    return (ptr_count != NULL ? *ptr_count : 0);
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
    boolean res   = TRUE;
    ub1     csfrm = 0;
    ub2     csid  = 0;
    void   *obuf  = NULL;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, char_count, FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, byte_count, FALSE);

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

                (*char_count) = OCI_StringLength((const char *) buffer, sizeof(char));

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

    csfrm = (lob->type == OCI_NCLOB) ? SQLCS_NCHAR : lob->type == OCI_NCLOB;

    OCI_CHECK_MIN(lob->con, NULL, (*byte_count), 1, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        OCI_CALL2
        (
            res, lob->con,

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
            res, lob->con,

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

    if (res)
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

    OCI_RESULT(res);

    return res;
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

    return (ptr_count  != NULL ? *ptr_count : 0);
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL2
        (
            res, lob->con,

            OCILobTrim2(lob->con->cxt, lob->con->err, lob->handle, (ub8) size)
        )
    }
    else

#endif

    {
        OCI_CALL2
        (
            res, lob->con,

            OCILobTrim(lob->con->cxt, lob->con->err, lob->handle, (ub4) size)
        )
    }

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);
    OCI_CHECK_MIN(lob->con, NULL, size, 1, 0);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 lob_size = (ub8) size;

        OCI_CALL2
        (
            res, lob->con,

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
            res, lob->con,

            OCILobErase(lob->con->cxt, lob->con->err, lob->handle,  &lob_size, (ub4) offset + 1)
        )

        size = (big_uint) lob_size;
    }

    OCI_RESULT(res);

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetLength
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_LobGetLength
(
    OCI_Lob *lob
)
{
    boolean res   = TRUE;
    big_uint size = 0;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 lob_size = 0;

        OCI_CALL2
        (
            res, lob->con,

            OCILobGetLength2(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &lob_size)
        )

        size = (big_uint) lob_size;
    }
    else

#endif

    {
        ub4 lob_size = 0;

        OCI_CALL2
        (
            res, lob->con,

            OCILobGetLength(lob->con->cxt, lob->con->err, lob->handle, &lob_size)
        )

        size = (big_uint) lob_size;
    }

    OCI_RESULT(res);

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetChunkSize
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_LobGetChunkSize
(
    OCI_Lob *lob
)
{
    boolean res = TRUE;
    ub4 size    = 0;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);

    OCI_CALL2
    (
        res, lob->con,

        OCILobGetChunkSize(lob->con->cxt, lob->con->err, lob->handle, &size)
    )

    OCI_RESULT(res);

    return (unsigned int) size;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, lob_src, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {

        OCI_CALL2
        (
            res, lob->con,

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
            res, lob->con,

            OCILobCopy(lob->con->cxt, lob->con->err, lob->handle,
                       lob_src->handle, (ub4) count,
                       (ub4) (offset_dst + 1),
                       (ub4) (offset_src + 1))
        )
    }

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob,   FALSE);
    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL2
        (
            res, lob->con,

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
            res, lob->con,

            OCILobLoadFromFile(lob->con->cxt, lob->con->err,
                               lob->handle, file->handle,
                               (ub4) count,
                               (ub4) (offset_dst + 1),
                               (ub4) (offset_src + 1))
        )
    }

    OCI_RESULT(res);

    return res;
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
    boolean res   = TRUE;
    ub1     csfrm = 0;
    ub2     csid  = 0;
    void   *obuf  = NULL;

    OCI_CHECK_PTR(OCI_IPC_LOB, char_count, FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, byte_count, FALSE);

    /* OCILobWriteAppend() seems to cause problems on Oracle client 8.1 and 9.0
       It's an Oracle known bug #886191
       So we use OCI_LobSeek() + OCI_LobWrite() instead */

    if (OCILib.version_runtime < OCI_10_1)
    {
        return OCI_LobSeek(lob, OCI_LobGetLength(lob), OCI_SEEK_SET) &&
               OCI_LobWrite2(lob, buffer, char_count, byte_count);
    }

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

                (*char_count) = OCI_StringLength((const char *) buffer, sizeof(char));

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

    csfrm = (lob->type == OCI_NCLOB) ? SQLCS_NCHAR : SQLCS_IMPLICIT;
  
    OCI_CHECK_MIN(lob->con, NULL, (*byte_count), 1, FALSE);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_in_out_char = (ub8) (*char_count);
        ub8 size_in_out_byte = (ub8) (*byte_count);

        OCI_CALL2
        (
            res, lob->con,

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
            res, lob->con,

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

    if (res)
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

    OCI_RESULT(res);

    return res;
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

    return (ptr_count != NULL ? *ptr_count : 0);
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
    boolean res     = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, lob_src, FALSE);

    /*
       this might cause an ORA-24805 on Oracle 8.1.x only !
       I couldn’t find a bug ID on Metalink, but Oracle 9i had many fixes for
       lobs !
    */

    OCI_CALL2
    (
        res, lob->con,

        OCILobAppend(lob->con->cxt, lob->con->err, lob->handle, lob_src->handle)
    )

    if (res)
    {
        lob->offset += OCI_LobGetLength(lob);
    }

    OCI_RESULT(res);

    return res;
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
    boolean res   = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    OCI_CALL2
    (
        res, lob->con,

        OCILobIsTemporary(lob->con->env, lob->con->err, lob->handle, &value)
    )

    OCI_RESULT(res);

    return value;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    OCI_CALL2
    (
        res, lob->con,

        OCILobOpen(lob->con->cxt, lob->con->err, lob->handle, (ub1) mode)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobClose
(
    OCI_Lob *lob
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    OCI_CALL2
    (
        res, lob->con,

        OCILobClose(lob->con->cxt, lob->con->err, lob->handle)
    )

    OCI_RESULT(res);

    return res;
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
    boolean res   = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob,FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, lob2, FALSE);

    OCI_CALL2
    (
        res, lob->con,

        OCILobIsEqual(lob->con->env, lob->handle, lob2->handle, &value)
    )

    OCI_RESULT(res);

    return value;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_LOB, lob_src, FALSE);

    if ((OCI_OBJECT_ALLOCATED == lob->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == lob->hstate))
    {
        OCI_CALL2
        (
            res, lob->con,

            OCILobLocatorAssign(lob->con->cxt, lob->con->err, lob_src->handle, &lob->handle)
        )
    }
    else
    {
        OCI_CALL2
        (
            res, lob->con,

            OCILobAssign(lob->con->env, lob->con->err, lob_src->handle, &lob->handle)
        )
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobGetMaxSize
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_LobGetMaxSize
(
    OCI_Lob *lob
)
{
    boolean res   = TRUE;
    big_uint size = 0;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, 0);

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL2
        (
            res, lob->con,

            OCILobGetStorageLimit(lob->con->cxt, lob->con->err, lob->handle, (ub8 *) &size)
        )
    }

#endif

    OCI_RESULT(res);

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFlush
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_LobFlush
(
    OCI_Lob *lob
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    OCI_CALL2
    (
        res, lob->con,

        OCILobFlushBuffer(lob->con->cxt, lob->con->err, lob->handle, (ub4) OCI_DEFAULT)
    )

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_LOB, lob, FALSE);

    if (value == TRUE)
    {
        OCI_CALL2
        (
            res, lob->con,

            OCILobEnableBuffering(lob->con->cxt, lob->con->err, lob->handle)
        )
    }
    else
    {
        OCI_CALL2
        (
            res, lob->con,

            OCILobDisableBuffering(lob->con->cxt, lob->con->err, lob->handle)
        )
    }

    OCI_RESULT(res);

    return res;
}

