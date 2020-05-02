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

#include "interval.h"

#include "array.h"
#include "helpers.h"
#include "macros.h"
#include "strings.h"

#if OCI_VERSION_COMPILE >= OCI_9_0

static const unsigned int IntervalTypeValues[] =
{
    OCI_INTERVAL_YM,
    OCI_INTERVAL_DS
};

#endif

/* --------------------------------------------------------------------------------------------- *
 * IntervalInit
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * IntervalInitialize
(
    OCI_Connection *con,
    OCI_Interval   *itv,
    OCIInterval    *buffer,
    ub4             type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Interval*, itv,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )

#if OCI_VERSION_COMPILE >= OCI_9_0

    ALLOC_DATA(OCI_IPC_INTERVAL, itv, 1);

    itv->con    = con;
    itv->handle = buffer;
    itv->type   = type;

    /* get the right error handle */

    itv->err = con ? con->err : Env.err;
    itv->env = con ? con->env : Env.env;

    /* allocate buffer if needed */

    if (NULL == itv->handle || (OCI_OBJECT_ALLOCATED_ARRAY == itv->hstate))
    {
        if (OCI_OBJECT_ALLOCATED_ARRAY != itv->hstate)
        {
            CHECK
            (
                MemoryAllocDescriptor
                (
                     (dvoid  *)itv->env, (dvoid **)(void *)&itv->handle,
                     (ub4)ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, itv->type)
                )
            )

                itv->hstate = OCI_OBJECT_ALLOCATED;
        }
    }
    else
    {
        itv->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    CLEANUP_AND_EXIT_FUNC
    (       
        if (FAILURE)
        {
            IntervalFree(itv);
            itv = NULL;
        }

        SET_RETVAL(itv)
    )

#else

    OCI_NOT_USED(con)
    OCI_NOT_USED(type)
    OCI_NOT_USED(buffer)

    CHECK(FALSE)

    EXIT_FUNC()

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * IntervalCreate
(
    OCI_Connection *con,
    unsigned int    type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Interval*, NULL,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )
    CHECK_INITIALIZED()
    CHECK_INTERVAL_ENABLED(con)

#if OCI_VERSION_COMPILE >= OCI_9_0
    CHECK_ENUM_VALUE(type, IntervalTypeValues, OTEXT("Interval type"))
#endif

    SET_RETVAL(IntervalInitialize(con, NULL, NULL, type))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalFree
 * --------------------------------------------------------------------------------------------- */

boolean IntervalFree
(
    OCI_Interval *itv
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_INTERVAL_ENABLED(itv->con)
    CHECK_OBJECT_FETCHED(itv);

    if (OCI_OBJECT_ALLOCATED == itv->hstate)
    {
        MemoryFreeDescriptor
        (
            (dvoid*)itv->handle,
            ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, itv->type)
        );
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != itv->hstate)
    {
        FREE(itv)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Interval ** IntervalCreateArray
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Interval**, NULL,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )

    OCI_Array *arr = NULL;

    CHECK_INTERVAL_ENABLED(con)

#if OCI_VERSION_COMPILE >= OCI_9_0
    CHECK_ENUM_VALUE(type, IntervalTypeValues, OTEXT("Interval type"))
#endif

#if OCI_VERSION_COMPILE >= OCI_9_0

    arr = ArrayCreate(con, nbelem, OCI_CDT_INTERVAL, type,
                      sizeof(OCIInterval*), sizeof(OCI_Interval),
                      ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, type), NULL);

    CHECK_NULL(arr)

    SET_RETVAL((OCI_Interval**)arr->tab_obj)

#else

    OCI_NOT_USED(arr)
    OCI_NOT_USED(type)
    OCI_NOT_USED(nbelem)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean IntervalFreeArray
(
    OCI_Interval **itvs
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_ARRAY, itvs)

    SET_RETVAL(ArrayFreeFromHandles((void**)itvs))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int IntervalGetType
(
    OCI_Interval *itv
)
{
    GET_PROP
    (
        /* result */ unsigned int, OCI_UNKNOWN,
        /* handle */ OCI_IPC_INTERVAL, itv,
        /* member */ type
    )
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalAssign
 * --------------------------------------------------------------------------------------------- */

boolean IntervalAssign
(
    OCI_Interval *itv,
    OCI_Interval *itv_src
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INTERVAL, itv_src)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err,
        OCIIntervalAssign, 
        (dvoid *) itv->env, itv->err, 
        itv_src->handle, itv->handle
    )

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalCheck
 * --------------------------------------------------------------------------------------------- */

int IntervalCheck
(
    OCI_Interval *itv
)
{
    ENTER_FUNC
    (
        /* returns */ int , OCI_ERROR,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    ub4 value = (ub4) OCI_ERROR;

    CHECK_PTR(OCI_IPC_INTERVAL, itv)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err, 
        OCIIntervalCheck, 
        (dvoid *) itv->env, itv->err,
        itv->handle, &value
    )

#endif

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalCompare
 * --------------------------------------------------------------------------------------------- */

int IntervalCompare
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    sword value = OCI_ERROR;

    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INTERVAL, itv2)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err, 
        OCIIntervalCompare,
        (dvoid *) itv->env, itv->err, 
        itv->handle, itv2->handle, 
        &value
    )

