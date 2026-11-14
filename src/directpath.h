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

#ifndef OCILIB_DIRPATH_H_INCLUDED
#define OCILIB_DIRPATH_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_DirectPath * OcilibDirectPathCreate
(
    OCI_TypeInfo *typinf,
    const otext  *partition,
    OCI_UInt32    nb_cols,
    OCI_UInt32    nb_rows
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathFree
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathSetColumn
(
    OCI_DirectPath *dp,
    OCI_UInt32      index,
    const otext    *name,
    OCI_UInt32      maxsize,
    const otext    *format
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathPrepare
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathSetEntry
(
    OCI_DirectPath *dp,
    OCI_UInt32      row,
    OCI_UInt32      index,
    const void     *value,
    OCI_UInt32      size,
    OCI_Boolean     complete
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathReset
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_DirPathResult OcilibDirectPathConvert
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_DirPathResult OcilibDirectPathLoad
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathFinish
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathAbort
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathSave
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathFlushRow
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathSetCurrentRows
(
    OCI_DirectPath *dp,
    OCI_UInt32      nb_rows
);

OCI_SYM_LOCAL OCI_UInt32 OcilibDirectPathGetCurrentRows
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_UInt32 OcilibDirectPathGetMaxRows
(
    OCI_DirectPath* dp
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathSetDateFormat
(
    OCI_DirectPath *dp,
    const otext    *format
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathSetParallel
(
    OCI_DirectPath *dp,
    OCI_Boolean     value
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathSetNoLog
(
    OCI_DirectPath *dp,
    OCI_Boolean     value
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathSetCacheSize
(
    OCI_DirectPath *dp,
    OCI_UInt32      size
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathSetBufferSize
(
    OCI_DirectPath *dp,
    OCI_UInt32      size
);

OCI_SYM_LOCAL OCI_Boolean OcilibDirectPathSetConvertMode
(
    OCI_DirectPath        *dp,
    OCI_DirPathConvertMode mode
);

OCI_SYM_LOCAL OCI_UInt32 OcilibDirectPathGetRowCount
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_UInt32 OcilibDirectPathGetAffectedRows
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_Int32 OcilibDirectPathGetErrorColumn
(
    OCI_DirectPath *dp
);

OCI_SYM_LOCAL OCI_Int32 OcilibDirectPathGetErrorRow
(
    OCI_DirectPath *dp
);

#endif /* OCILIB_DIRPATH_H_INCLUDED */
