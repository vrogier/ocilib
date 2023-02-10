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

#ifndef OCILIB_ENQUEUE_H_INCLUDED
#define OCILIB_ENQUEUE_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Enqueue * OcilibEnqueueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
);

OCI_SYM_LOCAL boolean OcilibEnqueueFree
(
    OCI_Enqueue *enqueue
);

OCI_SYM_LOCAL boolean OcilibEnqueuePut
(
    OCI_Enqueue *enqueue,
    OCI_Msg     *msg
);

OCI_SYM_LOCAL unsigned int OcilibEnqueueGetVisibility
(
    OCI_Enqueue *enqueue
);

OCI_SYM_LOCAL boolean OcilibEnqueueSetVisibility
(
    OCI_Enqueue *enqueue,
    unsigned int visibility
);

OCI_SYM_LOCAL unsigned int OcilibEnqueueGetSequenceDeviation
(
    OCI_Enqueue *enqueue
);

OCI_SYM_LOCAL boolean OcilibEnqueueSetSequenceDeviation
(
    OCI_Enqueue *enqueue,
    unsigned int sequence
);

OCI_SYM_LOCAL boolean OcilibEnqueueGetRelativeMsgID
(
    OCI_Enqueue  *enqueue,
    void         *id,
    unsigned int *len
);

OCI_SYM_LOCAL boolean OcilibEnqueueSetRelativeMsgID
(
    OCI_Enqueue *enqueue,
    const void  *id,
    unsigned int len
);

#endif /* OCILIB_ENQUEUE_H_INCLUDED */
