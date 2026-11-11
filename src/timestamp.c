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

#include "timestamp.h"

#include "array.h"
#include "environment.h"
#include "helpers.h"
#include "macros.h"
#include "stringutils.h"

#if OCI_VERSION_COMPILE >= OCI_9_0

static OCI_UInt32 TimestampTypeValues[] =
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
    OCI_Environment *env,
    OCI_Connection  *con,
    OCI_Timestamp   *tmsp,
    OCIDateTime     *buffer,
    ub4              type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp*, tmsp,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

#if OCI_VERSION_COMPILE >= OCI_9_0

    ALLOC_HANDLE(OCI_IPC_TIMESTAMP, tmsp);
    tmsp->handle = buffer;
    tmsp->type   = type;

    OCI_NOT_USED(con)

    /* allocate buffer if needed */

    if (NULL == tmsp->handle || (OCI_OBJECT_ALLOCATED_ARRAY == tmsp->hstate))
    {
        if (OCI_OBJECT_ALLOCATED_ARRAY != tmsp->hstate)
        {
            CHECK
            (
                OcilibMemoryAllocDescriptor
                (
                    env,
                    (dvoid  *)env->handle,
                    (dvoid **)(void *)&tmsp->handle,
                    (ub4)OcilibExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, type)
                )
            );

            tmsp->hstate = OCI_OBJECT_ALLOCATED;
        }
    }
    else
    {
        tmsp->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

#else

    OCI_NOT_USED(con);
    OCI_NOT_USED(type);
    OCI_NOT_USED(buffer);

    CHECK(FALSE);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibTimestampFree(tmsp);
            tmsp = NULL;
        }

        SET_RETVAL(tmsp);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OcilibTimestampCreate
(
    OCI_Environment  *env,
    OCI_TimestampType type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, TimestampTypeValues, OTEXT("Timestamp type"));

    SET_RETVAL(OcilibTimestampInitialize(env, NULL, NULL, NULL, type));

#else

    OCI_NOT_USED(env);
    OCI_NOT_USED(type);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampFree
(
    OCI_Timestamp *tmsp
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OBJECT_FETCHED(tmsp);

    if (OCI_OBJECT_ALLOCATED == tmsp->hstate)
    {
        OcilibMemoryFreeDescriptor
        (
            tmsp->env,
            (dvoid*)tmsp->handle,
            OcilibExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, tmsp->type)
        );
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != tmsp->hstate)
    {
        OcilibErrorResetSource(NULL, tmsp);

        FREE(tmsp);
    }

#endif

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp ** OcilibTimestampCreateArray
(
    OCI_Environment  *env,
    OCI_TimestampType type,
    OCI_UInt32        nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Timestamp**, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(type, TimestampTypeValues, OTEXT("Timestamp type"));

    arr = OcilibArrayCreate(env, NULL, nbelem, OCI_CDT_TIMESTAMP, type,
                            sizeof(OCIDateTime*), sizeof(OCI_Timestamp),
                            OcilibExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, type), NULL);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Timestamp**)arr->tab_obj);

#else

    OCI_NOT_USED(env);
    OCI_NOT_USED(arr);
    OCI_NOT_USED(type);
    OCI_NOT_USED(nbelem);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampFreeArray
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampFreeArray
(
    OCI_Timestamp **tmsps
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, tmsps);

    SET_RETVAL(OcilibArrayFreeFromHandles(tmsps[0] ? tmsps[0]->env : NULL, (void **) tmsps));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetType
 * --------------------------------------------------------------------------------------------- */

