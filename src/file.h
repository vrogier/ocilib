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

OCI_File * OcilibFileInitialize
(
    OCI_Connection *con,
    OCI_File       *file,
    OCILobLocator  *handle,
    ub4             type
);

boolean OcilibFileGetInfo
(
    OCI_File* file
);

OCI_File * OcilibFileCreate
(
    OCI_Connection *con,
    unsigned int    type
);

boolean OcilibFileFree
(
    OCI_File *file
);

OCI_File ** OcilibFileCreateArray
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

boolean OcilibFileFreeArray
(
    OCI_File **files
);

boolean OcilibFileSeek
(
    OCI_File    *file,
    big_uint     offset,
    unsigned int mode
);

big_uint OcilibFileGetOffset
(
    OCI_File *file
);

unsigned int OcilibFileRead
(
    OCI_File    *file,
    void        *buffer,
    unsigned int len
);

unsigned int OcilibFileGetType
(
    OCI_File *file
);

big_uint OcilibFileGetSize
(
    OCI_File *file
);

boolean OcilibFileExists
(
    OCI_File *file
);

boolean OcilibFileSetName
(
    OCI_File    *file,
    const otext *dir,
    const otext *name
);

const otext * OcilibFileGetDirectory
(
    OCI_File *file
);

const otext * OcilibFileGetName
(
    OCI_File *file
);

boolean OcilibFileOpen
(
    OCI_File *file
);

boolean OcilibFileIsOpen
(
    OCI_File *file
);

boolean OcilibFileClose
(
    OCI_File *file
);

boolean OcilibFileIsEqual
(
    OCI_File *file,
    OCI_File *file2
);

boolean OcilibFileAssign
(
    OCI_File *file,
    OCI_File *file_src
);

OCI_Connection* OcilibFileGetConnection
(
    OCI_File* file
);

#endif /* OCILIB_FILE_H_INCLUDED */
