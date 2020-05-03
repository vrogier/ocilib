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

static const unsigned int SeekModeValues[] =
{
    OCI_SEEK_SET,
    OCI_SEEK_END,
    OCI_SEEK_CUR
};

static const unsigned int FileTypeValues[] =
{
    OCI_CFILE,
    OCI_BFILE
};

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
    ENTER_FUNC
    (
        /* returns */ OCI_File*, file,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    ALLOC_DATA(OCI_IPC_FILE, file, 1)

    file->type   = type;
    file->con    = con;
    file->handle = handle;
    file->offset = 1;

    /* reset file info */

    if (NULL != file->dir)
    {
        file->dir[0] = 0;
    }

    if (NULL != file->name)
    {
        file->name[0] = 0;
    }

    if (NULL == file->handle)
    {
        /* allocate handle for non fetched file (local file object) */

        file->hstate = OCI_OBJECT_ALLOCATED;

        CHECK(MemoryAllocDescriptor((dvoid *)file->con->env,
                                    (dvoid **)(void *)&file->handle,
                                    (ub4)OCI_DTYPE_LOB))
    }
    else if (OCI_OBJECT_ALLOCATED_ARRAY != file->hstate)
    {
        file->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            FileFree(file);
            file = NULL;
        }

        SET_RETVAL(file)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetInfo
 * --------------------------------------------------------------------------------------------- */

boolean FileGetInfo
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    )

    dbtext* dbstr1 = NULL;
    dbtext* dbstr2 = NULL;

    int dbsize1 = 0;
    int dbsize2 = 0;

    ub2 usize1 = 0;
    ub2 usize2 = 0;

    CHECK_PTR(OCI_IPC_FILE, file)

    /* directory name */

    ALLOC_DATA(OCI_IPC_STRING, file->dir, OCI_SIZE_DIRECTORY + 1)

    file->dir[0] = 0;

    /* file name */

    ALLOC_DATA(OCI_IPC_STRING, file->name, OCI_SIZE_FILENAME + 1)

    file->name[0] = 0;

    /* retrieve name */

    dbsize1 = (int) OCI_SIZE_DIRECTORY  * (int) sizeof(otext);
    dbstr1  = StringGetDBString(file->dir, &dbsize1);

    dbsize2 = (int) OCI_SIZE_FILENAME  * (int) sizeof(otext);
    dbstr2  = StringGetDBString(file->name, &dbsize1);

    usize1 = (ub2) dbsize1;
    usize2 = (ub2) dbsize2;

    CHECK_OCI
    (
        file->con->err,
        OCILobFileGetName,
        file->con->env, file->con->err, file->handle,
        (OraText *) dbstr1, (ub2*) &usize1,
        (OraText *) dbstr2, (ub2*) &usize2
    )

    dbsize1 = (int) usize1;
    dbsize2 = (int) usize2;

    StringCopyDBStringToNativeString(dbstr1, file->dir,  dbcharcount(dbsize1));
    StringCopyDBStringToNativeString(dbstr2, file->name, dbcharcount(dbsize2));

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr1);
        StringReleaseDBString(dbstr2);
    )
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
    ENTER_FUNC
    (
        /* returns */ OCI_File*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(type, FileTypeValues, OTEXT("File Type"))

    SET_RETVAL(FileInitialize(con, NULL, NULL, type))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * FileFree
 * --------------------------------------------------------------------------------------------- */

boolean FileFree
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_FILE, file)
    CHECK_OBJECT_FETCHED(file)

    FREE(file->dir)
    FREE(file->name)

    if (OCI_OBJECT_ALLOCATED == file->hstate)
    {
        MemoryFreeDescriptor((dvoid*)file->handle, (ub4)OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != file->hstate)
    {
        FREE(file)
    }

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ OCI_File**, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    OCI_Array* arr = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con)
    CHECK_ENUM_VALUE(type, FileTypeValues, OTEXT("File Type"))

    arr = ArrayCreate(con, nbelem, OCI_CDT_FILE, type,
                      sizeof(OCILobLocator*), sizeof(OCI_File),
                      OCI_DTYPE_LOB, NULL);

    CHECK_NULL(arr)

    SET_RETVAL((OCI_File**)arr->tab_obj)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * FileArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean FileFreeArray
(
    OCI_File **files
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_ARRAY, files)

    SET_RETVAL(ArrayFreeFromHandles((void**)files))

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    )

    big_uint size = 0;

    boolean success = FALSE;

    CHECK_PTR(OCI_IPC_FILE, file)
    CHECK_ENUM_VALUE(mode, SeekModeValues, OTEXT("Seek Mode"))

    size = FileGetSize(file);

    switch (mode)
    {
        case OCI_SEEK_CUR:
        {
            if ((offset + file->offset - 1) <= size)
            {
                file->offset += offset;

                success = TRUE;
            }
            break;
        }
        case OCI_SEEK_SET:
        {
            if (offset <= size)
            {
                file->offset = offset + 1;

                success = TRUE;
            }
            break;
        }
        case OCI_SEEK_END:
        {
            if (offset <= size)
            {
                file->offset = size - offset + 1;

                success = TRUE;
            }
            break;
        }
    }

    SET_RETVAL(success)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetOffset
 * --------------------------------------------------------------------------------------------- */

big_uint FileGetOffset
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ big_uint, 0,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_FILE, file)

    SET_RETVAL(file->offset - 1)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_FILE, file
    )

    ub4 size_in = 0;
    ub4 size_out = 0;

    CHECK_PTR(OCI_IPC_FILE, file)
    CHECK_MIN(len, 1)

    size_out = size_in = len;

