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
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateInit
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_DateInit
(
    OCI_Connection *con,
    OCI_Date       *date,
    OCIDate        *buffer,
    boolean         allocate,
    boolean         ansi
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_ALLOCATE_DATA(OCI_IPC_DATE, date, 1);

    if (OCI_STATUS) 
    {
        date->con = con;

        /* get the right error handle */

        date->err = con ? con->err : OCILib.err;
        date->env = con ? con->env : OCILib.env;

        /* allocate buffer if needed */

        if (!date->handle && (allocate || ansi))
        {
            date->allocated = TRUE;

            if (allocate)
            {
                date->hstate = OCI_OBJECT_ALLOCATED;
            }

            OCI_ALLOCATE_DATA(OCI_IPC_OCIDATE, date->handle, 1);
        }
        else
        {
            if (OCI_OBJECT_ALLOCATED_ARRAY != date->hstate)
            {
                date->hstate = OCI_OBJECT_FETCHED_CLEAN;
            }

            date->handle = buffer;
        }

        /* if the input buffer is an SQLT_DAT buffer, we need to convert it */

        if (ansi && date->handle && buffer)
        {
            unsigned char *d = (unsigned char *) buffer;

            date->handle->OCIDateYYYY = (sb2) (((d[0] - 100) * 100) + (d[1] - 100));
            date->handle->OCIDateMM   = (ub1) d[2];
            date->handle->OCIDateDD   = (ub1) d[3];

            date->handle->OCIDateTime.OCITimeHH = (ub1) (d[4] - 1);
            date->handle->OCIDateTime.OCITimeMI = (ub1) (d[5] - 1);
            date->handle->OCIDateTime.OCITimeSS = (ub1) (d[6] - 1);
        }
    }

    /* check for failure */

    if (!OCI_STATUS && date)
    {
        OCI_DateFree(date);
        date = NULL;
    }

    return date;
}

