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
 * $Id: error.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OCI_ErrorCreate
(
    void
)
{
    OCI_Error *err = calloc(1, sizeof(*err));

    return err;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorFree
 * --------------------------------------------------------------------------------------------- */

void OCI_ErrorFree
(
    OCI_Error *err
)
{
    OCI_FREE(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorReset
 * --------------------------------------------------------------------------------------------- */

void OCI_ErrorReset
(
    OCI_Error *err
)
{
    if (err != NULL)
    {
        err->warning = FALSE;
        err->raise   = TRUE;
        err->active  = FALSE;
        err->con     = NULL;
        err->stmt    = NULL;
        err->ocode   = 0;
        err->icode   = 0;
        err->type    = 0;
        err->str[0]  = 0;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGet
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OCI_ErrorGet
(
    boolean check,
    boolean warning
)
{
    OCI_Error *err = NULL;

    if ((warning == TRUE) && (OCILib.warnings_on == FALSE))
    {
        return NULL;
    }

    if (OCILib.loaded == TRUE)
    {
        if (OCI_ThreadKeyGet(OCILib.key_errs, ( void **) (dvoid *) &err) == TRUE)
        {
            if (err == NULL)
            {
                err = OCI_ErrorCreate();

                if (err != NULL)
                    OCI_ThreadKeySet(OCILib.key_errs, err);
            }
            else if (check == TRUE)
            {
                if ((err->active == TRUE) || (err->warning != warning))
                    err = NULL;
            }
        }
    }
    else
    {
        err = &OCILib.lib_err;

        if (err != NULL)
        {
            if ((err->active == TRUE) || (err->warning != warning))
                err = NULL;
        }
    }

    return err;
}

/* ********************************************************************************************* *
 *                             PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGetString
 * --------------------------------------------------------------------------------------------- */

const mtext * OCI_API OCI_ErrorGetString
(
    OCI_Error *err
)
{
    OCI_CHECK(err == NULL, NULL);

    return err->str;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ErrorGetType
(
    OCI_Error *err
)
{
    OCI_CHECK(err == NULL, OCI_UNKNOWN);

    return err->type;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGetOCICode
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ErrorGetOCICode
(
    OCI_Error *err
)
{
    OCI_CHECK(err == NULL, OCI_UNKNOWN);

    return (int) err->ocode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGetInternalCode
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ErrorGetInternalCode
(
    OCI_Error *err
)
{
    OCI_CHECK_PTR(OCI_IPC_ERROR, err, 0);

    return err->icode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_ErrorGetConnection
(
    OCI_Error *err
)
{
    OCI_CHECK(err == NULL, NULL);

    return err->con;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGetStatement
 * --------------------------------------------------------------------------------------------- */

OCI_Statement * OCI_API OCI_ErrorGetStatement
(
    OCI_Error *err
)
{
    OCI_CHECK(err == NULL, NULL);

    return err->stmt;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGetRow
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_ErrorGetRow
(
    OCI_Error *err
)
{
    OCI_CHECK(err == NULL, 0);

    return err->row;
}
