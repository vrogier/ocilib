/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2022 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_DATABASE_H_INCLUDED
#define OCILIB_DATABASE_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL boolean OcilibDatabaseSetUserPassword
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    const otext* new_pwd
);

OCI_SYM_LOCAL boolean OcilibDatabaseStartup
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int sess_mode,
    unsigned int start_mode,
    unsigned int start_flag,
    const otext* spfile
);

OCI_SYM_LOCAL boolean OcilibDatabaseShutdown
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int sess_mode,
    unsigned int shut_mode,
    unsigned int shut_flag
);

#endif /* OCILIB_DATABASE_H_INCLUDED */