#endif

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalFromString
 * --------------------------------------------------------------------------------------------- */

boolean IntervalFromString
(
    OCI_Interval* itv,
    const otext * str
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_STRING, str)

    dbstr = StringGetDBString(str, &dbsize);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err,
        OCIIntervalFromText,
        (dvoid *) itv->env, itv->err, 
        (OraText *) dbstr, (size_t) dbsize,
        itv->handle
    )

#endif

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalToString
 * --------------------------------------------------------------------------------------------- */

boolean IntervalToString
(
    OCI_Interval *itv,
    int           leading_prec,
    int           fraction_prec,
    int           size,
    otext        *str
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    dbtext *dbstr  = NULL;
    int     dbsize = size * (int)   sizeof(otext);
    size_t  len    = 0;

    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_STRING, str)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    dbstr = StringGetDBString(str, &dbsize);

    len = (size_t) dbsize;

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err,
        OCIIntervalToText,
        (dvoid *) itv->env, itv->err,
        (OCIInterval *) itv->handle,
        (ub1) leading_prec, (ub1) fraction_prec,
        (OraText *) dbstr, (size_t) dbsize,
        (size_t *) &len
    )

#else

    OCI_NOT_USED(leading_prec)
    OCI_NOT_USED(fraction_prec)

#endif

    dbsize = (int)len;

    StringCopyDBStringToNativeString(dbstr, str, dbcharcount(dbsize));

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalFromTimeZone
 * --------------------------------------------------------------------------------------------- */

boolean IntervalFromTimeZone
(
    OCI_Interval *itv,
    const otext * str
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_STRING, str)

    dbstr = StringGetDBString(str, &dbsize);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err,
        OCIIntervalFromTZ,
        (dvoid *) itv->env, itv->err,
        (CONST OraText *) dbstr, 
        (size_t) dbsize, itv->handle
    )

#endif

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalGetDaySecond
 * --------------------------------------------------------------------------------------------- */

boolean IntervalGetDaySecond
(
    OCI_Interval *itv,
    int          *day,
    int          *hour,
    int          *min,
    int          *sec,
    int          *fsec
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INT, hour)
    CHECK_PTR(OCI_IPC_INT, min)
    CHECK_PTR(OCI_IPC_INT, sec)
    CHECK_PTR(OCI_IPC_INT, fsec)

    *day  = 0;
    *hour = 0;
    *min  = 0;
    *sec  = 0;
    *fsec = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err,
        OCIIntervalGetDaySecond,
        (dvoid *) itv->env, itv->err,
        (sb4 *) day, (sb4 *) hour, (sb4 *) min,
        (sb4 *) sec, (sb4 *) fsec, itv->handle
    )

#else

    OCI_NOT_USED(day)
    OCI_NOT_USED(hour)
    OCI_NOT_USED(min)
    OCI_NOT_USED(sec)
    OCI_NOT_USED(fsec)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalGetYearMonth
 * --------------------------------------------------------------------------------------------- */

boolean IntervalGetYearMonth
(
    OCI_Interval *itv,
    int          *year,
    int          *month
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INT, year)
    CHECK_PTR(OCI_IPC_INT, month)

    *year  = 0;
    *month = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err,
        OCIIntervalGetYearMonth,
        (dvoid *) itv->env, itv->err,
        (sb4 *) year, (sb4 *) month,
        itv->handle
    )

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalSetDaySecond
 * --------------------------------------------------------------------------------------------- */

boolean IntervalSetDaySecond
(
    OCI_Interval *itv,
    int           day,
    int           hour,
    int           min,
    int           sec,
    int           fsec
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    CHECK_PTR(OCI_IPC_INTERVAL, itv)


#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err,
        OCIIntervalSetDaySecond,
        (dvoid *) itv->env, itv->err,
        (sb4) day, (sb4) hour, (sb4) min,
        (sb4) sec, (sb4) fsec, itv->handle
    )

#else

    OCI_NOT_USED(day)
    OCI_NOT_USED(hour)
    OCI_NOT_USED(min)
    OCI_NOT_USED(sec)
    OCI_NOT_USED(fsec)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalSetYearMonth
 * --------------------------------------------------------------------------------------------- */

boolean IntervalSetYearMonth
(
    OCI_Interval *itv,
    int           year,
    int           month
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    CHECK_PTR(OCI_IPC_INTERVAL, itv)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err,
        OCIIntervalSetYearMonth, 
        (dvoid *) itv->env, itv->err, 
        (sb4) year, (sb4) month, itv->handle
    )

#else

    OCI_NOT_USED(year)
    OCI_NOT_USED(month)

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalAdd
 * --------------------------------------------------------------------------------------------- */

boolean IntervalAdd
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INTERVAL, itv2)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err,
        OCIIntervalAdd,
        (dvoid *) itv->env, itv->err, itv->handle,
        itv2->handle, itv->handle
    )

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalSubtract
 * --------------------------------------------------------------------------------------------- */

boolean IntervalSubtract
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    )

    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INTERVAL, itv2)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->err,
        OCIIntervalSubtract,
        (dvoid *) itv->env, itv->err, itv->handle,
        itv2->handle, itv->handle
    )

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}
