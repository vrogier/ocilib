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

#include "file.h"

#include "array.h"
#include "macros.h"
#include "memory.h"
#include "strings.h"

static const unsigned int SeekModeValues[] = { OCI_SEEK_SET, OCI_SEEK_END, OCI_SEEK_CUR };
static const unsigned int FileTypeValues[] = { OCI_CFILE, OCI_BFILE };

/* --------------------------------------------------------------------------------------------- *
 * FileInit
 * --------------------------------------------------------------------------------------------- */

OCI_File * FileInitialize
(
    OCI_Connection *con,
    OCI_File       *file,
    OCILobLocator  *handle,
    ub4             type
)
{
    DECLARE_CTX(TRUE)
    CALL_CONTEXT_FROM_CON(con)

    ALLOC_DATA(OCI_IPC_FILE, file, 1);

    if (STATUS)
    {
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

            STATUS = MemoryAllocDescriptor((dvoid *)file->con->env, (dvoid **)(void *)&file->handle, (ub4)OCI_DTYPE_LOB);
        }
        else if (OCI_OBJECT_ALLOCATED_ARRAY != file->hstate)
        {
            file->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }

    /* check for failure */

    if (!STATUS && file)
    {
        FileFree(file);
        file = NULL;
    }

    return file;
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetInfo
 * --------------------------------------------------------------------------------------------- */

boolean FileGetInfo
(
    OCI_File *file
)
{
    DECLARE_CTX(TRUE)

    CHECK(NULL == file, FALSE)

    CALL_CONTEXT_FROM_CON(file->con)

    /* directory name */

    ALLOC_DATA(OCI_IPC_STRING, file->dir, OCI_SIZE_DIRECTORY + 1)

    if (STATUS)
    {
        file->dir[0] = 0;
    }

    /* file name */

    ALLOC_DATA(OCI_IPC_STRING, file->name, OCI_SIZE_FILENAME + 1)
    
    if (STATUS)
    {
        file->name[0] = 0;
    }

    /* retrieve name */

    if (STATUS)
    {
        dbtext *dbstr1 = NULL;
        dbtext *dbstr2 = NULL;
        int    dbsize1 = 0;
        int    dbsize2 = 0;
        ub2    usize1  = 0;
        ub2    usize2  = 0;

        dbsize1 = (int) OCI_SIZE_DIRECTORY  * (int) sizeof(otext);
        dbstr1  = StringGetDBString(file->dir, &dbsize1);

        dbsize2 = (int) OCI_SIZE_FILENAME  * (int) sizeof(otext);
        dbstr2  = StringGetDBString(file->name, &dbsize1);

        usize1 = (ub2) dbsize1;
        usize2 = (ub2) dbsize2;

        EXEC
        (
            OCILobFileGetName(file->con->env, file->con->err, file->handle,
                              (OraText *) dbstr1, (ub2*) &usize1,
                              (OraText *) dbstr2, (ub2*) &usize2)
        )

        dbsize1 = (int) usize1;
        dbsize2 = (int) usize2;

        StringCopyDBStringToNativeString(dbstr1, file->dir,  dbcharcount(dbsize1));
        StringCopyDBStringToNativeString(dbstr2, file->name, dbcharcount(dbsize2));

        StringReleaseDBString(dbstr1);
        StringReleaseDBString(dbstr2);
    }

    return STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * FileCreate
 * --------------------------------------------------------------------------------------------- */

OCI_File * FileCreate
(
    OCI_Connection *con,
    unsigned int    type
)
{
    CALL_ENTER(OCI_File *, NULL)
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CHECK_ENUM_VALUE(con, NULL, type, FileTypeValues, OTEXT("File Type"))
    CALL_CONTEXT_FROM_CON(con)

    RETVAL = FileInitialize(con, NULL, NULL, type);
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileFree
 * --------------------------------------------------------------------------------------------- */

boolean FileFree
(
    OCI_File *file
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CHECK_OBJECT_FETCHED(file)
    CALL_CONTEXT_FROM_CON(file->con)

    FREE(file->dir)
    FREE(file->name)

    if (OCI_OBJECT_ALLOCATED == file->hstate)
    {
        MemoryFreeDescriptor((dvoid *) file->handle, (ub4) OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != file->hstate)
    {
        FREE(file)
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_File ** FileCreateArray
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    CALL_ENTER(OCI_File **, NULL)
    CALL_CHECK_PTR(OCI_IPC_CONNECTION, con)
    CALL_CHECK_ENUM_VALUE(con, NULL, type, FileTypeValues, OTEXT("File Type"))
    CALL_CONTEXT_FROM_CON(con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_FILE, type, sizeof(OCILobLocator *), sizeof(OCI_File), OCI_DTYPE_LOB, NULL);
    STATUS = (NULL != arr);

    if (arr)
    {
        RETVAL = (OCI_File **)arr->tab_obj;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean FileFreeArray
(
    OCI_File **files
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ARRAY, files)

    RETVAL = STATUS = ArrayFreeFromHandles((void **)files);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileSeek
 * --------------------------------------------------------------------------------------------- */

boolean FileSeek
(
    OCI_File    *file,
    big_uint     offset,
    unsigned int mode
)
{
    big_uint size = 0;

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CHECK_ENUM_VALUE(file->con, NULL, mode, SeekModeValues, OTEXT("Seek Mode"))
    CALL_CONTEXT_FROM_CON(file->con)

    size = FileGetSize(file);

    switch (mode)
    {
        case OCI_SEEK_CUR:
        {
            if ((offset + file->offset - 1) <= size) 
            {
                file->offset += offset;
                RETVAL   = TRUE;
            }
            break;
        }
        case OCI_SEEK_SET:
        {
            if (offset <= size) 
            {
                file->offset = offset + 1;
                RETVAL  = TRUE;
            }
            break;
        }
        case OCI_SEEK_END:
        {
            if (offset <= size) 
            {
                file->offset = size - offset + 1;
                RETVAL  = TRUE;
            }
            break;
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetOffset
 * --------------------------------------------------------------------------------------------- */

big_uint FileGetOffset
(
    OCI_File *file
)
{
    CALL_ENTER(big_uint, 0)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CONTEXT_FROM_CON(file->con)

    RETVAL = file->offset - 1;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileRead
 * --------------------------------------------------------------------------------------------- */

unsigned int FileRead
(
    OCI_File    *file,
    void        *buffer,
    unsigned int len
)
{
    ub4 size_in  = 0;
    ub4 size_out = 0;

    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CHECK_MIN(file->con, NULL, len, 1)
    CALL_CONTEXT_FROM_CON(file->con)

    size_out = size_in = len;

    STATUS = TRUE;

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;

        EXEC
        (
            OCILobRead2(file->con->cxt, file->con->err,
                        file->handle, &size_byte,
                        &size_char, (ub8) file->offset,
                        buffer, (ub8) size_in,
                        (ub1) OCI_ONE_PIECE, (dvoid *) NULL,
                        NULL, (ub2) 0, (ub1) SQLCS_IMPLICIT)
        )

        size_out = (ub4) size_byte;
    }

    else

 #endif

    {
        const ub4 offset = (ub4) file->offset;

        EXEC
        (
            OCILobRead(file->con->cxt, file->con->err,
                       file->handle,  &size_out, offset,
                       buffer, size_in, (dvoid *) NULL,
                       NULL, (ub2) 0, (ub1) SQLCS_IMPLICIT)
        )
    }

    if (STATUS)
    {
        file->offset += (big_uint) size_out;
        
        RETVAL = size_out;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int FileGetType
(
    OCI_File *file
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_FILE, file, type, file->con, NULL, file->con->err)
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetSize
 * --------------------------------------------------------------------------------------------- */

big_uint FileGetSize
(
    OCI_File *file
)
{
    big_uint size = 0;

    CALL_ENTER(big_uint, 0)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CONTEXT_FROM_CON(file->con)

#ifdef OCI_LOB2_API_ENABLED

    if (OCILib.use_lob_ub8)
    {
        EXEC(OCILobGetLength2(file->con->cxt, file->con->err, file->handle, (ub8 *) &size))
    }
    else

#endif

    {
        ub4 size32 = (ub4) size;

        EXEC(OCILobGetLength(file->con->cxt, file->con->err, file->handle, &size32))

        size = (big_uint) size32;
    }

    RETVAL = size;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFileExists
 * --------------------------------------------------------------------------------------------- */

boolean FileExists
(
    OCI_File *file
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CONTEXT_FROM_CON(file->con)

    EXEC(OCILobFileExists(file->con->cxt, file->con->err, file->handle, &RETVAL))

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileSetName
 * --------------------------------------------------------------------------------------------- */

boolean FileSetName
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

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CONTEXT_FROM_CON(file->con)

    dbstr1 = StringGetDBString(dir,  &dbsize1);
    dbstr2 = StringGetDBString(name, &dbsize2);

    EXEC
    (
        OCILobFileSetName(file->con->env, file->con->err,
                          &file->handle,
                          (OraText *) dbstr1, (ub2) dbsize1,
                          (OraText *) dbstr2, (ub2) dbsize2)
    )

    StringReleaseDBString(dbstr1);
    StringReleaseDBString(dbstr2);

    if (STATUS)
    {
        STATUS = FileGetInfo(file);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetDirectory
 * --------------------------------------------------------------------------------------------- */

const otext * FileGetDirectory
(
    OCI_File *file
)
{
    CALL_ENTER(const otext *, NULL)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CONTEXT_FROM_CON(file->con)

    if (!IS_STRING_VALID(file->dir))
    {
        STATUS = FileGetInfo(file);
    }

    RETVAL = file->dir;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetName
 * --------------------------------------------------------------------------------------------- */

const otext * FileGetName
(
    OCI_File *file
)
{
    CALL_ENTER(const otext *, NULL)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CONTEXT_FROM_CON(file->con)

    if (!IS_STRING_VALID(file->name))
    {
        STATUS = FileGetInfo(file);
    }

    RETVAL = file->name;
   
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileOpen
 * --------------------------------------------------------------------------------------------- */

boolean FileOpen
(
    OCI_File *file
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CONTEXT_FROM_CON(file->con)

    EXEC(OCILobFileOpen(file->con->cxt, file->con->err, file->handle, (ub1) OCI_LOB_READONLY))

    if (STATUS)
    {
        file->con->nb_files++;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFileIsOpen
 * --------------------------------------------------------------------------------------------- */

boolean FileIsOpen
(
    OCI_File *file
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CONTEXT_FROM_CON(file->con)

    EXEC(OCILobFileIsOpen(file->con->cxt, file->con->err, file->handle, &RETVAL))
    
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileClose
 * --------------------------------------------------------------------------------------------- */

boolean FileClose
(
    OCI_File *file
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CONTEXT_FROM_CON(file->con)

    EXEC(OCILobFileClose(file->con->cxt, file->con->err, file->handle))

    if (STATUS)
    {
        file->con->nb_files--;
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileIsEqual
 * --------------------------------------------------------------------------------------------- */

boolean FileIsEqual
(
    OCI_File *file,
    OCI_File *file2
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CHECK_PTR(OCI_IPC_FILE, file2)
    CALL_CONTEXT_FROM_CON(file->con)

    EXEC(OCILobIsEqual(file->con->env, file->handle, file2->handle, &RETVAL))

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * FileAssign
 * --------------------------------------------------------------------------------------------- */

boolean FileAssign
(
    OCI_File *file,
    OCI_File *file_src
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_FILE, file)
    CALL_CHECK_PTR(OCI_IPC_FILE, file_src)
    CALL_CONTEXT_FROM_CON(file->con)

    if ((OCI_OBJECT_ALLOCATED == file->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == file->hstate))
    {
        EXEC(OCILobLocatorAssign(file->con->cxt, file->con->err, file_src->handle, &file->handle))
    }
    else
    {
        EXEC(OCILobAssign(file->con->env, file->con->err, file_src->handle, &file->handle))
    }

    RETVAL = STATUS = STATUS && FileGetInfo(file);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
* FileGetConnection
* --------------------------------------------------------------------------------------------- */

OCI_Connection * FileGetConnection
(
    OCI_File *file
)
{
    GET_PROP(OCI_Connection *, NULL, OCI_IPC_FILE, file, con, file->con, NULL, file->con->err)
}
