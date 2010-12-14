/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: date.c, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

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
    OCI_Date      **pdate,
    OCIDate        *buffer,
    boolean         allocate,
    boolean         ansi
)
{
    OCI_Date *date = NULL;
    boolean res    = TRUE;

    OCI_CHECK(pdate == NULL, NULL);

    if (*pdate == NULL)
        *pdate = (OCI_Date *) OCI_MemAlloc(OCI_IPC_DATE, sizeof(*date),
                                           (size_t) 1, TRUE);

    if (*pdate != NULL)
    {
        date = *pdate;

        date->con = con;

        /* get the right error handle */

        if (con != NULL)
            date->err = con->err;
        else
            date->err = OCILib.err;

        /* allocate buffer if needed */

        if ((date->handle == NULL) && ((allocate == TRUE) || (ansi == TRUE)))
        {
            date->allocated = TRUE;

            if (allocate == TRUE)
                date->hstate = OCI_OBJECT_ALLOCATED;

            date->handle = (OCIDate *) OCI_MemAlloc(OCI_IPC_OCIDATE,
                                                    sizeof(*date->handle),
                                                    (size_t) 1, TRUE);

            res = (date->handle != NULL);
        }
        else
        {
            if (date->hstate != OCI_OBJECT_ALLOCATED_ARRAY)
            {
                date->hstate = OCI_OBJECT_FETCHED_CLEAN;
            }

            date->handle = buffer;
        }

        /* if the input buffer is an SQLT_DAT buffer, we need to convert it */

        if ((ansi == TRUE) && (buffer != NULL))
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
    else
        res = FALSE;

    /* check for failure */

    if (res == FALSE)
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
    OCI_Date *date = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    date = OCI_DateInit(con, &date, NULL, TRUE, FALSE);

    OCI_RESULT(date != NULL);

    return date;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateFree
(
    OCI_Date *date
)
{
    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);

    OCI_CHECK_OBJECT_FETCHED(date, FALSE);

    if (date->allocated == TRUE)
    {
        OCI_FREE(date->handle);
    }

    if (date->hstate != OCI_OBJECT_ALLOCATED_ARRAY)
    {
        OCI_FREE(date);
    }

    OCI_RESULT(TRUE);

    return TRUE;
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
    OCI_Array *arr   = NULL;
    OCI_Date **dates = NULL;

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_DATETIME, 0,
                          sizeof(OCIDate), sizeof(OCI_Date),
                          0, NULL);

    if (arr != NULL)
    {
        dates = (OCI_Date **) arr->tab_obj;
    }

    return dates;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateArrayFree
(
    OCI_Date **dates
)
{
    return OCI_ArrayFreeFromHandles((void **) dates);
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateAddDays(date->err, date->handle, (sb4) nb, date->handle)
    )

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateAddMonths(date->err, date->handle, (sb4) nb, date->handle)
    )

    OCI_RESULT(res);

    return res;
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DATE, date,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_DATE, date_src, FALSE);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateAssign(date->err, date_src->handle, date->handle)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateCheck
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_DateCheck
(
    OCI_Date *date
)
{
    boolean res = TRUE;
    uword valid = 0;

    OCI_CHECK_PTR(OCI_IPC_DATE, date, OCI_ERROR);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateCheck(date->err, date->handle, &valid)
    )

    OCI_RESULT(res);

    return (int) valid;
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
    boolean res = TRUE;
    sword value = -1;

    OCI_CHECK_PTR(OCI_IPC_DATE, date, -1);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateCompare(date->err, date->handle, date2->handle, &value)
    )

    OCI_RESULT(res);

    return (int) value;
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
    boolean res = TRUE;
    sb4 nb      = 0;

    OCI_CHECK_PTR(OCI_IPC_DATE, date,  OCI_ERROR);
    OCI_CHECK_PTR(OCI_IPC_DATE, date2, OCI_ERROR);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateDaysBetween(date->err, date->handle, date2->handle, &nb)
    )

    OCI_RESULT(res);

    return (sb4) nb;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateFromText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateFromText
