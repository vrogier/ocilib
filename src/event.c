/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2015 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: event.c, Vincent Rogier $
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
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_EVENT, event)

    call_retval = event->type;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_EventGetOperation
(
    OCI_Event *event
)
{
    OCI_LIB_CALL_ENTER(unsigned int, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_EVENT, event)

    call_retval = event->op;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetObject
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_EventGetDatabase
(
    OCI_Event *event
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_EVENT, event)

    call_retval = event->dbname;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetObject
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_EventGetObject
(
    OCI_Event *event
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_EVENT, event)

    call_retval = event->objname;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetRowid
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_EventGetRowid
(
    OCI_Event *event
)
{
    OCI_LIB_CALL_ENTER(const otext *, NULL)

    OCI_CHECK_PTR(OCI_IPC_EVENT, event)

    call_retval = event->rowid;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_EventGetSubscription
 * --------------------------------------------------------------------------------------------- */

OCI_Subscription * OCI_API OCI_EventGetSubscription
(
    OCI_Event *event
)
{
    OCI_LIB_CALL_ENTER(OCI_Subscription *, NULL)

    OCI_CHECK_PTR(OCI_IPC_EVENT, event)

    call_retval = event->sub;
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()}

