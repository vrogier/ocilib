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

#ifndef OCILIB_POOL_H_INCLUDED
#define OCILIB_POOL_H_INCLUDED

#include "types.h"

boolean PoolDispose
(
    OCI_Pool *pool
);

OCI_Pool * PoolCreate
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

boolean PoolFree
(
    OCI_Pool *pool
);

OCI_Connection * PoolGetConnection
(
    OCI_Pool    *pool,
    const otext *tag
);

unsigned int PoolGetTimeout
(
    OCI_Pool *pool
);

boolean PoolSetTimeout
(
    OCI_Pool    *pool,
    unsigned int value
);

boolean PoolGetNoWait
(
    OCI_Pool *pool
);

boolean PoolSetNoWait
(
    OCI_Pool *pool,
    boolean   value
);

unsigned int PoolGetBusyCount
(
    OCI_Pool *pool
);

unsigned int PoolGetOpenedCount
(
    OCI_Pool *pool
);

unsigned int PoolGetMin
(
    OCI_Pool *pool
);

unsigned int PoolGetMax
(
    OCI_Pool *pool
);

unsigned int PoolGetIncrement
(
    OCI_Pool *pool
);

boolean PoolSetStatementCacheSize
(
    OCI_Pool    *pool,
    unsigned int value
);

unsigned int PoolGetStatementCacheSize
(
    OCI_Pool *pool
);

#endif /* OCILIB_POOL_H_INCLUDED */
