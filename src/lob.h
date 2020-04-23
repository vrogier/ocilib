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

OCI_Lob * LobInit
(
    OCI_Connection    *con,
    OCI_Lob           *lob,
    OCILobLocator *handle,
    ub4            type
);

OCI_Lob * LobCreate
(
    OCI_Connection  *con,
    unsigned int type
);

boolean LobFree
(
    OCI_Lob     *lob
);

OCI_Lob ** LobArrayCreate
(
    OCI_Connection  *con,
    unsigned int type,
    unsigned int nbelem
);

boolean LobArrayFree
(
    OCI_Lob    **lobs
);

unsigned int LobGetType
(
    OCI_Lob     *lob
);

boolean LobSeek
(
    OCI_Lob         *lob,
    big_uint     offset,
    unsigned int mode
);

big_uint LobGetOffset
(
    OCI_Lob     *lob
);

boolean LobRead2
(
    OCI_Lob          *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

unsigned int LobRead
(
    OCI_Lob         *lob,
    void        *buffer,
    unsigned int len
);

boolean LobWrite2
(
    OCI_Lob          *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

unsigned int LobWrite
(
    OCI_Lob         *lob,
    void        *buffer,
    unsigned int len
);

boolean LobTruncate
(
    OCI_Lob     *lob,
    big_uint size
);

big_uint LobErase
(
    OCI_Lob     *lob,
    big_uint offset,
    big_uint size
);

big_uint LobGetLength
(
    OCI_Lob     *lob
);

unsigned int LobGetChunkSize
(
    OCI_Lob     *lob
);

boolean LobCopy
(
    OCI_Lob     *lob,
    OCI_Lob     *lob_src,
    big_uint offset_dst,
    big_uint offset_src,
    big_uint count
);

boolean LobCopyFromFile
(
    OCI_Lob     *lob,
    OCI_File    *file,
    big_uint offset_dst,
    big_uint offset_src,
    big_uint count
);

boolean LobAppend2
(
    OCI_Lob          *lob,
    void         *buffer,
    unsigned int *char_count,
    unsigned int *byte_count
);

unsigned int LobAppend
(
    OCI_Lob         *lob,
    void        *buffer,
    unsigned int len
);

boolean LobAppendLob
(
    OCI_Lob     *lob,
    OCI_Lob     *lob_src
);

boolean LobIsTemporary
(
    OCI_Lob     *lob
);

boolean LobOpen
(
    OCI_Lob *lob,
    unsigned int mode
);

boolean LobClose
(
    OCI_Lob     *lob
);

boolean LobIsEqual
(
    OCI_Lob     *lob,
    OCI_Lob     *lob2
);

boolean LobAssign
(
    OCI_Lob     *lob,
    OCI_Lob     *lob_src
);

big_uint LobGetMaxSize
(
    OCI_Lob     *lob
);

boolean LobFlush
(
    OCI_Lob     *lob
);

boolean LobEnableBuffering
(
    OCI_Lob     *lob,
    boolean  value
);

OCI_Connection * LobGetConnection
(
    OCI_Lob     *lob
);

boolean LobIsRemote
(
    OCI_Lob* lob
);

#endif /* OCILIB_LOB_H_INCLUDED */
