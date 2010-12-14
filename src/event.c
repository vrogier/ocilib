/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: event.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

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
    OCI_CHECK(event == NULL, FALSE);

    event->op   = OCI_UNKNOWN;
    event->type = OCI_UNKNOWN;

    if (event->dbname != NULL)
        event->dbname[0] = 0;

    if (event->objname != NULL)
        event->objname[0] = 0;

    if (event->rowid != NULL)
        event->rowid[0] = 0;

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
    OCI_CHECK_PTR(OCI_IPC_EVENT, event, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return event->type;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_EventGetOperation
(
    OCI_Event *event
)
{
    OCI_CHECK_PTR(OCI_IPC_EVENT, event, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return event->op;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetObject
 * --------------------------------------------------------------------------------------------- */

const dtext * OCI_API OCI_EventGetDatabase
(
    OCI_Event *event
)
{
    OCI_CHECK_PTR(OCI_IPC_EVENT, event, NULL);

    OCI_RESULT(TRUE);

    return event->dbname;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetObject
 * --------------------------------------------------------------------------------------------- */

const dtext * OCI_API OCI_EventGetObject
(
    OCI_Event *event
)
{
    OCI_CHECK_PTR(OCI_IPC_EVENT, event, NULL);

    OCI_RESULT(TRUE);

    return event->objname;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetRowid
 * --------------------------------------------------------------------------------------------- */

const dtext * OCI_API OCI_EventGetRowid
(
    OCI_Event *event
)
{
    OCI_CHECK_PTR(OCI_IPC_EVENT, event, NULL);

    OCI_RESULT(TRUE);

    return event->rowid;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetSubscription
 * --------------------------------------------------------------------------------------------- */

OCI_Subscription * OCI_API OCI_EventGetSubscription
(
    OCI_Event *event
)
{
    OCI_CHECK_PTR(OCI_IPC_EVENT, event, NULL);

    OCI_RESULT(TRUE);

    return event->sub;
}

