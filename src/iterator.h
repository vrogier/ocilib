/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_ITERATOR_H_INCLUDED
#define OCILIB_ITERATOR_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Iter * OcilibIteratorCreate
(
    OCI_Coll *coll
);

OCI_SYM_LOCAL boolean OcilibIteratorFree
(
    OCI_Iter *iter
);

OCI_SYM_LOCAL OCI_Elem* OcilibIteratorGetNext
(
    OCI_Iter *iter
);

OCI_SYM_LOCAL OCI_Elem* OcilibIteratorGetPrev
(
    OCI_Iter *iter
);

OCI_SYM_LOCAL OCI_Elem* OcilibIteratorGetCurrent
(
    OCI_Iter *iter
);

#endif /* OCILIB_ITERATOR_H_INCLUDED */
