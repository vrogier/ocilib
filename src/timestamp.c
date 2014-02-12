/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2014 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: timestamp.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampInit
 * --------------------------------------------------------------------------------------------- */

OCI_Timestamp * OCI_TimestampInit
(
    OCI_Connection *con,
    OCI_Timestamp **ptmsp,
    OCIDateTime    *buffer,
    ub4             type
)
{
    OCI_Timestamp *tmsp = NULL;

#if OCI_VERSION_COMPILE >= OCI_9_0

    boolean res = FALSE;

    OCI_CHECK(ptmsp == NULL, NULL);

    if (!*ptmsp)
    {
        *ptmsp = (OCI_Timestamp *) OCI_MemAlloc(OCI_IPC_TIMESTAMP, sizeof(*tmsp), (size_t) 1, TRUE);
    }

    if (*ptmsp)
    {
        tmsp = *ptmsp;

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
            ub4 htype = OCI_UNKNOWN;

            if (OCI_TIMESTAMP == tmsp->type)
            {
                htype = OCI_DTYPE_TIMESTAMP;
            }
            else if (OCI_TIMESTAMP_TZ == tmsp->type)
            {
                htype = OCI_DTYPE_TIMESTAMP_TZ;
            }
            else if (OCI_TIMESTAMP_LTZ == tmsp->type)
            {
                htype = OCI_DTYPE_TIMESTAMP_LTZ;
            }

            if (OCI_OBJECT_ALLOCATED_ARRAY != tmsp->hstate)
            {
                res = OCI_SUCCESSFUL(OCI_DescriptorAlloc((dvoid  *) tmsp->env,
                                                         (dvoid **) (void *) &tmsp->handle,
                                                         (ub4     ) htype, (size_t) 0,
                                                         (dvoid **) NULL));
                tmsp->hstate = OCI_OBJECT_ALLOCATED;
            }
        }
        else
        {
            tmsp->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }

        /* check for failure */

        if (!res)
        {
            OCI_TimestampFree(tmsp);
            tmsp = NULL;
        }
    }

#else

    OCI_NOT_USED(con);
    OCI_NOT_USED(type);
    OCI_NOT_USED(buffer);
    OCI_NOT_USED(ptmsp);

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
    OCI_Timestamp *tmsp = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_TIMESTAMP_ENABLED(con, NULL);

#if OCI_VERSION_COMPILE >= OCI_9_0

    tmsp = OCI_TimestampInit(con, &tmsp, NULL, type);

#else

    OCI_NOT_USED(type);

#endif

    OCI_RESULT(tmsp != NULL);

    return tmsp;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampFree
(
    OCI_Timestamp *tmsp
)
{
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CHECK_OBJECT_FETCHED(tmsp, FALSE);

    if (OCI_OBJECT_ALLOCATED == tmsp->hstate)
    {
        ub4 htype = OCI_UNKNOWN;

        if (OCI_TIMESTAMP == tmsp->type)
        {
            htype = OCI_DTYPE_TIMESTAMP;
        }
        else if (OCI_TIMESTAMP_TZ == tmsp->type)
        {
            htype = OCI_DTYPE_TIMESTAMP_TZ;
        }
        else if (OCI_TIMESTAMP_LTZ == tmsp->type)
        {
            htype = OCI_DTYPE_TIMESTAMP_LTZ;
        }

        OCI_DescriptorFree((dvoid *) tmsp->handle, htype);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != tmsp->hstate)
    {
        OCI_FREE(tmsp);
    }

#endif

    OCI_RESULT(TRUE);

    return TRUE;
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
    OCI_Array       *arr   = NULL;
    OCI_Timestamp **tmsps  = NULL;
    unsigned int    htype  = OCI_UNKNOWN;

    OCI_CHECK_TIMESTAMP_ENABLED(con, NULL);

#if OCI_VERSION_COMPILE >= OCI_9_0

    if (OCI_TIMESTAMP == type)
    {
        htype = OCI_DTYPE_TIMESTAMP;
    }
    else if (OCI_TIMESTAMP_TZ == type)
    {
        htype = OCI_DTYPE_TIMESTAMP_TZ;
    }
    else if (OCI_TIMESTAMP_LTZ == type)
    {
        htype = OCI_DTYPE_TIMESTAMP_LTZ;
    }

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_TIMESTAMP, type, sizeof(OCIDateTime *), 
                          sizeof(OCI_Timestamp), htype, NULL);

    if (arr)
    {
        tmsps = (OCI_Timestamp **) arr->tab_obj;
    }

#else

    OCI_NOT_USED(arr);
    OCI_NOT_USED(type);
    OCI_NOT_USED(nbelem);
    OCI_NOT_USED(htype);