OCI_TimestampType OcilibTimestampGetType
(
    OCI_Timestamp *tmsp
)
{
    GET_PROP
    (
        OCI_TimestampType, OCI_UNKNOWN,
        OCI_IPC_TIMESTAMP, tmsp,
        type
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampAssign
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    OCI_Timestamp *tmp_tmsp     = NULL;
    OCI_Timestamp *tmp_tmsp_src = NULL;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src);
    CHECK_TIMESTAMP_ENABLED();
    CHECK_COMPAT(tmsp->type == tmsp_src->type);

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeAssign() fails with OCI_TIMESTAMP_LTZ timestamps */

    if (OCI_TIMESTAMP_LTZ == tmsp_src->type)
    {
        tmp_tmsp_src = OcilibTimestampCreate(tmsp_src->env, OCI_TIMESTAMP_TZ);
        CHECK_NULL(tmp_tmsp_src);

        tmp_tmsp = OcilibTimestampCreate(tmsp->env, OCI_TIMESTAMP_TZ);
        CHECK_NULL(tmp_tmsp);

        CHECK(OcilibTimestampConvert(tmp_tmsp_src, tmsp_src));
        CHECK(OcilibTimestampConvert(tmp_tmsp, tmsp));
    }
    else
    {
        tmp_tmsp_src = tmsp_src;
        tmp_tmsp     = tmsp;
    }

    CHECK_OCI
    (
        tmp_tmsp->env->err,
        OCIDateTimeAssign,
        (dvoid *)tmp_tmsp->env->handle, tmp_tmsp->env->err,
        tmp_tmsp_src->handle, tmp_tmsp->handle
    );

    /* converting back */

    if (OCI_TIMESTAMP_LTZ == tmsp_src->type)
    {
        CHECK(OcilibTimestampConvert(tmsp_src, tmp_tmsp_src));
        CHECK(OcilibTimestampConvert(tmsp, tmp_tmsp));
    }

    SET_SUCCESS();

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
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampCheck
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibTimestampCheck
(
    OCI_Timestamp *tmsp
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, 0,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    ub4 value = 0;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeCheck,
        (dvoid *)tmsp->env->handle, tmsp->env->err,
        tmsp->handle, &value
    );

#endif

    SET_RETVAL((OCI_Int32) value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampCompare
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibTimestampCompare
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, OCI_ERROR,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    sword value = OCI_ERROR;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeCompare,
        (dvoid *)tmsp->env->handle, tmsp->env->err,
        tmsp->handle, tmsp2->handle,
        &value
    );

#endif

    SET_RETVAL(value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampConstruct
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampConstruct
(
    OCI_Timestamp *tmsp,
    OCI_Int32      year,
    OCI_Int32      month,
    OCI_Int32      day,
    OCI_Int32      hour,
    OCI_Int32      min,
    OCI_Int32      sec,
    OCI_Int32      fsec,
    const otext   *time_zone
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeConstruct,
        (dvoid *) tmsp->env->handle, tmsp->env->err,
        tmsp->handle,
        (sb2) year, (ub1) month, (ub1) day,
        (ub1) hour, (ub1) min,(ub1) sec,
        (ub4) fsec, (OraText *) time_zone,
        (OCI_Size) (time_zone ? otextsize(time_zone) : 0)
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(year);
    OCI_NOT_USED(month);
    OCI_NOT_USED(day);
    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);
    OCI_NOT_USED(sec);
    OCI_NOT_USED(fsec);
    OCI_NOT_USED(time_zone);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampConvert
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampConvert
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeConvert,
        (dvoid *)tmsp->env->handle, tmsp->env->err,
        tmsp_src->handle, tmsp->handle
    );

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampFromString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampFromString
(
    OCI_Timestamp *tmsp,
    const otext   *str,
    const otext   *fmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    dbtext   *dbstr1  = NULL;
    dbtext   *dbstr2  = NULL;
    OCI_Int32 dbsize1 = -1;
    OCI_Int32 dbsize2 = -1;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_STRING,    str);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (!IS_STRING_VALID(fmt))
    {
        fmt = OcilibEnvironmentGetFormat(tmsp->env, tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP);
        CHECK_NULL(fmt);
    }

    dbstr1 = (dbtext *) (str);
    dbsize1 = OcilibStringDBSize(str);
    dbstr2 = (dbtext *) (fmt);
    dbsize2 = OcilibStringDBSize(fmt);

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeFromText,
        (dvoid *) tmsp->env->handle, tmsp->env->err,
        (OraText *) dbstr1, (OCI_Size) dbsize1,
        (OraText *) dbstr2, (ub1) dbsize2,
        (OraText *) NULL, (OCI_Size) 0,
        tmsp->handle
    );

    SET_SUCCESS();

#else

    OCI_NOT_USED(dbstr1);
    OCI_NOT_USED(dbstr2);
    OCI_NOT_USED(dbsize1);
    OCI_NOT_USED(dbsize2);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampToString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampToString
(
    OCI_Timestamp *tmsp,
    const otext   *fmt,
    OCI_Int32      size,
    otext         *str,
    OCI_Int32      precision
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    dbtext   *dbstr1  = NULL;
    dbtext   *dbstr2  = NULL;
    OCI_Int32 dbsize1 = size * (OCI_Int32) sizeof(otext);
    OCI_Int32 dbsize2 = -1;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_STRING,    str);

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (!IS_STRING_VALID(fmt))
    {
        fmt = OcilibEnvironmentGetFormat(tmsp->env, tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP);
    }

    dbstr1 = (dbtext *) (str);
    dbstr2 = (dbtext *) (fmt);
    dbsize2 = OcilibStringDBSize(fmt);

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeToText,
        (dvoid *) tmsp->env->handle, tmsp->env->err,
        tmsp->handle, (OraText *) dbstr2,
        (ub1) dbsize2, (ub1) precision,
        (OraText *) NULL, (OCI_Size) 0,
        (ub4*) &dbsize1, (OraText *) dbstr1
    );


    /* set null string terminator */

    str[dbcharcount(dbsize1)] = 0;

    SET_SUCCESS();

