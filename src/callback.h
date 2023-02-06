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

#ifndef OCILIB_CALLBACK_H_INCLUDED
#define OCILIB_CALLBACK_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL sb4 OcilibCallbackInBind
(
    dvoid   *ictxp,
    OCIBind *bindp,
    ub4      iter,
    ub4      index,
    dvoid  **bufpp,
    ub4     *alenp,
    ub1     *piecep,
    dvoid  **indp
);

OCI_SYM_LOCAL sb4 OcilibCallbackOutBind
(
    dvoid   *octxp,
    OCIBind *bindp,
    ub4      iter,
    ub4      index,
    dvoid  **bufpp,
    ub4    **alenp,
    ub1     *piecep,
    void   **indp,
    ub2    **rcodep
);

OCI_SYM_LOCAL ub4 OcilibCallbackNotifyChanges
(
    void            *oci_ctx,
    OCISubscription *subscrhp,
    void            *payload,
    ub4              paylen,
    void            *desc,
    ub4              mode
);

OCI_SYM_LOCAL ub4 OcilibCallbackNotifyMessages
(
    void            *oci_ctx,
    OCISubscription *subscrhp,
    void            *payload,
    ub4              paylen,
    void            *desc,
    ub4              mode
);

OCI_SYM_LOCAL sb4 OcilibCallbackFailOver
(
    dvoid *svchp,
    dvoid *envhp,
    dvoid *fo_ctx,
    ub4    fo_type,
    ub4    fo_event
);

OCI_SYM_LOCAL void OcilibCallbackHAEvent
(
    dvoid *evtctx,
    dvoid *eventptr
);

#endif /* OCILIB_CALLBACK_H_INCLUDED */