#endif

    return tmsps;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampArrayFree
(
    OCI_Timestamp **tmsps
)
{
    return OCI_ArrayFreeFromHandles((void **) tmsps);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_TimestampGetType
(
    OCI_Timestamp *tmsp
)
{
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, OCI_UNKNOWN);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return tmsp->type;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeAssign((dvoid *) tmsp->env, tmsp->err,
                          tmsp_src->handle, tmsp->handle)
    )

#endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampCheck
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_TimestampCheck
(
    OCI_Timestamp *tmsp
)
{
    boolean res = TRUE;
    ub4 value   = 0;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, value);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, value);

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeCheck((dvoid *) tmsp->env, tmsp->err, tmsp->handle, &value)
    )

#endif

    OCI_RESULT(res);

    return (int) value;
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
    boolean res = TRUE;
    sword value = OCI_ERROR;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp,  value);
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2, value);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeCompare((dvoid *) tmsp->env, tmsp->err,
                           tmsp2->handle, tmsp2->handle, &value)
    )

#endif

    OCI_RESULT(res);

    return (int) value;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeConstruct((dvoid *) tmsp->env, tmsp->err,
                             tmsp->handle,
                             (sb2) year, (ub1) month, (ub1) day,
                             (ub1) hour, (ub1) min,(ub1) sec,
                             (ub4) fsec, (OraText *) time_zone,
                             (size_t) (time_zone ? otextsize(time_zone) : 0))
    )

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

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeConvert((dvoid *) tmsp->env, tmsp->err,
                           tmsp_src->handle, tmsp->handle)
    )

#endif

    OCI_RESULT(res);

    return res;
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
    boolean  res     = TRUE;
    dbtext  *dbstr1  = NULL;
    dbtext  *dbstr2  = NULL;
    int      dbsize1 = -1;
    int      dbsize2 = -1;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, str,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, fmt,  FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    dbstr1 = OCI_StringGetOracleString(str, &dbsize1);
    dbstr2 = OCI_StringGetOracleString(fmt, &dbsize2);

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeFromText((dvoid *) tmsp->env, tmsp->err,
                            (OraText *) dbstr1, (size_t) dbsize1,
                            (OraText *) dbstr2, (ub1) dbsize2,
                            (OraText *) NULL, (size_t) 0,
                            tmsp->handle)
    )

    OCI_StringReleaseOracleString(dbstr1);
    OCI_StringReleaseOracleString(dbstr2);

#else

    OCI_NOT_USED(dbstr1);
    OCI_NOT_USED(dbstr2);
    OCI_NOT_USED(dbsize1);
    OCI_NOT_USED(dbsize2);

#endif

    OCI_RESULT(res);

    return res;
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
    boolean res     = TRUE;
    dbtext *dbstr1  = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = size * (int) sizeof(otext);
    int     dbsize2 = -1;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, str,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, fmt,  FALSE);

    /* init output buffer in case of OCI failure */

    str[0] = 0;

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    dbstr1 = OCI_StringGetOracleString(str, &dbsize1);
    dbstr2 = OCI_StringGetOracleString(fmt, &dbsize2);

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

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

    OCI_NOT_USED(dbstr1);
    OCI_NOT_USED(dbstr2);
    OCI_NOT_USED(dbsize1);
    OCI_NOT_USED(dbsize2);
    OCI_NOT_USED(precision);

#endif

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;
    sb2 yr      = 0;
    ub1 mt      = 0;
    ub1 dy      = 0;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);

    OCI_CHECK_PTR(OCI_IPC_INT, year,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, month, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, day,   FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

    *year  = 0;
    *month = 0;
    *day   = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeGetDate((dvoid *) tmsp->env, tmsp->err, tmsp->handle,
                           &yr, &mt, &dy)
    )

    *year  = (int) yr;
    *month = (int) mt;
    *day   = (int) dy;

#else

    OCI_NOT_USED(year);
    OCI_NOT_USED(month);
    OCI_NOT_USED(day);
    OCI_NOT_USED(yr);
    OCI_NOT_USED(mt);
    OCI_NOT_USED(dy);