/* ********************************************************************************************* *
 *                             PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OCI_API OCI_DateCreate
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(OCI_Date*, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_RETVAL = OCI_DateInit(con, NULL, NULL, TRUE, FALSE);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateFree
(
    OCI_Date *date
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CHECK_OBJECT_FETCHED(date)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    if (date->allocated)
    {
        OCI_FREE(date->handle)
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != date->hstate)
    {
        OCI_FREE(date)
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Date ** OCI_API OCI_DateArrayCreate
(
    OCI_Connection *con,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_CALL_ENTER(OCI_Date **, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_DATETIME, 0, sizeof(OCIDate), sizeof(OCI_Date), 0, NULL);
    OCI_STATUS = (NULL != arr);

    if (OCI_STATUS)
    {
        OCI_RETVAL = (OCI_Date **) arr->tab_obj;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateArrayFree
(
    OCI_Date **dates
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ARRAY, dates)
 
    OCI_RETVAL = OCI_STATUS = OCI_ArrayFreeFromHandles((void **)dates);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateAddDays
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateAddDays
(
    OCI_Date *date,
    int       nb
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)
    
    OCI_EXEC(OCIDateAddDays(date->err, date->handle, (sb4)nb, date->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateAddMonths
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateAddMonths
(
    OCI_Date *date,
    int       nb
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCI_EXEC(OCIDateAddMonths(date->err, date->handle, (sb4) nb, date->handle) )

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateAssign
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateAssign
(
    OCI_Date *date,
    OCI_Date *date_src
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date_src)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCI_EXEC(OCIDateAssign(date->err, date_src->handle, date->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateCheck
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_DateCheck
(
    OCI_Date *date
)
{
    uword valid = 0;

    OCI_CALL_ENTER(int, valid)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCI_EXEC(OCIDateCheck(date->err, date->handle, &valid))
    
    OCI_RETVAL = (int) valid;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateCompare
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_DateCompare
(
    OCI_Date *date,
    OCI_Date *date2
)
{
    sword value = OCI_ERROR;

    OCI_CALL_ENTER(int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date2)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCI_EXEC(OCIDateCompare(date->err, date->handle, date2->handle, &value))

    OCI_RETVAL = (int) value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateDaysBetween
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_DateDaysBetween
(
    OCI_Date *date,
    OCI_Date *date2
)
{
    sb4 diff = 0;

    OCI_CALL_ENTER(int, diff)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date);
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date2);
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCI_EXEC(OCIDateDaysBetween(date->err, date->handle, date2->handle, &diff))

    OCI_RETVAL = (int) diff;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateFromText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateFromText
(
    OCI_Date    *date,
    const otext *str,
    const otext *fmt
)
{
    dbtext *dbstr1  = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = -1;
    int     dbsize2 = -1;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, str)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    if (!OCI_STRING_VALID(fmt))
    {
        fmt = OCI_GetFormat(date->con, OCI_FMT_DATE);
    }

    dbstr1 = OCI_StringGetOracleString(str, &dbsize1);
    dbstr2 = OCI_StringGetOracleString(fmt, &dbsize2);

    OCI_EXEC
    (
        OCIDateFromText(date->err,
                        (oratext *) dbstr1, (ub4) dbsize1,
                        (oratext *) dbstr2, (ub1) dbsize2,
                        (oratext *) NULL,  (ub4) 0, date->handle)
    )

    OCI_StringReleaseOracleString(dbstr1);
    OCI_StringReleaseOracleString(dbstr2);

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateGetDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateGetDate
(
    OCI_Date *date,
    int      *year,
    int      *month,
    int      *day
)
{
    sb2 yr = 0;
    ub1 mt = 0;
    ub1 dy = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, year)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, month)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, day)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCIDateGetDate(date->handle, &yr, &mt, &dy);

    *year  = (int) yr;
    *month = (int) mt;
    *day   = (int) dy;

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateGetTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateGetTime
(
    OCI_Date *date,
    int      *hour,
    int      *min,
    int      *sec
)
{
    ub1 hr = 0;
    ub1 mn = 0;
    ub1 sc = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, hour)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, min)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, sec)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCIDateGetTime(date->handle, &hr, &mn, &sc);

    *hour = (int) hr;
    *min  = (int) mn;
    *sec  = (int) sc;

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateGetDateTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateGetDateTime
(
    OCI_Date *date,
    int      *year,
    int      *month,
    int      *day,
    int      *hour,
    int      *min,
    int      *sec
)
{
    return (OCI_DateGetDate(date, year, month, day) && OCI_DateGetTime(date, hour, min, sec));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateLastDay
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateLastDay
(
    OCI_Date *date
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCI_EXEC(OCIDateLastDay(date->err, date->handle, date->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateNextDay
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateNextDay
(
    OCI_Date    *date,
    const otext *day
)
{
    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, day)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    dbstr = OCI_StringGetOracleString(day, &dbsize);

    OCI_EXEC(OCIDateNextDay(date->err, date->handle, (oratext *) dbstr, (ub4) dbsize, date->handle))

    OCI_StringReleaseOracleString(dbstr);

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateSetDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateSetDate
(
    OCI_Date *date,
    int       year,
    int       month,
    int       day
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCIDateSetDate(date->handle, (sb2) year, (ub1) month, (ub1) day);

    OCI_RETVAL = TRUE;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateSetTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateSetTime
(
    OCI_Date *date,
    int       hour,
    int       min,
    int       sec
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCIDateSetTime(date->handle, (ub1) hour, (ub1) min, (ub1) sec);

    OCI_RETVAL = TRUE;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateSetDateTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateSetDateTime
(
    OCI_Date *date,
    int       year,
    int       month,
    int       day,
    int       hour,
    int       min,
    int       sec
)
{
    return (OCI_DateSetDate(date, year, month, day) && OCI_DateSetTime(date, hour, min, sec));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateSysDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateSysDate
(
    OCI_Date *date
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    OCI_EXEC(OCIDateSysDate(date->err, date->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateToText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateToText
(
    OCI_Date    *date,
    const otext *fmt,
    int          size,
    otext       *str
)
{
    dbtext *dbstr1  = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = size * (int) sizeof(otext);
    int     dbsize2 = -1;
  
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, str)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    if (!OCI_STRING_VALID(fmt))
    {
        fmt = OCI_GetFormat(date->con, OCI_FMT_DATE);
    }

    dbstr1 = OCI_StringGetOracleString(str, &dbsize1);
    dbstr2 = OCI_StringGetOracleString(fmt, &dbsize2);

    OCI_EXEC
    (
        OCIDateToText(date->err, date->handle, (oratext *) dbstr2,
                      (ub1) dbsize2, (oratext *) NULL, (ub4) 0,
                      (ub4*) &dbsize1, (oratext *) dbstr1)
    )

    OCI_StringCopyOracleStringToNativeString(dbstr1, str, dbcharcount(dbsize1));

    OCI_StringReleaseOracleString(dbstr1);
    OCI_StringReleaseOracleString(dbstr2);

    /* set null string terminator */

    str[dbcharcount(dbsize1)] = 0;

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateZoneToZone
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateZoneToZone
(
    OCI_Date    *date,
    const otext *zone1,
    const otext *zone2
)
{
    dbtext *dbstr1  = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = -1;
    int     dbsize2 = -1;
 
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, zone1)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, zone2)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    dbstr1 = OCI_StringGetOracleString(zone1, &dbsize1);
    dbstr2 = OCI_StringGetOracleString(zone2, &dbsize2);

    OCI_EXEC
    (
        OCIDateZoneToZone(date->err, date->handle,
                          (oratext *) dbstr1, (ub4) dbsize1,
                          (oratext *) dbstr2, (ub4) dbsize2,
                          date->handle)
    )

    OCI_StringReleaseOracleString(dbstr1);
    OCI_StringReleaseOracleString(dbstr2);

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateToCTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateToCTime
(
    OCI_Date  *date,
    struct tm *ptm,
    time_t    *pt
)
{
    time_t time = (time_t) -1;
    struct tm t;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    memset(&t, 0, sizeof(t));

    t.tm_year = date->handle->OCIDateYYYY - 1900;
    t.tm_mon  = date->handle->OCIDateMM - 1;
    t.tm_mday = date->handle->OCIDateDD;

    t.tm_hour = date->handle->OCIDateTime.OCITimeHH;
    t.tm_min  = date->handle->OCIDateTime.OCITimeMI;
    t.tm_sec  = date->handle->OCIDateTime.OCITimeSS;

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

    OCI_RETVAL = (time != (time_t)-1);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateFromCTime
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateFromCTime
(
    OCI_Date  *date,
    struct tm *ptm,
    time_t     t
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_DATE, date)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(date)

    if (!ptm && (t == (time_t) 0))
    {
        OCI_RAISE_EXCEPTION(OCI_ExceptionNullPointer(OCI_IPC_TM))
    }

    if (!ptm)
    {
        ptm = localtime(&t);
    }

    if (ptm)
    {
        date->handle->OCIDateYYYY = (sb2) ptm->tm_year + 1900;
        date->handle->OCIDateMM   = (ub1) ptm->tm_mon  + 1;
        date->handle->OCIDateDD   = (ub1) ptm->tm_mday;

        date->handle->OCIDateTime.OCITimeHH = (ub1) ptm->tm_hour;
        date->handle->OCIDateTime.OCITimeMI = (ub1) ptm->tm_min;
        date->handle->OCIDateTime.OCITimeSS = (ub1) ptm->tm_sec;
    }
    else
    {
        OCI_RAISE_EXCEPTION(OCI_ExceptionNullPointer(OCI_IPC_TM))
    }

    OCI_RETVAL = TRUE;

    OCI_CALL_EXIT()
}
