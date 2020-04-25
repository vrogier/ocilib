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

boolean TransactionDispose
(
    OCI_Transaction * trans
);

OCI_Transaction * TransactionCreate
(
    OCI_Connection *con,
    unsigned int    timeout,
    unsigned int    mode,
    OCI_XID        *pxid
);

boolean TransactionFree
(
    OCI_Transaction * trans
);

boolean TransactionStart
(
    OCI_Transaction * trans
);

boolean TransactionStop
(
    OCI_Transaction * trans
);

boolean TransactionResume
(
    OCI_Transaction * trans
);

boolean TransactionPrepare
(
    OCI_Transaction * trans
);

boolean TransactionForget
(
    OCI_Transaction * trans
);

unsigned int TransactionGetMode
(
    OCI_Transaction * trans
);

unsigned int TransactionGetTimeout
(
    OCI_Transaction * trans
);

#endif /* OCILIB_TRANSACTION_H_INCLUDED */
