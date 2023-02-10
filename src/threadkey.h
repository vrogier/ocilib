/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_THREADKEY_H_INCLUDED
#define OCILIB_THREADKEY_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_ThreadKey * OcilibThreadKeyCreateInternal
(
    POCI_THREADKEYDEST destfunc
);

OCI_SYM_LOCAL boolean OcilibThreadKeyCreate
(
    const otext      * name,
    POCI_THREADKEYDEST destfunc
);

OCI_SYM_LOCAL boolean OcilibThreadKeyFree
(
    OCI_ThreadKey* key
);

OCI_SYM_LOCAL boolean OcilibThreadKeySet
(
    OCI_ThreadKey* key,
    void         * value
);

OCI_SYM_LOCAL boolean OcilibThreadKeyGet
(
    OCI_ThreadKey* key,
    void        ** value
);

OCI_SYM_LOCAL boolean OcilibThreadKeySetValue
(
    const otext *name,
    void        *value
);

OCI_SYM_LOCAL void * OcilibThreadKeyGetValue
(
    const otext *name
);

#endif /* OCILIB_THREADKEY_H_INCLUDED */
