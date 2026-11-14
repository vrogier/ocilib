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

#ifndef OCILIB_SODADOC_H_INCLUDED
#define OCILIB_SODADOC_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_SodaDocument * OcilibSodaDocumentInitialize
(
    OCI_SodaDatabase * sdb,
    OCI_SodaDocument * doc,
    OCISodaDoc        *handle,
    const void        *content,
    OCI_UInt32         contentSize,
    const otext       *key,
    ub4                docFlags
);

OCI_SYM_LOCAL OCI_SodaDocument * OcilibSodaDocumentCreateFromText
(
    OCI_SodaDatabase *sdb,
    const otext      *content,
    OCI_UInt32        contentSize,
    const otext      *key
);

OCI_SYM_LOCAL OCI_SodaDocument * OcilibSodaDocumentCreateFromJson
(
    OCI_SodaDatabase *sdb,
    OCI_Json         *json,
    const otext      *key
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaDocumentRefreshMetadata
(
    OCI_SodaDocument *doc,
    OCISodaDoc       *reshandle
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaDocumentFree
(
    OCI_SodaDocument *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocumentGetKey
(
    OCI_SodaDocument *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocumentGetVersion
(
    OCI_SodaDocument *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocumentGetCreatedOn
(
    OCI_SodaDocument *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocumentGetLastModified
(
    OCI_SodaDocument *doc
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaDocumentGetIsJson
(
    OCI_SodaDocument *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocumentGetMediaType
(
    OCI_SodaDocument *doc
);

OCI_SYM_LOCAL const otext * OcilibSodaDocumentGetText
(
    OCI_SodaDocument *doc
);

OCI_SYM_LOCAL OCI_Json * OcilibSodaDocumentGetJson
(
    OCI_SodaDocument *doc
);

#endif /* OCILIB_SODADOC_H_INCLUDED */
