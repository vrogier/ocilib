/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@gmail.com>
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
#include "stringutils.h"

#if OCI_VERSION_COMPILE >= OCI_9_0

static const OCI_UInt32 IntervalTypeValues[] =
{
    OCI_INTERVAL_YM,
    OCI_INTERVAL_DS
};

#endif

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OcilibIntervalInitialize
(
    OCI_Environment *env,
    OCI_Connection  *con,
    OCI_Interval    *itv,
    OCIInterval     *buffer,
    ub4              type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Interval*, itv,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

#if OCI_VERSION_COMPILE >= OCI_9_0

    ALLOC_HANDLE(OCI_IPC_INTERVAL, itv);
    itv->handle = buffer;
    itv->type   = type;

    OCI_NOT_USED(con)

    /* allocate buffer if needed */

    if (NULL == itv->handle || (OCI_OBJECT_ALLOCATED_ARRAY == itv->hstate))
    {
        if (OCI_OBJECT_ALLOCATED_ARRAY != itv->hstate)
        {
            CHECK
            (
                OcilibMemoryAllocDescriptor
                (
                    env,
                    (dvoid  *)env->handle, (dvoid **)(void *)&itv->handle,
                    (ub4)OcilibExternalSubTypeToHandleType(OCI_CDT_INTERVAL, itv->type)
                )
            );

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
            OcilibIntervalFree(itv);
            itv = NULL;
        }

        SET_RETVAL(itv);
    );

#else

    OCI_NOT_USED(con);
    OCI_NOT_USED(type);
    OCI_NOT_USED(buffer);

    CHECK(FALSE);

    EXIT_FUNC();

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OcilibIntervalCreate
(
    OCI_Environment *env,
    OCI_IntervalType type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Interval*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    CHECK_INTERVAL_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0
    CHECK_ENUM_VALUE(type, IntervalTypeValues, OTEXT("Interval type"));
#endif

    SET_RETVAL(OcilibIntervalInitialize(env, NULL, NULL, NULL, type));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalFree
(
    OCI_Interval *itv
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    CHECK_PTR(OCI_IPC_INTERVAL, itv);
    CHECK_INTERVAL_ENABLED();
    CHECK_OBJECT_FETCHED(itv);

    if (OCI_OBJECT_ALLOCATED == itv->hstate)
    {
        OcilibMemoryFreeDescriptor
        (
            itv->env,
            (dvoid*)itv->handle,
            OcilibExternalSubTypeToHandleType(OCI_CDT_INTERVAL, itv->type)
        );
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != itv->hstate)
    {
        OcilibErrorResetSource(NULL, itv);

        FREE(itv);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Interval ** OcilibIntervalCreateArray
(
    OCI_Environment *env,
    OCI_IntervalType type,
    OCI_UInt32       nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Interval**, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    CHECK_INTERVAL_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0
    CHECK_ENUM_VALUE(type, IntervalTypeValues, OTEXT("Interval type"));
#endif

#if OCI_VERSION_COMPILE >= OCI_9_0

    arr = OcilibArrayCreate(env, NULL, nbelem, OCI_CDT_INTERVAL, type,
                            sizeof(OCIInterval*), sizeof(OCI_Interval),
                            OcilibExternalSubTypeToHandleType(OCI_CDT_INTERVAL, type), NULL);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Interval**)arr->tab_obj);

#else

    OCI_NOT_USED(env);
    OCI_NOT_USED(arr);
    OCI_NOT_USED(type);
    OCI_NOT_USED(nbelem);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalArrayFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalFreeArray
(
    OCI_Interval **itvs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, itvs);

    SET_RETVAL(OcilibArrayFreeFromHandles(itvs[0] ? itvs[0]->env : NULL, (void**)itvs));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalGetType
 * --------------------------------------------------------------------------------------------- */

OCI_IntervalType OcilibIntervalGetType
(
    OCI_Interval *itv
)
{
    GET_PROP
    (
        /* result */ OCI_IntervalType, OCI_UNKNOWN,
        /* handle */ OCI_IPC_INTERVAL, itv,
        /* member */ type
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalAssign
(
    OCI_Interval *itv,
    OCI_Interval *itv_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    CHECK_PTR(OCI_IPC_INTERVAL, itv);
    CHECK_PTR(OCI_IPC_INTERVAL, itv_src);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalAssign,
        (dvoid *) itv->env->handle, itv->env->err,
        itv_src->handle, itv->handle
    );

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalCheck
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibIntervalCheck
(
    OCI_Interval *itv
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, OCI_ERROR,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    ub4 value = (ub4) OCI_ERROR;

    CHECK_PTR(OCI_IPC_INTERVAL, itv);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalCheck,
        (dvoid *) itv->env->handle, itv->env->err,
        itv->handle, &value
    );

#endif

    SET_RETVAL((OCI_Int32) value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalCompare
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibIntervalCompare
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, 0,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    sword value = OCI_ERROR;

    CHECK_PTR(OCI_IPC_INTERVAL, itv);
    CHECK_PTR(OCI_IPC_INTERVAL, itv2);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalCompare,
        (dvoid *) itv->env->handle, itv->env->err,
        itv->handle, itv2->handle,
        &value
    );

#endif

    SET_RETVAL((OCI_Int32) value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalFromString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalFromString
(
    OCI_Interval* itv,
    const otext * str
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    CHECK_PTR(OCI_IPC_INTERVAL, itv);
    CHECK_PTR(OCI_IPC_STRING,   str);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalFromText,
        (dvoid *) itv->env->handle, itv->env->err,
        (OraText *) (str), (OCI_Size) OcilibStringDBSize(str),
        itv->handle
    );

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalToString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalToString
(
    OCI_Interval *itv,
    OCI_Int32     leading_prec,
    OCI_Int32     fraction_prec,
    OCI_Int32     size,
    otext        *str
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    dbtext   *dbstr  = NULL;
    OCI_Int32 dbsize = size * (OCI_Int32) sizeof(otext);
    OCI_Size  len    = 0;

    CHECK_PTR(OCI_IPC_INTERVAL, itv);
    CHECK_PTR(OCI_IPC_STRING,   str);

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    dbstr = (dbtext *) (str);

    len = (OCI_Size) dbsize;

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalToText,
        (dvoid *) itv->env->handle, itv->env->err,
        (OCIInterval *) itv->handle,
        (ub1) leading_prec, (ub1) fraction_prec,
        (OraText *) dbstr, (OCI_Size) dbsize,
        (OCI_Size *) &len
    );

#else

    OCI_NOT_USED(leading_prec);
    OCI_NOT_USED(fraction_prec);

#endif

    dbsize = (OCI_Int32)len;

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalFromTimeZone
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalFromTimeZone
(
    OCI_Interval *itv,
    const otext * str
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    CHECK_PTR(OCI_IPC_INTERVAL, itv);
    CHECK_PTR(OCI_IPC_STRING,   str);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalFromTZ,
        (dvoid *) itv->env->handle, itv->env->err,
        (CONST OraText *) (str),
        (OCI_Size) OcilibStringDBSize(str), itv->handle
    );

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalGetDaySecond
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalGetDaySecond
(
    OCI_Interval *itv,
    OCI_Int32    *day,
    OCI_Int32    *hour,
    OCI_Int32    *min,
    OCI_Int32    *sec,
    OCI_Int32    *nsec
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    CHECK_PTR(OCI_IPC_INTERVAL, itv);
    CHECK_PTR(OCI_IPC_INT,      hour);
    CHECK_PTR(OCI_IPC_INT,      min);
    CHECK_PTR(OCI_IPC_INT,      sec);
    CHECK_PTR(OCI_IPC_INT,      nsec);

    *day  = 0;
    *hour = 0;
    *min  = 0;
    *sec  = 0;
    *nsec = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalGetDaySecond,
        (dvoid *) itv->env->handle, itv->env->err,
        (sb4 *) day, (sb4 *) hour, (sb4 *) min,
        (sb4 *) sec, (sb4 *) nsec, itv->handle
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(day);
    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);
    OCI_NOT_USED(sec);
    OCI_NOT_USED(nsec);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalGetYearMonth
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalGetYearMonth
(
    OCI_Interval *itv,
    OCI_Int32    *year,
    OCI_Int32    *month
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    CHECK_PTR(OCI_IPC_INTERVAL, itv);
    CHECK_PTR(OCI_IPC_INT,      year);
    CHECK_PTR(OCI_IPC_INT,      month);

    *year  = 0;
    *month = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalGetYearMonth,
        (dvoid *) itv->env->handle, itv->env->err,
        (sb4 *) year, (sb4 *) month,
        itv->handle
    );

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalSetDaySecond
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalSetDaySecond
(
    OCI_Interval *itv,
    OCI_Int32     day,
    OCI_Int32     hour,
    OCI_Int32     min,
    OCI_Int32     sec,
    OCI_Int32     nsec
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    CHECK_PTR(OCI_IPC_INTERVAL, itv);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalSetDaySecond,
        (dvoid *) itv->env->handle, itv->env->err,
        (sb4) day, (sb4) hour, (sb4) min,
        (sb4) sec, (sb4) nsec, itv->handle
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(day);
    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);
    OCI_NOT_USED(sec);
    OCI_NOT_USED(nsec);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalSetYearMonth
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalSetYearMonth
(
    OCI_Interval *itv,
    OCI_Int32     year,
    OCI_Int32     month
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    CHECK_PTR(OCI_IPC_INTERVAL, itv);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalSetYearMonth,
        (dvoid *) itv->env->handle, itv->env->err,
        (sb4) year, (sb4) month, itv->handle
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(year);
    OCI_NOT_USED(month);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalAdd
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalAdd
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    CHECK_PTR(OCI_IPC_INTERVAL, itv);
    CHECK_PTR(OCI_IPC_INTERVAL, itv2);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalAdd,
        (dvoid *) itv->env->handle, itv->env->err, itv->handle,
        itv2->handle, itv->handle
    );

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibIntervalSubtract
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibIntervalSubtract
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_INTERVAL, itv
    );

    CHECK_PTR(OCI_IPC_INTERVAL, itv);
    CHECK_PTR(OCI_IPC_INTERVAL, itv2);

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        itv->env->err,
        OCIIntervalSubtract,
        (dvoid *) itv->env->handle, itv->env->err, itv->handle,
        itv2->handle, itv->handle
    );

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}
