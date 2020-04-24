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

#include "date.h"

#include "array.h"
#include "library.h"
#include "macro.h"
#include "strings.h"

/* --------------------------------------------------------------------------------------------- *
 * DateInit
 * --------------------------------------------------------------------------------------------- */

OCI_Date * DateInitialize
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
        DateFree(date);
        date = NULL;
    }

    return date;
}

/* --------------------------------------------------------------------------------------------- *
 * DateCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * DateCreate
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(OCI_Date*, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_RETVAL = DateInitialize(con, NULL, NULL, TRUE, FALSE);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DateFree
 * --------------------------------------------------------------------------------------------- */

boolean DateFree
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
 * DateArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Date ** DateCreateArray
(
    OCI_Connection *con,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_CALL_ENTER(OCI_Date **, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_DATETIME, 0, sizeof(OCIDate), sizeof(OCI_Date), 0, NULL);
    OCI_STATUS = (NULL != arr);

    if (OCI_STATUS)
    {
        OCI_RETVAL = (OCI_Date **) arr->tab_obj;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DateFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean DateFreeArray
(
    OCI_Date **dates
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ARRAY, dates)
 
    OCI_RETVAL = OCI_STATUS = ArrayFreeFromHandles((void **)dates);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DateAddDays
 * --------------------------------------------------------------------------------------------- */

boolean DateAddDays
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
 * DateAddMonths
 * --------------------------------------------------------------------------------------------- */

boolean DateAddMonths
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
 * DateAssign
 * --------------------------------------------------------------------------------------------- */

boolean DateAssign
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
 * DateCheck
 * --------------------------------------------------------------------------------------------- */

int DateCheck
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
 * DateCompare
 * --------------------------------------------------------------------------------------------- */

int DateCompare
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
 * DateDaysBetween
 * --------------------------------------------------------------------------------------------- */

int DateDaysBetween
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
 * DateFromString
 * --------------------------------------------------------------------------------------------- */

boolean DateFromString
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
        fmt = GetFormat(date->con, OCI_FMT_DATE);
    }

    dbstr1 = StringGetDBString(str, &dbsize1);
    dbstr2 = StringGetDBString(fmt, &dbsize2);

    OCI_EXEC
    (
        OCIDateFromText(date->err,
                        (oratext *) dbstr1, (ub4) dbsize1,
                        (oratext *) dbstr2, (ub1) dbsize2,
                        (oratext *) NULL,  (ub4) 0, date->handle)
    )

    StringReleaseDBString(dbstr1);
    StringReleaseDBString(dbstr2);

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DateGetDate
 * --------------------------------------------------------------------------------------------- */

boolean DateGetDate
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
 * DateGetTime
 * --------------------------------------------------------------------------------------------- */

boolean DateGetTime
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
 * DateGetDateTime
 * --------------------------------------------------------------------------------------------- */

boolean DateGetDateTime
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
    return (DateGetDate(date, year, month, day) && DateGetTime(date, hour, min, sec));
}

/* --------------------------------------------------------------------------------------------- *
 * DateLastDay
 * --------------------------------------------------------------------------------------------- */

boolean DateLastDay
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
 * DateNextDay
 * --------------------------------------------------------------------------------------------- */

boolean DateNextDay
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

    dbstr = StringGetDBString(day, &dbsize);

    OCI_EXEC(OCIDateNextDay(date->err, date->handle, (oratext *) dbstr, (ub4) dbsize, date->handle))

    StringReleaseDBString(dbstr);

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DateSetDate
 * --------------------------------------------------------------------------------------------- */

boolean DateSetDate
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
 * DateSetTime
 * --------------------------------------------------------------------------------------------- */

boolean DateSetTime
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
 * DateSetDateTime
 * --------------------------------------------------------------------------------------------- */

boolean DateSetDateTime
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
    return (DateSetDate(date, year, month, day) && DateSetTime(date, hour, min, sec));
}

/* --------------------------------------------------------------------------------------------- *
 * DateSysDate
 * --------------------------------------------------------------------------------------------- */

boolean DateSysDate
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
 * DateToString
 * --------------------------------------------------------------------------------------------- */

boolean DateToString
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
        fmt = GetFormat(date->con, OCI_FMT_DATE);
    }

    dbstr1 = StringGetDBString(str, &dbsize1);
    dbstr2 = StringGetDBString(fmt, &dbsize2);

    OCI_EXEC
    (
        OCIDateToText(date->err, date->handle, (oratext *) dbstr2,
                      (ub1) dbsize2, (oratext *) NULL, (ub4) 0,
                      (ub4*) &dbsize1, (oratext *) dbstr1)
    )

    StringCopyDBStringToNativeString(dbstr1, str, dbcharcount(dbsize1));

    StringReleaseDBString(dbstr1);
    StringReleaseDBString(dbstr2);

    /* set null string terminator */

    str[dbcharcount(dbsize1)] = 0;

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DateZoneToZone
 * --------------------------------------------------------------------------------------------- */

boolean DateZoneToZone
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

    dbstr1 = StringGetDBString(zone1, &dbsize1);
    dbstr2 = StringGetDBString(zone2, &dbsize2);

    OCI_EXEC
    (
        OCIDateZoneToZone(date->err, date->handle,
                          (oratext *) dbstr1, (ub4) dbsize1,
                          (oratext *) dbstr2, (ub4) dbsize2,
                          date->handle)
    )

    StringReleaseDBString(dbstr1);
    StringReleaseDBString(dbstr2);

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DateToCTime
 * --------------------------------------------------------------------------------------------- */

boolean DateToCTime
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
 * DateFromCTime
 * --------------------------------------------------------------------------------------------- */

boolean DateFromCTime
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
        THROW(ExceptionNullPointer(OCI_IPC_TM))
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
        THROW(ExceptionNullPointer(OCI_IPC_TM))
    }

    OCI_RETVAL = TRUE;

    OCI_CALL_EXIT()
}
