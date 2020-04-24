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
#include "macros.h"
#include "strings.h"

#if OCI_VERSION_COMPILE >= OCI_9_0
static unsigned int TimestampTypeValues[] = { OCI_TIMESTAMP, OCI_TIMESTAMP_TZ, OCI_TIMESTAMP_LTZ };
#endif

/* --------------------------------------------------------------------------------------------- *
 * TimestampInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * TimestampInitialize
(
    OCI_Connection *con,
    OCI_Timestamp  *tmsp,
    OCIDateTime    *buffer,
    ub4             type
)
{
    DECLARE_CTX(TRUE)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    ALLOC_DATA(OCI_IPC_TIMESTAMP, tmsp, 1);

    if (STATUS)
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
                STATUS = MemoryAllocDescriptor((dvoid  *)tmsp->env, (dvoid **)(void *)&tmsp->handle, (ub4)ExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, type));
                tmsp->hstate = OCI_OBJECT_ALLOCATED;
            }
        }
        else
        {
            tmsp->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }

    /* check for failure */

    if (!STATUS && tmsp)
    {
        TimestampFree(tmsp);
        tmsp = NULL;
    }
#else

    OCI_NOT_USED(con)
    OCI_NOT_USED(type)
    OCI_NOT_USED(buffer)

#endif

    return tmsp;
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * TimestampCreate
(
    OCI_Connection *con,
    unsigned int    type
)
{
    CALL_ENTER(OCI_Timestamp*, NULL)
    CHECK_INITIALIZED()
    CHECK_TIMESTAMP_ENABLED(con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(con, NULL, type, TimestampTypeValues, OTEXT("Timestamp type"))

    RETVAL = TimestampInitialize(con, NULL, NULL, type);
    STATUS = (NULL != RETVAL);

#else

    OCI_NOT_USED(type)

#endif

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampFree
 * --------------------------------------------------------------------------------------------- */

boolean TimestampFree
(
    OCI_Timestamp *tmsp
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_OBJECT_FETCHED(tmsp)

    if (OCI_OBJECT_ALLOCATED == tmsp->hstate)
    {
        MemoryFreeDescriptor((dvoid *)tmsp->handle, ExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, tmsp->type));
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != tmsp->hstate)
    {
        FREE(tmsp)
    }

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp ** TimestampCreateArray
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    CALL_ENTER(OCI_Timestamp **, NULL)
    CHECK_TIMESTAMP_ENABLED(con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    CHECK_ENUM_VALUE(con, NULL, type, TimestampTypeValues, OTEXT("Timestamp type"))

    arr = ArrayCreate(con, nbelem, OCI_CDT_TIMESTAMP, type,
                      sizeof(OCIDateTime *), sizeof(OCI_Timestamp),
                      ExternalSubTypeToHandleType(OCI_CDT_TIMESTAMP, type), NULL);

    STATUS = (NULL != arr);

    if (STATUS)
    {
        RETVAL = (OCI_Timestamp **) arr->tab_obj;
    }

#else

    OCI_NOT_USED(arr)
    OCI_NOT_USED(type)
    OCI_NOT_USED(nbelem)

#endif

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean TimestampFreeArray
(
    OCI_Timestamp **tmsps
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_ARRAY, tmsps)

    RETVAL = STATUS = ArrayFreeFromHandles((void **) tmsps);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int TimestampGetType
(
    OCI_Timestamp *tmsp
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_TIMESTAMP, tmsp, type, tmsp->con, NULL, tmsp->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateZoneToZone
 * --------------------------------------------------------------------------------------------- */

boolean TimestampAssign
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
)
{
    OCI_Timestamp *tmp_tmsp = NULL;
    OCI_Timestamp *tmp_tmsp_src = NULL;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CHECK_COMPAT(tmsp->con, tmsp->type == tmsp_src->type)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeAssign() fails with OCI_TIMESTAMP_LTZ timestamps */

    if (OCI_TIMESTAMP_LTZ == tmsp_src->type)
    {
        tmp_tmsp_src = TimestampCreate(tmsp_src->con, OCI_TIMESTAMP_TZ);
        tmp_tmsp     = TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        STATUS = STATUS && TimestampConvert(tmp_tmsp_src, tmsp_src);
        STATUS = STATUS && TimestampConvert(tmp_tmsp, tmsp);
    }
    else
    {
        tmp_tmsp_src = tmsp_src;
        tmp_tmsp     = tmsp;
    }

    EXEC(OCIDateTimeAssign((dvoid *)tmp_tmsp->env, tmp_tmsp->err, tmp_tmsp_src->handle, tmp_tmsp->handle))

    /* converting back */

    if (OCI_TIMESTAMP_LTZ == tmsp_src->type)
    {
        STATUS = STATUS && TimestampConvert(tmsp_src, tmp_tmsp_src);
        STATUS = STATUS && TimestampConvert(tmsp, tmp_tmsp);
    }

    if (tmsp != tmp_tmsp)
    {
        TimestampFree(tmp_tmsp);
    }

    if (tmsp_src != tmp_tmsp_src)
    {
        TimestampFree(tmp_tmsp_src);
    }

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampCheck
 * --------------------------------------------------------------------------------------------- */

int TimestampCheck
(
    OCI_Timestamp *tmsp
)
{
    ub4 value = 0;

    CALL_ENTER(int, value)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIDateTimeCheck((dvoid *)tmsp->env, tmsp->err, tmsp->handle, &value))

#endif

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampCompare
 * --------------------------------------------------------------------------------------------- */

int TimestampCompare
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2
)
{
    sword value = OCI_ERROR;

    CALL_ENTER(int, value)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIDateTimeCompare((dvoid *)tmsp->env, tmsp->err, tmsp->handle, tmsp2->handle, &value))

#endif

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampConstruct
 * --------------------------------------------------------------------------------------------- */

boolean TimestampConstruct
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
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC
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

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampConvert
 * --------------------------------------------------------------------------------------------- */

boolean TimestampConvert
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIDateTimeConvert((dvoid *)tmsp->env, tmsp->err, tmsp_src->handle, tmsp->handle))

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampFromString
 * --------------------------------------------------------------------------------------------- */

boolean TimestampFromString
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

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_STRING, str)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (!IS_STRING_VALID(fmt))
    {
        fmt = GetFormat(tmsp->con, tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP);
    }

    dbstr1 = StringGetDBString(str, &dbsize1);
    dbstr2 = StringGetDBString(fmt, &dbsize2);

    EXEC
    (
        OCIDateTimeFromText((dvoid *) tmsp->env, tmsp->err,
                            (OraText *) dbstr1, (size_t) dbsize1,
                            (OraText *) dbstr2, (ub1) dbsize2,
                            (OraText *) NULL, (size_t) 0,
                            tmsp->handle)
    )

    StringReleaseDBString(dbstr1);
    StringReleaseDBString(dbstr2);

#else

    OCI_NOT_USED(dbstr1)
    OCI_NOT_USED(dbstr2)
    OCI_NOT_USED(dbsize1)
    OCI_NOT_USED(dbsize2)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampToString
 * --------------------------------------------------------------------------------------------- */

boolean TimestampToString
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

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_STRING, str)
    CTX_SET_FROM_OBJ(tmsp)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    CHECK_TIMESTAMP_ENABLED(tmsp->con)

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (!IS_STRING_VALID(fmt))
    {
        fmt = GetFormat(tmsp->con, tmsp->type == OCI_TIMESTAMP_TZ ? OCI_FMT_TIMESTAMP_TZ : OCI_FMT_TIMESTAMP);
    }

    dbstr1 = StringGetDBString(str, &dbsize1);
    dbstr2 = StringGetDBString(fmt, &dbsize2);

    EXEC
    (
        OCIDateTimeToText((dvoid *) tmsp->env, tmsp->err,
                          tmsp->handle, (OraText *) dbstr2,
                          (ub1) dbsize2, (ub1) precision,
                          (OraText *) NULL, (size_t) 0,
                          (ub4*) &dbsize1, (OraText *) dbstr1)
    )

    StringCopyDBStringToNativeString(dbstr1, str, dbcharcount(dbsize1));

    StringReleaseDBString(dbstr1);
    StringReleaseDBString(dbstr2);

    /* set null string terminator */

    str[dbcharcount(dbsize1)] = 0;

