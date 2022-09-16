/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2022 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_MSG_H_INCLUDED
#define OCILIB_MSG_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Msg* OcilibMessageCreate
(
    OCI_TypeInfo* typinf
);

OCI_SYM_LOCAL boolean OcilibMessageFree
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL boolean OcilibMessageReset
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL OCI_Object* OcilibMessageGetObject
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL boolean OcilibMessageSetObject
(
    OCI_Msg   * msg,
    OCI_Object* obj
);

OCI_SYM_LOCAL boolean OcilibMessageGetRaw
(
    OCI_Msg     * msg,
    void        * raw,
    unsigned int* size
);

OCI_SYM_LOCAL boolean OcilibMessageSetRaw
(
    OCI_Msg    * msg,
    const void * raw,
    unsigned int size
);

OCI_SYM_LOCAL int OcilibMessageGetAttemptCount
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL int OcilibMessageGetEnqueueDelay
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL boolean OcilibMessageSetEnqueueDelay
(
    OCI_Msg* msg,
    int      value
);

OCI_SYM_LOCAL OCI_Date* OcilibMessageGetEnqueueTime
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL int OcilibMessageGetExpiration
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL boolean OcilibMessageSetExpiration
(
    OCI_Msg* msg,
    int      value
);

OCI_SYM_LOCAL unsigned int OcilibMessageGetState
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL int OcilibMessageGetPriority
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL boolean OcilibMessageSetPriority
(
    OCI_Msg* msg,
    int      value
);

OCI_SYM_LOCAL boolean OcilibMessageGetID
(
    OCI_Msg     * msg,
    void        * id,
    unsigned int* len
);

OCI_SYM_LOCAL boolean OcilibMessageGetOriginalID
(
    OCI_Msg     * msg,
    void        * id,
    unsigned int* len
);

OCI_SYM_LOCAL boolean OcilibMessageSetOriginalID
(
    OCI_Msg    * msg,
    const void * id,
    unsigned int len
);

OCI_SYM_LOCAL const otext * OcilibMessageGetCorrelation
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL boolean OcilibMessageSetCorrelation
(
    OCI_Msg    * msg,
    const otext* correlation
);

OCI_SYM_LOCAL const otext * OcilibMessageGetExceptionQueue
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL boolean OcilibMessageSetExceptionQueue
(
    OCI_Msg    * msg,
    const otext* queue
);

OCI_SYM_LOCAL OCI_Agent * OcilibMessageGetSender
(
    OCI_Msg* msg
);

OCI_SYM_LOCAL boolean OcilibMessageSetSender
(
    OCI_Msg  * msg,
    OCI_Agent* sender
);

OCI_SYM_LOCAL boolean OcilibMessageSetConsumers
(
    OCI_Msg    * msg,
    OCI_Agent ** consumers,
    unsigned int count
);

#endif /* OCILIB_MSG_H_INCLUDED */