#else

    OCI_NOT_USED(dbstr1);
    OCI_NOT_USED(dbstr2);
    OCI_NOT_USED(dbsize1);
    OCI_NOT_USED(dbsize2);
    OCI_NOT_USED(precision);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampGetDate
(
    OCI_Timestamp *tmsp,
    OCI_Int32     *year,
    OCI_Int32     *month,
    OCI_Int32     *day
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    sb2 yr = 0;
    ub1 mt = 0;
    ub1 dy = 0;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_INT,       year);
    CHECK_PTR(OCI_IPC_INT,       month);
    CHECK_PTR(OCI_IPC_INT,       day);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeGetDate,
        (dvoid *)tmsp->env->handle, tmsp->env->err,
        tmsp->handle, &yr, &mt, &dy
    );

    *year  = (OCI_Int32) yr;
    *month = (OCI_Int32) mt;
    *day   = (OCI_Int32) dy;

    SET_SUCCESS();

#else

    OCI_NOT_USED(year);
    OCI_NOT_USED(month);
    OCI_NOT_USED(day);
    OCI_NOT_USED(yr);
    OCI_NOT_USED(mt);
    OCI_NOT_USED(dy);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampGetTime
(
    OCI_Timestamp *tmsp,
    OCI_Int32     *hour,
    OCI_Int32     *min,
    OCI_Int32     *sec,
    OCI_Int32     *fsec
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    ub1 hr = 0;
    ub1 mn = 0;
    ub1 sc = 0;
    ub4 fs = 0;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_INT,       hour);
    CHECK_PTR(OCI_IPC_INT,       min);
    CHECK_PTR(OCI_IPC_INT,       sec);
    CHECK_PTR(OCI_IPC_INT,       fsec);
    CHECK_TIMESTAMP_ENABLED();

    *hour = 0;
    *min  = 0;
    *sec  = 0;
    *fsec = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeGetTime,
        (dvoid *)tmsp->env->handle, tmsp->env->err,
        tmsp->handle, &hr, &mn, &sc, &fs
    );

    *hour = (OCI_Int32) hr;
    *min  = (OCI_Int32) mn;
    *sec  = (OCI_Int32) sc;
    *fsec = (OCI_Int32) fs;

    SET_SUCCESS();

#else

    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);
    OCI_NOT_USED(sec);
    OCI_NOT_USED(fsec);
    OCI_NOT_USED(hr);
    OCI_NOT_USED(mn);
    OCI_NOT_USED(sc);
    OCI_NOT_USED(fs);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetDateTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampGetDateTime
(
    OCI_Timestamp *tmsp,
    OCI_Int32     *year,
    OCI_Int32     *month,
    OCI_Int32     *day,
    OCI_Int32     *hour,
    OCI_Int32     *min,
    OCI_Int32     *sec,
    OCI_Int32     *fsec
)
{
    return (OcilibTimestampGetDate(tmsp, year, month, day) &&
        OcilibTimestampGetTime(tmsp, hour, min, sec, fsec));
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetTimeZoneName
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampGetTimeZoneName
(
    OCI_Timestamp *tmsp,
    OCI_Int32      size,
    otext         *str
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    dbtext   *dbstr  = NULL;
    OCI_Int32 dbsize = size * (OCI_Int32) sizeof(otext);

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_STRING,    str);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    dbstr = (dbtext *) (str);

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeGetTimeZoneName,
        (dvoid *)tmsp->env->handle, tmsp->env->err,
        tmsp->handle, (ub1*) dbstr,
        (ub4*) &dbsize
    );


    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

    SET_SUCCESS();

#else

    OCI_NOT_USED(str);
    OCI_NOT_USED(size);
    OCI_NOT_USED(dbstr);
    OCI_NOT_USED(dbsize);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampGetTimeZoneOffset
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampGetTimeZoneOffset
(
    OCI_Timestamp *tmsp,
    OCI_Int32     *hour,
    OCI_Int32     *min
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    sb1 sb_hour = 0, sb_min = 0;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_INT,       hour);
    CHECK_PTR(OCI_IPC_INT,       min);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeGetTimeZoneOffset,
        (dvoid *)tmsp->env->handle, tmsp->env->err,
        tmsp->handle, &sb_hour, &sb_min
    );

    *hour = sb_hour;
    *min  = sb_min;

    SET_SUCCESS();

