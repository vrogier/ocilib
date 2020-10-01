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

#ifndef OCILIB_MSG_H_INCLUDED
#define OCILIB_MSG_H_INCLUDED

#include "types.h"

OCI_Msg* OcilibMessageCreate
(
    OCI_TypeInfo* typinf
);

boolean OcilibMessageFree
(
    OCI_Msg* msg
);

boolean OcilibMessageReset
(
    OCI_Msg* msg
);

OCI_Object* OcilibMessageGetObject
(
    OCI_Msg* msg
);

boolean OcilibMessageSetObject
(
    OCI_Msg   * msg,
    OCI_Object* obj
);

boolean OcilibMessageGetRaw
(
    OCI_Msg     * msg,
    void        * raw,
    unsigned int* size
);

boolean OcilibMessageSetRaw
(
    OCI_Msg    * msg,
    const void * raw,
    unsigned int size
);

int OcilibMessageGetAttemptCount
(
    OCI_Msg* msg
);

int OcilibMessageGetEnqueueDelay
(
    OCI_Msg* msg
);

boolean OcilibMessageSetEnqueueDelay
(
    OCI_Msg* msg,
    int      value
);

OCI_Date* OcilibMessageGetEnqueueTime
(
    OCI_Msg* msg
);

int OcilibMessageGetExpiration
(
    OCI_Msg* msg
);

boolean OcilibMessageSetExpiration
(
    OCI_Msg* msg,
    int      value
);

unsigned int OcilibMessageGetState
(
    OCI_Msg* msg
);

int OcilibMessageGetPriority
(
    OCI_Msg* msg
);

boolean OcilibMessageSetPriority
(
    OCI_Msg* msg,
    int      value
);

boolean OcilibMessageGetID
(
    OCI_Msg     * msg,
    void        * id,
    unsigned int* len
);

boolean OcilibMessageGetOriginalID
(
    OCI_Msg     * msg,
    void        * id,
    unsigned int* len
);

boolean OcilibMessageSetOriginalID
(
    OCI_Msg    * msg,
    const void * id,
    unsigned int len
);

const otext* OcilibMessageGetCorrelation
(
    OCI_Msg* msg
);

boolean OcilibMessageSetCorrelation
(
    OCI_Msg    * msg,
    const otext* correlation
);

const otext* OcilibMessageGetExceptionQueue
(
    OCI_Msg* msg
);

boolean OcilibMessageSetExceptionQueue
(
    OCI_Msg    * msg,
    const otext* queue
);

OCI_Agent* OcilibMessageGetSender
(
    OCI_Msg* msg
);

boolean OcilibMessageSetSender
(
    OCI_Msg  * msg,
    OCI_Agent* sender
);

boolean OcilibMessageSetConsumers
(
    OCI_Msg    * msg,
    OCI_Agent ** consumers,
    unsigned int count
);

#endif /* OCILIB_MSG_H_INCLUDED */
