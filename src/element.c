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

#define ELEM_SET_VALUE(elemtype, type, func_init, func_assign)      \
                                                                    \
    ENTER_FUNC(boolean, TRUE, OCI_IPC_ELEMENT, elem)                \
                                                                    \
    CHECK_PTR(OCI_IPC_ELEMENT, elem)                                \
    CHECK_COMPAT((elemtype) == (elem)->typinf->cols[0].datatype)    \
                                                                    \
    if (NULL == value)                                              \
    {                                                               \
        CHECK(OcilibElementSetNull(elem))                           \
    }                                                               \
    else                                                            \
    {                                                               \
        if (!(elem)->obj)                                           \
        {                                                           \
            (elem)->obj = func_init;                                \
            CHECK_NULL((elem)->obj)                                 \
        }                                                           \
                                                                    \
        CHECK(func_assign)                                          \
                                                                    \
        CHECK(OcilibElementSetNullIndicator(elem, OCI_IND_NOTNULL)) \
        (elem)->handle = ((type) (elem)->obj)->handle;              \
    }                                                               \
                                                                    \
    SET_SUCCESS()                                                   \
                                                                    \
    EXIT_FUNC()

#define ELEM_GET_VALUE(elemtype, type, func)                     \
                                                                 \
    ENTER_FUNC(type, NULL, OCI_IPC_ELEMENT, elem)                \
                                                                 \
    CHECK_PTR(OCI_IPC_ELEMENT, elem)                             \
    CHECK_COMPAT((elemtype) == (elem)->typinf->cols[0].datatype) \
                                                                 \
    type tmp = NULL;                                             \
                                                                 \
    if (OCI_IND_NULL != *(elem)->pind)                           \
    {                                                            \
        if (!(elem)->init)                                       \
        {                                                        \
            (elem)->obj = func;                                  \
            CHECK_NULL((elem)->obj)                              \
            tmp = (elem)->obj;                                   \
            (elem)->init = (NULL != tmp);                        \
        }                                                        \
        else                                                     \
        {                                                        \
            tmp = (type) (elem)->obj;                            \
        }                                                        \
    }                                                            \
                                                                 \
    SET_RETVAL(tmp)                                              \
                                                                 \
    EXIT_FUNC()

#define ELEM_GET_NUMBER(elem, number_type, type, value)                                 \
                                                                                        \
    ENTER_FUNC(type, value, OCI_IPC_ELEMENT, elem)                                      \
                                                                                        \
    CHECK_PTR(OCI_IPC_ELEMENT, elem)                                                    \
    CHECK_COMPAT(OCI_CDT_NUMERIC == (elem)->typinf->cols[0].datatype)                   \
                                                                                        \
    type tmp = value;                                                                   \
                                                                                        \
    CHECK(OcilibElementGetNumberInternal(elem, (void *) &tmp, (uword) (number_type)))   \
                                                                                        \
    SET_RETVAL(tmp)                                                                     \
                                                                                        \
    EXIT_FUNC()

#define ELEM_SET_NUMBER(elem, value, number_type)                                           \
                                                                                            \
    ENTER_FUNC(boolean, FALSE, OCI_IPC_ELEMENT, elem)                                       \
                                                                                            \
    CHECK_PTR(OCI_IPC_ELEMENT, elem)                                                        \
    CHECK_COMPAT(OCI_CDT_NUMERIC == (elem)->typinf->cols[0].datatype)                       \
                                                                                            \
    CHECK(OcilibElementSetNumberInternal(elem, (void *) &(value), (uword) (number_type)))   \
                                                                                            \
    SET_SUCCESS()                                                                           \
                                                                                            \
    EXIT_FUNC()

/* --------------------------------------------------------------------------------------------- *
* OcilibElementFreeAllocatedData
* --------------------------------------------------------------------------------------------- */

