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
 * OCI_ErrorCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OCI_ErrorCreate
(
    void
)
{
    return (OCI_Error *) calloc(1, sizeof(OCI_Error));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorFree
 * --------------------------------------------------------------------------------------------- */

void OCI_ErrorFree
(
    OCI_Error *err
)
{
    if (err == &OCILib.lib_err)
    {
        return;
    }

    if (err)
    {
        free(err);
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorReset
 * --------------------------------------------------------------------------------------------- */

void OCI_ErrorReset
(
    OCI_Error *err
)
{
    if (err)
    {
        err->raise      = FALSE;
        err->active     = FALSE;
        err->con        = NULL;
        err->stmt       = NULL;
        err->sqlcode    = 0;
        err->libcode    = 0;
        err->type       = 0;
        err->str[0]     = 0;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGet
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OCI_ErrorGet
(
    boolean check
)
{
    OCI_Error *err = NULL;

    if (OCILib.loaded && OCI_LIB_THREADED)
    {
        if (OCI_ThreadKeyGet(OCILib.key_errs, (void **)(dvoid *)&err))
        {
            if (!err)
            {
                err = OCI_ErrorCreate();

                if (err)
                {
                    OCI_ThreadKeySet(OCILib.key_errs, err);
                }
            }
        }
    }
    else
    {
        err = &OCILib.lib_err;
    }

    if (check && err && err->active)
    {
        err = NULL;
    }

    return err;
}

/* ********************************************************************************************* *
 *                             PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OCI_API OCI_ErrorGetString
(
    OCI_Error *err
)
{
    OCI_CHECK(NULL == err, NULL);

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
    OCI_CHECK(NULL == err, OCI_UNKNOWN);

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
    OCI_CHECK(NULL == err, OCI_UNKNOWN);

    return (int) err->sqlcode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGetInternalCode
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_ErrorGetInternalCode
(
    OCI_Error *err
)
{
    OCI_CHECK(NULL == err, 0);

    return err->libcode;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ErrorGetConnection
 * --------------------------------------------------------------------------------------------- */

OCI_Connection * OCI_API OCI_ErrorGetConnection
(
    OCI_Error *err
)
{
    OCI_CHECK(NULL == err, NULL);

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
    OCI_CHECK(NULL == err, NULL);

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
    OCI_CHECK(NULL == err, 0);

    return err->row;
}
