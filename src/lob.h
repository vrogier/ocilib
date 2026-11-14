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

#ifndef OCILIB_LOB_H_INCLUDED
#define OCILIB_LOB_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Lob * OcilibLobInitialize
(
    OCI_Connection *con,
    OCI_Lob        *lob,
    OCILobLocator  *handle,
    ub4             type
);

OCI_SYM_LOCAL OCI_Lob * OcilibLobCreate
(
    OCI_Connection *con,
    OCI_LobType     type
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobFree
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_Lob ** OcilibLobCreateArray
(
    OCI_Connection *con,
    OCI_LobType     type,
    OCI_UInt32      nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobFreeArray
(
    OCI_Lob **lobs
);

OCI_SYM_LOCAL OCI_LobType OcilibLobGetType
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobSeek
(
    OCI_Lob     *lob,
    OCI_UInt64   offset,
    OCI_SeekMode mode
);

OCI_SYM_LOCAL OCI_UInt64 OcilibLobGetOffset
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobRead
(
    OCI_Lob    *lob,
    void       *buffer,
    OCI_UInt32 *char_count,
    OCI_UInt32 *byte_count
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobWrite
(
    OCI_Lob    *lob,
    void       *buffer,
    OCI_UInt32 *char_count,
    OCI_UInt32 *byte_count
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobTruncate
(
    OCI_Lob   *lob,
    OCI_UInt64 size
);

OCI_SYM_LOCAL OCI_UInt64 OcilibLobErase
(
    OCI_Lob   *lob,
    OCI_UInt64 offset,
    OCI_UInt64 size
);

OCI_SYM_LOCAL OCI_UInt64 OcilibLobGetLength
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_UInt32 OcilibLobGetChunkSize
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobCopy
(
    OCI_Lob   *lob,
    OCI_Lob   *lob_src,
    OCI_UInt64 offset_dst,
    OCI_UInt64 offset_src,
    OCI_UInt64 count
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobCopyFromFile
(
    OCI_Lob   *lob,
    OCI_File  *file,
    OCI_UInt64 offset_dst,
    OCI_UInt64 offset_src,
    OCI_UInt64 count
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobAppend
(
    OCI_Lob    *lob,
    void       *buffer,
    OCI_UInt32 *char_count,
    OCI_UInt32 *byte_count
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobAppendLob
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobIsTemporary
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobOpen
(
    OCI_Lob    *lob,
    OCI_LobMode mode
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobClose
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobIsEqual
(
    OCI_Lob *lob,
    OCI_Lob *lob2
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobAssign
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
);

OCI_SYM_LOCAL OCI_UInt64 OcilibLobGetMaxSize
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobFlush
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobEnableBuffering
(
    OCI_Lob    *lob,
    OCI_Boolean value
);

OCI_SYM_LOCAL OCI_Connection * OcilibLobGetConnection
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_Boolean OcilibLobIsRemote
(
    OCI_Lob* lob
);

#endif /* OCILIB_LOB_H_INCLUDED */
