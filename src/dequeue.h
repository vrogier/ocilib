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

#ifndef OCILIB_DEQUEUE_H_INCLUDED
#define OCILIB_DEQUEUE_H_INCLUDED

#include "types.h"

OCI_Dequeue * OcilibDequeueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
);

boolean OcilibDequeueFree
(
    OCI_Dequeue *dequeue
);

OCI_Agent * OcilibDequeueListen
(
    OCI_Dequeue *dequeue,
    int          timeout
);

OCI_Msg * OcilibDequeueGetMessage
(
    OCI_Dequeue *dequeue
);

const otext * OcilibDequeueGetConsumer
(
    OCI_Dequeue *dequeue
);

boolean OcilibDequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const otext *consumer
);

const otext * OcilibDequeueGetCorrelation
(
    OCI_Dequeue *dequeue
);

boolean OcilibDequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const otext *pattern
);

boolean OcilibDequeueGetRelativeMsgID
(
    OCI_Dequeue  *dequeue,
    void         *id,
    unsigned int *len
);

boolean OcilibDequeueSetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    const void  *id,
    unsigned int len
);

unsigned int OcilibDequeueGetVisibility
(
    OCI_Dequeue *dequeue
);

boolean OcilibDequeueSetVisibility
(
    OCI_Dequeue *dequeue,
    unsigned int visibility
);

unsigned int OcilibDequeueGetMode
(
    OCI_Dequeue *dequeue
);

boolean OcilibDequeueSetMode
(
    OCI_Dequeue *dequeue,
    unsigned int mode
);

unsigned int OcilibDequeueGetNavigation
(
    OCI_Dequeue *dequeue
);

boolean OcilibDequeueSetNavigation
(
    OCI_Dequeue *dequeue,
    unsigned int position
);

int OcilibDequeueGetWaitTime
(
    OCI_Dequeue *dequeue
);

boolean OcilibDequeueSetWaitTime
(
    OCI_Dequeue *dequeue,
    int          timeout
);

boolean OcilibDequeueSetAgentList
(
    OCI_Dequeue *dequeue,
    OCI_Agent  **consumers,
    unsigned int count
);

boolean OcilibDequeueSubscribe
(
    OCI_Dequeue   *dequeue,
    unsigned int   port,
    unsigned int   timeout,
    POCI_NOTIFY_AQ callback
);

boolean OcilibDequeueUnsubscribe
(
    OCI_Dequeue *dequeue
);

#endif /* OCILIB_DEQUEUE_H_INCLUDED */