#else

    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampIntervalAdd
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampIntervalAdd
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    OCI_Timestamp *tmp = NULL;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_INTERVAL,  itv);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeIntervalAdd() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        tmp = OcilibTimestampCreate(tmsp->env, OCI_TIMESTAMP_TZ);
        CHECK_NULL(tmp);

        CHECK(OcilibTimestampConvert(tmp, tmsp));
    }
    else
    {
        tmp = tmsp;
    }

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeIntervalAdd,
        (dvoid *)tmp->env->handle, tmp->env->err,
        tmp->handle, itv->handle,
        tmp->handle
    );

    /* converting back */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        CHECK(OcilibTimestampConvert(tmsp, tmp));
    }

    SET_SUCCESS();

#else

    OCI_NOT_USED(tmp);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmp && tmsp != tmp)
        {
            OcilibTimestampFree(tmp);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampIntervalSub
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampIntervalSub
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    OCI_Timestamp *tmp = NULL;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_INTERVAL,  itv);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeIntervalSub() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        tmp = OcilibTimestampCreate(tmsp->env, OCI_TIMESTAMP_TZ);
        CHECK_NULL(tmp);

        CHECK(OcilibTimestampConvert(tmp, tmsp));
    }
    else
    {
        tmp = tmsp;
    }

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeIntervalSub,
        (dvoid *)tmp->env->handle, tmp->env->err,
        tmp->handle, itv->handle,
        tmp->handle
    );

    /* converting back */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        CHECK(OcilibTimestampConvert(tmsp, tmp));
    }

    SET_SUCCESS();

#else

    OCI_NOT_USED(tmp);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmp && tmsp != tmp)
        {
            OcilibTimestampFree(tmp);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampSubtract
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampSubtract
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2,
    OCI_Interval  *itv
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2);
    CHECK_PTR(OCI_IPC_INTERVAL,  itv);
    CHECK_TIMESTAMP_ENABLED();

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OCI
    (
        tmsp->env->err,
        OCIDateTimeSubtract,
        (dvoid *)tmsp->env->handle, tmsp->env->err,
        tmsp->handle, tmsp2->handle,
        itv->handle
    );

    SET_SUCCESS();

#endif

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampSysTimestamp
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampSysTimestamp
(
    OCI_Timestamp *tmsp
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    OCI_Timestamp *tmp_tmsp = tmsp;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_TIMESTAMP_ENABLED();

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
        tmp_tmsp = OcilibTimestampCreate(tmsp->env, OCI_TIMESTAMP_TZ);
    }

    CHECK_OCI
    (
        tmp_tmsp->env->err,
        OCIDateTimeSysTimeStamp,
        (dvoid *)tmp_tmsp->env->handle, tmp_tmsp->env->err, tmp_tmsp->handle
    );

    if (OCI_TIMESTAMP == tmsp->type)
    {
        CHECK(OcilibTimestampConvert(tmsp, tmp_tmsp));
    }

    SET_SUCCESS();

#else

    OCI_NOT_USED(tmp_tmsp);

#endif

    CLEANUP_AND_EXIT_FUNC
    (
        if (NULL != tmp_tmsp && tmsp != tmp_tmsp)
        {
            OcilibTimestampFree(tmp_tmsp);
        }
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampToCTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampToCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t        *pt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    time_t    time = (time_t) -1;
    OCI_Int32 msec = 0;
    struct tm t;

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_TIMESTAMP_ENABLED();

    memset(&t, 0, sizeof(t));

    CHECK(OcilibTimestampGetDateTime(tmsp, &t.tm_year, &t.tm_mon, &t.tm_mday,
                                     &t.tm_hour, &t.tm_min, &t.tm_sec, &msec));

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

    SET_RETVAL(time != (time_t)-1);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibTimestampFromCTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibTimestampFromCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t         t
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_TIMESTAMP, tmsp
    );

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp);
    CHECK_TIMESTAMP_ENABLED();

    if (NULL == ptm && (t == (time_t)0))
    {
        THROW(OcilibExceptionNullPointer, OCI_IPC_TM);
    }

    if (NULL == ptm)
    {
        ptm = localtime(&t);
    }

    CHECK(OcilibTimestampConstruct(tmsp, ptm->tm_year + 1900,  ptm->tm_mon  + 1,
                                   ptm->tm_mday,  ptm->tm_hour,  ptm->tm_min,
                                   ptm->tm_sec, (OCI_Int32) 0, (const otext *) NULL));

    SET_SUCCESS();

    EXIT_FUNC();
}
