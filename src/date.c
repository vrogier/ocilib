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
    OCI_Environment *env,
    OCI_Date        *date,
    OCIDate         *buffer,
    OCI_Boolean      allocate,
    OCI_Boolean      ansi
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Date*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    ALLOC_HANDLE(OCI_IPC_DATE, date);

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

        SET_RETVAL(date);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Date * OcilibDateCreate
(
    OCI_Environment *env
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Date*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);
    SET_RETVAL(OcilibDateInitialize(env, NULL, NULL, TRUE, FALSE));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateFree
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateFree
(
    OCI_Date *date
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    CHECK_PTR(OCI_IPC_DATE, date);
    CHECK_OBJECT_FETCHED(date);

    if (date->allocated)
    {
        FREE(date->handle);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != date->hstate)
    {
        OcilibErrorResetSource(NULL, date);

        FREE(date);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Date ** OcilibDateCreateArray
(
    OCI_Environment *env,
    OCI_UInt32       nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Date**, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);

    arr = OcilibArrayCreate(env, NULL, nbelem, OCI_CDT_DATETIME, 0,
                            sizeof(OCIDate), sizeof(OCI_Date), 0, NULL);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Date**)arr->tab_obj);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateFreeArray
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateFreeArray
(
    OCI_Date **dates
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, dates);

    SET_RETVAL(OcilibArrayFreeFromHandles(((OCI_Date *) dates[0])->env, (void**)dates));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateAddDays
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateAddDays
(
    OCI_Date *date,
    OCI_Int32 nb
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    CHECK_PTR(OCI_IPC_DATE, date);

    CHECK_OCI
    (
        date->env->err,
        OCIDateAddDays,
        date->env->err, date->handle,
        (sb4)nb, date->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateAddMonths
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateAddMonths
(
    OCI_Date *date,
    OCI_Int32 nb
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    CHECK_PTR(OCI_IPC_DATE, date);

    CHECK_OCI
    (
        date->env->err,
        OCIDateAddMonths,
        date->env->err, date->handle,
        (sb4) nb, date->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateAssign
(
    OCI_Date *date,
    OCI_Date *date_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    CHECK_PTR(OCI_IPC_DATE, date);
    CHECK_PTR(OCI_IPC_DATE, date_src);

    CHECK_OCI
    (
        date->env->err,
        OCIDateAssign,
        date->env->err, date_src->handle,
        date->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateCheck
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibDateCheck
(
    OCI_Date *date
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, OCI_ERROR,
        /* context */ OCI_IPC_DATE, date
    );

    uword valid = 0;

    CHECK_PTR(OCI_IPC_DATE, date);

    CHECK_OCI
    (
        date->env->err,
        OCIDateCheck,
        date->env->err, date->handle,
        &valid
    );

    SET_RETVAL((OCI_Int32) valid);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateCompare
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibDateCompare
(
    OCI_Date *date,
    OCI_Date *date2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, 0,
        /* context */ OCI_IPC_DATE, date
    );

    sword value = OCI_ERROR;

    CHECK_PTR(OCI_IPC_DATE, date);
    CHECK_PTR(OCI_IPC_DATE, date2);

    CHECK_OCI
    (
        date->env->err,
        OCIDateCompare,
        date->env->err, date->handle,
        date2->handle, &value
    );

    SET_RETVAL((OCI_Int32) value);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateDaysBetween
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibDateDaysBetween
(
    OCI_Date *date,
    OCI_Date *date2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, 0,
        /* context */ OCI_IPC_DATE, date
    );

    sb4 diff = 0;

    CHECK_PTR(OCI_IPC_DATE, date);
    CHECK_PTR(OCI_IPC_DATE, date2);

    CHECK_OCI
    (
        date->env->err,
        OCIDateDaysBetween,
        date->env->err, date->handle,
        date2->handle, &diff
    );

    SET_RETVAL((OCI_Int32) diff);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateFromString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateFromString
(
    OCI_Date    *date,
    const otext *str,
    const otext *fmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );


    CHECK_PTR(OCI_IPC_DATE,   date);
    CHECK_PTR(OCI_IPC_STRING, str);

    if (!IS_STRING_VALID(fmt))
    {
        fmt = OcilibEnvironmentGetFormat(date->env, OCI_FMT_DATE);
        CHECK_NULL(fmt);
    }

    CHECK_OCI
    (
        date->env->err,
        OCIDateFromText,
        date->env->err,
        (oratext *) (str), (ub4) OcilibStringDBSize(str),
        (oratext *) (fmt), (ub1) OcilibStringDBSize(fmt),
        (oratext *) NULL,  (ub4) 0,
        date->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateGetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateGetDate
(
    OCI_Date  *date,
    OCI_Int32 *year,
    OCI_Int32 *month,
    OCI_Int32 *day
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    sb2 yr = 0;
    ub1 mt = 0;
    ub1 dy = 0;

    CHECK_PTR(OCI_IPC_DATE, date);
    CHECK_PTR(OCI_IPC_INT,  year);
    CHECK_PTR(OCI_IPC_INT,  month);
    CHECK_PTR(OCI_IPC_INT,  day);

    OCIDateGetDate(date->handle, &yr, &mt, &dy);

    *year  = (OCI_Int32) yr;
    *month = (OCI_Int32) mt;
    *day   = (OCI_Int32) dy;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateGetTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateGetTime
(
    OCI_Date  *date,
    OCI_Int32 *hour,
    OCI_Int32 *min,
    OCI_Int32 *sec
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    ub1 hr = 0;
    ub1 mn = 0;
    ub1 sc = 0;

    CHECK_PTR(OCI_IPC_DATE, date);
    CHECK_PTR(OCI_IPC_INT,  hour);
    CHECK_PTR(OCI_IPC_INT,  min);
    CHECK_PTR(OCI_IPC_INT,  sec);

    OCIDateGetTime(date->handle, &hr, &mn, &sc);

    *hour = (OCI_Int32) hr;
    *min  = (OCI_Int32) mn;
    *sec  = (OCI_Int32) sc;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateGetDateTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateGetDateTime
(
    OCI_Date  *date,
    OCI_Int32 *year,
    OCI_Int32 *month,
    OCI_Int32 *day,
    OCI_Int32 *hour,
    OCI_Int32 *min,
    OCI_Int32 *sec
)
{
    return (OcilibDateGetDate(date, year, month, day) && OcilibDateGetTime(date, hour, min, sec));
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateLastDay
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateLastDay
(
    OCI_Date *date
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    CHECK_PTR(OCI_IPC_DATE, date);

    CHECK_OCI
    (
        date->env->err,
        OCIDateLastDay,
        date->env->err, date->handle,
        date->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateNextDay
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateNextDay
(
    OCI_Date    *date,
    const otext *day
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );


    CHECK_PTR(OCI_IPC_DATE,   date);
    CHECK_PTR(OCI_IPC_STRING, day);

    CHECK_OCI
    (
        date->env->err,
        OCIDateNextDay,
        date->env->err, date->handle,
        (oratext *) (day), (ub4) OcilibStringDBSize(day),
        date->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateSetDate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateSetDate
(
    OCI_Date *date,
    OCI_Int32 year,
    OCI_Int32 month,
    OCI_Int32 day
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    CHECK_PTR(OCI_IPC_DATE, date);

    OCIDateSetDate(date->handle, (sb2) year, (ub1) month, (ub1) day);

    SET_RETVAL(OcilibDateCheck(date) == 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateSetTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateSetTime
(
    OCI_Date *date,
    OCI_Int32 hour,
    OCI_Int32 min,
    OCI_Int32 sec
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    CHECK_PTR(OCI_IPC_DATE, date);

    OCIDateSetTime(date->handle, (ub1) hour, (ub1) min, (ub1) sec);

    SET_RETVAL(OcilibDateCheck(date) == 0);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateSetDateTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateSetDateTime
(
    OCI_Date *date,
    OCI_Int32 year,
    OCI_Int32 month,
    OCI_Int32 day,
    OCI_Int32 hour,
    OCI_Int32 min,
    OCI_Int32 sec
)
{
    return (OcilibDateSetDate(date, year, month, day) && OcilibDateSetTime(date, hour, min, sec));
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateSysDate
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateSysDate
(
    OCI_Date *date
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    CHECK_PTR(OCI_IPC_DATE, date);

    CHECK_OCI
    (
        date->env->err,
        OCIDateSysDate,
        date->env->err, date->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateToString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateToString
(
    OCI_Date    *date,
    const otext *fmt,
    OCI_Int32    size,
    otext       *str
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    dbtext   *dbstr1  = NULL;
    OCI_Int32 dbsize1 = size * (OCI_Int32) sizeof(otext);

    CHECK_PTR(OCI_IPC_DATE,   date);
    CHECK_PTR(OCI_IPC_STRING, str);

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    if (!IS_STRING_VALID(fmt))
    {
        fmt = OcilibEnvironmentGetFormat(date->env, OCI_FMT_DATE);
        CHECK_NULL(fmt);
    }

    dbstr1 = (dbtext *) (str);


    CHECK_OCI
    (
        date->env->err,
        OCIDateToText,
        date->env->err, date->handle, (oratext *) (fmt),
        (ub1) OcilibStringDBSize(fmt), (oratext *) NULL, (ub4) 0,
        (ub4*) &dbsize1, (oratext *) dbstr1
    );


    /* set null string terminator */

    str[dbcharcount(dbsize1)] = 0;

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateZoneToZone
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateZoneToZone
(
    OCI_Date    *date,
    const otext *zone1,
    const otext *zone2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );


    CHECK_PTR(OCI_IPC_DATE,   date);
    CHECK_PTR(OCI_IPC_STRING, zone1);
    CHECK_PTR(OCI_IPC_STRING, zone2);

    CHECK_OCI
    (
        date->env->err,
        OCIDateZoneToZone,
        date->env->err, date->handle,
        (oratext *) (zone1), (ub4) OcilibStringDBSize(zone1),
        (oratext *) (zone2), (ub4) OcilibStringDBSize(zone2),
        date->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateToCTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateToCTime
(
    OCI_Date  *date,
    struct tm *ptm,
    time_t    *pt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    time_t time = (time_t) -1;

    struct tm t;

    CHECK_PTR(OCI_IPC_DATE, date);

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

    SET_RETVAL((time != (time_t)-1));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibDateFromCTime
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibDateFromCTime
(
    OCI_Date  *date,
    struct tm *ptm,
    time_t     t
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_DATE, date
    );

    CHECK_PTR(OCI_IPC_DATE, date);

    if (NULL == ptm && (t == (time_t) 0))
    {
        THROW(OcilibExceptionNullPointer, OCI_IPC_TM);
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
        THROW(OcilibExceptionNullPointer, OCI_IPC_TM);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}
