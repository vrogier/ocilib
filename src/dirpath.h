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

OCI_DirPath * DirPathCreate
(
    OCI_TypeInfo *typinf,
    const otext  *partition,
    unsigned int  nb_cols,
    unsigned int  nb_rows
);

boolean DirPathFree
(
    OCI_DirPath *dp
);

boolean DirPathSetColumn
(
    OCI_DirPath *dp,
    unsigned int index,
    const otext *name,
    unsigned int maxsize,
    const otext *format
);

boolean DirPathPrepare
(
    OCI_DirPath *dp
);

boolean DirPathSetEntry
(
    OCI_DirPath *dp,
    unsigned int row,
    unsigned int index,
    void        *value,
    unsigned int size,
    boolean      complete
);

boolean DirPathReset
(
    OCI_DirPath *dp
);

unsigned int DirPathConvert
(
    OCI_DirPath *dp
);

unsigned int DirPathLoad
(
    OCI_DirPath *dp
);

boolean DirPathFinish
(
    OCI_DirPath *dp
);

boolean DirPathAbort
(
    OCI_DirPath *dp
);

boolean DirPathSave
(
    OCI_DirPath *dp
);

boolean DirPathFlushRow
(
    OCI_DirPath *dp
);

boolean DirPathSetCurrentRows
(
    OCI_DirPath *dp,
    unsigned int nb_rows
);

unsigned int DirPathGetCurrentRows
(
    OCI_DirPath *dp
);

unsigned int DirPathGetMaxRows
(
    OCI_DirPath* dp
);

boolean DirPathSetDateFormat
(
    OCI_DirPath *dp,
    const otext *format
);

boolean DirPathSetParallel
(
    OCI_DirPath *dp,
    boolean      value
);

boolean DirPathSetNoLog
(
    OCI_DirPath *dp,
    boolean      value
);

boolean DirPathSetCacheSize
(
    OCI_DirPath *dp,
    unsigned int size
);

boolean DirPathSetBufferSize
(
    OCI_DirPath *dp,
    unsigned int size
);

boolean DirPathSetConvertMode
(
    OCI_DirPath *dp,
    unsigned int mode
);

unsigned int DirPathGetRowCount
(
    OCI_DirPath *dp
);

unsigned int DirPathGetAffectedRows
(
    OCI_DirPath *dp
);

unsigned int DirPathGetErrorColumn
(
    OCI_DirPath *dp
);

unsigned int DirPathGetErrorRow
(
    OCI_DirPath *dp
);

#endif /* OCILIB_DIRPATH_H_INCLUDED */
