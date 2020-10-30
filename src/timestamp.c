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

#include "timestamp.h"

#include "array.h"
#include "environment.h"
#include "helpers.h"
#include "macros.h"
#include "stringutils.h"

#if OCI_VERSION_COMPILE >= OCI_9_0

static unsigned int TimestampTypeValues[] =
{
    OCI_TIMESTAMP,
    OCI_TIMESTAMP_TZ,
    OCI_TIMESTAMP_LTZ
};

#endif

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibTimestampInitialize
(
    OCI_Connection *con,
    OCI_Timestamp  *tmsp,
    OCIDateTime    *buffer,
    ub4             type
)
{
    ENTER_FUNC_NO_CONTEXT
    (
        /* returns */ OCI_Timestamp*, tmsp
    )

#if OCI_VERSION_COMPILE >= OCI_9_0

    ALLOC_DATA(OCI_IPC_TIMESTAMP, tmsp, 1);

    tmsp->con    = con;
    tmsp->handle = buffer;
    tmsp->type   = type;

    /* get the right error handle */

    if (con)
    {
        tmsp->err = con->err;
        tmsp->env = con->env;
    }
    else
    {
        tmsp->err = Env.err;
        tmsp->env = Env.env;
    }

    /* allocate buffer if needed */

    if (NULL == tmsp->handle || (OCI_OBJECT_ALLOCATED_ARRAY == tmsp->hstate))
    {
        if (OCI_OBJECT_ALLOCATED_ARRAY != tmsp->hstate)
        {
            CHECK
            (
                OcilibMemoryAllocDescriptor
                (
                    (dvoid  *)tmsp->env,
                    (dvoid **)(void *)&tmsp->handle,
                    (ub4)OcilibExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, type)
                )
            )

            tmsp->hstate = OCI_OBJECT_ALLOCATED;
        }
    }
    else
    {
        tmsp->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

#else

    OCI_NOT_USED(con)
    OCI_NOT_USED(type)
    OCI_NOT_USED(buffer)

    CHECK(FALSE)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibTimestampFree(tmsp);
            tmsp = NULL;
        }

        SET_RETVAL(tmsp)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibTimestampCreate
(
    OCI_Connection *con,
    unsigned int    type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp*, NULL,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )

    CHECK_INITIALIZED()
    CHECK_TIMESTAMP_ENABLED(con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, TimestampTypeValues, OTEXT("Timestamp type"))

    SET_RETVAL(OcilibTimestampInitialize(con, NULL, NULL, type))

#else

    OCI_NOT_USED(type)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampFree
(
    OCI_Timestamp *tmsp
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OBJECT_FETCHED(tmsp)

    if (OCI_OBJECT_ALLOCATED == tmsp->hstate)
    {
        OcilibMemoryFreeDescriptor
        (
            (dvoid*)tmsp->handle,
            OcilibExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, tmsp->type)
        );
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != tmsp->hstate)
    {
        OcilibErrorResetSource(NULL, tmsp);

        FREE(tmsp)
    }

#endif

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp ** OcilibTimestampCreateArray
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp**, NULL,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )

    OCI_Array *arr = NULL;

    CHECK_TIMESTAMP_ENABLED(con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, TimestampTypeValues, OTEXT("Timestamp type"))

    arr = OcilibArrayCreate(con, nbelem, OCI_CDT_TIMESTAMP, type,
                            sizeof(OCIDateTime*), sizeof(OCI_Timestamp),
                            OcilibExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, type), NULL);

    CHECK_NULL(arr)

    SET_RETVAL((OCI_Timestamp**)arr->tab_obj)

#else

    OCI_NOT_USED(arr)
    OCI_NOT_USED(type)
    OCI_NOT_USED(nbelem)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampFreeArray
(
    OCI_Timestamp **tmsps
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_ARRAY, tmsps)

    SET_RETVAL(OcilibArrayFreeFromHandles((void **) tmsps))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OcilibTimestampGetType
