/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_VECTOR_H_INCLUDED
#define OCILIB_VECTOR_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Vector * OcilibVectorInitialize
(
    OCI_Connection *con,
    OCI_Vector     *vect,
    OCIVector      *handle
);

OCI_SYM_LOCAL OCI_Vector * OcilibVectorCreate
(
    OCI_Connection *con
);

OCI_SYM_LOCAL boolean OcilibVectorFree
(
    OCI_Vector *vect
);

OCI_SYM_LOCAL boolean OcilibVectorGetInfo
(
    OCI_Vector   *vect,
    unsigned int *format,
    unsigned int *dimensions
);

OCI_SYM_LOCAL boolean OcilibVectorGetValues
(
    OCI_Vector       *vect,
    void             *values
);

OCI_SYM_LOCAL boolean OcilibVectorSetValues
(
    OCI_Vector      *vect,
    unsigned int     format,
    unsigned int     dimensions,
    void            *values
);

OCI_SYM_LOCAL OCI_Vector ** OcilibVectorCreateArray
(
    OCI_Connection *con,
    unsigned int    nbelem
);

OCI_SYM_LOCAL boolean OcilibVectorFreeArray
(
    OCI_Vector **vects
);

OCI_SYM_LOCAL boolean OcilibVectorFromString
(
    OCI_Vector  *vect,
    const otext* str,
    unsigned int size,
    unsigned int format,
    unsigned int dimensions
);

OCI_SYM_LOCAL boolean OcilibVectorToString
(
    OCI_Vector    *vect,
    unsigned int  *size,
    otext         *str
);

#endif /* OCILIB_VECTOR_H_INCLUDED */
