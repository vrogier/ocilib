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

#include "event.h"

#include "macros.h"

/* --------------------------------------------------------------------------------------------- *
 * EventReset
 * --------------------------------------------------------------------------------------------- */

boolean EventReset
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
 * EventGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int EventGetType
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
 * EventGetOperation
 * --------------------------------------------------------------------------------------------- */

unsigned int EventGetOperation
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
 * EventGetDatabase
 * --------------------------------------------------------------------------------------------- */

const otext * EventGetDatabase
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
 * EventGetObject
 * --------------------------------------------------------------------------------------------- */

const otext * EventGetObject
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
 * EventGetRowid
 * --------------------------------------------------------------------------------------------- */

const otext * EventGetRowid
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
 * EventGetSubscription
 * --------------------------------------------------------------------------------------------- */

OCI_Subscription * EventGetSubscription
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
