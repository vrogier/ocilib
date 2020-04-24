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

OCI_Msg* MessageCreate
(
    OCI_TypeInfo* typinf
);

boolean MessageFree
(
    OCI_Msg* msg
);

boolean MessageReset
(
    OCI_Msg* msg
);

OCI_Object* MessageGetObject
(
    OCI_Msg* msg
);

boolean MessageSetObject
(
    OCI_Msg* msg,
    OCI_Object* obj
);

boolean MessageGetRaw
(
    OCI_Msg* msg,
    void* raw,
    unsigned int* size
);

boolean MessageSetRaw
(
    OCI_Msg* msg,
    const void* raw,
    unsigned int size
);

int MessageGetAttemptCount
(
    OCI_Msg* msg
);

int MessageGetEnqueueDelay
(
    OCI_Msg* msg
);

boolean MessageSetEnqueueDelay
(
    OCI_Msg* msg,
    int      value
);

OCI_Date* MessageGetEnqueueTime
(
    OCI_Msg* msg
);

int MessageGetExpiration
(
    OCI_Msg* msg
);

boolean MessageSetExpiration
(
    OCI_Msg* msg,
    int      value
);

unsigned int MessageGetState
(
    OCI_Msg* msg
);

int MessageGetPriority
(
    OCI_Msg* msg
);

boolean MessageSetPriority
(
    OCI_Msg* msg,
    int      value
);

boolean MessageGetID
(
    OCI_Msg* msg,
    void* id,
    unsigned int* len
);

boolean MessageGetOriginalID
(
    OCI_Msg* msg,
    void* id,
    unsigned int* len
);

boolean MessageSetOriginalID
(
    OCI_Msg* msg,
    const void* id,
    unsigned int len
);

const otext* MessageGetCorrelation
(
    OCI_Msg* msg
);

boolean MessageSetCorrelation
(
    OCI_Msg* msg,
    const otext* correlation
);

const otext* MessageGetExceptionQueue
(
    OCI_Msg* msg
);

boolean MessageSetExceptionQueue
(
    OCI_Msg* msg,
    const otext* queue
);

OCI_Agent* MessageGetSender
(
    OCI_Msg* msg
);

boolean MessageSetSender
(
    OCI_Msg* msg,
    OCI_Agent* sender
);

boolean MessageSetConsumers
(
    OCI_Msg* msg,
    OCI_Agent** consumers,
    unsigned int count
);

#endif /* OCILIB_MSG_H_INCLUDED */