#endif

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    ub1 hr = 0;
    ub1 mn = 0;
    ub1 sc = 0;
    ub4 fs = 0;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);

    OCI_CHECK_PTR(OCI_IPC_INT, hour, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, min,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, sec,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, fsec, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

    *hour = 0;
    *min  = 0;
    *sec  = 0;
    *fsec = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeGetTime((dvoid *) tmsp->env, tmsp->err, tmsp->handle,
                           &hr, &mn, &sc, &fs)
    )

    *hour = (int) hr;
    *min  = (int) mn;
    *sec  = (int) sc;
    *fsec = (int) fs;

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

    OCI_RESULT(res);

    return res;
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
    boolean res    = TRUE;
    dbtext *dbstr  = NULL;
    int     dbsize  = size * (int) sizeof(otext);

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, str, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    dbstr = OCI_StringGetOracleString(str, &dbsize);

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeGetTimeZoneName((dvoid *) tmsp->env, tmsp->err, tmsp->handle,
                                   (ub1*) dbstr, (ub4*) &dbsize)
    )

    OCI_StringCopyOracleStringToNativeString(dbstr, str, dbcharcount(dbsize));
    OCI_StringReleaseOracleString(dbstr);

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;

#else

    OCI_NOT_USED(str);
    OCI_NOT_USED(size);
    OCI_NOT_USED(dbstr);
    OCI_NOT_USED(dbsize);

#endif

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, hour, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, min, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeGetTimeZoneOffset((dvoid *) tmsp->env, tmsp->err,
                                     tmsp->handle, (sb1*) hour, (sb1*) min)
    )

#else

    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);

#endif

    OCI_RESULT(res);

    return res;
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
    boolean res        = TRUE;
    OCI_Timestamp *tmp = NULL;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INTERVAL, itv,  FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeIntervalAdd() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        tmp = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        res = OCI_TimestampConvert(tmp, tmsp);
    }
    else
    {
        tmp = tmsp;
    }

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeIntervalAdd((dvoid *) tmp->env, tmp->err, tmp->handle,
                               itv->handle, tmp->handle)
    )

    /* converting back */

    if (res && (OCI_TIMESTAMP_TZ != tmsp->type))
    {
        res = OCI_TimestampConvert(tmsp, tmp);
    }

    if (tmsp != tmp)
    {
        OCI_TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp);

#endif

    OCI_RESULT(res);

    return res;
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
    boolean res        = TRUE;
    OCI_Timestamp *tmp = NULL;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INTERVAL, itv,  FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    /* OCIDateTimeIntervalSub() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if (OCI_TIMESTAMP_TZ != tmsp->type)
    {
        tmp = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        res = OCI_TimestampConvert(tmp, tmsp);
    }
    else
    {
        tmp = tmsp;
    }

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeIntervalSub((dvoid *) tmp->env, tmp->err, tmp->handle,
                               itv->handle, tmp->handle)
    )

    /* converting back */

    if (res && (OCI_TIMESTAMP_TZ != tmsp->type))
    {
        res = OCI_TimestampConvert(tmsp, tmp);
    }

    if (tmsp != tmp)
    {
        OCI_TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp);

#endif

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INTERVAL, itv,   FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeSubtract((dvoid *) tmsp->env, tmsp->err, tmsp->handle,
                            tmsp2->handle, itv->handle)
    )

#endif

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TimestampSysTimestamp
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_TimestampSysTimestamp
(
    OCI_Timestamp *tmsp
)
{
    boolean res         = TRUE;
    OCI_Timestamp *tmp  = NULL;
    OCIDateTime *handle = NULL;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

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

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,

        OCIDateTimeSysTimeStamp((dvoid *) tmsp->env, tmsp->err, handle)
    )

    if (res && (OCI_TIMESTAMP == tmsp->type))
    {
        res = OCI_TimestampConvert(tmsp, tmp);
    }

    if (tmsp != tmp)
    {
        OCI_TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp);
    OCI_NOT_USED(handle);

#endif

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;
    time_t time = (time_t) -1;
    int msec    = 0;
    struct tm t;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

    res = OCI_TimestampGetDateTime(tmsp, &t.tm_year, &t.tm_mon, &t.tm_mday,
                                   &t.tm_hour, &t.tm_min, &t.tm_sec,
                                   &msec);

    if (res == TRUE)
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

    OCI_RESULT(res);

    return (time != (time_t) -1);
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

    if (ptm && t == (time_t) 0)
    {
        OCI_ExceptionNullPointer(OCI_IPC_TM);

        return FALSE;
    }

    if (ptm)
    {
        ptm = localtime(&t);
    }

    if (ptm)
    {
        res =  OCI_TimestampConstruct(tmsp,
                                      ptm->tm_year + 1900,
                                      ptm->tm_mon  + 1,
                                      ptm->tm_mday,
                                      ptm->tm_hour,
                                      ptm->tm_min,
                                      ptm->tm_sec,
                                      (int) 0,
                                      (const otext *) NULL);
    }
    else
    {
        OCI_ExceptionNullPointer(OCI_IPC_TM);

        res = FALSE;
    }

    OCI_RESULT(res);

    return res;       
}
