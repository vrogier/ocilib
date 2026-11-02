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

#ifndef OCILIB_ARRAY_H_INCLUDED
#define OCILIB_ARRAY_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Boolean OcilibArrayInitialize
(
    OCI_Array    *arr,
    OCI_TypeInfo *typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibArrayDispose
(
    OCI_Array* arr
);

OCI_SYM_LOCAL OCI_Array* OcilibArrayCreate
(
    OCI_Environment* env,
    OCI_Connection * con,
    OCI_UInt32       nb_elem,
    OCI_UInt32       elem_type,
    OCI_UInt32       elem_subtype,
    OCI_UInt32       elem_size,
    OCI_UInt32       struct_size,
    OCI_UInt32       handle_type,
    OCI_TypeInfo   * typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibArrayFreeFromHandles
(
    OCI_Environment* env,
    void          ** handles
);

#endif /* OCILIB_ARRAY_H_INCLUDED */
