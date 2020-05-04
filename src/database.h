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

#ifndef OCILIB_DATABASE_H_INCLUDED
#define OCILIB_DATABASE_H_INCLUDED

#include "types.h"

boolean DatabaseSetUserPassword
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    const otext* new_pwd
);

boolean DatabaseStartup
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int sess_mode,
    unsigned int start_mode,
    unsigned int start_flag,
    const otext* spfile
);

boolean DatabaseShutdown
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    unsigned int sess_mode,
    unsigned int shut_mode,
    unsigned int shut_flag
);

#endif /* OCILIB_DATABASE_H_INCLUDED */
