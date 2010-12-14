/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: file.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileInit
 * --------------------------------------------------------------------------------------------- */

OCI_File * OCI_FileInit
(
    OCI_Connection *con,
    OCI_File      **pfile,
    OCILobLocator  *handle,
    ub4             type
)
{
    OCI_File *file = NULL;
    boolean res    = TRUE;

    OCI_CHECK(pfile == NULL, NULL);

    if (*pfile == NULL)
        *pfile = (OCI_File *) OCI_MemAlloc(OCI_IPC_FILE, sizeof(*file),
                                           (size_t) 1, TRUE);

    if (*pfile != NULL)
    {
        file = *pfile;

        file->type   = type;
        file->con    = con;
        file->handle = handle;
        file->offset = 1;

        /* reset file info */

        if (file->dir != NULL)
            file->dir[0] = 0;

        if (file->name != NULL)
            file->name[0] = 0;

        if (file->handle == NULL)
        {
            /* allocate handle for non fetched file (local file object) */

            file->hstate = OCI_OBJECT_ALLOCATED;

            res = (OCI_SUCCESS == OCI_DescriptorAlloc((dvoid *) OCILib.env,
                                                      (dvoid **) (void *) &file->handle,
                                                      (ub4) OCI_DTYPE_LOB,
                                                      (size_t) 0, (dvoid **) NULL));
        }
        else if (file->hstate != OCI_OBJECT_ALLOCATED_ARRAY)
        {
            file->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }
    else
        res = FALSE;

    /* check for failure */

    if (res == FALSE)
    {
        OCI_FileFree(file);
        file = NULL;
    }

    return file;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetInfo
 * --------------------------------------------------------------------------------------------- */

boolean OCI_FileGetInfo
(
    OCI_File *file
)
{
    boolean res = TRUE;
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int osize1  = 0;
    int osize2  = 0;
    ub2 usize1  = 0;
    ub2 usize2  = 0;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

    /* directory name */

    if (file->dir == NULL)
    {
        if (res == TRUE)
        {
            file->dir = (mtext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(mtext),
                                               (size_t) (OCI_SIZE_DIRECTORY + 1),
                                               TRUE);

            res = (file->dir != NULL);
        }
    }
    else
        file->dir[0] = 0;

    /* file name */

    if (file->name == NULL)
    {
        if (res == TRUE)
        {
            file->name = (mtext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(mtext),
                                                (size_t)( OCI_SIZE_FILENAME + 1),
                                                TRUE);

            res = (file->name != NULL);
        }
    }
    else
        file->name[0] = 0;

    /* retrieve name */

    if (res == TRUE)
    {
        osize1 = (int   ) OCI_SIZE_DIRECTORY  * (int) sizeof(mtext);
        ostr1  = (void *) OCI_GetInputMetaString(file->dir, &osize1);

        osize2 = (int   ) OCI_SIZE_FILENAME  * (int) sizeof(mtext);
        ostr2  = (void *) OCI_GetInputMetaString(file->name, &osize1);

        usize1 = (ub2) osize1;
        usize2 = (ub2) osize2;

        OCI_CALL2
        (
            res, file->con,

            OCILobFileGetName(OCILib.env, file->con->err, file->handle,
                              (OraText *) ostr1, (ub2*) &usize1,
                              (OraText *) ostr2, (ub2*) &usize2)
        )

        osize1 = (int) usize1;
        osize2 = (int) usize2;

        OCI_GetOutputMetaString(ostr1, file->dir,  &osize1);
        OCI_GetOutputMetaString(ostr2, file->name, &osize2);

        OCI_ReleaseMetaString(ostr1);
        OCI_ReleaseMetaString(ostr2);
    }

    return res;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileCreate
 * --------------------------------------------------------------------------------------------- */

OCI_File * OCI_API OCI_FileCreate
(
    OCI_Connection *con,
    unsigned int    type
)
{
    OCI_File *file = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con, NULL);

    file = OCI_FileInit(con, &file, NULL, type);

    OCI_RESULT(file != NULL);

    return file;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileFree
(
    OCI_File *file
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);
    OCI_CHECK_OBJECT_FETCHED(file, FALSE);

    OCI_FREE(file->dir);
    OCI_FREE(file->name);

    if (file->hstate == OCI_OBJECT_ALLOCATED)
    {
        OCI_DescriptorFree((dvoid *) file->handle, (ub4) OCI_DTYPE_LOB);
    }

    if (file->hstate != OCI_OBJECT_ALLOCATED_ARRAY)
    {
        OCI_FREE(file);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_File ** OCI_API OCI_FileArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    OCI_Array *arr   = NULL;
    OCI_File **files = NULL;

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_FILE, type,
                          sizeof(OCILobLocator *), sizeof(OCI_File),
                          OCI_DTYPE_LOB, NULL);

    if (arr != NULL)
    {
        files = (OCI_File **) arr->tab_obj;
    }

    return files;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileArrayFree
(
    OCI_File **files
)
{
    return OCI_ArrayFreeFromHandles((void **) files);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileSeek
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileSeek
(
    OCI_File    *file,
    big_uint     offset,
    unsigned int mode
)
{
    boolean res   = TRUE;
    big_uint size = 0;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

    size = OCI_FileGetSize(file);

    if ((mode == OCI_SEEK_CUR && (offset + file->offset-1) > size))
        res = FALSE;
    else if (mode == OCI_SEEK_SET)
        file->offset = offset + 1;
    else if (mode == OCI_SEEK_END)
        file->offset = size-offset + 1;
    else if (mode == OCI_SEEK_CUR)
        file->offset += offset;
    else
        res = FALSE;

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetOffset
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_FileGetOffset
(
    OCI_File *file
)
{
    OCI_CHECK_PTR(OCI_IPC_FILE, file, 0);

    OCI_RESULT(TRUE);

    return file->offset - 1;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileRead
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_FileRead
(
    OCI_File    *file,
    void        *buffer,
    unsigned int len
)
{
    boolean res  = TRUE;
    ub4 size_in  = 0;
    ub4 size_out = 0;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, 0);
    OCI_CHECK_MIN(file->con, NULL, len, 1, 0);

    size_out = size_in = len;

    #ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;

        OCI_CALL2
        (
            res, file->con,

            OCILobRead2(file->con->cxt, file->con->err,
                        file->handle, &size_byte,
                        &size_char, (ub8) file->offset,
                        buffer, (ub8) size_in,
                        (ub1) OCI_ONE_PIECE, (dvoid *) NULL,
                        NULL, (ub2) 0, (ub1) SQLCS_IMPLICIT)
        )
    }

    else

    #endif

    {
        ub4 offset = (ub4) file->offset;

        OCI_CALL2
        (
            res, file->con,

            OCILobRead(file->con->cxt, file->con->err,
                       file->handle,  &size_out, offset,
                       buffer, size_in, (dvoid *) NULL,
                       NULL, (ub2) 0, (ub1) SQLCS_IMPLICIT)
        )
    }

    if (res == TRUE)
        file->offset += (big_uint) size_out;

    OCI_RESULT(res);

    return size_out;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_FileGetType
(
    OCI_File *file
)
{
    OCI_CHECK_PTR(OCI_IPC_FILE, file, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return file->type;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetSize
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_FileGetSize
(
    OCI_File *file
)
{
    boolean res   = TRUE;
    big_uint size = 0;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, 0);

    #ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL2
        (
            res, file->con,

            OCILobGetLength2(file->con->cxt, file->con->err,
                             file->handle, (ub8 *) &size)
        )

    }
    else

    #endif

    {
        ub4 size32 = (ub4) size;

        OCI_CALL2
        (
            res, file->con,

            OCILobGetLength(file->con->cxt, file->con->err,
                            file->handle, &size32)
        )

        size = (big_uint) size32;
    }

    OCI_RESULT(res);

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFileExists
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileExists
(
    OCI_File *file
)
{
    boolean res   = TRUE;
    boolean value = FALSE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

    OCI_CALL2
    (
        res, file->con,

        OCILobFileExists(file->con->cxt, file->con->err, file->handle, &value)
    )

    OCI_RESULT(res);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileSetName
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileSetName
(
    OCI_File    *file,
    const mtext *dir,
    const mtext *name
)
{
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int osize1  = -1;
    int osize2  = -1;
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

    ostr1 = OCI_GetInputMetaString(dir,  &osize1);
    ostr2 = OCI_GetInputMetaString(name, &osize2);

    OCI_CALL2
    (
        res, file->con,

        OCILobFileSetName(OCILib.env, file->con->err,
                          &file->handle,
                          (OraText *) ostr1, (ub2) osize1,
                          (OraText *) ostr2, (ub2) osize2)
    )

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    if (res == TRUE)
        res = OCI_FileGetInfo(file);

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetDirectory
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_FileGetDirectory
(
    OCI_File *file
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, NULL);

    if ((file->dir == NULL) || (file->dir[0] == 0))
        res = OCI_FileGetInfo(file);

    return file->dir;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetName
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_FileGetName
(
    OCI_File *file
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, NULL);

    if ((file->name == NULL) || (file->name[0] == 0))
        res = OCI_FileGetInfo(file);

    OCI_RESULT(res);

    return file->name;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileOpen
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileOpen
(
    OCI_File *file
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

    OCI_CALL2
    (
        res, file->con,

        OCILobFileOpen(file->con->cxt, file->con->err,
                       file->handle, (ub1) OCI_LOB_READONLY)
    )

    if (res == TRUE)
        file->con->nb_files++;

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFileIsOpen
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileIsOpen
(
    OCI_File *file
)
{
    boolean res   = TRUE;
    boolean value = FALSE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

    OCI_CALL2
    (
        res, file->con,

        OCILobFileIsOpen(file->con->cxt, file->con->err, file->handle, &value)
    )

    OCI_RESULT(res);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileClose
(
    OCI_File *file
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

    OCI_CALL2
    (
        res, file->con,

        OCILobFileClose(file->con->cxt, file->con->err, file->handle)
    )

    if (res == TRUE)
        file->con->nb_files--;

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileIsEqual
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileIsEqual
(
    OCI_File *file,
    OCI_File *file2
)
{
    boolean res   = TRUE;
    boolean value = FALSE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_FILE, file2, FALSE);

    OCI_CALL2
    (
        res, file->con,

        OCILobIsEqual(OCILib.env, file->handle, file2->handle, &value)
    )

    OCI_RESULT(res);

    return value;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileAssign
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileAssign
(
    OCI_File *file,
    OCI_File *file_src
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_FILE, file_src, FALSE);

    if (file->hstate == OCI_OBJECT_ALLOCATED)
    {
        OCI_CALL2
        (
            res, file->con,

            OCILobLocatorAssign(file->con->cxt, file->con->err,
                                file_src->handle, &file->handle)
        )
    }
    else
    {
        OCI_CALL2
        (
            res, file->con,

            OCILobAssign(OCILib.env, file->con->err,
                         file_src->handle, &file->handle)
        )
    }

    if (res == TRUE)
        OCI_FileGetInfo(file);

    OCI_RESULT(res);

    return res;
}
