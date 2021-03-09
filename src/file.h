/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2021 Vincent ROGIER <vince.rogier@ocilib.net>
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

OCI_SYM_LOCAL boolean OcilibFileGetInfo
(
    OCI_File* file
);

OCI_SYM_LOCAL OCI_File * OcilibFileCreate
(
    OCI_Connection *con,
    unsigned int    type
);

OCI_SYM_LOCAL boolean OcilibFileFree
(
    OCI_File *file
);

OCI_SYM_LOCAL OCI_File ** OcilibFileCreateArray
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

OCI_SYM_LOCAL boolean OcilibFileFreeArray
(
    OCI_File **files
);

OCI_SYM_LOCAL boolean OcilibFileSeek
(
    OCI_File    *file,
    big_uint     offset,
    unsigned int mode
);

OCI_SYM_LOCAL big_uint OcilibFileGetOffset
(
    OCI_File *file
);

OCI_SYM_LOCAL unsigned int OcilibFileRead
(
    OCI_File    *file,
    void        *buffer,
    unsigned int len
);

OCI_SYM_LOCAL unsigned int OcilibFileGetType
(
    OCI_File *file
);

OCI_SYM_LOCAL big_uint OcilibFileGetSize
(
    OCI_File *file
);

OCI_SYM_LOCAL boolean OcilibFileExists
(
    OCI_File *file
);

OCI_SYM_LOCAL boolean OcilibFileSetName
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

OCI_SYM_LOCAL boolean OcilibFileOpen
(
    OCI_File *file
);

OCI_SYM_LOCAL boolean OcilibFileIsOpen
(
    OCI_File *file
);

OCI_SYM_LOCAL boolean OcilibFileClose
(
    OCI_File *file
);

OCI_SYM_LOCAL boolean OcilibFileIsEqual
(
    OCI_File *file,
    OCI_File *file2
);

OCI_SYM_LOCAL boolean OcilibFileAssign
(
    OCI_File *file,
    OCI_File *file_src
);

OCI_SYM_LOCAL OCI_Connection* OcilibFileGetConnection
(
    OCI_File* file
);

#endif /* OCILIB_FILE_H_INCLUDED */
