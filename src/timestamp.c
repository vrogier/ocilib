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
 *                             PRIVATE VARIABLES
 * ********************************************************************************************* */

#if OCI_VERSION_COMPILE >= OCI_9_0
static unsigned int TimestampTypeValues[] = { OCI_TIMESTAMP, OCI_TIMESTAMP_TZ, OCI_TIMESTAMP_LTZ };
#endif

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampInit
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_TimestampInit
(
    OCI_Connection *con,
    OCI_Timestamp  *tmsp,
    OCIDateTime    *buffer,
    ub4             type
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_ALLOCATE_DATA(OCI_IPC_TIMESTAMP, tmsp, 1);

    if (OCI_STATUS)
    {
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
            tmsp->err = OCILib.err;
            tmsp->env = OCILib.env;
        }

        /* allocate buffer if needed */

        if (!tmsp->handle || (OCI_OBJECT_ALLOCATED_ARRAY == tmsp->hstate))
        {
            if (OCI_OBJECT_ALLOCATED_ARRAY != tmsp->hstate)
            {
                OCI_STATUS = OCI_DescriptorAlloc((dvoid  *)tmsp->env, (dvoid **)(void *)&tmsp->handle, (ub4)OCI_ExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, type));
                tmsp->hstate = OCI_OBJECT_ALLOCATED;
            }
        }
        else
        {
            tmsp->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }

    /* check for failure */

    if (!OCI_STATUS && tmsp)
    {
        OCI_TimestampFree(tmsp);
        tmsp = NULL;
    }
#else

    OCI_NOT_USED(con)
    OCI_NOT_USED(type)
    OCI_NOT_USED(buffer)
    OCI_NOT_USED(ptmsp)

#endif

    return tmsp;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_API OCI_TimestampCreate
(
    OCI_Connection *con,
    unsigned int    type
)
{
    OCI_CALL_ENTER(OCI_Timestamp*, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL_CHECK_ENUM_VALUE(con, NULL, type, TimestampTypeValues, OTEXT("Timestamp type"))

    OCI_RETVAL = OCI_TimestampInit(con, NULL, NULL, type);
    OCI_STATUS = (NULL != OCI_RETVAL);

#else

    OCI_NOT_USED(type)

#endif

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampFree
(
    OCI_Timestamp *tmsp
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL_CHECK_OBJECT_FETCHED(tmsp)

    if (OCI_OBJECT_ALLOCATED == tmsp->hstate)
    {
        OCI_DescriptorFree((dvoid *)tmsp->handle, OCI_ExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, tmsp->type));
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != tmsp->hstate)
    {
        OCI_FREE(tmsp)
    }

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp ** OCI_API OCI_TimestampArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_CALL_ENTER(OCI_Timestamp **, NULL)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL_CHECK_ENUM_VALUE(con, NULL, type, TimestampTypeValues, OTEXT("Timestamp type"))

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_TIMESTAMP, type,
                          sizeof(OCIDateTime *), sizeof(OCI_Timestamp),
                          OCI_ExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, type), NULL);

    OCI_STATUS = (NULL != arr);

    if (OCI_STATUS)
    {
        OCI_RETVAL = (OCI_Timestamp **) arr->tab_obj;
    }

#else

    OCI_NOT_USED(arr)
    OCI_NOT_USED(type)
    OCI_NOT_USED(nbelem)

#endif

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampArrayFree
(
    OCI_Timestamp **tmsps
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ARRAY, tmsps)

    OCI_RETVAL = OCI_STATUS = OCI_ArrayFreeFromHandles((void **) tmsps);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_TimestampGetType
