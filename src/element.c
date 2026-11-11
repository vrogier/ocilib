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

#include "element.h"

#include "collection.h"
#include "date.h"
#include "exception.h"
#include "file.h"
#include "helpers.h"
#include "interval.h"
#include "lob.h"
#include "macros.h"
#include "memory.h"
#include "number.h"
#include "object.h"
#include "reference.h"
#include "stringutils.h"
#include "timestamp.h"

#define ELEM_SET_VALUE(elemtype, type, func_init, func_assign)           \
                                                                         \
        ENTER_FUNC(OCI_Boolean, FALSE, OCI_IPC_ELEMENT, elem);           \
                                                                         \
        CHECK_PTR(OCI_IPC_ELEMENT, elem);                                \
        CHECK_COMPAT((elemtype) == (elem)->typinf->cols[0].datatype);    \
                                                                         \
        if (NULL == value)                                               \
        {                                                                \
            CHECK(OcilibElementSetNull(elem));                           \
            (elem)->init = FALSE;                                        \
        }                                                                \
        else                                                             \
        {                                                                \
            if (!(elem)->obj)                                            \
            {                                                            \
                (elem)->obj = func_init;                                 \
                CHECK_NULL((elem)->obj);                                 \
            }                                                            \
                                                                         \
            CHECK(func_assign);                                          \
                                                                         \
            CHECK(OcilibElementSetNullIndicator(elem, OCI_IND_NOTNULL)); \
            (elem)->handle = ((type) (elem)->obj)->handle;               \
            (elem)->init   = TRUE;                                       \
        }                                                                \
                                                                         \
        SET_SUCCESS();                                                   \
                                                                         \
        EXIT_FUNC();

#define ELEM_GET_VALUE(elemtype, type, func)                          \
                                                                      \
        ENTER_FUNC(type, NULL, OCI_IPC_ELEMENT, elem);                \
                                                                      \
        CHECK_PTR(OCI_IPC_ELEMENT, elem);                             \
        CHECK_COMPAT((elemtype) == (elem)->typinf->cols[0].datatype); \
                                                                      \
        type tmp = NULL;                                              \
                                                                      \
        if (OCI_IND_NULL != *(elem)->pind)                            \
        {                                                             \
            if (!(elem)->init)                                        \
            {                                                         \
                (elem)->obj = func;                                   \
                CHECK_NULL((elem)->obj);                              \
                tmp          = (elem)->obj;                           \
                (elem)->init = (NULL != tmp);                         \
            }                                                         \
            else                                                      \
            {                                                         \
                tmp = (type) (elem)->obj;                             \
            }                                                         \
        }                                                             \
                                                                      \
        SET_RETVAL(tmp);                                              \
                                                                      \
        EXIT_FUNC();

#define ELEM_GET_NUMBER(elem, number_type, type, value)                                    \
                                                                                           \
        ENTER_FUNC(type, value, OCI_IPC_ELEMENT, elem);                                    \
                                                                                           \
        CHECK_PTR(OCI_IPC_ELEMENT, elem);                                                  \
                                                                                           \
        type tmp = value;                                                                  \
                                                                                           \
        CHECK(OcilibElementGetNumberInternal(elem, (void *) &tmp, (uword) (number_type))); \
                                                                                           \
        SET_RETVAL(tmp);                                                                   \
                                                                                           \
        EXIT_FUNC();

#define ELEM_SET_NUMBER(elem, value, number_type)                                              \
                                                                                               \
        ENTER_FUNC(OCI_Boolean, FALSE, OCI_IPC_ELEMENT, elem);                                 \
                                                                                               \
        CHECK_PTR(OCI_IPC_ELEMENT, elem);                                                      \
        CHECK_COMPAT(OCI_CDT_NUMERIC == (elem)->typinf->cols[0].datatype);                     \
                                                                                               \
        CHECK(OcilibElementSetNumberInternal(elem, (void *) &(value), (uword) (number_type))); \
                                                                                               \
        SET_SUCCESS();                                                                         \
                                                                                               \
        EXIT_FUNC()