#else

    OCI_NOT_USED(dbstr1)
    OCI_NOT_USED(dbstr2)
    OCI_NOT_USED(dbsize1)
    OCI_NOT_USED(dbsize2)
    OCI_NOT_USED(precision)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampGetDate
 * --------------------------------------------------------------------------------------------- */

boolean TimestampGetDate
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

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_INT, year)
    CHECK_PTR(OCI_IPC_INT, month)
    CHECK_PTR(OCI_IPC_INT, day)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIDateTimeGetDate((dvoid *)tmsp->env, tmsp->err, tmsp->handle, &yr, &mt, &dy))

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

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampGetTime
 * --------------------------------------------------------------------------------------------- */

boolean TimestampGetTime
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

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_INT, hour)
    CHECK_PTR(OCI_IPC_INT, min)
    CHECK_PTR(OCI_IPC_INT, sec)
    CHECK_PTR(OCI_IPC_INT, fsec)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

    *hour = 0;
    *min  = 0;
    *sec  = 0;
    *fsec = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIDateTimeGetTime((dvoid *)tmsp->env, tmsp->err, tmsp->handle, &hr, &mn, &sc, &fs))

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

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampGetDateTime
 * --------------------------------------------------------------------------------------------- */

boolean TimestampGetDateTime
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
    return (TimestampGetDate(tmsp, year, month, day) &&
            TimestampGetTime(tmsp, hour, min, sec, fsec));
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampGetTimeZoneName
 * --------------------------------------------------------------------------------------------- */

