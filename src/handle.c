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

#include "handle.h"

#include "macros.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetEnvironment
 * --------------------------------------------------------------------------------------------- */

OCI_Environment * OcilibHandleGetEnvironment
(
    void *handle
)
{
    return (NULL != handle) ? ((OCI_Handle *) handle)->env : NULL;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleGetUserData
 * --------------------------------------------------------------------------------------------- */

void * OcilibHandleGetUserData
(
    void *handle
)
{
    ENTER_FUNC
    (
        /* returns */ void*, NULL,
        /* context */ OCI_IPC_VOID, handle
    );

    CHECK_NULL(handle);

    SET_RETVAL(((OCI_Handle *) handle)->usrdata);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibHandleSetUserData
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibHandleSetUserData
(
    void *handle,
    void *data
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, handle
    );

    CHECK_NULL(handle);

    ((OCI_Handle *) handle)->usrdata = data;

    SET_SUCCESS();

    EXIT_FUNC();
}
