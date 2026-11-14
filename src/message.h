/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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

OCI_SYM_LOCAL OCI_Message* OcilibMessageCreate
(
    OCI_TypeInfo* typinf
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageFree
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageReset
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Object* OcilibMessageGetObject
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageSetObject
(
    OCI_Message * msg,
    OCI_Object  * obj
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageGetRaw
(
    OCI_Message * msg,
    void        * raw,
    OCI_UInt32  * size
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageSetRaw
(
    OCI_Message * msg,
    const void  * raw,
    OCI_UInt32    size
);

OCI_SYM_LOCAL OCI_Int32 OcilibMessageGetAttemptCount
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Int32 OcilibMessageGetEnqueueDelay
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageSetEnqueueDelay
(
    OCI_Message * msg,
    OCI_Int32     value
);

OCI_SYM_LOCAL OCI_Date* OcilibMessageGetEnqueueTime
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Int32 OcilibMessageGetExpiration
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageSetExpiration
(
    OCI_Message * msg,
    OCI_Int32     value
);

OCI_SYM_LOCAL OCI_MsgState OcilibMessageGetState
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Int32 OcilibMessageGetPriority
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageSetPriority
(
    OCI_Message * msg,
    OCI_Int32     value
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageGetID
(
    OCI_Message * msg,
    void        * id,
    OCI_UInt32  * len
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageGetOriginalID
(
    OCI_Message * msg,
    void        * id,
    OCI_UInt32  * len
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageSetOriginalID
(
    OCI_Message * msg,
    const void  * id,
    OCI_UInt32    len
);

OCI_SYM_LOCAL const otext * OcilibMessageGetCorrelation
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageSetCorrelation
(
    OCI_Message * msg,
    const otext * correlation
);

OCI_SYM_LOCAL const otext * OcilibMessageGetExceptionQueue
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageSetExceptionQueue
(
    OCI_Message * msg,
    const otext * queue
);

OCI_SYM_LOCAL OCI_Agent * OcilibMessageGetSender
(
    OCI_Message* msg
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageSetSender
(
    OCI_Message * msg,
    OCI_Agent   * sender
);

OCI_SYM_LOCAL OCI_Boolean OcilibMessageSetConsumers
(
    OCI_Message * msg,
    OCI_Agent  ** consumers,
    OCI_UInt32    count
);

#endif /* OCILIB_MSG_H_INCLUDED */
