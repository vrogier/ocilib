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

#ifndef OCILIB_DEQUEUE_H_INCLUDED
#define OCILIB_DEQUEUE_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Dequeue * OcilibDequeueCreate
(
    OCI_TypeInfo *typinf,
    const otext  *name
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueFree
(
    OCI_Dequeue *dequeue
);

OCI_SYM_LOCAL OCI_Agent * OcilibDequeueListen
(
    OCI_Dequeue *dequeue,
    OCI_Int32    timeout
);

OCI_SYM_LOCAL OCI_Message * OcilibDequeueGetMessage
(
    OCI_Dequeue *dequeue
);

OCI_SYM_LOCAL const otext * OcilibDequeueGetConsumer
(
    OCI_Dequeue *dequeue
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueSetConsumer
(
    OCI_Dequeue *dequeue,
    const otext *consumer
);

OCI_SYM_LOCAL const otext * OcilibDequeueGetCorrelation
(
    OCI_Dequeue *dequeue
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueSetCorrelation
(
    OCI_Dequeue *dequeue,
    const otext *pattern
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueGetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    void        *id,
    OCI_UInt32  *len
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueSetRelativeMsgID
(
    OCI_Dequeue *dequeue,
    const void  *id,
    OCI_UInt32   len
);

OCI_SYM_LOCAL OCI_QueueVisibility OcilibDequeueGetVisibility
(
    OCI_Dequeue *dequeue
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueSetVisibility
(
    OCI_Dequeue        *dequeue,
    OCI_QueueVisibility visibility
);

OCI_SYM_LOCAL OCI_DequeueMode OcilibDequeueGetMode
(
    OCI_Dequeue *dequeue
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueSetMode
(
    OCI_Dequeue    *dequeue,
    OCI_DequeueMode mode
);

OCI_SYM_LOCAL OCI_DequeueNav OcilibDequeueGetNavigation
(
    OCI_Dequeue *dequeue
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueSetNavigation
(
    OCI_Dequeue   *dequeue,
    OCI_DequeueNav position
);

OCI_SYM_LOCAL OCI_Int32 OcilibDequeueGetWaitTime
(
    OCI_Dequeue *dequeue
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueSetWaitTime
(
    OCI_Dequeue *dequeue,
    OCI_Int32    timeout
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueSetAgentList
(
    OCI_Dequeue *dequeue,
    OCI_Agent  **consumers,
    OCI_UInt32   count
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueSubscribe
(
    OCI_Connection* con,
    OCI_Dequeue   **dequeues,
    OCI_UInt32      count,
    OCI_UInt32      timeout,
    POCI_NOTIFY_AQ  callback
);

OCI_SYM_LOCAL OCI_Boolean OcilibDequeueUnsubscribe
(
    OCI_Dequeue *dequeue
);

#endif /* OCILIB_DEQUEUE_H_INCLUDED */
