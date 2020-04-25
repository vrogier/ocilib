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

#include "element.h"

#include "collection.h"
#include "date.h"
#include "exception.h"
#include "file.h"
#include "interval.h"
#include "lob.h"
#include "macros.h"
#include "memory.h"
#include "number.h"
#include "object.h"
#include "reference.h"
#include "strings.h"
#include "timestamp.h"

#define ELEM_SET_VALUE(elemtype, type, func_init, func_assign)                          \
                                                                                        \
    CALL_ENTER(boolean, FALSE)                                                          \
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)                                               \
    CALL_CHECK_COMPAT((elem)->con, (elemtype) == (elem)->typinf->cols[0].datatype)      \
    CALL_CONTEXT_FROM_CON((elem)->con)                                                  \
                                                                                        \
    if (!value)                                                                         \
    {                                                                                   \
        STATUS = ElementSetNull(elem);                                                  \
    }                                                                                   \
    else                                                                                \
    {                                                                                   \
        if (!(elem)->obj)                                                               \
        {                                                                               \
            (elem)->obj = func_init;                                                    \
        }                                                                               \
                                                                                        \
        if ((elem)->obj)                                                                \
        {                                                                               \
            STATUS = func_assign;                                                       \
                                                                                        \
            if (STATUS)                                                                 \
            {                                                                           \
                ElementSetNullIndicator(elem, OCI_IND_NOTNULL);                         \
                (elem)->handle = ((type) (elem)->obj)->handle;                          \
            }                                                                           \
        }                                                                               \
    }                                                                                   \
                                                                                        \
    RETVAL = STATUS;                                                                    \
    CALL_EXIT()

#define ELEM_GET_VALUE(elemtype, type, func)                                            \
                                                                                        \
    CALL_ENTER(type, NULL)                                                              \
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)                                               \
    CALL_CHECK_COMPAT((elem)->con, (elemtype) == (elem)->typinf->cols[0].datatype)      \
    CALL_CONTEXT_FROM_CON((elem)->con)                                                  \
                                                                                        \
    if (OCI_IND_NULL != *(elem)->pind)                                                  \
    {                                                                                   \
        if (!(elem)->init)                                                              \
        {                                                                               \
            (elem)->obj = RETVAL = func;                                                \
                                                                                        \
            (elem)->init = (NULL != RETVAL);                                            \
        }                                                                               \
        else                                                                            \
        {                                                                               \
            RETVAL = (type) (elem)->obj;                                                \
        }                                                                               \
                                                                                        \
        STATUS = (elem)->init;                                                          \
    }                                                                                   \
                                                                                        \
    CALL_EXIT()


#define ELEM_GET_NUMBER(elem, number_type, type, value)                                 \
                                                                                        \
    CALL_ENTER(type, value)                                                             \
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)                                               \
    CALL_CHECK_COMPAT((elem)->con, OCI_CDT_NUMERIC == (elem)->typinf->cols[0].datatype) \
    CALL_CONTEXT_FROM_CON((elem)->con)                                                  \
                                                                                        \
    STATUS = ElemGetNumberInternal(elem, (void *) (&RETVAL),                            \
                                       (uword) (number_type));                          \
                                                                                        \
    CALL_EXIT()

#define ELEM_SET_NUMBER(elem, value, number_type)                                       \
                                                                                        \
    CALL_ENTER(boolean, FALSE)                                                          \
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)                                               \
    CALL_CHECK_COMPAT((elem)->con, OCI_CDT_NUMERIC == (elem)->typinf->cols[0].datatype) \
    CALL_CONTEXT_FROM_CON((elem)->con)                                                  \
                                                                                        \
    RETVAL = STATUS = ElementSetNumberInternal(elem, (void *) &(value),                 \
                                                        (uword) (number_type));         \
                                                                                        \
    CALL_EXIT()

/* --------------------------------------------------------------------------------------------- *
* ElemFreeAllocatedData
* --------------------------------------------------------------------------------------------- */

void ElemFreeAllocatedData
(
    OCI_Elem *elem
)
{
    if (elem->alloc && elem->handle)
    {
        switch (elem->typinf->cols[0].datatype)
        {
        case OCI_CDT_TEXT:
            StringFreeStringPtr(elem->con->env, (OCIString **) &elem->handle, elem->con->err);
            break;
        default:
            FREE(elem->handle)
                break;
        }
    }
}

