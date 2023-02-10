/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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
#include "stringutils.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibDateInitialize
(
    OCI_Connection *con,
    OCI_Date       *date,
    OCIDate        *buffer,
    boolean         allocate,
    boolean         ansi
)
{
    ENTER_FUNC_NO_CONTEXT
    (
        /* returns */ OCI_Date*, NULL
    )

    ALLOC_DATA(OCI_IPC_DATE, date, 1);

    date->con = con;

    /* get the right error handle */

    date->err = con ? con->err : Env.err;
    date->env = con ? con->env : Env.env;

    /* allocate buffer if needed */

    if (NULL == date->handle && (allocate || ansi))
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

    if (ansi && NULL != date->handle && NULL != buffer)
    {
        unsigned char *d = (unsigned char *) buffer;

        date->handle->OCIDateYYYY = (sb2) (((d[0] - 100) * 100) + (d[1] - 100));
        date->handle->OCIDateMM   = (ub1) d[2];
        date->handle->OCIDateDD   = (ub1) d[3];

        date->handle->OCIDateTime.OCITimeHH = (ub1) (d[4] - 1);
        date->handle->OCIDateTime.OCITimeMI = (ub1) (d[5] - 1);
        date->handle->OCIDateTime.OCITimeSS = (ub1) (d[6] - 1);
    }

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            OcilibDateFree(date);
            date = NULL;
        }

        SET_RETVAL(date)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibDateCreate
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Date*, NULL,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )

    CHECK_INITIALIZED()

    SET_RETVAL(OcilibDateInitialize(con, NULL, NULL, TRUE, FALSE))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateFree
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateFree
(
    OCI_Date *date
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    CHECK_PTR(OCI_IPC_DATE, date)
    CHECK_OBJECT_FETCHED(date)

    if (date->allocated)
    {
        FREE(date->handle)
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != date->hstate)
    {
        OcilibErrorResetSource(NULL, date);

        FREE(date)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Date ** OcilibDateCreateArray
(
    OCI_Connection *con,
    unsigned int    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Date**, FALSE,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )

    OCI_Array *arr = NULL;

    CHECK_INITIALIZED()

    arr = OcilibArrayCreate(con, nbelem, OCI_CDT_DATETIME, 0,
                            sizeof(OCIDate), sizeof(OCI_Date), 0, NULL);

    CHECK_NULL(arr)

    SET_RETVAL((OCI_Date**)arr->tab_obj)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateFreeArray
(
    OCI_Date **dates
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_ARRAY, dates)

    SET_RETVAL(OcilibArrayFreeFromHandles((void**)dates))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateAddDays
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateAddDays
(
    OCI_Date *date,
    int       nb
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    CHECK_PTR(OCI_IPC_DATE, date)

    CHECK_OCI
    (
        date->err,
        OCIDateAddDays,
        date->err, date->handle,
        (sb4)nb, date->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateAddMonths
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateAddMonths
(
    OCI_Date *date,
    int       nb
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    CHECK_PTR(OCI_IPC_DATE, date)

    CHECK_OCI
    (
        date->err,
        OCIDateAddMonths,
        date->err, date->handle,
        (sb4) nb, date->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateAssign
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateAssign
(
    OCI_Date *date,
    OCI_Date *date_src
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    CHECK_PTR(OCI_IPC_DATE, date)
    CHECK_PTR(OCI_IPC_DATE, date_src)

    CHECK_OCI
    (
        date->err,
        OCIDateAssign,
        date->err, date_src->handle,
        date->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateCheck
 * --------------------------------------------------------------------------------------------- */

int OcilibDateCheck
(
    OCI_Date *date
)
{
    ENTER_FUNC
    (
        /* returns */ int, OCI_ERROR,
        /* context */ OCI_IPC_DATE, date
    )

    uword valid = 0;

    CHECK_PTR(OCI_IPC_DATE, date)

    CHECK_OCI
    (
        date->err,
        OCIDateCheck,
        date->err, date->handle,
        &valid
    )

    SET_RETVAL((int) valid)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateCompare
 * --------------------------------------------------------------------------------------------- */

int OcilibDateCompare
(
    OCI_Date *date,
    OCI_Date *date2
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_DATE, date
    )

    sword value = OCI_ERROR;

    CHECK_PTR(OCI_IPC_DATE, date)
    CHECK_PTR(OCI_IPC_DATE, date2)

    CHECK_OCI
    (
        date->err,
        OCIDateCompare,
        date->err, date->handle,
        date2->handle, &value
    )

    SET_RETVAL((int) value)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateDaysBetween
 * --------------------------------------------------------------------------------------------- */

int OcilibDateDaysBetween
(
    OCI_Date *date,
    OCI_Date *date2
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_DATE, date
    )

    sb4 diff = 0;

    CHECK_PTR(OCI_IPC_DATE, date);
    CHECK_PTR(OCI_IPC_DATE, date2);

    CHECK_OCI
    (
        date->err,
        OCIDateDaysBetween,
        date->err, date->handle,
        date2->handle, &diff
    )

    SET_RETVAL((int) diff)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateFromString
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateFromString
(
    OCI_Date    *date,
    const otext *str,
    const otext *fmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    dbtext *dbstr1 = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = -1;
    int     dbsize2 = -1;

    CHECK_PTR(OCI_IPC_DATE,   date)
    CHECK_PTR(OCI_IPC_STRING, str)

    if (!IS_STRING_VALID(fmt))
    {
        fmt = OcilibEnvironmentGetFormat(date->con, OCI_FMT_DATE);
        CHECK_NULL(fmt)
    }

    dbstr1 = OcilibStringGetDBString(str, &dbsize1);
    dbstr2 = OcilibStringGetDBString(fmt, &dbsize2);

    CHECK_OCI
    (
        date->err,
        OCIDateFromText,
        date->err,
        (oratext *) dbstr1, (ub4) dbsize1,
        (oratext *) dbstr2, (ub1) dbsize2,
        (oratext *) NULL,  (ub4) 0,
        date->handle
    )

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr1);
        OcilibStringReleaseDBString(dbstr2);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateGetDate
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateGetDate
(
    OCI_Date *date,
    int      *year,
    int      *month,
    int      *day
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    sb2 yr = 0;
    ub1 mt = 0;
    ub1 dy = 0;

    CHECK_PTR(OCI_IPC_DATE, date)
    CHECK_PTR(OCI_IPC_INT,  year)
    CHECK_PTR(OCI_IPC_INT,  month)
    CHECK_PTR(OCI_IPC_INT,  day)

    OCIDateGetDate(date->handle, &yr, &mt, &dy);

    *year  = (int) yr;
    *month = (int) mt;
    *day   = (int) dy;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateGetTime
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateGetTime
(
    OCI_Date *date,
    int      *hour,
    int      *min,
    int      *sec
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    ub1 hr = 0;
    ub1 mn = 0;
    ub1 sc = 0;

    CHECK_PTR(OCI_IPC_DATE, date)
    CHECK_PTR(OCI_IPC_INT,  hour)
    CHECK_PTR(OCI_IPC_INT,  min)
    CHECK_PTR(OCI_IPC_INT,  sec)

    OCIDateGetTime(date->handle, &hr, &mn, &sc);

    *hour = (int) hr;
    *min  = (int) mn;
    *sec  = (int) sc;

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateGetDateTime
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateGetDateTime
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
    return (OcilibDateGetDate(date, year, month, day) && OcilibDateGetTime(date, hour, min, sec));
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateLastDay
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateLastDay
(
    OCI_Date *date
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    CHECK_PTR(OCI_IPC_DATE, date)

    CHECK_OCI
    (
        date->err,
        OCIDateLastDay,
        date->err, date->handle,
        date->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateNextDay
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateNextDay
(
    OCI_Date    *date,
    const otext *day
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    dbtext *dbstr = NULL;
    int dbsize = -1;

    CHECK_PTR(OCI_IPC_DATE,   date)
    CHECK_PTR(OCI_IPC_STRING, day)

    dbstr = OcilibStringGetDBString(day, &dbsize);

    CHECK_OCI
    (
        date->err,
        OCIDateNextDay,
        date->err, date->handle,
        (oratext *) dbstr, (ub4) dbsize,
        date->handle
    )

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateSetDate
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateSetDate
(
    OCI_Date *date,
    int       year,
    int       month,
    int       day
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    CHECK_PTR(OCI_IPC_DATE, date)

    OCIDateSetDate(date->handle, (sb2) year, (ub1) month, (ub1) day);

    SET_RETVAL(OcilibDateCheck(date) == 0)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateSetTime
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateSetTime
(
    OCI_Date *date,
    int       hour,
    int       min,
    int       sec
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    CHECK_PTR(OCI_IPC_DATE, date)

    OCIDateSetTime(date->handle, (ub1) hour, (ub1) min, (ub1) sec);

    SET_RETVAL(OcilibDateCheck(date) == 0)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateSetDateTime
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateSetDateTime
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
    return (OcilibDateSetDate(date, year, month, day) && OcilibDateSetTime(date, hour, min, sec));
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateSysDate
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateSysDate
(
    OCI_Date *date
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    CHECK_PTR(OCI_IPC_DATE, date)

    CHECK_OCI
    (
        date->err,
        OCIDateSysDate,
        date->err, date->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateToString
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateToString
(
    OCI_Date    *date,
    const otext *fmt,
    int          size,
    otext       *str
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    dbtext *dbstr1 = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = size * (int) sizeof(otext);
    int     dbsize2 = -1;

    CHECK_PTR(OCI_IPC_DATE,   date)
    CHECK_PTR(OCI_IPC_STRING, str)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    if (!IS_STRING_VALID(fmt))
    {
        fmt = OcilibEnvironmentGetFormat(date->con, OCI_FMT_DATE);
        CHECK_NULL(fmt)
    }

    dbstr1 = OcilibStringGetDBString(str, &dbsize1);
    dbstr2 = OcilibStringGetDBString(fmt, &dbsize2);

    CHECK_OCI
    (
        date->err,
        OCIDateToText,
        date->err, date->handle, (oratext *) dbstr2,
        (ub1) dbsize2, (oratext *) NULL, (ub4) 0,
        (ub4*) &dbsize1, (oratext *) dbstr1
    )

    OcilibStringCopyDBStringToNativeString(dbstr1, str, dbcharcount(dbsize1));

    /* set null string terminator */

    str[dbcharcount(dbsize1)] = 0;

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr1);
        OcilibStringReleaseDBString(dbstr2);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateZoneToZone
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateZoneToZone
(
    OCI_Date    *date,
    const otext *zone1,
    const otext *zone2
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    dbtext *dbstr1 = NULL;
    dbtext *dbstr2  = NULL;
    int     dbsize1 = -1;
    int     dbsize2 = -1;

    CHECK_PTR(OCI_IPC_DATE,   date)
    CHECK_PTR(OCI_IPC_STRING, zone1)
    CHECK_PTR(OCI_IPC_STRING, zone2)

    dbstr1 = OcilibStringGetDBString(zone1, &dbsize1);
    dbstr2 = OcilibStringGetDBString(zone2, &dbsize2);

    CHECK_OCI
    (
        date->err,
        OCIDateZoneToZone,
        date->err, date->handle,
        (oratext *) dbstr1, (ub4) dbsize1,
        (oratext *) dbstr2, (ub4) dbsize2,
        date->handle
    )

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(dbstr1);
        OcilibStringReleaseDBString(dbstr2);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateToCTime
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateToCTime
(
    OCI_Date  *date,
    struct tm *ptm,
    time_t    *pt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    time_t time = (time_t) -1;

    struct tm t;

    CHECK_PTR(OCI_IPC_DATE, date)

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

    if (NULL != ptm)
    {
        memcpy(ptm, &t, sizeof(t));
    }

    if (NULL != pt)
    {
        *pt = time;
    }

    SET_RETVAL((time != (time_t)-1))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateFromCTime
 * --------------------------------------------------------------------------------------------- */

boolean OcilibDateFromCTime
(
    OCI_Date  *date,
    struct tm *ptm,
    time_t     t
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    )

    CHECK_PTR(OCI_IPC_DATE, date)

    if (NULL == ptm && (t == (time_t) 0))
    {
        THROW(OcilibExceptionNullPointer, OCI_IPC_TM)
    }

    if (NULL == ptm)
    {
        ptm = localtime(&t);
    }

    if (NULL != ptm)
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
        THROW(OcilibExceptionNullPointer, OCI_IPC_TM)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}