(
    OCI_Timestamp *tmsp
)
{
    GET_PROP
    (
        unsigned int, OCI_UNKNOWN,
        OCI_IPC_TIMESTAMP, tmsp,
        type
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampAssign
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampAssign
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    OCI_Timestamp *tmp_tmsp = NULL;
    OCI_Timestamp *tmp_tmsp_src = NULL;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CHECK_COMPAT(tmsp->type == tmsp_src->type)

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeAssign() fails with OCI_TIMESTAMP_LTZ timestamps */

    if (OCI_TIMESTAMP_LTZ == tmsp_src->type)
    {
        tmp_tmsp_src = OcilibTimestampCreate(tmsp_src->con, OCI_TIMESTAMP_TZ);
        CHECK_NULL(tmp_tmsp_src)

        tmp_tmsp = OcilibTimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);
        CHECK_NULL(tmp_tmsp)

        CHECK(OcilibTimestampConvert(tmp_tmsp_src, tmsp_src))
        CHECK(OcilibTimestampConvert(tmp_tmsp, tmsp))
    }
    else
    {
        tmp_tmsp_src = tmsp_src;
        tmp_tmsp     = tmsp;
    }

    CHECK_OCI
    (
        tmp_tmsp->err,
        OCIDateTimeAssign,
        (dvoid *)tmp_tmsp->env, tmp_tmsp->err,
        tmp_tmsp_src->handle, tmp_tmsp->handle
    )

    /* converting back */

    if (OCI_TIMESTAMP_LTZ == tmsp_src->type)
    {
        CHECK(OcilibTimestampConvert(tmsp_src, tmp_tmsp_src))
        CHECK(OcilibTimestampConvert(tmsp, tmp_tmsp))
    }

    SET_SUCCESS()

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmp_tmsp && tmsp != tmp_tmsp)
        {
            OcilibTimestampFree(tmp_tmsp);
        }

        if (NULL != tmp_tmsp_src && tmsp_src != tmp_tmsp_src)
        {
            OcilibTimestampFree(tmp_tmsp_src);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampCheck
 * --------------------------------------------------------------------------------------------- */

int OcilibTimestampCheck
(
    OCI_Timestamp *tmsp
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    ub4 value = 0;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeCheck,
        (dvoid *)tmsp->env, tmsp->err,
        tmsp->handle, &value
    )

#endif

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampCompare
 * --------------------------------------------------------------------------------------------- */

int OcilibTimestampCompare
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2
)
{
    ENTER_FUNC
    (
        /* returns */ int, OCI_ERROR,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    sword value = OCI_ERROR;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeCompare,
        (dvoid *)tmsp->env, tmsp->err,
        tmsp->handle, tmsp2->handle,
        &value
    )

#endif

    SET_RETVAL(value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampConstruct
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampConstruct
(
    OCI_Timestamp *tmsp,
    int            year,
    int            month,
    int            day,
    int            hour,
    int            min,
    int            sec,
    int            fsec,
    const otext   *time_zone
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeConstruct,
        (dvoid *) tmsp->env, tmsp->err,
        tmsp->handle,
        (sb2) year, (ub1) month, (ub1) day,
        (ub1) hour, (ub1) min,(ub1) sec,
        (ub4) fsec, (OraText *) time_zone,
        (size_t) (time_zone ? otextsize(time_zone) : 0)
    )

    SET_SUCCESS()

#else

    OCI_NOT_USED(year)
    OCI_NOT_USED(month)
    OCI_NOT_USED(day)
    OCI_NOT_USED(hour)
    OCI_NOT_USED(min)
    OCI_NOT_USED(sec)
    OCI_NOT_USED(fsec)
    OCI_NOT_USED(time_zone)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampConvert
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampConvert
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeConvert,
        (dvoid *)tmsp->env, tmsp->err,
        tmsp_src->handle, tmsp->handle
    )

    SET_SUCCESS()

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampFromString
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampFromString
(
    OCI_Timestamp *tmsp,
    const otext   *str,
    const otext   *fmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    dbtext *dbstr1 = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = -1;
    int     dbsize2 = -1;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_STRING,    str)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (!IS_STRING_VALID(fmt))
    {
        fmt = OcilibEnvironmentGetFormat(tmsp->con, tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP);
        CHECK_NULL(fmt)
    }

    dbstr1 = OcilibStringGetDBString(str, &dbsize1);
    dbstr2 = OcilibStringGetDBString(fmt, &dbsize2);

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeFromText,
        (dvoid *) tmsp->env, tmsp->err,
        (OraText *) dbstr1, (size_t) dbsize1,
        (OraText *) dbstr2, (ub1) dbsize2,
        (OraText *) NULL, (size_t) 0,
        tmsp->handle
    )

    SET_SUCCESS()

#else

    OCI_NOT_USED(dbstr1)
    OCI_NOT_USED(dbstr2)
    OCI_NOT_USED(dbsize1)
    OCI_NOT_USED(dbsize2)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr1);
        OcilibStringReleaseDBString(dbstr2);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampToString
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampToString
(
    OCI_Timestamp *tmsp,
    const otext   *fmt,
    int            size,
    otext         *str,
    int            precision
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    dbtext *dbstr1 = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = size * (int) sizeof(otext);
    int     dbsize2 = -1;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_STRING,    str)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (!IS_STRING_VALID(fmt))
    {
        fmt = OcilibEnvironmentGetFormat(tmsp->con, tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP);
    }

    dbstr1 = OcilibStringGetDBString(str, &dbsize1);
    dbstr2 = OcilibStringGetDBString(fmt, &dbsize2);

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeToText,
        (dvoid *) tmsp->env, tmsp->err,
        tmsp->handle, (OraText *) dbstr2,
        (ub1) dbsize2, (ub1) precision,
        (OraText *) NULL, (size_t) 0,
        (ub4*) &dbsize1, (OraText *) dbstr1
    )

    OcilibStringCopyDBStringToNativeString(dbstr1, str, dbcharcount(dbsize1));

    /* set null string terminator */

    str[dbcharcount(dbsize1)] = 0;

    SET_SUCCESS()