/* --------------------------------------------------------------------------------------------- *
 * ElementInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * ElementInitialize
(
    OCI_Connection *con,
    OCI_Elem       *elem,
    void           *handle,
    OCIInd         *pind,
    OCI_TypeInfo   *typinf
)
{
    DECLARE_CTX(TRUE)
    CALL_CONTEXT_FROM_CON(con)

    ALLOC_DATA(OCI_IPC_ELEMENT, elem, 1);

    if (STATUS)
    {   
        elem->con = con;

        // Free previously allocated data before reassign new one
        ElemFreeAllocatedData(elem);

        // Reset members
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

        if (pind)
        {
            elem->pind = pind;
            elem->ind  = *elem->pind;
        }
        else
        {
            elem->pind = &elem->ind;
        }
    }

    /* check for failure */

    if (!STATUS && elem)
    {
        ElementFree(elem);
        elem = NULL;
    }

    return elem;
}

/* --------------------------------------------------------------------------------------------- *
 * ElementSetNullIndicator
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetNullIndicator
(
    OCI_Elem *elem,
    OCIInd    value
)
{
    boolean set = FALSE;

    if (!elem->pind)
    {
        return FALSE;
    }

    if (OCI_CDT_OBJECT == elem->typinf->cols[0].datatype)
    {
        OCI_Object *obj = (OCI_Object*) elem->obj;

        if (obj)
        {
            elem->pind = obj->tab_ind;
            set = TRUE;            
        }
        else
        {
            value = OCI_IND_NULL;
        }

    }
    
    if (!set)
    {
        *elem->pind = value;
    }

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * ElementSetNumberInternal
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetNumberInternal
(
    OCI_Elem *elem,
    void     *value,
    uword     flag
)
{
    OCI_Column *col = &elem->typinf->cols[0];
    const boolean res = NumberTranslateValue(elem->typinf->con, value, flag,
                                              elem->handle, col->subtype);

    if (res)
    {
        ElementSetNullIndicator(elem, OCI_IND_NOTNULL);
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetNumberInternal
 * --------------------------------------------------------------------------------------------- */

