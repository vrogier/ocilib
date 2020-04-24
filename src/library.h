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
 
#ifndef OCILIB_LIBRARY_H_INCLUDED
#define OCILIB_LIBRARY_H_INCLUDED

#include "types.h"

unsigned int ExternalSubTypeToSQLType
(
    unsigned int type,
    unsigned int subtype
);

unsigned int ExternalSubTypeToHandleType
(
    unsigned int type,
    unsigned int subtype
);

boolean FreeObjectFromType
(
    void* obj, 
    unsigned int type
);

boolean KeyMapFree
(
    void
);

void CallEnter
(
    OCI_Context* ctx
);

void CallExit
(
    OCI_Context* ctx
);

char* GetEnvVariable
(
    const char* name
);

OCI_Error* EnvironmentGetLastError
(
    void
);

boolean Initialize
(
    POCI_ERROR   err_handler,
    const otext* lib_path,
    unsigned int mode
);

boolean Cleanup
(
    void
);

unsigned int GetOCICompileVersion
(
    void
);

unsigned int GetOCIRuntimeVersion
(
    void
);

unsigned int GetImportMode
(
    void
);

unsigned int GetCharset
(
    void
);

big_uint GetAllocatedBytes
(
    unsigned int mem_type
);

boolean EnableWarnings
(
    boolean value
);

boolean SetErrorHandler
(
    POCI_ERROR handler
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

boolean SetHAHandler
(
    POCI_HA_HANDLER  handler
);

boolean SetFormat
(
    OCI_Connection* con,
    unsigned int    type,
    const otext* format
);

const otext* GetFormat
(
    OCI_Connection* con,
    unsigned int    type
);

boolean SetUserPassword
(
    const otext* db,
    const otext* user,
    const otext* pwd,
    const otext* new_pwd
);


#endif /* OCILIB_LIBRARY_H_INCLUDED */
