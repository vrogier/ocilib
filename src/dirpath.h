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

#ifndef OCILIB_DIRPATH_H_INCLUDED
#define OCILIB_DIRPATH_H_INCLUDED

#include "types.h"

OCI_DirPath * OcilibDirPathCreate
(
    OCI_TypeInfo *typinf,
    const otext  *partition,
    unsigned int  nb_cols,
    unsigned int  nb_rows
);

boolean OcilibDirPathFree
(
    OCI_DirPath *dp
);

boolean OcilibDirPathSetColumn
(
    OCI_DirPath *dp,
    unsigned int index,
    const otext *name,
    unsigned int maxsize,
    const otext *format
);

boolean OcilibDirPathPrepare
(
    OCI_DirPath *dp
);

boolean OcilibDirPathSetEntry
(
    OCI_DirPath *dp,
    unsigned int row,
    unsigned int index,
    void        *value,
    unsigned int size,
    boolean      complete
);

boolean OcilibDirPathReset
(
    OCI_DirPath *dp
);

unsigned int OcilibDirPathConvert
(
    OCI_DirPath *dp
);

unsigned int OcilibDirPathLoad
(
    OCI_DirPath *dp
);

boolean OcilibDirPathFinish
(
    OCI_DirPath *dp
);

boolean OcilibDirPathAbort
(
    OCI_DirPath *dp
);

boolean OcilibDirPathSave
(
    OCI_DirPath *dp
);

boolean OcilibDirPathFlushRow
(
    OCI_DirPath *dp
);

boolean OcilibDirPathSetCurrentRows
(
    OCI_DirPath *dp,
    unsigned int nb_rows
);

unsigned int OcilibDirPathGetCurrentRows
(
    OCI_DirPath *dp
);

unsigned int OcilibDirPathGetMaxRows
(
    OCI_DirPath* dp
);

boolean OcilibDirPathSetDateFormat
(
    OCI_DirPath *dp,
    const otext *format
);

boolean OcilibDirPathSetParallel
(
    OCI_DirPath *dp,
    boolean      value
);

boolean OcilibDirPathSetNoLog
(
    OCI_DirPath *dp,
    boolean      value
);

boolean OcilibDirPathSetCacheSize
(
    OCI_DirPath *dp,
    unsigned int size
);

boolean OcilibDirPathSetBufferSize
(
    OCI_DirPath *dp,
    unsigned int size
);

boolean OcilibDirPathSetConvertMode
(
    OCI_DirPath *dp,
    unsigned int mode
);

unsigned int OcilibDirPathGetRowCount
(
    OCI_DirPath *dp
);

unsigned int OcilibDirPathGetAffectedRows
(
    OCI_DirPath *dp
);

unsigned int OcilibDirPathGetErrorColumn
(
    OCI_DirPath *dp
);

unsigned int OcilibDirPathGetErrorRow
(
    OCI_DirPath *dp
);

#endif /* OCILIB_DIRPATH_H_INCLUDED */
