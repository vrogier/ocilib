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

#ifndef OCILIB_BIND_H_INCLUDED
#define OCILIB_BIND_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Bind* OcilibBindCreate
(
    OCI_Statement* stmt,
    void         * data,
    const otext  * name,
    unsigned int   mode,
    ub4            size,
    ub1            type,
    unsigned int   code,
    unsigned int   subtype,
    OCI_TypeInfo * typinf,
    unsigned int   nbelem
);

OCI_SYM_LOCAL boolean OcilibBindFree
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL int OcilibBindGetIndex
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL boolean OcilibBindSetNullIndicator
(
    OCI_Bind   * bnd,
    unsigned int position,
    sb2          value
);

OCI_SYM_LOCAL const otext* OcilibBindGetName
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL unsigned int OcilibBindGetType
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL unsigned int OcilibBindGetSubtype
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL unsigned int OcilibBindGetDataCount
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL void* OcilibBindGetData
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL OCI_Statement* OcilibBindGetStatement
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL boolean OcilibBindSetDataSize
(
    OCI_Bind   * bnd,
    unsigned int size
);

OCI_SYM_LOCAL boolean OcilibBindSetDataSizeAtPos
(
    OCI_Bind   * bnd,
    unsigned int position,
    unsigned int size
);

OCI_SYM_LOCAL unsigned int OcilibBindGetDataSize
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL unsigned int OcilibBindGetDataSizeAtPos
(
    OCI_Bind   * bnd,
    unsigned int position
);

OCI_SYM_LOCAL boolean OcilibBindSetNullAtPos
(
    OCI_Bind   * bnd,
    unsigned int position
);

OCI_SYM_LOCAL boolean OcilibBindSetNull
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL boolean OcilibBindSetNotNullAtPos
(
    OCI_Bind   * bnd,
    unsigned int position
);

OCI_SYM_LOCAL boolean OcilibBindSetNotNull
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL boolean OcilibBindIsNullAtPos
(
    OCI_Bind   * bnd,
    unsigned int position
);

OCI_SYM_LOCAL boolean OcilibBindIsNull
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL boolean OcilibBindSetCharsetForm
(
    OCI_Bind    *bnd,
    unsigned int csfrm
);

OCI_SYM_LOCAL boolean OcilibBindSetDirection
(
    OCI_Bind    *bnd,
    unsigned int direction
);

OCI_SYM_LOCAL unsigned int OcilibBindGetDirection
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL unsigned int OcilibBindGetAllocationMode
(
    OCI_Bind* bnd
);

#endif /* OCILIB_BIND_H_INCLUDED */