/* --------------------------------------------------------------------------------------------- *
* OcilibElementFreeAllocatedData
* --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibElementFreeAllocatedData
(
    OCI_Element *elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);

    if (elem->alloc && elem->handle)
    {
        switch (elem->typinf->cols[0].datatype)
        {
            case OCI_CDT_TEXT:
                OcilibStringFreeStringPtr(elem->typinf->con->env, elem->typinf->con->env->handle, (OCIString**)&elem->handle,
                                          elem->typinf->con->err);
                break;
            default:
                FREE(elem->handle);
                break;
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Element * OcilibElementInitialize
(
    OCI_Element  *elem,
    void         *handle,
    OCIInd       *pind,
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Element*, elem,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    ALLOC_HANDLE(OCI_IPC_ELEMENT, elem);

    /* Assign type info before any other calls */
    elem->typinf = typinf;

    /* Free previously allocated data before reassign new one */
    OcilibElementFreeAllocatedData(elem);

    if (NULL != elem->obj)
    {
        OCI_DataType* data = (OCI_DataType*) elem->obj;

        if (OCI_OBJECT_FETCHED_CLEAN == data->hstate)
        {
            data->hstate = OCI_OBJECT_FETCHED_DIRTY;
        }

        OcilibFreeObjectFromType(elem->obj, elem->typinf->cols[0].datatype);

        elem->obj = NULL;
    }

    /* Reset members */
    elem->ind    = OCI_IND_NULL;
    elem->handle = handle;
    elem->init   = FALSE;
    elem->alloc  = FALSE;
    elem->hstate = handle ? OCI_OBJECT_FETCHED_CLEAN : OCI_OBJECT_ALLOCATED;

    switch (elem->typinf->cols[0].datatype)
    {
        case OCI_CDT_NUMERIC:
        {
            if (!elem->handle)
            {
                ALLOC_BUFFER(OCI_IPC_VOID, elem->handle, elem->typinf->cols[0].size, 1);
                elem->alloc = TRUE;
            }
            break;
        }
        case OCI_CDT_BOOLEAN:
        {
            if (!elem->handle)
            {
                ALLOC_BUFFER(OCI_IPC_VOID, elem->handle, sizeof(OCI_Boolean), 1);
                elem->alloc = TRUE;
            }
            break;
        }
        case OCI_CDT_TEXT:
        case OCI_CDT_TIMESTAMP:
        case OCI_CDT_INTERVAL:
        case OCI_CDT_RAW:
        case OCI_CDT_LOB:
        case OCI_CDT_FILE:
        case OCI_CDT_REF:
        {
            if (elem->handle)
            {
                elem->handle = * (void **) handle;
            }
            break;
        }
    }

    if (NULL != pind)
    {
        elem->pind = pind;
        elem->ind  = *elem->pind;
    }
    else
    {
        elem->pind = &elem->ind;
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibElementFree(elem);
            elem = NULL;
        }

        SET_RETVAL(elem);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetNullIndicator
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetNullIndicator
(
    OCI_Element *elem,
    OCIInd       value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);

    CHECK_NULL(elem->pind);

    OCI_Boolean done = FALSE;

    if (OCI_CDT_OBJECT == elem->typinf->cols[0].datatype)
    {
        OCI_Object* obj = (OCI_Object*)elem->obj;

        if (obj)
        {
            elem->pind = obj->tab_ind;
            done       = TRUE;
        }
        else
        {
            value = OCI_IND_NULL;
        }
    }

    if (!done)
    {
        *elem->pind = value;
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetNumberInternal
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibElementSetNumberInternal
(
    OCI_Element *elem,
    void        *value,
    uword        flag
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);

    CHECK(OcilibNumberTranslateValue(elem->typinf->con->env, value, flag, elem->handle,
                                     elem->typinf->cols[0].subtype));

    CHECK(OcilibElementSetNullIndicator(elem, OCI_IND_NOTNULL));

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetNumberInternal
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibElementGetNumberInternal
(
    OCI_Element *elem,
    void        *value,
    uword        flag
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);
    CHECK_PTR(OCI_IPC_VOID,    value);

    if (!OcilibElementIsNull(elem))
    {
        if (OCI_CDT_NUMERIC == elem->typinf->cols[0].datatype)
        {
            CHECK(OcilibNumberTranslateValue(elem->typinf->con->env, elem->handle,
                                             elem->typinf->cols[0].subtype,
                                             value, flag));
        }
        else if (OCI_CDT_TEXT == elem->typinf->cols[0].datatype)
        {
            CHECK(OcilibNumberFromStringInternal(elem->typinf->con->env, value, flag,
                                                 OcilibElementGetString(elem), NULL));
        }
        else
        {
            THROW_NO_ARGS(OcilibExceptionTypeNotCompatible);
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Element * OcilibElementCreate
(
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Element*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    );

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf);

    SET_RETVAL(OcilibElementInitialize(NULL, NULL, (OCIInd*)NULL, typinf));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementFree
(
    OCI_Element *elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);
    CHECK_OBJECT_FETCHED(elem);

    /* if the element has sub-objects that have been fetched, we need to free
       these objects */

    if (NULL != elem->obj)
    {
        OCI_DataType* data = (OCI_DataType*) elem->obj;

        if (OCI_OBJECT_FETCHED_CLEAN == data->hstate)
        {
            data->hstate = OCI_OBJECT_FETCHED_DIRTY;
        }

        OcilibFreeObjectFromType(elem->obj, elem->typinf->cols[0].datatype);
    }

    OcilibElementFreeAllocatedData(elem);

    OcilibErrorResetSource(NULL, elem);

    FREE(elem->tmpbuf);
    FREE(elem);

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetBoolean
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementGetBoolean
(
    OCI_Element *elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);
    CHECK_COMPAT(OCI_CDT_BOOLEAN == elem->typinf->cols[0].datatype);

    CHECK(!OcilibElementIsNull(elem));

    SET_RETVAL(elem->handle ? *((OCI_Boolean*)elem->handle) : FALSE);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OcilibElementGetNumber
(
    OCI_Element *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_NUMERIC,
        OCI_Number *,
        OcilibNumberInitialize(elem->typinf->con->env, (OCI_Number *) elem->obj, (OCINumber *) elem->handle)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Int16 OcilibElementGetInt16
(
    OCI_Element *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_INT16, OCI_Int16, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetUInt16
 * --------------------------------------------------------------------------------------------- */

OCI_UInt16 OcilibElementGetUInt16
(
    OCI_Element *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_UINT16, OCI_UInt16, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetInt32
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibElementGetInt32
(
    OCI_Element *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_INT32, OCI_Int32, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetUInt32
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibElementGetUInt32
(
    OCI_Element *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_UINT32, OCI_UInt32, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetInt64
 * --------------------------------------------------------------------------------------------- */

OCI_Int64 OcilibElementGetInt64
(
    OCI_Element *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_INT64, OCI_Int64, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetUInt64
 * --------------------------------------------------------------------------------------------- */

OCI_UInt64 OcilibElementGetUInt64
(
    OCI_Element *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_UINT64, OCI_UInt64, 0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetDouble
 * --------------------------------------------------------------------------------------------- */

OCI_Double OcilibElementGetDouble
(
    OCI_Element *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_DOUBLE, OCI_Double, 0.0);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetFloat
 * --------------------------------------------------------------------------------------------- */

OCI_Float OcilibElementGetFloat
(
    OCI_Element *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_FLOAT, OCI_Float, 0.0f);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibElementGetString
(
    OCI_Element *elem
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);
    CHECK_COMPAT(OCI_CDT_TEXT == elem->typinf->cols[0].datatype);

    CHECK_NULL(elem->handle);

    SET_RETVAL(OcilibStringFromStringPtr(elem->typinf->con->env,
                                         elem->typinf->con->env->handle,
                                         (OCIString*)elem->handle,
                                         &elem->tmpbuf, &elem->tmpsize));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetRaw
 * --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibElementGetRaw
(
    OCI_Element *elem,
    void        *value,
    OCI_UInt32   len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);
    CHECK_COMPAT(OCI_CDT_RAW == elem->typinf->cols[0].datatype);

    CHECK_NULL(elem->handle);

    OCIRaw *raw = (OCIRaw *) elem->handle;
    ub4     raw_len;

    OCI_CALL(raw_len, OCIRawSize, elem->typinf->con->env->handle, raw);

    if (len > raw_len)
    {
        len = raw_len;
    }

    oratext *rawptr;

    OCI_CALL(rawptr, OCIRawPtr, elem->typinf->con->env->handle, raw);

    CHECK_NULL(rawptr);

    memcpy(value, rawptr, (OCI_Size) len);

    SET_RETVAL(raw_len);

    EXIT_FUNC();
}

/* -------------------------------------------------------------------------------------------- *
* OcilibElementGetRawSize
* --------------------------------------------------------------------------------------------- */

OCI_UInt32 OcilibElementGetRawSize
(
    OCI_Element *elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_UInt32, 0,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);
    CHECK_COMPAT(OCI_CDT_RAW == elem->typinf->cols[0].datatype);

    CHECK_NULL(elem->handle);

    ub4 rawsize;

    OCI_CALL(rawsize, OCIRawSize, elem->typinf->con->env->handle, (OCIRaw*)elem->handle);

    SET_RETVAL(rawsize);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibElementGetDate
(
    OCI_Element *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date *,
        OcilibDateInitialize(elem->typinf->con->env, (OCI_Date *) elem->obj, (OCIDate *) elem->handle, FALSE, FALSE)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibElementGetTimestamp
(
    OCI_Element *elem
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    ELEM_GET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        OCI_Timestamp *,
        OcilibTimestampInitialize(elem->typinf->con->env, elem->typinf->con, (OCI_Timestamp *) elem->obj,
                                  (OCIDateTime *) elem->handle,
                                  elem->typinf->cols[0].subtype)
    );

#else

    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp *, NULL,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);

    EXIT_FUNC();

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OcilibElementGetInterval
(
    OCI_Element *elem
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    ELEM_GET_VALUE
    (
        OCI_CDT_INTERVAL,
        OCI_Interval *,
        OcilibIntervalInitialize(elem->typinf->con->env, elem->typinf->con, (OCI_Interval *) elem->obj,
                                 (OCIInterval *) elem->handle,
                                 elem->typinf->cols[0].subtype)
    );
#else

    ENTER_FUNC
    (
        /* returns */ struct OCI_Interval *, NULL,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);

    EXIT_FUNC();

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OcilibElementGetLob
(
    OCI_Element *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_LOB,
        OCI_Lob*,
        OcilibLobInitialize(elem->typinf->con, (OCI_Lob *) elem->obj,
                            (OCILobLocator *) elem->handle,
                            elem->typinf->cols[0].subtype)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * OcilibElementGetFile
(
    OCI_Element *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File*,
        OcilibFileInitialize(elem->typinf->con, (OCI_File *) elem->obj,
                             (OCILobLocator *) elem->handle,
                             elem->typinf->cols[0].subtype)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetReference
 * --------------------------------------------------------------------------------------------- */

OCI_Reference * OcilibElementGetReference
(
    OCI_Element *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_REF,
        OCI_Reference*,
        OcilibReferenceInitialize(elem->typinf->con, elem->typinf->cols[0].typinf,
                                  (OCI_Reference *) elem->obj, (OCIRef *) elem->handle)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OcilibElementGetObject
(
    OCI_Element *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object*,
        OcilibObjectInitialize(elem->typinf->con, (OCI_Object *) elem->obj, elem->handle,
                               elem->typinf->cols[0].typinf, NULL, -1, TRUE)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetCollection
 * --------------------------------------------------------------------------------------------- */

OCI_Collection * OcilibElementGetCollection
(
    OCI_Element *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Collection*,
        OcilibCollectionInitialize(elem->typinf->con, (OCI_Collection *) elem->obj,
                                   (OCIColl *) elem->handle,
                                   elem->typinf->cols[0].typinf)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetBoolean
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetBoolean
(
    OCI_Element *elem,
    OCI_Boolean  value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);
    CHECK_COMPAT(OCI_CDT_BOOLEAN == elem->typinf->cols[0].datatype);

    if (NULL != elem->handle)
    {
        OCI_Boolean *data = (OCI_Boolean *) elem->handle;

        *data = value;

        CHECK(OcilibElementSetNullIndicator(elem, OCI_IND_NOTNULL));
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetNumber
(
    OCI_Element *elem,
    OCI_Number  *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_NUMERIC,
        OCI_Number*,
        OcilibNumberInitialize(elem->typinf->con->env, (OCI_Number *) elem->obj, (OCINumber *) elem->handle),
        OcilibNumberAssign((OCI_Number *) elem->obj, value)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetInt16
(
    OCI_Element *elem,
    OCI_Int16    value
)
{
    ELEM_SET_NUMBER(elem,value, OCI_NUM_INT16);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetUInt16
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetUInt16
(
    OCI_Element *elem,
    OCI_UInt16   value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_UINT16);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetInt32
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetInt32
(
    OCI_Element *elem,
    OCI_Int32    value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_INT32);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetUInt32
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetUInt32
(
    OCI_Element *elem,
    OCI_UInt32   value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_UINT32);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetInt64
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetInt64
(
    OCI_Element *elem,
    OCI_Int64    value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_INT64);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetUInt64
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetUInt64
(
    OCI_Element *elem,
    OCI_UInt64   value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_UINT64);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetDouble
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetDouble
(
    OCI_Element *elem,
    OCI_Double   value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_DOUBLE);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetFloat
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetFloat
(
    OCI_Element *elem,
    OCI_Float    value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_FLOAT);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetString
(
    OCI_Element *elem,
    const otext *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);
    CHECK_COMPAT(OCI_CDT_TEXT == elem->typinf->cols[0].datatype);

    if (NULL == value)
    {
        CHECK(OcilibElementSetNull(elem));
    }
    else
    {
        elem->alloc = (elem->alloc || elem->handle == NULL);

        CHECK(OcilibStringToStringPtr(elem->typinf->con->env,
                                      elem->typinf->con->env->handle,
                                      (OCIString **) &elem->handle,
                                      elem->typinf->con->err, value));

        CHECK(OcilibElementSetNullIndicator(elem, OCI_IND_NOTNULL));
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetRaw
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetRaw
(
    OCI_Element *elem,
    void       * value,
    OCI_UInt32   len
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);
    CHECK_COMPAT(OCI_CDT_RAW == elem->typinf->cols[0].datatype);

    if (NULL == value)
    {
        CHECK(OcilibElementSetNull(elem));
    }
    else
    {
        CHECK_OCI
        (
            elem->typinf->con->err,
            OCIRawAssignBytes,
            elem->typinf->con->env->handle, elem->typinf->con->err,
            (ub1*) value, len, (OCIRaw **) &elem->handle
        );

        CHECK(OcilibElementSetNullIndicator(elem, OCI_IND_NOTNULL));
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetDate
(
    OCI_Element *elem,
    OCI_Date    *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date*,
        OcilibDateInitialize(elem->typinf->con ? elem->typinf->con->env : NULL,
                             (OCI_Date *) elem->obj,
                             (OCIDate *) elem->handle, TRUE, FALSE),
        OcilibDateAssign((OCI_Date *) elem->obj, value)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetTimestamp
(
    OCI_Element   *elem,
    OCI_Timestamp *value
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    ELEM_SET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        OCI_Timestamp*,
        OcilibTimestampInitialize(elem->typinf->con->env, elem->typinf->con, (OCI_Timestamp *) elem->obj,
                                  (OCIDateTime *) elem->handle,
                                  elem->typinf->cols[0].subtype),
        OcilibTimestampAssign((OCI_Timestamp *) elem->obj, value)
    );

#else

    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);

    EXIT_FUNC();

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetInterval
(
    OCI_Element  *elem,
    OCI_Interval *value
)
{

#if OCI_VERSION_COMPILE >= OCI_9_0

    ELEM_SET_VALUE
    (
        OCI_CDT_INTERVAL,
        OCI_Interval*,
        OcilibIntervalInitialize(elem->typinf->con->env, elem->typinf->con, (OCI_Interval *) elem->obj,
                                 (OCIInterval *) elem->handle,
                                 elem->typinf->cols[0].subtype),
        OcilibIntervalAssign((OCI_Interval *) elem->obj, value)
    );

#else

    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);

    EXIT_FUNC();

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetCollection
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetCollection
(
    OCI_Element    *elem,
    OCI_Collection *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Collection*,
        OcilibCollectionInitialize(elem->typinf->con, (OCI_Collection *) elem->obj, (OCIColl *) elem->handle,
                                   elem->typinf->cols[0].typinf),
        OcilibCollectionAssign((OCI_Collection *) elem->obj, value)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetObject
(
    OCI_Element *elem,
    OCI_Object  *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object*,
        OcilibObjectInitialize(elem->typinf->con, (OCI_Object *) elem->obj, elem->handle,
                               elem->typinf->cols[0].typinf, NULL, -1, TRUE),
        OcilibObjectAssign((OCI_Object *) elem->obj, value)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetLob
(
    OCI_Element *elem,
    OCI_Lob     *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_LOB,
        OCI_Lob*,
        OcilibLobInitialize(elem->typinf->con, (OCI_Lob *) elem->obj,
                            (OCILobLocator *) elem->handle,
                            elem->typinf->cols[0].subtype),
        OcilibLobAssign((OCI_Lob *) elem->obj, value)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetFile
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetFile
(
    OCI_Element *elem,
    OCI_File    *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File*,
        OcilibFileInitialize(elem->typinf->con, (OCI_File *) elem->obj,
                             (OCILobLocator *) elem->handle,
                             elem->typinf->cols[0].subtype),
        OcilibFileAssign((OCI_File *) elem->obj, value)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetReference
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetReference
(
    OCI_Element   *elem,
    OCI_Reference *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_REF,
        OCI_Reference*,
        OcilibReferenceInitialize(elem->typinf->con, elem->typinf->cols[0].typinf,
                                  (OCI_Reference *) elem->obj, (OCIRef *) elem->handle),
        OcilibReferenceAssign((OCI_Reference *) elem->obj, value)
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementIsNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementIsNull
(
    OCI_Element *elem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    );

    CHECK_PTR(OCI_IPC_ELEMENT, elem);

    CHECK_NULL(elem->pind);

    SET_RETVAL(OCI_IND_NULL == *elem->pind);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetNull
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibElementSetNull
(
    OCI_Element *elem
)
{
    return OcilibElementSetNullIndicator(elem, OCI_IND_NULL);
}
