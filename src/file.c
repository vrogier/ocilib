/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "stringutils.h"

static const OCI_UInt32 SeekModeValues[] =
{
    OCI_SEEK_SET,
    OCI_SEEK_END,
    OCI_SEEK_CUR
};

static const OCI_UInt32 FileTypeValues[] =
{
    OCI_CFILE,
    OCI_BFILE
};

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileInit
 * --------------------------------------------------------------------------------------------- */

OCI_File * OcilibFileInitialize
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
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);

    ALLOC_HANDLE(OCI_IPC_FILE, file);
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

        CHECK(OcilibMemoryAllocDescriptor(file->con->env,
                                          (dvoid *)file->con->env->handle,
                                          (dvoid **)(void *)&file->handle,
                                          (ub4)OCI_DTYPE_LOB));
    }
    else if (OCI_OBJECT_ALLOCATED_ARRAY != file->hstate)
    {
        file->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibFileFree(file);
            file = NULL;
        }

        SET_RETVAL(file);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileGetInfo
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileGetInfo
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    );

    dbtext* dbstr1 = NULL;
    dbtext* dbstr2 = NULL;

    OCI_Int32 dbsize1 = 0;
    OCI_Int32 dbsize2 = 0;

    ub2 usize1 = 0;
    ub2 usize2 = 0;

    CHECK_PTR(OCI_IPC_FILE, file);

    /* directory name */

    ALLOC_DATA(OCI_IPC_STRING, file->dir, OCI_SIZE_DIRECTORY + 1);

    file->dir[0] = 0;

    /* file name */

    ALLOC_DATA(OCI_IPC_STRING, file->name, OCI_SIZE_FILENAME + 1);

    file->name[0] = 0;

    /* retrieve name */

    dbsize1 = (OCI_Int32) OCI_SIZE_DIRECTORY  * (OCI_Int32) sizeof(otext);
    dbstr1  = OcilibStringGetDBString(file->con->env, file->dir, &dbsize1);

    dbsize2 = (OCI_Int32) OCI_SIZE_FILENAME  * (OCI_Int32) sizeof(otext);
    dbstr2  = OcilibStringGetDBString(file->con->env, file->name, &dbsize2);

    usize1 = (ub2) dbsize1;
    usize2 = (ub2) dbsize2;

    CHECK_OCI
    (
        file->con->err,
        OCILobFileGetName,
        file->con->env->handle, file->con->err, file->handle,
        (OraText *) dbstr1, (ub2*) &usize1,
        (OraText *) dbstr2, (ub2*) &usize2
    );

    dbsize1 = (OCI_Int32) usize1;
    dbsize2 = (OCI_Int32) usize2;

    OcilibStringCopyDBStringToNativeString(file->con->env, dbstr1, file->dir,  dbcharcount(dbsize1));
    OcilibStringCopyDBStringToNativeString(file->con->env, dbstr2, file->name, dbcharcount(dbsize2));

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(file->con->env, dbstr1);
        OcilibStringReleaseDBString(file->con->env, dbstr2);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileCreate
 * --------------------------------------------------------------------------------------------- */

OCI_File * OcilibFileCreate
(
    OCI_Connection *con,
    OCI_FileType    type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_File*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_ENUM_VALUE(type, FileTypeValues, OTEXT("File Type"));

    SET_RETVAL(OcilibFileInitialize(con, NULL, NULL, type));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileFree
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    );

    CHECK_PTR(OCI_IPC_FILE, file);
    CHECK_OBJECT_FETCHED(file);

    FREE(file->dir);
    FREE(file->name);

    if (OCI_OBJECT_ALLOCATED == file->hstate)
    {
        OcilibMemoryFreeDescriptor(file->con->env, (dvoid*)file->handle, (ub4)OCI_DTYPE_LOB);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != file->hstate)
    {
        OcilibErrorResetSource(NULL, file);

        FREE(file);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_File ** OcilibFileCreateArray
(
    OCI_Connection *con,
    OCI_FileType    type,
    OCI_UInt32      nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_File**, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    );

    OCI_Array* arr = NULL;

    CHECK_PTR(OCI_IPC_CONNECTION, con);
    CHECK_ENUM_VALUE(type, FileTypeValues, OTEXT("File Type"));

    arr = OcilibArrayCreate(con->env,con, nbelem, OCI_CDT_FILE, type,
                            sizeof(OCILobLocator*), sizeof(OCI_File),
                            OCI_DTYPE_LOB, NULL);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_File**)arr->tab_obj);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileArrayFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileFreeArray
