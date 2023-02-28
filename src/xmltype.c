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

#include "xmltype.h"

#include "long.h"
#include "macros.h"
#include "memory.h"
#include "stringutils.h"
#include "xmltype.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibXmlTypeInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_XmlType * OcilibXmlTypeInitialize
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    OCI_XmlType    *xmlType,
    OCI_Define     *def
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_XmlType*, xmlType,
        /* context */ OCI_IPC_XMLTYPE, xmlType 
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    ALLOC_DATA(OCI_IPC_XMLTYPE, xmlType, 1);

    xmlType->con    = con;
    xmlType->lng    = OcilibLongInitialize(stmt, xmlType->lng, def, OCI_CLONG);

    if (def)
    {
        xmlType->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }
    else if (OCI_OBJECT_ALLOCATED_ARRAY != xmlType->hstate)
    {
        xmlType->hstate = OCI_OBJECT_ALLOCATED;
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibXmlTypeFree(xmlType);
            xmlType = NULL;
        }

        SET_RETVAL(xmlType)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibXmlTypeCreate
 * --------------------------------------------------------------------------------------------- */

OCI_XmlType * OcilibXmlTypeCreate
(
    OCI_Connection* con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_XmlType*, NULL,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    SET_RETVAL(OcilibXmlTypeInitialize(con, NULL, NULL, NULL))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibXmlTypeFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibXmlTypeFree
(
    OCI_XmlType *xmlType
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_XMLTYPE, xmlType
    )

    CHECK_PTR(OCI_IPC_XMLTYPE, xmlType)
    CHECK_OBJECT_FETCHED(xmlType)

    OcilibErrorResetSource(NULL, xmlType);

    xmlType->lng->hstate = OCI_OBJECT_FETCHED_DIRTY;

    OcilibLongFree(xmlType->lng);

    if (OCI_OBJECT_ALLOCATED_ARRAY != xmlType->hstate)
    {
        OcilibErrorResetSource(NULL, xmlType);

        FREE(xmlType)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibXmlTypeGetContent
 * --------------------------------------------------------------------------------------------- */

OCI_SYM_LOCAL const otext * OcilibXmlTypeGetContent
(
    OCI_XmlType    *xmlType
)
{
    ENTER_FUNC
    (
        /* returns */ const otext *, NULL,
        /* context */ OCI_IPC_XMLTYPE, xmlType
    )

    CHECK_PTR(OCI_IPC_XMLTYPE, xmlType)

    if (NULL != xmlType->lng)
    {
        SET_RETVAL((otext*)xmlType->lng->buffer);
    }

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibXmlTypeGetContentSize
 * --------------------------------------------------------------------------------------------- */

OCI_SYM_LOCAL unsigned int OcilibXmlTypeGetContentSize
(
    OCI_XmlType    *xmlType
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_XMLTYPE, xmlType
    )

    CHECK_PTR(OCI_IPC_XMLTYPE, xmlType)

    if (NULL != xmlType->lng)
    {
        SET_RETVAL(xmlType->lng->size / sizeof(otext));
    }

    EXIT_FUNC()
}
