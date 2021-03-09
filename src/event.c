/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2021 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "event.h"

#include "macros.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibEventReset
 * --------------------------------------------------------------------------------------------- */

boolean OcilibEventReset
(
    OCI_Event *event
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_EVENT, event
    )

    CHECK_PTR(OCI_IPC_EVENT, event)

    event->op   = OCI_UNKNOWN;
    event->type = OCI_UNKNOWN;

    if (event->dbname)
    {
        event->dbname[0] = 0;
    }

    if (event->objname)
    {
        event->objname[0] = 0;
    }

    if (event->rowid)
    {
        event->rowid[0] = 0;
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEventGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibEventGetType
(
    OCI_Event *event
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_EVENT, event,
        /* member */ type
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEventGetOperation
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibEventGetOperation
(
    OCI_Event *event
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_EVENT, event,
        /* member */ op
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEventGetDatabase
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibEventGetDatabase
(
    OCI_Event *event
)
{
    GET_PROP
    (
        /* result */ const otext *, NULL,
        /* handle */ OCI_IPC_EVENT, event,
        /* member */ dbname
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEventGetObject
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibEventGetObject
(
    OCI_Event *event
)
{
    GET_PROP
    (
        /* result */ const otext *, NULL,
        /* handle */ OCI_IPC_EVENT, event,
        /* member */ objname
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEventGetRowid
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibEventGetRowid
(
    OCI_Event *event
)
{
    GET_PROP
    (
        /* result */ const otext *, NULL,
        /* handle */ OCI_IPC_EVENT, event,
        /* member */ rowid
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibEventGetSubscription
 * --------------------------------------------------------------------------------------------- */

OCI_Subscription * OcilibEventGetSubscription
(
    OCI_Event *event
)
{
    GET_PROP
    (
        /* result */ OCI_Subscription *, NULL,
        /* handle */ OCI_IPC_EVENT, event,
        /* member */ sub
    )
}