(
    OCI_File **files
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, files);

    SET_RETVAL(OcilibArrayFreeFromHandles(((OCI_File *) files[0])->con->env, (void**)files));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileSeek
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileSeek
(
    OCI_File    *file,
    OCI_UInt64   offset,
    OCI_SeekMode mode
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    );

    OCI_UInt64 size = 0;

    OCI_Boolean success = FALSE;

    CHECK_PTR(OCI_IPC_FILE, file);
    CHECK_ENUM_VALUE(mode, SeekModeValues, OTEXT("Seek Mode"));

    size = OcilibFileGetSize(file);

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

    SET_RETVAL(success);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileGetOffset
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibFileGetOffset
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt64, 0,
        /* context */ OCI_IPC_FILE, file
    );

    CHECK_PTR(OCI_IPC_FILE, file);

    SET_RETVAL(file->offset - 1);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileRead
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibFileRead
(
    OCI_File  *file,
    void      *buffer,
    OCI_UInt32 len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_FILE, file
    );

    ub4 size_in  = 0;
    ub4 size_out = 0;

    CHECK_PTR(OCI_IPC_FILE, file);
    CHECK_MIN(len, 1);

    size_out = size_in = len;

#ifdef OCI_LOB2_API_ENABLED

    if (file->con->env->use_lob_ub8)
    {
        ub8 size_char = (ub8) len;
        ub8 size_byte = (ub8) size_in;

        CHECK_OCI
        (
            file->con->err,
            OCILobRead2,
            file->con->handle, file->con->err,
            file->handle, &size_byte,
            &size_char, (ub8) file->offset,
            buffer, (ub8) size_in,
            (ub1) OCI_ONE_PIECE, (dvoid *) NULL,
            NULL, (ub2) 0, (ub1) SQLCS_IMPLICIT
        );

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
            file->con->handle, file->con->err,
            file->handle,  &size_out, offset,
            buffer, size_in, (dvoid *) NULL,
            NULL, (ub2) 0, (ub1) SQLCS_IMPLICIT
        );
    }

    file->offset += (OCI_UInt64) size_out;

    SET_RETVAL(size_out);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileGetType
 * --------------------------------------------------------------------------------------------- */

