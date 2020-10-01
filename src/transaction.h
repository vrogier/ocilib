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

#ifndef OCILIB_TRANSACTION_H_INCLUDED
#define OCILIB_TRANSACTION_H_INCLUDED

#include "types.h"

boolean OcilibTransactionDispose
(
    OCI_Transaction * trans
);

OCI_Transaction * OcilibTransactionCreate
(
    OCI_Connection *con,
    unsigned int    timeout,
    unsigned int    mode,
    OCI_XID        *pxid
);

boolean OcilibTransactionFree
(
    OCI_Transaction * trans
);

boolean OcilibTransactionStart
(
    OCI_Transaction * trans
);

boolean OcilibTransactionStop
(
    OCI_Transaction * trans
);

boolean OcilibTransactionResume
(
    OCI_Transaction * trans
);

boolean OcilibTransactionPrepare
(
    OCI_Transaction * trans
);

boolean OcilibTransactionForget
(
    OCI_Transaction * trans
);

unsigned int OcilibTransactionGetMode
(
    OCI_Transaction * trans
);

unsigned int OcilibTransactionGetTimeout
(
    OCI_Transaction * trans
);

#endif /* OCILIB_TRANSACTION_H_INCLUDED */
