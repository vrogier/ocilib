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

#ifndef OCILIB_DIRPATH_H_INCLUDED
#define OCILIB_DIRPATH_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_DirPath * OcilibDirPathCreate
(
    OCI_TypeInfo *typinf,
    const otext  *partition,
    OCI_UInt32    nb_cols,
    OCI_UInt32    nb_rows
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathFree
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathSetColumn
(
    OCI_DirPath *dp,
    OCI_UInt32   index,
    const otext *name,
    OCI_UInt32   maxsize,
    const otext *format
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathPrepare
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathSetEntry
(
    OCI_DirPath *dp,
    OCI_UInt32   row,
    OCI_UInt32   index,
    const void  *value,
    OCI_UInt32   size,
    OCI_Boolean  complete
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathReset
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_DirPathResult OcilibDirPathConvert
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_DirPathResult OcilibDirPathLoad
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathFinish
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathAbort
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathSave
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathFlushRow
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathSetCurrentRows
(
    OCI_DirPath *dp,
    OCI_UInt32   nb_rows
);

OCI_SYM_LOCAL OCI_UInt32 OcilibDirPathGetCurrentRows
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_UInt32 OcilibDirPathGetMaxRows
(
    OCI_DirPath* dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathSetDateFormat
(
    OCI_DirPath *dp,
    const otext *format
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathSetParallel
(
    OCI_DirPath *dp,
    OCI_Boolean  value
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathSetNoLog
(
    OCI_DirPath *dp,
    OCI_Boolean  value
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathSetCacheSize
(
    OCI_DirPath *dp,
    OCI_UInt32   size
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathSetBufferSize
(
    OCI_DirPath *dp,
    OCI_UInt32   size
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirPathSetConvertMode
(
    OCI_DirPath           *dp,
    OCI_DirPathConvertMode mode
);

OCI_SYM_LOCAL OCI_UInt32 OcilibDirPathGetRowCount
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_UInt32 OcilibDirPathGetAffectedRows
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_UInt32 OcilibDirPathGetErrorColumn
(
    OCI_DirPath *dp
);

OCI_SYM_LOCAL OCI_UInt32 OcilibDirPathGetErrorRow
(
    OCI_DirPath *dp
);

#endif /* OCILIB_DIRPATH_H_INCLUDED */