#else

    OCI_NOT_USED(dbstr1)
    OCI_NOT_USED(dbstr2)
    OCI_NOT_USED(dbsize1)
    OCI_NOT_USED(dbsize2)
    OCI_NOT_USED(precision)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr1);
        OcilibStringReleaseDBString(dbstr2);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetDate
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampGetDate
(
    OCI_Timestamp *tmsp,
    int           *year,
    int           *month,
    int           *day
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    sb2 yr = 0;
    ub1 mt = 0;
    ub1 dy = 0;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_INT,       year)
    CHECK_PTR(OCI_IPC_INT,       month)
    CHECK_PTR(OCI_IPC_INT,       day)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeGetDate,
        (dvoid *)tmsp->env, tmsp->err,
        tmsp->handle, &yr, &mt, &dy
    )

    *year  = (int) yr;
    *month = (int) mt;
    *day   = (int) dy;

    SET_SUCCESS()

#else

    OCI_NOT_USED(year)
    OCI_NOT_USED(month)
    OCI_NOT_USED(day)
    OCI_NOT_USED(yr)
    OCI_NOT_USED(mt)
    OCI_NOT_USED(dy)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetTime
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampGetTime
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min,
    int           *sec,
    int           *fsec
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    ub1 hr = 0;
    ub1 mn = 0;
    ub1 sc = 0;
    ub4 fs = 0;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_INT,       hour)
    CHECK_PTR(OCI_IPC_INT,       min)
    CHECK_PTR(OCI_IPC_INT,       sec)
    CHECK_PTR(OCI_IPC_INT,       fsec)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

    *hour = 0;
    *min  = 0;
    *sec  = 0;
    *fsec = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeGetTime,
        (dvoid *)tmsp->env, tmsp->err,
        tmsp->handle, &hr, &mn, &sc, &fs
    )

    *hour = (int) hr;
    *min  = (int) mn;
    *sec  = (int) sc;
    *fsec = (int) fs;

    SET_SUCCESS()

#else

    OCI_NOT_USED(hour)
    OCI_NOT_USED(min)
    OCI_NOT_USED(sec)
    OCI_NOT_USED(fsec)
    OCI_NOT_USED(hr)
    OCI_NOT_USED(mn)
    OCI_NOT_USED(sc)
    OCI_NOT_USED(fs)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetDateTime
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampGetDateTime
(
    OCI_Timestamp *tmsp,
    int           *year,
    int           *month,
    int           *day,
    int           *hour,
    int           *min,
    int           *sec,
    int           *fsec
)
{
    return (OcilibTimestampGetDate(tmsp, year, month, day) &&
            OcilibTimestampGetTime(tmsp, hour, min, sec, fsec));
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetTimeZoneName
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampGetTimeZoneName
(
    OCI_Timestamp *tmsp,
    int            size,
    otext         *str
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    dbtext *dbstr = NULL;
    int dbsize = size * (int) sizeof(otext);

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_STRING,    str)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    dbstr = OcilibStringGetDBString(str, &dbsize);

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeGetTimeZoneName,
        (dvoid *)tmsp->env, tmsp->err,
        tmsp->handle, (ub1*) dbstr,
        (ub4*) &dbsize
    )

    OcilibStringCopyDBStringToNativeString(dbstr, str, dbcharcount(dbsize));

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

    SET_SUCCESS()

#else

    OCI_NOT_USED(str)
    OCI_NOT_USED(size)
    OCI_NOT_USED(dbstr)
    OCI_NOT_USED(dbsize)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetTimeZoneOffset
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampGetTimeZoneOffset
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    sb1 sb_hour = 0, sb_min = 0;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_INT,       hour)
    CHECK_PTR(OCI_IPC_INT,       min)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeGetTimeZoneOffset,
        (dvoid *)tmsp->env, tmsp->err,
        tmsp->handle, &sb_hour, &sb_min
    )

    *hour = sb_hour;
    *min  = sb_min;

    SET_SUCCESS()

