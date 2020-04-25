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
#include "environment.h"
#include "macros.h"
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
    DECLARE_CTX(TRUE)
    CALL_CONTEXT_FROM_CON(con)

    ALLOC_DATA(OCI_IPC_DATE, date, 1);

    if (STATUS)
    {
        date->con = con;

        /* get the right error handle */

        date->err = con ? con->err : Env.err;
        date->env = con ? con->env : Env.env;

        /* allocate buffer if needed */

        if (!date->handle && (allocate || ansi))
        {
            date->allocated = TRUE;

            if (allocate)
            {
                date->hstate = OCI_OBJECT_ALLOCATED;
            }

            ALLOC_DATA(OCI_IPC_OCIDATE, date->handle, 1);
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

    if (!STATUS && date)
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
    CALL_ENTER(OCI_Date*, NULL)
    CALL_CHECK_INITIALIZED()
    CALL_CONTEXT_FROM_CON(con)

    RETVAL = DateInitialize(con, NULL, NULL, TRUE, FALSE);
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DateFree
 * --------------------------------------------------------------------------------------------- */

boolean DateFree
(
    OCI_Date *date
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CHECK_OBJECT_FETCHED(date)
    CALL_CONTEXT_FROM_OBJ(date)

    if (date->allocated)
    {
        FREE(date->handle)
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != date->hstate)
    {
        FREE(date)
    }

    RETVAL = STATUS;

    CALL_EXIT()
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

    CALL_ENTER(OCI_Date **, NULL)
    CALL_CHECK_INITIALIZED()
    CALL_CONTEXT_FROM_CON(con)

    arr = ArrayCreate(con, nbelem, OCI_CDT_DATETIME, 0, sizeof(OCIDate), sizeof(OCI_Date), 0, NULL);

    STATUS = (NULL != arr);

    if (STATUS)
    {
        RETVAL = (OCI_Date **) arr->tab_obj;
    }

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * DateFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean DateFreeArray
(
    OCI_Date **dates
)
{
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_ARRAY, dates)

    RETVAL = STATUS = ArrayFreeFromHandles((void **)dates);

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CONTEXT_FROM_OBJ(date)

    EXEC(OCIDateAddDays(date->err, date->handle, (sb4)nb, date->handle))

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CONTEXT_FROM_OBJ(date)

    EXEC(OCIDateAddMonths(date->err, date->handle, (sb4) nb, date->handle) )

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CHECK_PTR(OCI_IPC_DATE, date_src)
    CALL_CONTEXT_FROM_OBJ(date)

    EXEC(OCIDateAssign(date->err, date_src->handle, date->handle))

    RETVAL = STATUS;

    CALL_EXIT()
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

    CALL_ENTER(int, valid)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CONTEXT_FROM_OBJ(date)

    EXEC(OCIDateCheck(date->err, date->handle, &valid))

    RETVAL = (int) valid;

    CALL_EXIT()
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

    CALL_ENTER(int, value)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CHECK_PTR(OCI_IPC_DATE, date2)
    CALL_CONTEXT_FROM_OBJ(date)

    EXEC(OCIDateCompare(date->err, date->handle, date2->handle, &value))

    RETVAL = (int) value;

    CALL_EXIT()
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

    CALL_ENTER(int, diff)
    CALL_CHECK_PTR(OCI_IPC_DATE, date);
    CALL_CHECK_PTR(OCI_IPC_DATE, date2);
    CALL_CONTEXT_FROM_OBJ(date)

    EXEC(OCIDateDaysBetween(date->err, date->handle, date2->handle, &diff))

    RETVAL = (int) diff;

    CALL_EXIT()
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

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CHECK_PTR(OCI_IPC_STRING, str)
    CALL_CONTEXT_FROM_OBJ(date)

    if (!IS_STRING_VALID(fmt))
    {
        fmt = EnvironmentGetFormat(date->con, OCI_FMT_DATE);
    }

    dbstr1 = StringGetDBString(str, &dbsize1);
    dbstr2 = StringGetDBString(fmt, &dbsize2);

    EXEC
    (
        OCIDateFromText(date->err,
                        (oratext *) dbstr1, (ub4) dbsize1,
                        (oratext *) dbstr2, (ub1) dbsize2,
                        (oratext *) NULL,  (ub4) 0, date->handle)
    )

    StringReleaseDBString(dbstr1);
    StringReleaseDBString(dbstr2);

    RETVAL = STATUS;

    CALL_EXIT()
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

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CHECK_PTR(OCI_IPC_INT, year)
    CALL_CHECK_PTR(OCI_IPC_INT, month)
    CALL_CHECK_PTR(OCI_IPC_INT, day)
    CALL_CONTEXT_FROM_OBJ(date)

    OCIDateGetDate(date->handle, &yr, &mt, &dy);

    *year  = (int) yr;
    *month = (int) mt;
    *day   = (int) dy;

    RETVAL = STATUS;

    CALL_EXIT()
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

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CHECK_PTR(OCI_IPC_INT, hour)
    CALL_CHECK_PTR(OCI_IPC_INT, min)
    CALL_CHECK_PTR(OCI_IPC_INT, sec)
    CALL_CONTEXT_FROM_OBJ(date)

    OCIDateGetTime(date->handle, &hr, &mn, &sc);

    *hour = (int) hr;
    *min  = (int) mn;
    *sec  = (int) sc;

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CONTEXT_FROM_OBJ(date)

    EXEC(OCIDateLastDay(date->err, date->handle, date->handle))

    RETVAL = STATUS;

    CALL_EXIT()
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

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CHECK_PTR(OCI_IPC_STRING, day)
    CALL_CONTEXT_FROM_OBJ(date)

    dbstr = StringGetDBString(day, &dbsize);

    EXEC(OCIDateNextDay(date->err, date->handle, (oratext *) dbstr, (ub4) dbsize, date->handle))

    StringReleaseDBString(dbstr);

    RETVAL = STATUS;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CONTEXT_FROM_OBJ(date)

    OCIDateSetDate(date->handle, (sb2) year, (ub1) month, (ub1) day);

    RETVAL = TRUE;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CONTEXT_FROM_OBJ(date)

    OCIDateSetTime(date->handle, (ub1) hour, (ub1) min, (ub1) sec);

    RETVAL = TRUE;

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CONTEXT_FROM_OBJ(date)

    EXEC(OCIDateSysDate(date->err, date->handle))

    RETVAL = STATUS;

    CALL_EXIT()
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

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CHECK_PTR(OCI_IPC_STRING, str)
    CALL_CONTEXT_FROM_OBJ(date)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    if (!IS_STRING_VALID(fmt))
    {
        fmt = EnvironmentGetFormat(date->con, OCI_FMT_DATE);
    }

    dbstr1 = StringGetDBString(str, &dbsize1);
    dbstr2 = StringGetDBString(fmt, &dbsize2);

    EXEC
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

    RETVAL = STATUS;

    CALL_EXIT()
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

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CHECK_PTR(OCI_IPC_STRING, zone1)
    CALL_CHECK_PTR(OCI_IPC_STRING, zone2)
    CALL_CONTEXT_FROM_OBJ(date)

    dbstr1 = StringGetDBString(zone1, &dbsize1);
    dbstr2 = StringGetDBString(zone2, &dbsize2);

    EXEC
    (
        OCIDateZoneToZone(date->err, date->handle,
                          (oratext *) dbstr1, (ub4) dbsize1,
                          (oratext *) dbstr2, (ub4) dbsize2,
                          date->handle)
    )

    StringReleaseDBString(dbstr1);
    StringReleaseDBString(dbstr2);

    RETVAL = STATUS;

    CALL_EXIT()
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

    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CONTEXT_FROM_OBJ(date)

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

    RETVAL = (time != (time_t)-1);

    CALL_EXIT()
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
    CALL_ENTER(boolean, FALSE)
    CALL_CHECK_PTR(OCI_IPC_DATE, date)
    CALL_CONTEXT_FROM_OBJ(date)

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

    RETVAL = TRUE;

    CALL_EXIT()
}