(
    OCI_Timestamp *tmsp
)
{
    OCI_GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_TIMESTAMP, tmsp, type, tmsp->con, NULL, tmsp->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateZoneToZone
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampAssign
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
)
{
    OCI_Timestamp *tmp_tmsp = NULL;
    OCI_Timestamp *tmp_tmsp_src = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CHECK_COMPAT(tmsp->con, tmsp->type == tmsp_src->type)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeAssign() fails with OCI_TIMESTAMP_LTZ timestamps */

    if (OCI_TIMESTAMP_LTZ == tmsp_src->type)
    {
        tmp_tmsp_src = OCI_TimestampCreate(tmsp_src->con, OCI_TIMESTAMP_TZ);
        tmp_tmsp     = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        OCI_STATUS = OCI_STATUS && OCI_TimestampConvert(tmp_tmsp_src, tmsp_src);
        OCI_STATUS = OCI_STATUS && OCI_TimestampConvert(tmp_tmsp, tmsp);
    }
    else
    {
        tmp_tmsp_src = tmsp_src;
        tmp_tmsp     = tmsp;
    }

    OCI_EXEC(OCIDateTimeAssign((dvoid *)tmp_tmsp->env, tmp_tmsp->err, tmp_tmsp_src->handle, tmp_tmsp->handle))

    /* converting back */

    if (OCI_TIMESTAMP_LTZ == tmsp_src->type)
    {
        OCI_STATUS = OCI_STATUS && OCI_TimestampConvert(tmsp_src, tmp_tmsp_src);
        OCI_STATUS = OCI_STATUS && OCI_TimestampConvert(tmsp, tmp_tmsp);
    }

    if (tmsp != tmp_tmsp)
    {
        OCI_TimestampFree(tmp_tmsp);
    }

    if (tmsp_src != tmp_tmsp_src)
    {
        OCI_TimestampFree(tmp_tmsp_src);
    }

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampCheck
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_TimestampCheck
(
    OCI_Timestamp *tmsp
)
{
    ub4 value = 0;

    OCI_CALL_ENTER(int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIDateTimeCheck((dvoid *)tmsp->env, tmsp->err, tmsp->handle, &value))

#endif

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampCompare
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_TimestampCompare
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2
)
{
    sword value = OCI_ERROR;

    OCI_CALL_ENTER(int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIDateTimeCompare((dvoid *)tmsp->env, tmsp->err, tmsp->handle, tmsp2->handle, &value))

#endif

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampConstruct
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampConstruct
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
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC
    (
        OCIDateTimeConstruct((dvoid *) tmsp->env, tmsp->err,
                             tmsp->handle,
                             (sb2) year, (ub1) month, (ub1) day,
                             (ub1) hour, (ub1) min,(ub1) sec,
                             (ub4) fsec, (OraText *) time_zone,
                             (size_t) (time_zone ? otextsize(time_zone) : 0))
    )

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

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampConvert
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampConvert
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIDateTimeConvert((dvoid *)tmsp->env, tmsp->err, tmsp_src->handle, tmsp->handle))

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampFromText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampFromText
(
    OCI_Timestamp *tmsp,
    const otext   *str,
    const otext   *fmt
)
{
    dbtext  *dbstr1  = NULL;
    dbtext  *dbstr2  = NULL;
    int      dbsize1 = -1;
    int      dbsize2 = -1;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, str)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (!OCI_STRING_VALID(fmt))
    {
        fmt = OCI_GetFormat(tmsp->con, tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP);
    }

    dbstr1 = OCI_StringGetOracleString(str, &dbsize1);
    dbstr2 = OCI_StringGetOracleString(fmt, &dbsize2);

    OCI_EXEC
    (
        OCIDateTimeFromText((dvoid *) tmsp->env, tmsp->err,
                            (OraText *) dbstr1, (size_t) dbsize1,
                            (OraText *) dbstr2, (ub1) dbsize2,
                            (OraText *) NULL, (size_t) 0,
                            tmsp->handle)
    )

    OCI_StringReleaseOracleString(dbstr1);
    OCI_StringReleaseOracleString(dbstr2);

#else

    OCI_NOT_USED(dbstr1)
    OCI_NOT_USED(dbstr2)
    OCI_NOT_USED(dbsize1)
    OCI_NOT_USED(dbsize2)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampToText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampToText
(
    OCI_Timestamp *tmsp,
    const otext   *fmt,
    int            size,
    otext         *str,
    int            precision
)
{
    dbtext *dbstr1  = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = size * (int) sizeof(otext);
    int     dbsize2 = -1;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, str)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (!OCI_STRING_VALID(fmt))
    {
        fmt = OCI_GetFormat(tmsp->con, tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP);
    }

    dbstr1 = OCI_StringGetOracleString(str, &dbsize1);
    dbstr2 = OCI_StringGetOracleString(fmt, &dbsize2);

    OCI_EXEC
    (
        OCIDateTimeToText((dvoid *) tmsp->env, tmsp->err,
                          tmsp->handle, (OraText *) dbstr2,
                          (ub1) dbsize2, (ub1) precision,
                          (OraText *) NULL, (size_t) 0,
                          (ub4*) &dbsize1, (OraText *) dbstr1)
    )

    OCI_StringCopyOracleStringToNativeString(dbstr1, str, dbcharcount(dbsize1));

    OCI_StringReleaseOracleString(dbstr1);
    OCI_StringReleaseOracleString(dbstr2);

    /* set null string terminator */

    str[dbcharcount(dbsize1)] = 0;

#else

    OCI_NOT_USED(dbstr1)
    OCI_NOT_USED(dbstr2)
    OCI_NOT_USED(dbsize1)
    OCI_NOT_USED(dbsize2)
    OCI_NOT_USED(precision)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampGetDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampGetDate
(
    OCI_Timestamp *tmsp,
    int           *year,
    int           *month,
    int           *day
)
{
    sb2 yr = 0;
    ub1 mt = 0;
    ub1 dy = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, year)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, month)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, day)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIDateTimeGetDate((dvoid *)tmsp->env, tmsp->err, tmsp->handle, &yr, &mt, &dy))

    *year  = (int) yr;
    *month = (int) mt;
    *day   = (int) dy;

