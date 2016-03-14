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
static unsigned int FileTypeValues[] = { OCI_CFILE, OCI_BFILE };

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
    boolean   res  = FALSE;

    OCI_CHECK(NULL == pfile, NULL);

    if (!*pfile)
    {
        *pfile = (OCI_File *) OCI_MemAlloc(OCI_IPC_FILE, sizeof(*file), (size_t) 1, TRUE);
    }

    if (*pfile)
    {

        res = TRUE;

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

    /* check for failure */

    if (!res && file)
    {
        OCI_FileFree(file);
        *pfile = file = NULL;
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

    OCI_CHECK(NULL == file, FALSE)

    /* directory name */

    if (res)
    {
        if (!file->dir)
        {
            file->dir = (otext *) OCI_MemAlloc(OCI_IPC_STRING, sizeof(otext),
                                                (size_t) (OCI_SIZE_DIRECTORY + 1), TRUE);

            res = (NULL != file->dir);
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

            res = (NULL != file->name);

        }
        else
        {
            file->name[0] = 0;
        }
    }

    /* retrieve name */

    if (res)
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
    OCI_LIB_CALL_ENTER(OCI_File *, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_ENUM_VALUE(con, NULL, type, FileTypeValues, OTEXT("File Type"))

    call_retval = OCI_FileInit(con, &call_retval, NULL, type);
    call_status = (NULL != call_retval);

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileFree
(
    OCI_File *file
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)
    OCI_CHECK_OBJECT_FETCHED(file)

    OCI_FREE(file->dir)
    OCI_FREE(file->name)

    if (OCI_OBJECT_ALLOCATED == file->hstate)
    {
        OCI_DescriptorFree((dvoid *) file->handle, (ub4) OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != file->hstate)
    {
        OCI_FREE(file)
    }

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    OCI_Array *arr = NULL;

    OCI_LIB_CALL_ENTER(OCI_File **, NULL)

    OCI_CHECK_PTR(OCI_IPC_CONNECTION, con)
    OCI_CHECK_ENUM_VALUE(con, NULL, type, FileTypeValues, OTEXT("File Type"))

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_FILE, type, sizeof(OCILobLocator *),
                          sizeof(OCI_File), OCI_DTYPE_LOB, NULL);

    if (arr)
    {
        call_retval = (OCI_File **)arr->tab_obj;
        call_status = (NULL != call_retval);
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileArrayFree
(
    OCI_File **files
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ARRAY, files)

    call_retval = call_status = OCI_ArrayFreeFromHandles((void **)files);

    OCI_LIB_CALL_EXIT()
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
    big_uint size = 0;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)
    OCI_CHECK_ENUM_VALUE(file->con, NULL, mode, SeekModeValues, OTEXT("Seek Mode"))

    size = OCI_FileGetSize(file);

    switch (mode)
    {
        case OCI_SEEK_CUR:
        {
            if ((offset + file->offset - 1) <= size) 
            {
                file->offset += offset;
                call_retval   = TRUE;
            }
            break;
        }
        case OCI_SEEK_SET:
        {
            if (offset <= size) 
            {
                file->offset = offset + 1;
                call_retval  = TRUE;
            }
            break;
        }
        case OCI_SEEK_END:
        {
            if (offset <= size) 
            {
                file->offset = size - offset + 1;
                call_retval  = TRUE;
            }
            break;
        }
    }

    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetOffset
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_FileGetOffset
(
    OCI_File *file
)
{
    OCI_LIB_CALL_ENTER(big_uint, 0)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_retval = file->offset - 1;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
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
    ub4 size_in  = 0;
    ub4 size_out = 0;

    OCI_LIB_CALL_ENTER(unsigned int, 0)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)
    OCI_CHECK_MIN(file->con, NULL, len, 1)

    size_out = size_in = len;

    call_status = TRUE;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;

        OCI_CALL2
        (
            call_status, file->con,

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
            call_status, file->con,

            OCILobRead(file->con->cxt, file->con->err,
                       file->handle,  &size_out, offset,
                       buffer, size_in, (dvoid *) NULL,
                       NULL, (ub2) 0, (ub1) SQLCS_IMPLICIT)
        )
    }

    if (call_status)
    {
        file->offset += (big_uint) size_out;
        call_retval   = size_out;
    }

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_FileGetType
(
    OCI_File *file
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_retval = file->type;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetSize
 * --------------------------------------------------------------------------------------------- */

big_uint OCI_API OCI_FileGetSize
(
    OCI_File *file
)
{
    big_uint size = 0;

    OCI_LIB_CALL_ENTER(big_uint, 0)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_status = TRUE;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        OCI_CALL2
        (
            call_status, file->con,

            OCILobGetLength2(file->con->cxt, file->con->err, file->handle, (ub8 *) &size)
        )

    }
    else

#endif

    {
        ub4 size32 = (ub4) size;

        OCI_CALL2
        (
            call_status, file->con,

            OCILobGetLength(file->con->cxt, file->con->err, file->handle, &size32)
        )

        size = (big_uint) size32;
    }

    call_retval = size;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFileExists
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileExists
(
    OCI_File *file
)
{
    boolean value = FALSE;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, file->con,

        OCILobFileExists(file->con->cxt, file->con->err, file->handle, &value)
    )

    call_retval = value;

    OCI_LIB_CALL_EXIT()
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

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_status = TRUE;

    dbstr1 = OCI_StringGetOracleString(dir,  &dbsize1);
    dbstr2 = OCI_StringGetOracleString(name, &dbsize2);

    OCI_CALL2
    (
        call_status, file->con,

        OCILobFileSetName(file->con->env, file->con->err,
                          &file->handle,
                          (OraText *) dbstr1, (ub2) dbsize1,
                          (OraText *) dbstr2, (ub2) dbsize2)
    )

    OCI_StringReleaseOracleString(dbstr1);
    OCI_StringReleaseOracleString(dbstr2);

    if (call_status)
    {
        call_status = OCI_FileGetInfo(file);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetDirectory
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_FileGetDirectory
(
    OCI_File *file
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_status = TRUE;

    if (!file->dir || !file->dir[0])
    {
        call_status = OCI_FileGetInfo(file);
    }

    call_retval = file->dir;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_FileGetName
(
    OCI_File *file
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_status = TRUE;

    if (!file->name || !file->name[0])
    {
        call_status = OCI_FileGetInfo(file);
    }

    call_retval = file->name;
   
    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileOpen
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileOpen
(
    OCI_File *file
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, file->con,

        OCILobFileOpen(file->con->cxt, file->con->err, file->handle, (ub1) OCI_LOB_READONLY)
    )

    if (call_status)
    {
        file->con->nb_files++;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFileIsOpen
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileIsOpen
(
    OCI_File *file
)
{
    boolean value = FALSE;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, file->con,

        OCILobFileIsOpen(file->con->cxt, file->con->err, file->handle, &value)
    )

    call_retval = value;
    
    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_FileClose
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_FileClose
(
    OCI_File *file
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, file->con,

        OCILobFileClose(file->con->cxt, file->con->err, file->handle)
    )

    if (call_status)
    {
        file->con->nb_files--;
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
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
    boolean value = FALSE;

    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)
    OCI_CHECK_PTR(OCI_IPC_FILE, file2)

    call_status = TRUE;

    OCI_CALL2
    (
        call_status, file->con,

        OCILobIsEqual(file->con->env, file->handle, file2->handle, &value)
    )

    call_retval = value;

    OCI_LIB_CALL_EXIT()
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
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)
    OCI_CHECK_PTR(OCI_IPC_FILE, file_src)

    call_status = TRUE;

    if ((OCI_OBJECT_ALLOCATED == file->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == file->hstate))
    {
        OCI_CALL2
        (
            call_status, file->con,

            OCILobLocatorAssign(file->con->cxt, file->con->err, file_src->handle, &file->handle)
        )
    }
    else
    {
        OCI_CALL2
        (
            call_status, file->con,

            OCILobAssign(file->con->env, file->con->err, file_src->handle, &file->handle)
        )
    }

    if (call_status)
    {
        OCI_FileGetInfo(file);
    }

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
* OCI_FileGetConnection
* --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_FileGetConnection
(
    OCI_File *file
)
{
    OCI_LIB_CALL_ENTER(OCI_Connection*,  NULL)

    OCI_CHECK_PTR(OCI_IPC_FILE, file)

    call_retval = file->con;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}
