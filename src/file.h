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
 
#ifndef OCILIB_FILE_H_INCLUDED
#define OCILIB_FILE_H_INCLUDED

#include "types.h"

OCI_File * FileInitialize
(
    OCI_Connection    *con,
    OCI_File          *file,
    OCILobLocator *handle,
    ub4            type
);

boolean FileGetInfo
(
    OCI_File* file
);

OCI_File * FileCreate
(
    OCI_Connection  *con,
    unsigned int type
);

boolean FileFree
(
    OCI_File    *file
);

OCI_File ** FileCreateArray
(
    OCI_Connection  *con,
    unsigned int type,
    unsigned int nbelem
);

boolean FileFreeArray
(
    OCI_File   **files
);

boolean FileSeek
(
    OCI_File        *file,
    big_uint     offset,
    unsigned int mode
);

big_uint FileGetOffset
(
    OCI_File    *file
);

unsigned int FileRead
(
    OCI_File        *file,
    void        *buffer,
    unsigned int len
);

unsigned int FileGetType
(
    OCI_File    *file
);

big_uint FileGetSize
(
    OCI_File    *file
);

boolean FileExists
(
    OCI_File    *file
);

boolean FileSetName
(
    OCI_File        *file,
    const otext *dir,
    const otext *name
);

const otext * FileGetDirectory
(
    OCI_File    *file
);

const otext * FileGetName
(
    OCI_File    *file
);

boolean FileOpen
(
    OCI_File    *file
);

boolean FileIsOpen
(
    OCI_File    *file
);

boolean FileClose
(
    OCI_File    *file
);

boolean FileIsEqual
(
    OCI_File    *file,
    OCI_File    *file2
);

boolean FileAssign
(
    OCI_File    *file,
    OCI_File    *file_src
);

OCI_Connection* FileGetConnection
(
    OCI_File* file
);

#endif /* OCILIB_FILE_H_INCLUDED */