boolean TimestampGetTimeZoneName
(
    OCI_Timestamp *tmsp,
    int            size,
    otext         *str
)
{
    dbtext *dbstr  = NULL;
    int     dbsize  = size * (int) sizeof(otext);

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_STRING, str)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    dbstr = StringGetDBString(str, &dbsize);

    EXEC(OCIDateTimeGetTimeZoneName((dvoid *)tmsp->env, tmsp->err, tmsp->handle, (ub1*) dbstr, (ub4*) &dbsize))

    StringCopyDBStringToNativeString(dbstr, str, dbcharcount(dbsize));
    StringReleaseDBString(dbstr);

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

#else

    OCI_NOT_USED(str)
    OCI_NOT_USED(size)
    OCI_NOT_USED(dbstr)
    OCI_NOT_USED(dbsize)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampGetTimeZoneOffset
 * --------------------------------------------------------------------------------------------- */

boolean TimestampGetTimeZoneOffset
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min
)
{
    sb1 sb_hour = 0, sb_min = 0;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_INT, hour)
    CHECK_PTR(OCI_IPC_INT, min)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIDateTimeGetTimeZoneOffset((dvoid *)tmsp->env, tmsp->err, tmsp->handle, &sb_hour, &sb_min))

    *hour = sb_hour;
    *min  = sb_min;

#else

    OCI_NOT_USED(hour)
    OCI_NOT_USED(min)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampIntervalAdd
 * --------------------------------------------------------------------------------------------- */

boolean TimestampIntervalAdd
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
)
{
    OCI_Timestamp *tmp = NULL;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeIntervalAdd() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        tmp = TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        STATUS = TimestampConvert(tmp, tmsp);
    }
    else
    {
        tmp = tmsp;
    }

    EXEC(OCIDateTimeIntervalAdd((dvoid *)tmp->env, tmp->err, tmp->handle, itv->handle, tmp->handle))

    /* converting back */

    if (STATUS && (OCI_TIMESTAMP_TZ != tmsp->type))
    {
        STATUS = TimestampConvert(tmsp, tmp);
    }

    if (tmsp != tmp)
    {
        TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampIntervalSub
 * --------------------------------------------------------------------------------------------- */

boolean TimestampIntervalSub
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
)
{
    OCI_Timestamp *tmp = NULL;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeIntervalSub() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        tmp = TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        STATUS = TimestampConvert(tmp, tmsp);
    }
    else
    {
        tmp = tmsp;
    }

    EXEC(OCIDateTimeIntervalSub((dvoid *)tmp->env, tmp->err, tmp->handle, itv->handle, tmp->handle))

    /* converting back */

    if (STATUS && (OCI_TIMESTAMP_TZ != tmsp->type))
    {
        STATUS = TimestampConvert(tmsp, tmp);
    }

    if (tmsp != tmp)
    {
        TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampSubtract
 * --------------------------------------------------------------------------------------------- */

boolean TimestampSubtract
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2,
    OCI_Interval  *itv
)
{
    CALL_ENTER(boolean, FALSE)

    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIDateTimeSubtract((dvoid *)tmsp->env, tmsp->err, tmsp->handle, tmsp2->handle, itv->handle))

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampSysTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean TimestampSysTimestamp
(
    OCI_Timestamp *tmsp
)
{
    OCI_Timestamp *tmp  = NULL;
    OCIDateTime *handle = NULL;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

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
        tmp = TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        handle = tmp->handle;
    }
    else
    {
        handle = tmsp->handle;
    }

    EXEC(OCIDateTimeSysTimeStamp((dvoid *) tmsp->env, tmsp->err, handle))

    if (STATUS && (OCI_TIMESTAMP == tmsp->type))
    {
        STATUS = TimestampConvert(tmsp, tmp);
    }

    if (tmp && tmsp != tmp)
    {
        TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp)
    OCI_NOT_USED(handle)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampToCTime
 * --------------------------------------------------------------------------------------------- */

boolean TimestampToCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t        *pt
)
{
    time_t time = (time_t) -1;
    int    msec = 0;
    struct tm t;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

    memset(&t, 0, sizeof(t));

    STATUS = TimestampGetDateTime(tmsp, &t.tm_year, &t.tm_mon, &t.tm_mday,
                                          &t.tm_hour, &t.tm_min, &t.tm_sec, &msec);

    if (STATUS)
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

    if (STATUS)
    {
        RETVAL = (time != (time_t)-1);
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * TimestampFromCTime
 * --------------------------------------------------------------------------------------------- */

boolean TimestampFromCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t         t
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp)
    CHECK_TIMESTAMP_ENABLED(tmsp->con)
    CTX_SET_FROM_OBJ(tmsp)

    if (!ptm &&  t != (time_t)0)
    {
        ptm = localtime(&t);
    }

    if (!ptm)
    {
        THROW(ExceptionNullPointer(OCI_IPC_TM))
    }

    RETVAL = STATUS = TimestampConstruct(tmsp, ptm->tm_year + 1900,  ptm->tm_mon  + 1,
                                                     ptm->tm_mday,  ptm->tm_hour,  ptm->tm_min,
                                                     ptm->tm_sec, (int) 0, (const otext *) NULL);

   CALL_EXIT()
}
