/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

#ifndef OCILIB_FILE_H_INCLUDED
#define OCILIB_FILE_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_File * OcilibFileInitialize
(
    OCI_Connection *con,
    OCI_File       *file,
    OCILobLocator  *handle,
    ub4             type
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileGetInfo
(
    OCI_File* file
);

OCI_SYM_LOCAL OCI_File * OcilibFileCreate
(
    OCI_Connection *con,
    OCI_FileType    type
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileFree
(
    OCI_File *file
);

OCI_SYM_LOCAL OCI_File ** OcilibFileCreateArray
(
    OCI_Connection *con,
    OCI_FileType    type,
    OCI_UInt32      nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileFreeArray
(
    OCI_File **files
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileSeek
(
    OCI_File    *file,
    OCI_UInt64   offset,
    OCI_SeekMode mode
);

OCI_SYM_LOCAL OCI_UInt64 OcilibFileGetOffset
(
    OCI_File *file
);

OCI_SYM_LOCAL OCI_UInt32 OcilibFileRead
(
    OCI_File  *file,
    void      *buffer,
    OCI_UInt32 len
);

OCI_SYM_LOCAL OCI_FileType OcilibFileGetType
(
    OCI_File *file
);

OCI_SYM_LOCAL OCI_UInt64 OcilibFileGetSize
(
    OCI_File *file
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileExists
(
    OCI_File *file
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileSetName
(
    OCI_File    *file,
    const otext *dir,
    const otext *name
);

OCI_SYM_LOCAL const otext * OcilibFileGetDirectory
(
    OCI_File *file
);

OCI_SYM_LOCAL const otext * OcilibFileGetName
(
    OCI_File *file
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileOpen
(
    OCI_File *file
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileIsOpen
(
    OCI_File *file
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileClose
(
    OCI_File *file
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileIsEqual
(
    OCI_File *file,
    OCI_File *file2
);

OCI_SYM_LOCAL OCI_Boolean OcilibFileAssign
(
    OCI_File *file,
    OCI_File *file_src
);

OCI_SYM_LOCAL OCI_Connection* OcilibFileGetConnection
(
    OCI_File* file
);

#endif /* OCILIB_FILE_H_INCLUDED */
