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
    unsigned int    type
);

OCI_SYM_LOCAL boolean OcilibLobFree
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL OCI_Lob ** OcilibLobCreateArray
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

OCI_SYM_LOCAL boolean OcilibLobFreeArray
(
    OCI_Lob **lobs
);

OCI_SYM_LOCAL unsigned int OcilibLobGetType
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL boolean OcilibLobSeek
(
    OCI_Lob     *lob,
    big_uint     offset,
    unsigned int mode
);

OCI_SYM_LOCAL big_uint OcilibLobGetOffset
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL boolean OcilibLobRead2
(
    OCI_Lob      *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

OCI_SYM_LOCAL unsigned int OcilibLobRead
(
    OCI_Lob     *lob,
    void        *buffer,
    unsigned int len
);

OCI_SYM_LOCAL boolean OcilibLobWrite2
(
    OCI_Lob      *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

OCI_SYM_LOCAL unsigned int OcilibLobWrite
(
    OCI_Lob     *lob,
    void        *buffer,
    unsigned int len
);

OCI_SYM_LOCAL boolean OcilibLobTruncate
(
    OCI_Lob *lob,
    big_uint size
);

OCI_SYM_LOCAL big_uint OcilibLobErase
(
    OCI_Lob *lob,
    big_uint offset,
    big_uint size
);

OCI_SYM_LOCAL big_uint OcilibLobGetLength
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL unsigned int OcilibLobGetChunkSize
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL boolean OcilibLobCopy
(
    OCI_Lob *lob,
    OCI_Lob *lob_src,
    big_uint offset_dst,
    big_uint offset_src,
    big_uint count
);

OCI_SYM_LOCAL boolean OcilibLobCopyFromFile
(
    OCI_Lob  *lob,
    OCI_File *file,
    big_uint  offset_dst,
    big_uint  offset_src,
    big_uint  count
);

OCI_SYM_LOCAL boolean OcilibLobAppend2
(
    OCI_Lob      *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

OCI_SYM_LOCAL unsigned int OcilibLobAppend
(
    OCI_Lob     *lob,
    void        *buffer,
    unsigned int len
);

OCI_SYM_LOCAL boolean OcilibLobAppendLob
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
);

OCI_SYM_LOCAL boolean OcilibLobIsTemporary
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL boolean OcilibLobOpen
(
    OCI_Lob     *lob,
    unsigned int mode
);

OCI_SYM_LOCAL boolean OcilibLobClose
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL boolean OcilibLobIsEqual
(
    OCI_Lob *lob,
    OCI_Lob *lob2
);

OCI_SYM_LOCAL boolean OcilibLobAssign
(
    OCI_Lob *lob,
    OCI_Lob *lob_src
);

OCI_SYM_LOCAL big_uint OcilibLobGetMaxSize
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL boolean OcilibLobFlush
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL boolean OcilibLobEnableBuffering
(
    OCI_Lob *lob,
    boolean  value
);

OCI_SYM_LOCAL OCI_Connection * OcilibLobGetConnection
(
    OCI_Lob *lob
);

OCI_SYM_LOCAL boolean OcilibLobIsRemote
(
    OCI_Lob* lob
);

#endif /* OCILIB_LOB_H_INCLUDED */
