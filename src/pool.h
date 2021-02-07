/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2021 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_POOL_H_INCLUDED
#define OCILIB_POOL_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL boolean OcilibPoolDispose
(
    OCI_Pool *pool
);

OCI_SYM_LOCAL OCI_Pool * OcilibPoolCreate
(
    const otext *db,
    const otext *user,
    const otext *pwd,
    unsigned int type,
    unsigned int mode,
    unsigned int min_con,
    unsigned int max_con,
    unsigned int incr_con
);

OCI_SYM_LOCAL boolean OcilibPoolFree
(
    OCI_Pool *pool
);

OCI_SYM_LOCAL OCI_Connection * OcilibPoolGetConnection
(
    OCI_Pool    *pool,
    const otext *tag
);

OCI_SYM_LOCAL unsigned int OcilibPoolGetTimeout
(
    OCI_Pool *pool
);

OCI_SYM_LOCAL boolean OcilibPoolSetTimeout
(
    OCI_Pool    *pool,
    unsigned int value
);

OCI_SYM_LOCAL boolean OcilibPoolGetNoWait
(
    OCI_Pool *pool
);

OCI_SYM_LOCAL boolean OcilibPoolSetNoWait
(
    OCI_Pool *pool,
    boolean   value
);

OCI_SYM_LOCAL unsigned int OcilibPoolGetBusyCount
(
    OCI_Pool *pool
);

OCI_SYM_LOCAL unsigned int OcilibPoolGetOpenedCount
(
    OCI_Pool *pool
);

OCI_SYM_LOCAL unsigned int OcilibPoolGetMin
(
    OCI_Pool *pool
);

OCI_SYM_LOCAL unsigned int OcilibPoolGetMax
(
    OCI_Pool *pool
);

OCI_SYM_LOCAL unsigned int OcilibPoolGetIncrement
(
    OCI_Pool *pool
);

OCI_SYM_LOCAL boolean OcilibPoolSetStatementCacheSize
(
    OCI_Pool    *pool,
    unsigned int value
);

OCI_SYM_LOCAL unsigned int OcilibPoolGetStatementCacheSize
(
    OCI_Pool *pool
);

#endif /* OCILIB_POOL_H_INCLUDED */