(
    OCI_Date    *date,
    const mtext *str,
    const mtext *fmt
)
{
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int osize1  = -1;
    int osize2  = -1;
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, str,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, fmt,  FALSE);

    ostr1 = OCI_GetInputMetaString(str, &osize1);
    ostr2 = OCI_GetInputMetaString(fmt, &osize2);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateFromText(date->err,
                        (oratext *) ostr1, (ub4) osize1,
                        (oratext *) ostr2, (ub1) osize2,
                        (oratext *) NULL,  (ub4) 0, date->handle)
    )

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    OCI_RESULT(res);

    return res;
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

    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, year,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, month, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, day,   FALSE);

    *year  = 0;
    *month = 0;
    *day   = 0;

    OCIDateGetDate(date->handle, &yr, &mt, &dy);

    *year  = (int) yr;
    *month = (int) mt;
    *day   = (int) dy;

    OCI_RESULT(TRUE);

    return TRUE;
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

    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, hour,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, min,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, sec,   FALSE);

    *hour = 0;
    *min  = 0;
    *sec  = 0;

    OCIDateGetTime(date->handle, &hr, &mn, &sc);

    *hour = (int) hr;
    *min  = (int) mn;
    *sec  = (int) sc;

    OCI_RESULT(TRUE);

    return TRUE;
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
    return (OCI_DateGetDate(date, year, month, day) &&
            OCI_DateGetTime(date, hour, min, sec));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateLastDay
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateLastDay
(
    OCI_Date *date
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateLastDay(date->err, date->handle, date->handle)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateNextDay
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateNextDay
(
    OCI_Date    *date,
    const mtext *day
)
{
    boolean res = TRUE;
    void *ostr  = NULL;
    int osize   = -1;

    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, day,  FALSE);

    ostr = OCI_GetInputMetaString(day, &osize);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateNextDay(date->err, date->handle, (oratext *) ostr,
                       (ub4) osize, date->handle)
    )

    OCI_ReleaseMetaString(ostr);

    OCI_RESULT(res);

    return res;
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
    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);

    OCIDateSetDate(date->handle, (sb2) year, (ub1) month, (ub1) day);

    OCI_RESULT(TRUE);

    return TRUE;
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
    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);

    OCIDateSetTime(date->handle, (ub1) hour, (ub1) min, (ub1) sec);

    OCI_RESULT(TRUE);

    return TRUE;
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
    return (OCI_DateSetDate(date, year, month, day) &&
            OCI_DateSetTime(date, hour, min, sec));
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateSysDate
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateSysDate
(
    OCI_Date *date
)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateSysDate(date->err, date->handle)
    )

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateToText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateToText
(
    OCI_Date    *date,
    const mtext *fmt,
    int          size,
    mtext       *str
)
{
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int osize1  = size * (int) sizeof(mtext);
    int osize2  = -1;
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DATE, date,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, str, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, fmt, FALSE);

    /* init output buffer in case of OCI failure */

    str[0] = 0;

    ostr1 = OCI_GetInputMetaString(str, &osize1);
    ostr2 = OCI_GetInputMetaString(fmt, &osize2);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateToText(date->err, date->handle, (oratext *) ostr2,
                      (ub1) osize2, (oratext *) NULL, (ub4) 0,
                      (ub4*) &osize1, (oratext *) ostr1)
    )

    OCI_GetOutputMetaString(ostr1, str, &osize1);

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    /* set null string terminator*/

    osize1 /= (int) sizeof(mtext);

    str[osize1] = 0;

    OCI_RESULT(res);

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_DateZoneToZone
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_DateZoneToZone
(
    OCI_Date    *date,
    const mtext *zone1,
    const mtext *zone2
)
{
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int osize1  = -1;
    int osize2  = -1;
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DATE, date,    FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, zone1, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, zone2, FALSE);

    ostr1 = OCI_GetInputMetaString(zone1, &osize1);
    ostr2 = OCI_GetInputMetaString(zone2, &osize2);

    OCI_CALL4
    (
        res, date->err, date->con,

        OCIDateZoneToZone(date->err, date->handle,
                          (oratext *) ostr1, (ub4) osize1,
                          (oratext *) ostr2, (ub4) osize2,
                          date->handle)
    )

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    OCI_RESULT(res);

    return res;
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

    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);

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

    if (ptm != NULL)
        memcpy(ptm, &t, sizeof(t));

    if (pt != NULL)
        *pt = time;

    OCI_RESULT(TRUE);

    return (time != (time_t) -1);
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
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_DATE, date, FALSE);

    if ((ptm == NULL) && (t == (time_t) 0))
        OCI_ExceptionNullPointer(OCI_IPC_TM);

    if (ptm == NULL)
        ptm = localtime(&t);

    if (ptm != NULL)
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
        OCI_ExceptionNullPointer(OCI_IPC_TM);

        res = FALSE;
    }

    OCI_RESULT(res);

    return res;
}
