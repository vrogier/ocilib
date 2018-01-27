/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventReset
 * --------------------------------------------------------------------------------------------- */

boolean OCI_EventReset
(
    OCI_Event *event
)
{
    OCI_CHECK(NULL == event, FALSE)

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

    return TRUE;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_EventGetType
(
    OCI_Event *event
)
{
    OCI_GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_EVENT, event, type, event->sub->con, NULL, event->sub->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetOperation
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_EventGetOperation
(
    OCI_Event *event
)
{
    OCI_GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_EVENT, event, op, event->sub->con, NULL, event->sub->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetDatabase
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_EventGetDatabase
(
    OCI_Event *event
)
{
    OCI_GET_PROP(const otext *, NULL, OCI_IPC_EVENT, event, dbname, event->sub->con, NULL, event->sub->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetObject
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_EventGetObject
(
    OCI_Event *event
)
{
    OCI_GET_PROP(const otext *, NULL, OCI_IPC_EVENT, event, objname, event->sub->con, NULL, event->sub->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetRowid
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_EventGetRowid
(
    OCI_Event *event
)
{
    OCI_GET_PROP(const otext *, NULL, OCI_IPC_EVENT, event, rowid, event->sub->con, NULL, event->sub->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetSubscription
 * --------------------------------------------------------------------------------------------- */

OCI_Subscription * OCI_API OCI_EventGetSubscription
(
    OCI_Event *event
)
{
    OCI_GET_PROP(OCI_Subscription *, NULL, OCI_IPC_EVENT, event, sub, event->sub->con, NULL, event->sub->err)
}
