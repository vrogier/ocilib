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

/* IMPORTANT NOTICE
 *
 * This file contains explanations about Oracle and OCI technologies. 
 * OCILIB is a wrapper around OCI and thus exposes OCI features. 
 * The OCILIB documentation intends to explain Oracle / OCI concepts
 * and is naturally based on the official Oracle OCI documentation. 
 * 
 * Some parts of OCILIB documentation may include some information 
 * taken and adapted from the following Oracle documentations :
 *  - Oracle Call Interface Programmer's Guide
 *  - Oracle Streams - Advanced Queuing User's Guide
 */

#ifndef OCILIB_H_INCLUDED
#define OCILIB_H_INCLUDED

#ifdef __cplusplus
extern "C" {
#endif  /* __cplusplus */

/**
 * @defgroup OcilibCApi C API
 * @{
 *
 */

/**
* @defgroup OcilibCApiSupportedCharsets Character sets
* @defgroup OcilibCApiDatatypes Data types
* @defgroup OcilibCApiEnvironmentVariables Environment Variables
*/

#include "ocilibc/platform.h"
#include "ocilibc/defines.h"
#include "ocilibc/types.h"
#include "ocilibc/api.h"
#include "ocilibc/compat.h"

#ifdef  __cplusplus
}
#endif

/**
 * @} OcilibCApi
 */


#endif  /* OCILIB_H_INCLUDED */