static boolean OcilibElementFreeAllocatedData
(
    OCI_Elem *elem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)

    if (elem->alloc && elem->handle)
    {
        switch (elem->typinf->cols[0].datatype)
        {
            case OCI_CDT_TEXT:
                OcilibStringFreeStringPtr(elem->con->env, (OCIString**)&elem->handle, elem->con->err);
                break;
            default:
                FREE(elem->handle)
                break;
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * OcilibElementInitialize
(
    OCI_Connection *con,
    OCI_Elem       *elem,
    void           *handle,
    OCIInd         *pind,
    OCI_TypeInfo   *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Elem*, elem,
        /* context */ OCI_IPC_CONNECTION, con
    )

    CHECK_PTR(OCI_IPC_CONNECTION, con)

    ALLOC_DATA(OCI_IPC_ELEMENT, elem, 1);

    elem->con = con;

    /* Free previously allocated data before reassign new one */
    OcilibElementFreeAllocatedData(elem);

    /* Reset members */
    elem->ind    = OCI_IND_NULL;
    elem->typinf = typinf;
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
                ALLOC_BUFFER(OCI_IPC_VOID, elem->handle, elem->typinf->cols[0].size, 1)
                elem->alloc = TRUE;
            }
            break;
        }
        case OCI_CDT_BOOLEAN:
        {
            if (!elem->handle)
            {
                ALLOC_BUFFER(OCI_IPC_VOID, elem->handle, sizeof(boolean), 1)
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

        SET_RETVAL(elem)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetNullIndicator
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetNullIndicator
(
    OCI_Elem *elem,
    OCIInd    value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)

    CHECK_NULL(elem->pind)

    boolean done = FALSE;

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

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetNumberInternal
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibElementSetNumberInternal
(
    OCI_Elem *elem,
    void     *value,
    uword     flag
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)

    CHECK(OcilibNumberTranslateValue(elem->typinf->con, value, flag, elem->handle,
                                     elem->typinf->cols[0].subtype))

    CHECK(OcilibElementSetNullIndicator(elem, OCI_IND_NOTNULL))

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetNumberInternal
 * --------------------------------------------------------------------------------------------- */

static boolean OcilibElementGetNumberInternal
(
    OCI_Elem *elem,
    void     *value,
    uword     flag
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_PTR(OCI_IPC_VOID,    value)

    if (!OcilibElementIsNull(elem))
    {
        if (OCI_CDT_NUMERIC == elem->typinf->cols[0].datatype)
        {
            CHECK(OcilibNumberTranslateValue(elem->typinf->con, elem->handle,
                                             elem->typinf->cols[0].subtype,
                                             value, flag))
        }
        else if (OCI_CDT_TEXT == elem->typinf->cols[0].datatype)
        {
            CHECK(OcilibNumberFromStringInternal(elem->con, value, flag,
                                                 OcilibElementGetString(elem), NULL))
        }
        else
        {
            THROW_NO_ARGS(OcilibExceptionTypeNotCompatible)
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * OcilibElementCreate
(
    OCI_TypeInfo *typinf
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Elem*, NULL,
        /* context */ OCI_IPC_TYPE_INFO, typinf
    )

    CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    SET_RETVAL(OcilibElementInitialize(typinf->con, NULL, NULL, (OCIInd*)NULL, typinf))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementFree
(
    OCI_Elem *elem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_OBJECT_FETCHED(elem)

    /* if the element has sub-objects that have been fetched, we need to free
       these objects */

    if (NULL != elem->obj)
    {
        OCI_Datatype * data = (OCI_Datatype *) elem->obj;

        if (OCI_OBJECT_FETCHED_CLEAN == data->hstate)
        {
            data->hstate = OCI_OBJECT_FETCHED_DIRTY;
        }

        OcilibFreeObjectFromType(elem->obj, elem->typinf->cols[0].datatype);
    }

    OcilibElementFreeAllocatedData(elem);

    OcilibErrorResetSource(NULL, elem);

    FREE(elem->tmpbuf)
    FREE(elem)

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetBoolean
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementGetBoolean
(
    OCI_Elem *elem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_COMPAT(OCI_CDT_BOOLEAN == elem->typinf->cols[0].datatype)

    CHECK(!OcilibElementIsNull(elem))

    SET_RETVAL(elem->handle ? *((boolean*)elem->handle) : FALSE)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OcilibElementGetNumber
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_NUMERIC,
        OCI_Number *,
        OcilibNumberInitialize(elem->con, (OCI_Number *) elem->obj, (OCINumber *) elem->handle)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetShort
 * --------------------------------------------------------------------------------------------- */

short OcilibElementGetShort
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_SHORT, short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

unsigned short OcilibElementGetUnsignedShort
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_USHORT, unsigned short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetInt
 * --------------------------------------------------------------------------------------------- */

int OcilibElementGetInt
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_INT, int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibElementGetUnsignedInt
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_UINT, unsigned int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetBigInt
 * --------------------------------------------------------------------------------------------- */

big_int OcilibElementGetBigInt
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_BIGINT, big_int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

big_uint OcilibElementGetUnsignedBigInt
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_BIGUINT, big_uint, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetDouble
 * --------------------------------------------------------------------------------------------- */

double OcilibElementGetDouble
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_DOUBLE, double, 0.0)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetFloat
 * --------------------------------------------------------------------------------------------- */

float OcilibElementGetFloat
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_FLOAT, float, 0.0f)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetString
 * --------------------------------------------------------------------------------------------- */

const otext * OcilibElementGetString
(
    OCI_Elem *elem
)
{
    ENTER_FUNC
    (
        /* returns */ const otext*, NULL,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_COMPAT(OCI_CDT_TEXT == elem->typinf->cols[0].datatype)

    CHECK_NULL(elem->handle)

    SET_RETVAL(OcilibStringFromStringPtr(elem->con->env,
                                         (OCIString*)elem->handle,
                                         &elem->tmpbuf, &elem->tmpsize))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetRaw
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibElementGetRaw
(
    OCI_Elem    *elem,
    void        *value,
    unsigned int len
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_COMPAT(OCI_CDT_RAW == elem->typinf->cols[0].datatype)

    CHECK_NULL(elem->handle)

    OCIRaw   *raw = (OCIRaw *) elem->handle;
    const ub4 raw_len = OCIRawSize(elem->con->env, raw);

    if (len > raw_len)
    {
        len = raw_len;
    }

    memcpy(value, OCIRawPtr(elem->con->env, raw), (size_t) len);

    SET_RETVAL(raw_len)

    EXIT_FUNC()
}

/* -------------------------------------------------------------------------------------------- *
* OcilibElementGetRawSize
* --------------------------------------------------------------------------------------------- */

unsigned int OcilibElementGetRawSize
(
    OCI_Elem *elem
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned int, 0,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_COMPAT(OCI_CDT_RAW == elem->typinf->cols[0].datatype)

    CHECK_NULL(elem->handle)

    SET_RETVAL(OCIRawSize(elem->con->env, (OCIRaw*)elem->handle))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibElementGetDate
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date *,
        OcilibDateInitialize(elem->con, (OCI_Date *) elem->obj, (OCIDate *) elem->handle, FALSE, FALSE)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibElementGetTimestamp
(
    OCI_Elem *elem
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    ELEM_GET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        OCI_Timestamp *,
        OcilibTimestampInitialize(elem->con, (OCI_Timestamp *) elem->obj,
                                  (OCIDateTime *) elem->handle,
                                  elem->typinf->cols[0].subtype)
    )

#else

    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp *, NULL,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)

    EXIT_FUNC()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OcilibElementGetInterval
(
    OCI_Elem *elem
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    ELEM_GET_VALUE
    (
        OCI_CDT_INTERVAL,
        OCI_Interval *,
        OcilibIntervalInitialize(elem->con, (OCI_Interval *) elem->obj,
                                 (OCIInterval *) elem->handle,
                                 elem->typinf->cols[0].subtype)
    )
#else

    ENTER_FUNC
    (
        /* returns */ struct OCI_Interval *, NULL,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)

    EXIT_FUNC()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * OcilibElementGetLob
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_LOB,
        OCI_Lob*,
        OcilibLobInitialize(elem->con, (OCI_Lob *) elem->obj,
                           (OCILobLocator *) elem->handle,
                            elem->typinf->cols[0].subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * OcilibElementGetFile
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File*,
        OcilibFileInitialize(elem->con, (OCI_File *) elem->obj,
                             (OCILobLocator *) elem->handle,
                             elem->typinf->cols[0].subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetReference
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * OcilibElementGetReference
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_REF,
        OCI_Ref*,
        OcilibReferenceInitialize(elem->con, elem->typinf->cols[0].typinf,
                                  (OCI_Ref *) elem->obj, (OCIRef *) elem->handle)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * OcilibElementGetObject
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object*,
        OcilibObjectInitialize(elem->con, (OCI_Object *) elem->obj, elem->handle,
                               elem->typinf->cols[0].typinf, NULL, -1, TRUE)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementGetCollection
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * OcilibElementGetCollection
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Coll*,
        OcilibCollectionInitialize(elem->con, (OCI_Coll *) elem->obj,
                                   (OCIColl *) elem->handle,
                                   elem->typinf->cols[0].typinf)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetBoolean
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetBoolean
(
    OCI_Elem *elem,
    boolean   value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_COMPAT(OCI_CDT_BOOLEAN == elem->typinf->cols[0].datatype)

    if (NULL != elem->handle)
    {
        boolean *data = (boolean *) elem->handle;

        *data = value;

        CHECK(OcilibElementSetNullIndicator(elem, OCI_IND_NOTNULL))
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetNumber
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetNumber
(
    OCI_Elem   *elem,
    OCI_Number *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_NUMERIC,
        OCI_Number*,
        OcilibNumberInitialize(elem->con, (OCI_Number *) elem->obj, (OCINumber *) elem->handle),
        OcilibNumberAssign((OCI_Number *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetShort
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetShort
(
    OCI_Elem *elem,
    short     value
)
{
    ELEM_SET_NUMBER(elem,value, OCI_NUM_SHORT)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetUnsignedShort
(
    OCI_Elem      *elem,
    unsigned short value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_USHORT)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetInt
(
    OCI_Elem *elem,
    int       value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_INT)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetUnsignedInt
(
    OCI_Elem    *elem,
    unsigned int value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_UINT)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetBigInt
(
    OCI_Elem *elem,
    big_int   value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_BIGINT)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetUnsignedBigInt
(
    OCI_Elem *elem,
    big_uint  value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_BIGUINT)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetDouble
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetDouble
(
    OCI_Elem *elem,
    double    value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_DOUBLE)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetFloat
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetFloat
(
    OCI_Elem *elem,
    float     value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_FLOAT)
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetString
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetString
(
    OCI_Elem    *elem,
    const otext *value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_COMPAT(OCI_CDT_TEXT == elem->typinf->cols[0].datatype)

    if (NULL == value)
    {
        CHECK(OcilibElementSetNull(elem))
    }
    else
    {
        elem->alloc = (elem->alloc || elem->handle == NULL);

        CHECK(OcilibStringToStringPtr(elem->con->env,
                                      (OCIString **) &elem->handle,
                                      elem->con->err, value))

        CHECK(OcilibElementSetNullIndicator(elem, OCI_IND_NOTNULL))
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetRaw
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetRaw
(
    OCI_Elem    *elem,
    void       * value,
    unsigned int len
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CHECK_COMPAT(OCI_CDT_TEXT == elem->typinf->cols[0].datatype)

    if (NULL == value)
    {
        CHECK(OcilibElementSetNull(elem))
    }
    else
    {
        CHECK_OCI
        (
            elem->con->err,
            OCIRawAssignBytes,
            elem->con->env, elem->con->err,
            (ub1*) value, len, (OCIRaw **) &elem->handle
        )

        CHECK(OcilibElementSetNullIndicator(elem, OCI_IND_NOTNULL))
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetDate
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetDate
(
    OCI_Elem *elem,
    OCI_Date *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date*,
        OcilibDateInitialize(elem->con, (OCI_Date *) elem->obj,
                             (OCIDate *) elem->handle, TRUE, FALSE),
        OcilibDateAssign((OCI_Date *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetTimestamp
(
    OCI_Elem      *elem,
    OCI_Timestamp *value
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    ELEM_SET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        OCI_Timestamp*,
        OcilibTimestampInitialize(elem->con, (OCI_Timestamp *) elem->obj,
                                  (OCIDateTime *) elem->handle,
                                  elem->typinf->cols[0].subtype),
        OcilibTimestampAssign((OCI_Timestamp *) elem->obj, value)
    )

#else

    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)

    EXIT_FUNC()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetInterval
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetInterval
(
    OCI_Elem     *elem,
    OCI_Interval *value
)
{

#if OCI_VERSION_COMPILE >= OCI_9_0

    ELEM_SET_VALUE
    (
        OCI_CDT_INTERVAL,
        OCI_Interval*,
        OcilibIntervalInitialize(elem->con, (OCI_Interval *) elem->obj,
                                 (OCIInterval *) elem->handle,
                                 elem->typinf->cols[0].subtype),
        OcilibIntervalAssign((OCI_Interval *) elem->obj, value)
    )

#else

    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)

    EXIT_FUNC()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetCollection
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetCollection
(
    OCI_Elem *elem,
    OCI_Coll *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Coll*,
        OcilibCollectionInitialize(elem->con, (OCI_Coll *) elem->obj, (OCIColl *) elem->handle,
                                   elem->typinf->cols[0].typinf),
        OcilibCollectionAssign((OCI_Coll *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetObject
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetObject
(
    OCI_Elem   *elem,
    OCI_Object *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object*,
        OcilibObjectInitialize(elem->con, (OCI_Object *) elem->obj, elem->handle,
                               elem->typinf->cols[0].typinf, NULL, -1, TRUE),
        OcilibObjectAssign((OCI_Object *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetLob
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetLob
(
    OCI_Elem *elem,
    OCI_Lob  *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_LOB,
        OCI_Lob*,
        OcilibLobInitialize(elem->con, (OCI_Lob *) elem->obj,
                            (OCILobLocator *) elem->handle,
                            elem->typinf->cols[0].subtype),
        OcilibLobAssign((OCI_Lob *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetFile
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetFile
(
    OCI_Elem *elem,
    OCI_File *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File*,
        OcilibFileInitialize(elem->con, (OCI_File *) elem->obj,
                             (OCILobLocator *) elem->handle,
                             elem->typinf->cols[0].subtype),
        OcilibFileAssign((OCI_File *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetReference
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetReference
(
    OCI_Elem *elem,
    OCI_Ref  *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_REF,
        OCI_Ref*,
        OcilibReferenceInitialize(elem->con, elem->typinf->cols[0].typinf,
                                  (OCI_Ref *) elem->obj, (OCIRef *) elem->handle),
        OcilibReferenceAssign((OCI_Ref *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementIsNull
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementIsNull
(
    OCI_Elem *elem
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_ELEMENT, elem
    )

    CHECK_PTR(OCI_IPC_ELEMENT, elem)

    CHECK_NULL(elem->pind)

    SET_RETVAL(OCI_IND_NULL == *elem->pind)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibElementSetNull
 * --------------------------------------------------------------------------------------------- */

boolean OcilibElementSetNull
(
    OCI_Elem *elem
)
{
    return OcilibElementSetNullIndicator(elem, OCI_IND_NULL);
}