boolean ElemGetNumberInternal
(
    OCI_Elem *elem,
    void     *value,
    uword     flag
)
{
    boolean res = FALSE;

    CHECK(NULL == elem, FALSE)
    CHECK(NULL == value, FALSE)

    if (ElementIsNull(elem))
    {
        res = TRUE;
    }
    else if (OCI_CDT_NUMERIC == elem->typinf->cols[0].datatype)
    {
        OCI_Column *col = &elem->typinf->cols[0];
        res = NumberTranslateValue(elem->typinf->con, elem->handle, col->subtype, value, flag);
    }
    else if (OCI_CDT_TEXT == elem->typinf->cols[0].datatype)
    {
        res = NumberFromStringInternal(elem->con, value, flag, ElementGetString(elem), NULL);
    }
    else
    {
        ExceptionTypeNotCompatible(elem->con);
        res = FALSE;
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * ElementCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Elem * ElementCreate
(
    OCI_TypeInfo *typinf
)
{
    CALL_ENTER(OCI_Elem*, NULL)
    CALL_CHECK_PTR(OCI_IPC_TYPE_INFO, typinf)

    RETVAL = ElementInitialize(typinf->con, NULL, NULL, (OCIInd *)NULL, typinf);
    STATUS = (NULL != RETVAL);
  
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ElementFree
 * --------------------------------------------------------------------------------------------- */

boolean ElementFree
(
    OCI_Elem *elem
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_OBJECT_FETCHED(elem)
    CALL_CONTEXT_FROM_CON(elem->con)

    /* if the element has sub-objects that have been fetched, we need to free
       these objects */

    if (elem->obj)
    {
        OCI_Datatype * data = (OCI_Datatype *) elem->obj;

        if (OCI_OBJECT_FETCHED_CLEAN == data->hstate)
        {
            data->hstate = OCI_OBJECT_FETCHED_DIRTY;
        }

        FreeObjectFromType(elem->obj, elem->typinf->cols[0].datatype);
    }

    ElemFreeAllocatedData(elem);

    FREE(elem->tmpbuf)
    FREE(elem)

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ElementGetBoolean
 * --------------------------------------------------------------------------------------------- */

boolean ElementGetBoolean
(
    OCI_Elem *elem
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_COMPAT(elem->con, OCI_CDT_BOOLEAN == elem->typinf->cols[0].datatype)
    CALL_CONTEXT_FROM_CON(elem->con)

    if (!ElementIsNull(elem))
    {
        boolean *data = (boolean *)elem->handle;

        if (data)
        {
            RETVAL = *data;
        }
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ElementGetNumber
 * --------------------------------------------------------------------------------------------- */

OCI_Number * ElementGetNumber
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_NUMERIC,
        OCI_Number *,
        NumberInitialize(elem->con, (OCI_Number *) elem->obj, (OCINumber *) elem->handle)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElementGetShort
 * --------------------------------------------------------------------------------------------- */

short ElementGetShort
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_SHORT, short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ElementGetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

unsigned short ElementGetUnsignedShort
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_USHORT, unsigned short, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetInt
 * --------------------------------------------------------------------------------------------- */

int ElemGetInt
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_INT, int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ElementGetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

unsigned int ElementGetUnsignedInt
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_UINT, unsigned int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ElementGetBigInt
 * --------------------------------------------------------------------------------------------- */

big_int ElementGetBigInt
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_BIGINT, big_int, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ElementGetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

big_uint ElementGetUnsignedBigInt
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_BIGUINT, big_uint, 0)
}

/* --------------------------------------------------------------------------------------------- *
 * ElementGetDouble
 * --------------------------------------------------------------------------------------------- */

double ElementGetDouble
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_DOUBLE, double, 0.0)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetFloat
 * --------------------------------------------------------------------------------------------- */

float ElementGetFloat
(
    OCI_Elem *elem
)
{
    ELEM_GET_NUMBER(elem, OCI_NUM_FLOAT, float, 0.0f)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetString
 * --------------------------------------------------------------------------------------------- */

const otext * ElementGetString
(
    OCI_Elem *elem
)
{
    CALL_ENTER(otext *, NULL)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_COMPAT(elem->con, OCI_CDT_TEXT == elem->typinf->cols[0].datatype)
    CALL_CONTEXT_FROM_CON(elem->con)

    if (elem->handle)
    {
        RETVAL = StringFromStringPtr(elem->con->env, (OCIString *) elem->handle, &elem->tmpbuf, &elem->tmpsize);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetRaw
 * --------------------------------------------------------------------------------------------- */

unsigned int ElementGetRaw
(
    OCI_Elem    *elem,
    void        *value,
    unsigned int len
)
{
    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_COMPAT(elem->con, OCI_CDT_RAW == elem->typinf->cols[0].datatype)
    CALL_CONTEXT_FROM_CON(elem->con)

    if (elem->handle)
    {
        OCIRaw *raw = (OCIRaw *) elem->handle;
        const ub4 raw_len = OCIRawSize(elem->con->env, raw);

        if (len > raw_len)
        {
            len = raw_len;
        }

        memcpy(value, OCIRawPtr(elem->con->env, raw), (size_t) len);
        
        RETVAL = raw_len;
    }

    CALL_EXIT()
}

/* -------------------------------------------------------------------------------------------- *
* OCI_ElemGetRawSize
* --------------------------------------------------------------------------------------------- */

unsigned int ElementGetRawSize
(
    OCI_Elem    *elem
)
{
    CALL_ENTER(unsigned int, 0)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_COMPAT(elem->con, OCI_CDT_RAW == elem->typinf->cols[0].datatype)
    CALL_CONTEXT_FROM_CON(elem->con)

    if (elem->handle)
    {
        OCIRaw *raw = (OCIRaw *)elem->handle;
        RETVAL = OCIRawSize(elem->con->env, raw);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * ElementGetDate
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date *,
        DateInitialize(elem->con, (OCI_Date *) elem->obj, (OCIDate *) elem->handle, FALSE, FALSE)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * ElementGetTimestamp
(
    OCI_Elem *elem
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    ELEM_GET_VALUE
    (
        OCI_CDT_TIMESTAMP,
        OCI_Timestamp *,
        TimestampInitialize(elem->con, (OCI_Timestamp *) elem->obj, (OCIDateTime *) elem->handle, elem->typinf->cols[0].subtype)
    )

#else

    CALL_ENTER( OCI_Timestamp *, NULL)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetInterval
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * ElementGetInterval
(
    OCI_Elem *elem
)
{
#if OCI_VERSION_COMPILE >= OCI_9_0

    ELEM_GET_VALUE
    (
        OCI_CDT_INTERVAL,
        OCI_Interval *,
        IntervalInitialize(elem->con, (OCI_Interval *) elem->obj, (OCIInterval *) elem->handle, elem->typinf->cols[0].subtype)
    )
#else

    CALL_ENTER( OCI_Interval *, NULL)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetLob
 * --------------------------------------------------------------------------------------------- */

OCI_Lob * ElementGetLob
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_LOB, 
        OCI_Lob*,
        LobInitialize(elem->con, (OCI_Lob *) elem->obj, (OCILobLocator *) elem->handle, elem->typinf->cols[0].subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetFile
 * --------------------------------------------------------------------------------------------- */

OCI_File * ElementGetFile
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File*,
        FileInitialize(elem->con, (OCI_File *) elem->obj, (OCILobLocator *) elem->handle, elem->typinf->cols[0].subtype)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElementGetReference
 * --------------------------------------------------------------------------------------------- */

OCI_Ref * ElementGetReference
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_REF,
        OCI_Ref*,
        ReferenceInitialize(elem->con, elem->typinf->cols[0].typinf, (OCI_Ref *) elem->obj, (OCIRef *) elem->handle)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetObject
 * --------------------------------------------------------------------------------------------- */

OCI_Object * ElementGetObject
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object*,
        ObjectInitialize(elem->con, (OCI_Object *) elem->obj, elem->handle, elem->typinf->cols[0].typinf, NULL, -1, TRUE)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElemGetColl
 * --------------------------------------------------------------------------------------------- */

OCI_Coll * ElementGetCollection
(
    OCI_Elem *elem
)
{
    ELEM_GET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Coll*,
        CollectionInitialize(elem->con, (OCI_Coll *) elem->obj,  (OCIColl *) elem->handle, elem->typinf->cols[0].typinf)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetBoolean
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetBoolean
(
    OCI_Elem *elem,
    boolean   value
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_COMPAT(elem->con, OCI_CDT_BOOLEAN == elem->typinf->cols[0].datatype)
    CALL_CONTEXT_FROM_CON(elem->con)

    if (elem->handle)
    {
        boolean *data = (boolean *) elem->handle;
 
        *data = value;
     
        ElementSetNullIndicator(elem, OCI_IND_NOTNULL);
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ElementSetNumber
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetNumber
(
    OCI_Elem   *elem,
    OCI_Number *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_NUMERIC,
        OCI_Number*,
        NumberInitialize(elem->con, (OCI_Number *) elem->obj, (OCINumber *) elem->handle),
        NumberAssign((OCI_Number *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetShort
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetShort
(
    OCI_Elem *elem,
    short     value
)
{
    ELEM_SET_NUMBER(elem,value, OCI_NUM_SHORT)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetUnsignedShort
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetUnsignedShort
(
    OCI_Elem      *elem,
    unsigned short value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_USHORT)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetInt
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetInt
(
    OCI_Elem *elem,
    int       value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_INT)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetUnsignedInt
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetUnsignedInt
(
    OCI_Elem    *elem,
    unsigned int value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_UINT)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetBigInt
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetBigInt
(
    OCI_Elem *elem,
    big_int   value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_BIGINT)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetUnsignedBigInt
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetUnsignedBigInt
(
    OCI_Elem *elem,
    big_uint  value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_BIGUINT)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetDouble
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetDouble
(
    OCI_Elem *elem,
    double    value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_DOUBLE)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetFloat
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetFloat
(
    OCI_Elem *elem,
    float     value
)
{
    ELEM_SET_NUMBER(elem, value, OCI_NUM_FLOAT)
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetString
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetString
(
    OCI_Elem    *elem,
    const otext *value
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_COMPAT(elem->con, OCI_CDT_TEXT == elem->typinf->cols[0].datatype)
    CALL_CONTEXT_FROM_CON(elem->con)

    if (!value)
    {
        STATUS = ElementSetNull(elem);
    }
    else
    {
        elem->alloc = (elem->alloc || elem->handle == NULL);

        STATUS = StringToStringPtr(elem->con->env,  (OCIString **) &elem->handle, elem->con->err, value);

        if (STATUS)
        {
            ElementSetNullIndicator(elem, OCI_IND_NOTNULL);
        }
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetRaw
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetRaw
(
    OCI_Elem    *elem,
    void       * value,
    unsigned int len
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CHECK_COMPAT(elem->con, OCI_CDT_TEXT == elem->typinf->cols[0].datatype)
    CALL_CONTEXT_FROM_CON(elem->con)

    if (!value)
    {
        STATUS = ElementSetNull(elem);
    }
    else
    {
        EXEC(OCIRawAssignBytes(elem->con->env, elem->con->err, (ub1*) value, len, (OCIRaw **) &elem->handle))

        if (STATUS)
        {
            ElementSetNullIndicator(elem, OCI_IND_NOTNULL);
        }
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetDate
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetDate
(
    OCI_Elem *elem,
    OCI_Date *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_DATETIME,
        OCI_Date*,
        DateInitialize(elem->con, (OCI_Date *) elem->obj, (OCIDate *) elem->handle, TRUE, FALSE),
        DateAssign((OCI_Date *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetTimestamp
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
        TimestampInitialize(elem->con, (OCI_Timestamp *) elem->obj, (OCIDateTime *) elem->handle,  elem->typinf->cols[0].subtype),
        TimestampAssign((OCI_Timestamp *) elem->obj, value)
    )

#else

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetInterval
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetInterval
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
        IntervalInitialize(elem->con, (OCI_Interval *) elem->obj, (OCIInterval *) elem->handle, elem->typinf->cols[0].subtype),
        IntervalAssign((OCI_Interval *) elem->obj, value)
    )

#else

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_EXIT()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetColl
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetCollection
(
    OCI_Elem *elem,
    OCI_Coll *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_COLLECTION,
        OCI_Coll*,
        CollectionInitialize(elem->con, (OCI_Coll *) elem->obj, (OCIColl *) elem->handle, elem->typinf->cols[0].typinf),
        CollectionAssign((OCI_Coll *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetObject
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetObject
(
    OCI_Elem   *elem,
    OCI_Object *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_OBJECT,
        OCI_Object*,
        ObjectInitialize(elem->con, (OCI_Object *) elem->obj, elem->handle, elem->typinf->cols[0].typinf, NULL, -1, TRUE),
        ObjectAssign((OCI_Object *) elem->obj, value)
    )
 }

/* --------------------------------------------------------------------------------------------- *
 * ElemSetLob
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetLob
(
    OCI_Elem *elem,
    OCI_Lob  *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_LOB,
        OCI_Lob*,
        LobInitialize(elem->con, (OCI_Lob *) elem->obj, (OCILobLocator *) elem->handle, elem->typinf->cols[0].subtype),
        LobAssign((OCI_Lob *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElemSetFile
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetFile
(
    OCI_Elem *elem,
    OCI_File *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_FILE,
        OCI_File*,
        FileInitialize(elem->con, (OCI_File *) elem->obj, (OCILobLocator *) elem->handle, elem->typinf->cols[0].subtype),
        FileAssign((OCI_File *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElementSetReference
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetReference
(
    OCI_Elem *elem,
    OCI_Ref  *value
)
{
    ELEM_SET_VALUE
    (
        OCI_CDT_REF,
        OCI_Ref*,
        ReferenceInitialize(elem->con, elem->typinf->cols[0].typinf, (OCI_Ref *) elem->obj, (OCIRef *) elem->handle),
        ReferenceAssign((OCI_Ref *) elem->obj, value)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * ElemIsNull
 * --------------------------------------------------------------------------------------------- */

boolean ElementIsNull
(
    OCI_Elem *elem
)
{
    CALL_ENTER(boolean, TRUE)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CONTEXT_FROM_CON(elem->con)

    if (elem->pind)
    {
        RETVAL = (OCI_IND_NULL == *elem->pind);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * ElementSetNull
 * --------------------------------------------------------------------------------------------- */

boolean ElementSetNull
(
    OCI_Elem *elem
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ELEMENT, elem)
    CALL_CONTEXT_FROM_CON(elem->con)

    RETVAL = STATUS = ElementSetNullIndicator(elem, OCI_IND_NULL);

    CALL_EXIT()
}
