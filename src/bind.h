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

#ifndef OCILIB_BIND_H_INCLUDED
#define OCILIB_BIND_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Bind* OcilibBindCreate
(
    OCI_Statement* stmt,
    void         * data,
    const otext  * name,
    OCI_UInt32   mode,
    ub4            size,
    ub1            type,
    OCI_SqlType    code,
    OCI_SubType    subtype,
    OCI_TypeInfo * typinf,
    OCI_UInt32   nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindFree
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL OCI_Int32 OcilibBindGetIndex
(
    OCI_Statement* stmt,
    const otext  * name
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindSetNullIndicator
(
    OCI_Bind   * bnd,
    OCI_UInt32 position,
    sb2          value
);

OCI_SYM_LOCAL const otext* OcilibBindGetName
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL OCI_SqlType OcilibBindGetType
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL OCI_SubType OcilibBindGetSubtype
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL OCI_UInt32 OcilibBindGetDataCount
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

OCI_SYM_LOCAL OCI_Boolean OcilibBindSetDataSize
(
    OCI_Bind   * bnd,
    OCI_UInt32 size
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindSetDataSizeAtPos
(
    OCI_Bind   * bnd,
    OCI_UInt32 position,
    OCI_UInt32 size
);

OCI_SYM_LOCAL OCI_UInt32 OcilibBindGetDataSize
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL OCI_UInt32 OcilibBindGetDataSizeAtPos
(
    OCI_Bind   * bnd,
    OCI_UInt32 position
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindSetNullAtPos
(
    OCI_Bind   * bnd,
    OCI_UInt32 position
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindSetNull
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindSetNotNullAtPos
(
    OCI_Bind   * bnd,
    OCI_UInt32 position
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindSetNotNull
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindIsNullAtPos
(
    OCI_Bind   * bnd,
    OCI_UInt32 position
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindIsNull
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindSetCharsetForm
(
    OCI_Bind       *bnd,
    OCI_CharsetForm csfrm
);

OCI_SYM_LOCAL OCI_Boolean OcilibBindSetDirection
(
    OCI_Bind         *bnd,
    OCI_BindDirection direction
);

OCI_SYM_LOCAL OCI_BindDirection OcilibBindGetDirection
(
    OCI_Bind* bnd
);

OCI_SYM_LOCAL OCI_BindAllocationMode OcilibBindGetAllocationMode
(
    OCI_Bind* bnd
);

#endif /* OCILIB_BIND_H_INCLUDED */