OCI_FileType OcilibFileGetType
(
    OCI_File *file
)
{
    GET_PROP
    (
        /* result */ OCI_FileType, OCI_UNKNOWN,
        /* handle */ OCI_IPC_FILE, file,
        /* member */ type
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileGetSize
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibFileGetSize
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt64, 0,
        /* context */ OCI_IPC_FILE, file
    );

    OCI_UInt64 size = 0;

    CHECK_PTR(OCI_IPC_FILE, file);

#ifdef OCI_LOB2_API_ENABLED

    if (file->con->env->use_lob_ub8)
    {
        CHECK_OCI
        (
            file->con->err,
            OCILobGetLength2,
            file->con->handle, file->con->err,
            file->handle, (ub8 *) &size
        );
    }
    else

#endif

    {
        ub4 size32 = (ub4) size;

        CHECK_OCI
        (
            file->con->err,
            OCILobGetLength,
            file->con->handle, file->con->err,
            file->handle, &size32
        );

        size = (OCI_UInt64) size32;
    }

    SET_RETVAL(size);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileExists
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileExists
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    );

    CHECK_PTR(OCI_IPC_FILE, file);

    OCI_Boolean exists = FALSE;

    CHECK_OCI
    (
        file->con->err,
        OCILobFileExists, file->con->handle,
        file->con->err, file->handle,
        (boolean*) &exists
    );

    SET_RETVAL(exists);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileSetName
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileSetName
(
    OCI_File    *file,
    const otext *dir,
    const otext *name
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    );

    dbtext   *dbstr1  = NULL;
    dbtext   *dbstr2  = NULL;
    OCI_Int32 dbsize1 = -1;
    OCI_Int32 dbsize2 = -1;

    CHECK_PTR(OCI_IPC_FILE, file);

    dbstr1 = OcilibStringGetDBString(file->con->env, dir,  &dbsize1);
    dbstr2 = OcilibStringGetDBString(file->con->env, name, &dbsize2);

    CHECK_OCI
    (
        file->con->err,
        OCILobFileSetName,
        file->con->env->handle,
        file->con->err,
        &file->handle,
        (OraText *) dbstr1, (ub2) dbsize1,
        (OraText *) dbstr2, (ub2) dbsize2
    );

    CHECK(OcilibFileGetInfo(file));

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(file->con->env, dbstr1);
        OcilibStringReleaseDBString(file->con->env, dbstr2);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileGetDirectory
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibFileGetDirectory
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_FILE, file
    );

    CHECK_PTR(OCI_IPC_FILE, file);

    if (!IS_STRING_VALID(file->dir))
    {
        CHECK(OcilibFileGetInfo(file));
    }

    SET_RETVAL(file->dir);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileGetName
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibFileGetName
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ const otext *, NULL,
        /* context */ OCI_IPC_FILE, file
    );

    CHECK_PTR(OCI_IPC_FILE, file);

    if (!IS_STRING_VALID(file->name))
    {
        CHECK(OcilibFileGetInfo(file));
    }

    SET_RETVAL(file->name);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileOpen
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileOpen
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    );

    CHECK_PTR(OCI_IPC_FILE, file);

    CHECK_OCI
    (
        file->con->err,
        OCILobFileOpen,
        file->con->handle, file->con->err,
        file->handle, (ub1) OCI_LOB_READONLY
    );

    file->con->nb_files++;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileIsOpen
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileIsOpen
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    );

    CHECK_PTR(OCI_IPC_FILE, file);

    OCI_Boolean is_opened = FALSE;

    CHECK_OCI
    (
        file->con->err,
        OCILobFileIsOpen, file->con->handle,
        file->con->err, file->handle,
        (boolean*) &is_opened
    );

    SET_RETVAL(is_opened);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileClose
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileClose
(
    OCI_File *file
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    );

    CHECK_PTR(OCI_IPC_FILE, file);

    CHECK_OCI
    (
        file->con->err,
        OCILobFileClose,
        file->con->handle, file->con->err,
        file->handle
    );

    file->con->nb_files--;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileIsEqual
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileIsEqual
(
    OCI_File *file,
    OCI_File *file2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    );

    CHECK_PTR(OCI_IPC_FILE, file);
    CHECK_PTR(OCI_IPC_FILE, file2);

    OCI_Boolean is_equal = FALSE;

    CHECK_OCI
    (
        file->con->err,
        OCILobIsEqual,
        file->con->env->handle, file->handle,
        file2->handle, (boolean*) &is_equal
    );

    SET_RETVAL(is_equal);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibFileAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibFileAssign
(
    OCI_File *file,
    OCI_File *file_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_FILE, file
    );

    CHECK_PTR(OCI_IPC_FILE, file);
    CHECK_PTR(OCI_IPC_FILE, file_src);

    if ((OCI_OBJECT_ALLOCATED == file->hstate) || (OCI_OBJECT_ALLOCATED_ARRAY == file->hstate))
    {
        CHECK_OCI
        (
            file->con->err,
            OCILobLocatorAssign,
            file->con->handle, file->con->err,
            file_src->handle, &file->handle
        );
    }
    else
    {
        CHECK_OCI
        (
            file->con->err,
            OCILobAssign,
            file->con->env->handle, file->con->err,
            file_src->handle, &file->handle
        );
    }

    CHECK(OcilibFileGetInfo(file));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
* OcilibFileGetConnection
* --------------------------------------------------------------------------------------------- */

OCI_Connection * OcilibFileGetConnection
(
    OCI_File *file
)
{
    GET_PROP
    (
        /* result */ OCI_Connection *, NULL,
        /* handle */ OCI_IPC_FILE, file,
        /* member */ con
    );
}
