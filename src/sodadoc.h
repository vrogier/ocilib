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

#ifndef OCILIB_SODADOC_H_INCLUDED
#define OCILIB_SODADOC_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_SodaDoc * OcilibSodaDocInitialize
(
    OCI_SodaDb * sdb,
    OCI_SodaDoc* doc,
    OCISodaDoc  *handle,
    const void  *content,
    OCI_UInt32   contentSize,
    const otext *key,
    ub4          docFlags
);

OCI_SYM_LOCAL OCI_SodaDoc * OcilibSodaDocCreateFromText
(
    OCI_SodaDb  *sdb,
    const otext *content,
    OCI_UInt32   contentSize,
    const otext *key
);

OCI_SYM_LOCAL OCI_SodaDoc * OcilibSodaDocCreateFromJson
(
    OCI_SodaDb  *sdb,
    OCI_Json    *json,
    const otext *key
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaDocRefreshMetadata
(
    OCI_SodaDoc *doc,
    OCISodaDoc  *reshandle
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaDocFree
(
    OCI_SodaDoc *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocGetKey
(
    OCI_SodaDoc *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocGetVersion
(
    OCI_SodaDoc *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocGetCreatedOn
(
    OCI_SodaDoc *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocGetLastModified
(
    OCI_SodaDoc *doc
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaDocGetIsJson
(
    OCI_SodaDoc *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocGetMediaType
(
    OCI_SodaDoc *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocGetText
(
    OCI_SodaDoc *doc
);

OCI_SYM_LOCAL OCI_Json * OcilibSodaDocGetJson
(
    OCI_SodaDoc *doc
);

#endif /* OCILIB_SODADOC_H_INCLUDED */
