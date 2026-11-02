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

#ifndef OCILIB_SODAOPTIONS_H_INCLUDED
#define OCILIB_SODAOPTIONS_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_SodaOptions * OcilibSodaOptionsCreate
(
    OCI_SodaDb *sdb
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaOptionsFree
(
    OCI_SodaOptions *options
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaOptionsSetKey
(
    OCI_SodaOptions *options,
    const otext     *key
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaOptionsSetKeys
(
    OCI_SodaOptions *options,
    const otext    **keys,
    OCI_UInt32     count
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaOptionsSetKeyAttr
(
    OCI_SodaOptions *options,
    const otext     *key
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaOptionsSetVersion
(
    OCI_SodaOptions *options,
    const otext     *version
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaOptionsSetFilter
(
    OCI_SodaOptions *options,
    const otext     *filterSpec
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaOptionsSetSkip
(
    OCI_SodaOptions *options,
    OCI_UInt32     count
);

OCI_SYM_LOCAL OCI_Boolean OcilibSodaOptionsSetLimit
(
    OCI_SodaOptions *options,
    OCI_UInt32     count
);

#endif /* OCILIB_SODAOPTIONS_H_INCLUDED */