#ifdef OCI_LOB2_API_ENABLED

    if (Env.use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;

        CHECK_OCI
        (
            file->con->err,
            OCILobRead2,
            file->con->cxt, file->con->err,
            file->handle, &size_byte,
            &size_char, (ub8) file->offset,
            buffer, (ub8) size_in,
            (ub1) OCI_ONE_PIECE, (dvoid *) NULL,
            NULL, (ub2) 0, (ub1) SQLCS_IMPLICIT
        )

        size_out = (ub4) size_byte;
    }

    else

#endif

    {
        const ub4 offset = (ub4) file->offset;

        CHECK_OCI
        (
            file->con->err,
            OCILobRead,
            file->con->cxt, file->con->err,
            file->handle,  &size_out, offset,
            buffer, size_in, (dvoid *) NULL,
            NULL, (ub2) 0, (ub1) SQLCS_IMPLICIT
        )
    }

    file->offset += (big_uint) size_out;

    SET_RETVAL(size_out)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int FileGetType
(
    OCI_File *file
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_FILE, file,
        /* member */ type
    )
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetSize
 * --------------------------------------------------------------------------------------------- */

big_uint FileGetSize
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ big_uint, 0,
        /* context */ OCI_IPC_FILE, file
    )

    big_uint size = 0;

    CHECK_PTR(OCI_IPC_FILE, file)

#ifdef OCI_LOB2_API_ENABLED

    if (Env.use_lob_ub8)
    {
        CHECK_OCI
        (
            file->con->err,
            OCILobGetLength2,
            file->con->cxt, file->con->err,
            file->handle, (ub8 *) &size
        )
    }
    else

#endif

    {
        ub4 size32 = (ub4) size;

        CHECK_OCI
        (
            file->con->err,
            OCILobGetLength,
            file->con->cxt, file->con->err,
            file->handle, &size32
        )

        size = (big_uint) size32;
    }

    SET_RETVAL(size)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFileExists
 * --------------------------------------------------------------------------------------------- */

boolean FileExists
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_FILE, file)

    boolean exists = FALSE;

    CHECK_OCI
    (
        file->con->err,
        OCILobFileExists, file->con->cxt,
        file->con->err, file->handle,
        &exists
    )

    SET_RETVAL(exists)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    )

    dbtext *dbstr1 = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = -1;
    int     dbsize2 = -1;

    CHECK_PTR(OCI_IPC_FILE, file)

    dbstr1 = StringGetDBString(dir,  &dbsize1);
    dbstr2 = StringGetDBString(name, &dbsize2);

    CHECK_OCI
    (
        file->con->err,
        OCILobFileSetName,
        file->con->env,
        file->con->err,
        &file->handle,
        (OraText *) dbstr1, (ub2) dbsize1,
        (OraText *) dbstr2, (ub2) dbsize2
    )

    CHECK(FileGetInfo(file))

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr1);
        StringReleaseDBString(dbstr2);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetDirectory
 * --------------------------------------------------------------------------------------------- */

const otext * FileGetDirectory
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_FILE, file)

    if (!IS_STRING_VALID(file->dir))
    {
        CHECK(FileGetInfo(file))
    }

    SET_RETVAL(file->dir)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * FileGetName
 * --------------------------------------------------------------------------------------------- */

const otext * FileGetName
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ const otext *, NULL,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_FILE, file)

    if (!IS_STRING_VALID(file->name))
    {
        CHECK(FileGetInfo(file))
    }

    SET_RETVAL(file->name)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * FileOpen
 * --------------------------------------------------------------------------------------------- */

boolean FileOpen
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_FILE, file)

    CHECK_OCI
    (
        file->con->err,
        OCILobFileOpen,
        file->con->cxt, file->con->err,
        file->handle, (ub1) OCI_LOB_READONLY
    )

    file->con->nb_files++;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_LobFileIsOpen
 * --------------------------------------------------------------------------------------------- */

boolean FileIsOpen
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_FILE, file)

    boolean is_opened = FALSE;

    CHECK_OCI
    (
        file->con->err,
        OCILobFileIsOpen, file->con->cxt,
        file->con->err, file->handle,
        &is_opened
    )

    SET_RETVAL(is_opened)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * FileClose
 * --------------------------------------------------------------------------------------------- */

boolean FileClose
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_FILE, file)

    CHECK_OCI
    (
        file->con->err,
        OCILobFileClose,
        file->con->cxt, file->con->err,
        file->handle
    )

    file->con->nb_files--;

    SET_SUCCESS()

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_FILE, file)
    CHECK_PTR(OCI_IPC_FILE, file2)

    boolean is_equal = FALSE;

    CHECK_OCI
    (
        file->con->err,
        OCILobIsEqual,
        file->con->env, file->handle,
        file2->handle, &is_equal
    )

    SET_RETVAL(is_equal)

    EXIT_FUNC()
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
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    )

    CHECK_PTR(OCI_IPC_FILE, file)
    CHECK_PTR(OCI_IPC_FILE, file_src)

    if ((OCI_OBJECT_ALLOCATED == file->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == file->hstate))
    {
        CHECK_OCI
        (
            file->con->err,
            OCILobLocatorAssign,
            file->con->cxt, file->con->err,
            file_src->handle, &file->handle
        )
    }
    else
    {
        CHECK_OCI
        (
            file->con->err,
            OCILobAssign,
            file->con->env, file->con->err,
            file_src->handle, &file->handle
        )
    }

    CHECK(FileGetInfo(file))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
* FileGetConnection
* --------------------------------------------------------------------------------------------- */

OCI_Connection * FileGetConnection
(
    OCI_File *file
)
{
    GET_PROP
    (
        /* result */ OCI_Connection *, NULL,
        /* handle */ OCI_IPC_FILE, file,
        /* member */ con
    )
}
