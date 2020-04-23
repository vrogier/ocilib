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
 
#ifndef OCILIB_LIST_H_INCLUDED
#define OCILIB_LIST_H_INCLUDED

#include "types.h"


OCI_List* ListCreate
(
    int type
);

boolean ListFree
(
    OCI_List* list
);

void* ListAppend
(
    OCI_List* list,
    int       size
);

boolean ListClear
(
    OCI_List* list
);

typedef void (*POCI_LIST_FOR_EACH)(void* data);

boolean ListForEach
(
    OCI_List* list,
    POCI_LIST_FOR_EACH proc
);

typedef void(*POCI_LIST_FOR_EACH_WITH_PARAM)(void* data, void* param);

boolean ListForEachWithParam
(
    OCI_List* list,
    void* param,
    POCI_LIST_FOR_EACH_WITH_PARAM proc
);

boolean ListRemove
(
    OCI_List* list,
    void* data
);

boolean ListExists
(
    OCI_List* list,
    void* data
);

typedef boolean(*POCI_LIST_FIND)(void* data, void* param);

void* ListFind
(
    OCI_List* list,
    POCI_LIST_FIND   proc,
    void* param
);


#endif /* OCILIB_LIST_H_INCLUDED */
