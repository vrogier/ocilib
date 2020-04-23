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

OCI_Msg* MsgCreate
(
    OCI_TypeInfo* typinf
);

boolean MsgFree
(
    OCI_Msg* msg
);

boolean MsgReset
(
    OCI_Msg* msg
);

OCI_Object* MsgGetObject
(
    OCI_Msg* msg
);

boolean MsgSetObject
(
    OCI_Msg* msg,
    OCI_Object* obj
);

boolean MsgGetRaw
(
    OCI_Msg* msg,
    void* raw,
    unsigned int* size
);

boolean MsgSetRaw
(
    OCI_Msg* msg,
    const void* raw,
    unsigned int size
);

int MsgGetAttemptCount
(
    OCI_Msg* msg
);

int MsgGetEnqueueDelay
(
    OCI_Msg* msg
);

boolean MsgSetEnqueueDelay
(
    OCI_Msg* msg,
    int      value
);

OCI_Date* MsgGetEnqueueTime
(
    OCI_Msg* msg
);

int MsgGetExpiration
(
    OCI_Msg* msg
);

boolean MsgSetExpiration
(
    OCI_Msg* msg,
    int      value
);

unsigned int MsgGetState
(
    OCI_Msg* msg
);

int MsgGetPriority
(
    OCI_Msg* msg
);

boolean MsgSetPriority
(
    OCI_Msg* msg,
    int      value
);

boolean MsgGetID
(
    OCI_Msg* msg,
    void* id,
    unsigned int* len
);

boolean MsgGetOriginalID
(
    OCI_Msg* msg,
    void* id,
    unsigned int* len
);

boolean MsgSetOriginalID
(
    OCI_Msg* msg,
    const void* id,
    unsigned int len
);

const otext* MsgGetCorrelation
(
    OCI_Msg* msg
);

boolean MsgSetCorrelation
(
    OCI_Msg* msg,
    const otext* correlation
);

const otext* MsgGetExceptionQueue
(
    OCI_Msg* msg
);

boolean MsgSetExceptionQueue
(
    OCI_Msg* msg,
    const otext* queue
);

OCI_Agent* MsgGetSender
(
    OCI_Msg* msg
);

boolean MsgSetSender
(
    OCI_Msg* msg,
    OCI_Agent* sender
);

boolean MsgSetConsumers
(
    OCI_Msg* msg,
    OCI_Agent** consumers,
    unsigned int count
);

#endif /* OCILIB_MSG_H_INCLUDED */