#else

    OCI_NOT_USED(year)
    OCI_NOT_USED(month)
    OCI_NOT_USED(day)
    OCI_NOT_USED(yr)
    OCI_NOT_USED(mt)
    OCI_NOT_USED(dy)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampGetTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampGetTime
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min,
    int           *sec,
    int           *fsec
)
{
    ub1 hr = 0;
    ub1 mn = 0;
    ub1 sc = 0;
    ub4 fs = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, hour)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, min)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, sec)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, fsec)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

    *hour = 0;
    *min  = 0;
    *sec  = 0;
    *fsec = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIDateTimeGetTime((dvoid *)tmsp->env, tmsp->err, tmsp->handle, &hr, &mn, &sc, &fs))

    *hour = (int) hr;
    *min  = (int) mn;
    *sec  = (int) sc;
    *fsec = (int) fs;

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

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampGetDateTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampGetDateTime
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
    return (OCI_TimestampGetDate(tmsp, year, month, day) &&
            OCI_TimestampGetTime(tmsp, hour, min, sec, fsec));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampGetTimeZoneName
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampGetTimeZoneName
(
    OCI_Timestamp *tmsp,
    int            size,
    otext         *str
)
{
    dbtext *dbstr  = NULL;
    int     dbsize  = size * (int) sizeof(otext);

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, str)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    dbstr = OCI_StringGetOracleString(str, &dbsize);

    OCI_EXEC(OCIDateTimeGetTimeZoneName((dvoid *)tmsp->env, tmsp->err, tmsp->handle, (ub1*) dbstr, (ub4*) &dbsize))

    OCI_StringCopyOracleStringToNativeString(dbstr, str, dbcharcount(dbsize));
    OCI_StringReleaseOracleString(dbstr);

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

#else

    OCI_NOT_USED(str)
    OCI_NOT_USED(size)
    OCI_NOT_USED(dbstr)
    OCI_NOT_USED(dbsize)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampGetTimeZoneOffset
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampGetTimeZoneOffset
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min
)
{
    sb1 sb_hour = 0, sb_min = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, hour)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, min)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIDateTimeGetTimeZoneOffset((dvoid *)tmsp->env, tmsp->err, tmsp->handle, &sb_hour, &sb_min))

    *hour = sb_hour;
    *min  = sb_min;

