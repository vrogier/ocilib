/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2024 Vincent ROGIER <vince.rogier@ocilib.net>
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

OCI_SYM_LOCAL boolean OcilibArrayInitialize
(
    OCI_Array    *arr,
    OCI_TypeInfo *typinf
);

OCI_SYM_LOCAL boolean OcilibArrayDispose
(
    OCI_Array* arr
);

OCI_SYM_LOCAL OCI_Array* OcilibArrayCreate
(
    OCI_Connection* con,
    unsigned int    nb_elem,
    unsigned int    elem_type,
    unsigned int    elem_subtype,
    unsigned int    elem_size,
    unsigned int    struct_size,
    unsigned int    handle_type,
    OCI_TypeInfo  * typinf
);

OCI_SYM_LOCAL boolean OcilibArrayFreeFromHandles
(
    void** handles
);

#endif /* OCILIB_ARRAY_H_INCLUDED */