#else

    OCI_NOT_USED(hour)
    OCI_NOT_USED(min)

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampIntervalAdd
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampIntervalAdd
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    OCI_Timestamp *tmp = NULL;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_INTERVAL,  itv)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeIntervalAdd() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        tmp = OcilibTimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);
        CHECK_NULL(tmp);

        CHECK(OcilibTimestampConvert(tmp, tmsp))
    }
    else
    {
        tmp = tmsp;
    }

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeIntervalAdd,
        (dvoid *)tmp->env, tmp->err,
        tmp->handle, itv->handle,
        tmp->handle
    )

    /* converting back */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        CHECK(OcilibTimestampConvert(tmsp, tmp))
    }

    SET_SUCCESS()

#else

    OCI_NOT_USED(tmp)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmp && tmsp != tmp)
        {
            OcilibTimestampFree(tmp);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampIntervalSub
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampIntervalSub
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    OCI_Timestamp *tmp = NULL;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_INTERVAL,  itv)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeIntervalSub() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        tmp = OcilibTimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);
        CHECK_NULL(tmp)

        CHECK(OcilibTimestampConvert(tmp, tmsp))
    }
    else
    {
        tmp = tmsp;
    }

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeIntervalSub,
        (dvoid *)tmp->env, tmp->err,
        tmp->handle, itv->handle,
        tmp->handle
    )

    /* converting back */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        CHECK(OcilibTimestampConvert(tmsp, tmp))
    }

    SET_SUCCESS()

#else

    OCI_NOT_USED(tmp)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmp && tmsp != tmp)
        {
            OcilibTimestampFree(tmp);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampSubtract
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampSubtract
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2,
    OCI_Interval  *itv
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2)
    CHECK_PTR(OCI_IPC_INTERVAL,  itv)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeSubtract,
        (dvoid *)tmsp->env, tmsp->err,
        tmsp->handle, tmsp2->handle,
        itv->handle
    )

    SET_SUCCESS()

#endif

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampSysTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampSysTimestamp
(
    OCI_Timestamp *tmsp
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    OCI_Timestamp *tmp = NULL;
    OCIDateTime *handle = NULL;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* Filling a timestamp handle of type OCI_TIMESTAMP with
       OCIDateTimeSysTimestamp() can lead later to an error ORA-01483 when
       binding the given timestamp to some SQL Statement (Oracle BUG).
       The only way to avoid that is to pass to OCIDateTimeSysTimestamp()
       a timestamp handle of type OCI_TIMESTAMP_TZ and convert it back to
       OCI_TIMESTAMP if needed
    */

    if (OCI_TIMESTAMP == tmsp->type)
    {
        tmp = OcilibTimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);
        CHECK_NULL(tmp)

        handle = tmp->handle;
    }
    else
    {
        handle = tmsp->handle;
    }

    CHECK_OCI
    (
        tmsp->err,
        OCIDateTimeSysTimeStamp,
        (dvoid *) tmsp->env, tmsp->err, handle
    )

    if (OCI_TIMESTAMP == tmsp->type)
    {
        CHECK(OcilibTimestampConvert(tmsp, tmp))
    }

    SET_SUCCESS()

#else

    OCI_NOT_USED(tmp)
    OCI_NOT_USED(handle)

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmp && tmsp != tmp)
        {
            OcilibTimestampFree(tmp);
        }
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampToCTime
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampToCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t        *pt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    time_t time = (time_t) -1;
    int       msec = 0;
    struct tm t;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

    memset(&t, 0, sizeof(t));

    CHECK(OcilibTimestampGetDateTime(tmsp, &t.tm_year, &t.tm_mon, &t.tm_mday,
                                     &t.tm_hour, &t.tm_min, &t.tm_sec, &msec))

    t.tm_year -= 1900;
    t.tm_mon  -= 1;
    t.tm_wday  = 0;
    t.tm_yday  = 0;
    t.tm_isdst = -1;

    time = mktime(&t);

    if (NULL != ptm)
    {
        memcpy(ptm, &t, sizeof(t));
    }

    if (NULL != pt)
    {
        *pt = time;
    }

    SET_RETVAL(time != (time_t)-1)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampFromCTime
 * --------------------------------------------------------------------------------------------- */

boolean OcilibTimestampFromCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t         t
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    )

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)

    if (NULL == ptm && (t == (time_t)0))
    {
        THROW(OcilibExceptionNullPointer, OCI_IPC_TM)
    }

    if (NULL == ptm)
    {
        ptm = localtime(&t);
    }

    CHECK(OcilibTimestampConstruct(tmsp, ptm->tm_year + 1900,  ptm->tm_mon  + 1,
                                   ptm->tm_mday,  ptm->tm_hour,  ptm->tm_min,
                                   ptm->tm_sec, (int) 0, (const otext *) NULL))

    SET_SUCCESS()

    EXIT_FUNC()
}
