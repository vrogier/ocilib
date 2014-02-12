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
 * $Id: file.c, Vincent Rogier $
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

    if (!*pfile)
    {
        *pfile = (OCI_File *) OCI_MemAlloc(OCI_IPC_FILE, sizeof(*file), (size_t) 1, TRUE);
    }

    if (*pfile)
    {
        file = *pfile;

        file->type   = type;
        file->con    = con;
        file->handle = handle;
        file->offset = 1;

        /* reset file info */

        if (file->dir)
        {
            file->dir[0] = 0;
        }

        if (file->name)
        {
            file->name[0] = 0;
        }

        if (!file->handle)
        {
            /* allocate handle for non fetched file (local file object) */

            file->hstate = OCI_OBJECT_ALLOCATED;

            res = OCI_SUCCESSFUL(OCI_DescriptorAlloc((dvoid *) file->con->env,
                                                     (dvoid **) (void *) &file->handle,
                                                     (ub4) OCI_DTYPE_LOB,
                                                     (size_t) 0, (dvoid **) NULL));
        }
        else if (OCI_OBJECT_ALLOCATED_ARRAY != file->hstate)
        {
            file->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }
    else
    {
        res = FALSE;
    }

    /* check for failure */

    if (!res)
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

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

    /* directory name */

    if (res)
    {
        if (!file->dir)
        {
            file->dir = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext),
                                                (size_t) (OCI_SIZE_DIRECTORY + 1), TRUE);

            res = (file->dir != NULL);
        }
        else
        {
            file->dir[0] = 0;
        }
    }

    /* file name */

    if (res)
    {
        if (!file->name )
        {
            file->name = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext),
                                                (size_t)( OCI_SIZE_FILENAME + 1),
                                                TRUE);

            res = (file->name != NULL);

        }
        else
        {
            file->name[0] = 0;
        }
    }

    /* retrieve name */

    if (res == TRUE)
    {
        dbtext *dbstr1 = NULL;
        dbtext *dbstr2 = NULL;
        int    dbsize1 = 0;
        int    dbsize2 = 0;
        ub2    usize1  = 0;
        ub2    usize2  = 0;

        dbsize1 = (int) OCI_SIZE_DIRECTORY  * (int) sizeof(otext);
        dbstr1  = OCI_StringGetOracleString(file->dir, &dbsize1);

        dbsize2 = (int) OCI_SIZE_FILENAME  * (int) sizeof(otext);
        dbstr2  = OCI_StringGetOracleString(file->name, &dbsize1);

        usize1 = (ub2) dbsize1;
        usize2 = (ub2) dbsize2;

        OCI_CALL2
        (
            res, file->con,

            OCILobFileGetName(file->con->env, file->con->err, file->handle,
                              (OraText *) dbstr1, (ub2*) &usize1,
                              (OraText *) dbstr2, (ub2*) &usize2)
        )

        dbsize1 = (int) usize1;
        dbsize2 = (int) usize2;

        OCI_StringCopyOracleStringToNativeString(dbstr1, file->dir,  dbcharcount(dbsize1));
        OCI_StringCopyOracleStringToNativeString(dbstr2, file->name, dbcharcount(dbsize2));

        OCI_StringReleaseOracleString(dbstr1);
        OCI_StringReleaseOracleString(dbstr2);
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

    if (OCI_OBJECT_ALLOCATED == file->hstate)
    {
        OCI_DescriptorFree((dvoid *) file->handle, (ub4) OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != file->hstate)
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

    if (arr)
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
    boolean  res  =  TRUE;
    big_uint size = 0;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

    size  = OCI_FileGetSize(file);

    if ((offset + file->offset - 1) > size)
    {
        res = FALSE;
    }

    if (res)
    {
        switch (mode)
        {
            case OCI_SEEK_CUR:
                file->offset += offset;
                break;
            case OCI_SEEK_SET:
                file->offset = offset + 1;
                break;
            case OCI_SEEK_END:
                file->offset = size - offset + 1;
                break;
            default:
                res = FALSE;
        }
    }

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

    if (res)
    {
        file->offset += (big_uint) size_out;
    }

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

            OCILobGetLength2(file->con->cxt, file->con->err, file->handle, (ub8 *) &size)
        )

    }
    else

#endif

    {
        ub4 size32 = (ub4) size;

        OCI_CALL2
        (
            res, file->con,

            OCILobGetLength(file->con->cxt, file->con->err, file->handle, &size32)
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
    const otext *dir,
    const otext *name
)
{
    dbtext *dbstr1  = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = -1;
    int     dbsize2 = -1;
    boolean res     = TRUE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, FALSE);

    dbstr1 = OCI_StringGetOracleString(dir,  &dbsize1);
    dbstr2 = OCI_StringGetOracleString(name, &dbsize2);

    OCI_CALL2
    (
        res, file->con,

        OCILobFileSetName(file->con->env, file->con->err,
                          &file->handle,
                          (OraText *) dbstr1, (ub2) dbsize1,
                          (OraText *) dbstr2, (ub2) dbsize2)
    )

    OCI_StringReleaseOracleString(dbstr1);
    OCI_StringReleaseOracleString(dbstr2);

    if (res)
    {
        res = OCI_FileGetInfo(file);
    }

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetDirectory
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_FileGetDirectory
(
    OCI_File *file
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, NULL);

    if (!file->dir || !file->dir[0])
    {
        res = OCI_FileGetInfo(file);
    }

    OCI_RESULT(res);

    return file->dir;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_FileGetName
(
    OCI_File *file
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_FILE, file, NULL);

    if (!file->name || !file->name[0])
    {
        res = OCI_FileGetInfo(file);
    }

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

        OCILobFileOpen(file->con->cxt, file->con->err, file->handle, (ub1) OCI_LOB_READONLY)
    )

    if (res)
    {
        file->con->nb_files++;
    }

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

    if (res)
    {
        file->con->nb_files--;
    }

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

        OCILobIsEqual(file->con->env, file->handle, file2->handle, &value)
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

    if ((OCI_OBJECT_ALLOCATED == file->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == file->hstate))
    {
        OCI_CALL2
        (
            res, file->con,

            OCILobLocatorAssign(file->con->cxt, file->con->err, file_src->handle, &file->handle)
        )
    }
    else
    {
        OCI_CALL2
        (
            res, file->con,

            OCILobAssign(file->con->env, file->con->err, file_src->handle, &file->handle)
        )
    }

    if (res)
    {
        OCI_FileGetInfo(file);
    }

    OCI_RESULT(res);

    return res;
}