#else

    OCI_NOT_USED(hour)
    OCI_NOT_USED(min)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampIntervalAdd
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampIntervalAdd
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
)
{
    OCI_Timestamp *tmp = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeIntervalAdd() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        tmp = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        OCI_STATUS = OCI_TimestampConvert(tmp, tmsp);
    }
    else
    {
        tmp = tmsp;
    }

    OCI_EXEC(OCIDateTimeIntervalAdd((dvoid *)tmp->env, tmp->err, tmp->handle, itv->handle, tmp->handle))

    /* converting back */

    if (OCI_STATUS && (OCI_TIMESTAMP_TZ != tmsp->type))
    {
        OCI_STATUS = OCI_TimestampConvert(tmsp, tmp);
    }

    if (tmsp != tmp)
    {
        OCI_TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampIntervalSub
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampIntervalSub
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
)
{
    OCI_Timestamp *tmp = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeIntervalSub() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        tmp = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        OCI_STATUS = OCI_TimestampConvert(tmp, tmsp);
    }
    else
    {
        tmp = tmsp;
    }

    OCI_EXEC(OCIDateTimeIntervalSub((dvoid *)tmp->env, tmp->err, tmp->handle, itv->handle, tmp->handle))

    /* converting back */

    if (OCI_STATUS && (OCI_TIMESTAMP_TZ != tmsp->type))
    {
        OCI_STATUS = OCI_TimestampConvert(tmsp, tmp);
    }

    if (tmsp != tmp)
    {
        OCI_TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampSubtract
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampSubtract
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2,
    OCI_Interval  *itv
)
{
    OCI_CALL_ENTER(boolean, FALSE)

    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIDateTimeSubtract((dvoid *)tmsp->env, tmsp->err, tmsp->handle, tmsp2->handle, itv->handle))

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampSysTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampSysTimestamp
(
    OCI_Timestamp *tmsp
)
{
    OCI_Timestamp *tmp  = NULL;
    OCIDateTime *handle = NULL;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

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
        tmp = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        handle = tmp->handle;
    }
    else
    {
        handle = tmsp->handle;
    }

    OCI_EXEC(OCIDateTimeSysTimeStamp((dvoid *) tmsp->env, tmsp->err, handle))

    if (OCI_STATUS && (OCI_TIMESTAMP == tmsp->type))
    {
        OCI_STATUS = OCI_TimestampConvert(tmsp, tmp);
    }

    if (tmp && tmsp != tmp)
    {
        OCI_TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp)
    OCI_NOT_USED(handle)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampToCTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampToCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t        *pt
)
{
    time_t time = (time_t) -1;
    int    msec = 0;
    struct tm t;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

    memset(&t, 0, sizeof(t));

    OCI_STATUS = OCI_TimestampGetDateTime(tmsp, &t.tm_year, &t.tm_mon, &t.tm_mday,
                                          &t.tm_hour, &t.tm_min, &t.tm_sec, &msec);

    if (OCI_STATUS)
    {
        t.tm_year -= 1900;
        t.tm_mon  -= 1;
        t.tm_wday  = 0;
        t.tm_yday  = 0;
        t.tm_isdst = -1;

        time = mktime(&t);

        if (ptm)
        {
            memcpy(ptm, &t, sizeof(t));
        }

        if (pt)
        {
            *pt = time;
        }
    }

    if (OCI_STATUS)
    {
        OCI_RETVAL = (time != (time_t)-1);
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampFromCTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampFromCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t         t
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    OCI_CALL_CHECK_TIMESTAMP_ENABLED(tmsp->con)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(tmsp)

    if (!ptm &&  t != (time_t)0)
    {
        ptm = localtime(&t);
    }

    if (!ptm)
    {
        OCI_RAISE_EXCEPTION(OCI_ExceptionNullPointer(OCI_IPC_TM))
    }

    OCI_RETVAL = OCI_STATUS = OCI_TimestampConstruct(tmsp, ptm->tm_year + 1900,  ptm->tm_mon  + 1,
                                                     ptm->tm_mday,  ptm->tm_hour,  ptm->tm_min,
                                                     ptm->tm_sec, (int) 0, (const otext *) NULL);

   OCI_CALL_EXIT()
}
