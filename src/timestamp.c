/*
   +----------------------------------------------------------------------+   
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://ocilib.net                     |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2009 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Library General Public          |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Library General Public License for more details.                     |
   |                                                                      |
   | You should have received a copy of the GNU Library General Public    |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+ 
*/

/* ------------------------------------------------------------------------ *
 * $Id: timestamp.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_TimestampInit
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_TimestampInit(OCI_Connection *con, OCI_Timestamp **ptmsp,
                                  OCIDateTime *buffer, ub4 type)
{
    OCI_Timestamp *tmsp = NULL;

#if OCI_VERSION_COMPILE >= OCI_9

    boolean res = TRUE;

    OCI_CHECK(ptmsp == NULL, NULL);

    if (*ptmsp == NULL)
        *ptmsp = (OCI_Timestamp *) OCI_MemAlloc(OCI_IPC_TIMESTAMP, sizeof(*tmsp), 
                                                1, TRUE);

    if (*ptmsp != NULL)
    {
        tmsp = *ptmsp;

        tmsp->con    = con;
        tmsp->handle = buffer;
        tmsp->type   = type;

        /* get the right error handle */

        if (con != NULL)
            tmsp->err = con->err;
        else
            tmsp->err = OCILib.err;
       
        /* allocate buffer if needed */
        
        if (tmsp != NULL && tmsp->handle == NULL)
        {
            ub4 htype = 0;

            tmsp->hstate = OCI_OBJECT_ALLOCATED;

            if (tmsp->type == OCI_TIMESTAMP)
                htype = OCI_DTYPE_TIMESTAMP;
            else if (tmsp->type == OCI_TIMESTAMP_TZ)
                htype = OCI_DTYPE_TIMESTAMP_TZ;
            else if (tmsp->type == OCI_TIMESTAMP_LTZ)
                htype = OCI_DTYPE_TIMESTAMP_LTZ;

            res = (OCI_SUCCESS == OCI_DescriptorAlloc((dvoid  *) OCILib.env, 
                                                      (dvoid **) (void *) &tmsp->handle,
                                                      (ub4     ) htype, (size_t) 0, 
                                                      (dvoid **) NULL));
        }
        else
            tmsp->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }
    else
        res = FALSE;

    /* check for failure */

    if (res == FALSE)
    {
        OCI_TimestampFree(tmsp);
        tmsp = NULL;
    }

#else

    OCI_NOT_USED(con);
    OCI_NOT_USED(type);
    OCI_NOT_USED(buffer);
    OCI_NOT_USED(ptmsp);

#endif

    return tmsp;
}

/* ************************************************************************ *
 *                            PUBLIC FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_TimestampCreate
 * ------------------------------------------------------------------------ */

OCI_Timestamp * OCI_API OCI_TimestampCreate(OCI_Connection *con, 
                                            unsigned int type)
{
    OCI_Timestamp *tmsp = NULL;

    OCI_CHECK_INITIALIZED(NULL);

    OCI_CHECK_TIMESTAMP_ENABLED(con, NULL);

#if OCI_VERSION_COMPILE >= OCI_9

    tmsp = OCI_TimestampInit(con, &tmsp, NULL, type);

#else

    OCI_NOT_USED(type);

#endif

    OCI_RESULT(tmsp != NULL);

    return tmsp;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampFree
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampFree(OCI_Timestamp *tmsp)
{
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    
    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CHECK_OBJECT_FETCHED(tmsp, FALSE);

    if (tmsp->hstate == OCI_OBJECT_ALLOCATED)
    {
        ub4 htype  = 0;

        if (tmsp->type == OCI_TIMESTAMP)
            htype = OCI_DTYPE_TIMESTAMP;
        else if (tmsp->type == OCI_TIMESTAMP_TZ)
            htype = OCI_DTYPE_TIMESTAMP_TZ;
        else if (tmsp->type == OCI_TIMESTAMP_LTZ)
            htype = OCI_DTYPE_TIMESTAMP_LTZ;

       OCI_DescriptorFree((dvoid *) tmsp->handle, htype);
    }

    OCI_FREE(tmsp);
   
#endif
   
   OCI_RESULT(TRUE);

   return TRUE;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetType
 * ------------------------------------------------------------------------ */

unsigned int OCI_API OCI_TimestampGetType(OCI_Timestamp *tmsp)
{
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, OCI_UNKNOWN);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, OCI_UNKNOWN);

    OCI_RESULT(TRUE);

    return tmsp->type;
}

/* ------------------------------------------------------------------------ *
 * OCI_DateZoneToZone
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampAssign(OCI_Timestamp *tmsp, OCI_Timestamp *tmsp_src)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeAssign((dvoid *) OCILib.env, tmsp->err,
                          tmsp_src->handle, tmsp->handle)
    )

#endif

   OCI_RESULT(res);

   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampCheck
 * ------------------------------------------------------------------------ */

int OCI_API OCI_TimestampCheck(OCI_Timestamp *tmsp)
{
    boolean res = TRUE;
    ub4 value   = 0;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, value);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, value);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeCheck((dvoid *) OCILib.env, tmsp->err, tmsp->handle, &value)
    )

#endif
 
    OCI_RESULT(res);

    return (int) value;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampCompare
 * ------------------------------------------------------------------------ */

int OCI_API OCI_TimestampCompare(OCI_Timestamp *tmsp, OCI_Timestamp *tmsp2)
{
    boolean res = TRUE;
    sword value = OCI_ERROR;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp,  value);
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2, value);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeCompare((dvoid *) OCILib.env, tmsp->err,
                            tmsp2->handle, tmsp2->handle, &value)
    )

#endif

    OCI_RESULT(res);

    return (int) value;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampConstruct
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampConstruct(OCI_Timestamp *tmsp, int year,int month,
                                      int day, int hour,  int min, int sec,
                                      int fsec, const mtext *timezone)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeConstruct((dvoid *) OCILib.env, tmsp->err,
                                         tmsp->handle,
                                         (sb2) year, (ub1) month, (ub1) day,
                                         (ub1) hour, (ub1) min,(ub1) sec,
                                         (ub4) fsec, (OraText *) timezone,
                                         (size_t) (timezone ? mtextsize(timezone) : 0))
    )

#else

    OCI_NOT_USED(year);
    OCI_NOT_USED(month);
    OCI_NOT_USED(day);
    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);
    OCI_NOT_USED(sec);
    OCI_NOT_USED(fsec);
    OCI_NOT_USED(timezone);

#endif

   OCI_RESULT(res);

   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampConvert
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampConvert(OCI_Timestamp *tmsp, OCI_Timestamp *tmsp_src)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp,     FALSE);
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp_src, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeConvert((dvoid *) OCILib.env, tmsp->err,
                           tmsp_src->handle, tmsp->handle)
    )

#endif

   OCI_RESULT(res);

   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampFromText
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampFromText(OCI_Timestamp *tmsp, const mtext *str, 
                                      const mtext *fmt)
{
    boolean res = TRUE;
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int  osize1 = -1;
    int  osize2 = -1;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, str,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, fmt,  FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    ostr1 = OCI_GetInputMetaString(str, &osize1);
    ostr2 = OCI_GetInputMetaString(fmt, &osize2);

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeFromText((dvoid *) OCILib.env, tmsp->err,
                            (OraText *) ostr1, (size_t) osize1,
                            (OraText *) ostr2, (ub1) osize2,
                            (OraText *) NULL, (size_t) 0, 
                            tmsp->handle)
    )

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

#else

    OCI_NOT_USED(ostr1);
    OCI_NOT_USED(ostr2);
    OCI_NOT_USED(osize1);
    OCI_NOT_USED(osize2);

#endif

   OCI_RESULT(res);

   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampToText
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampToText(OCI_Timestamp *tmsp, const mtext *fmt, 
                                    int size, mtext *str, int precision)
{
    boolean res = TRUE;
    void *ostr1 = NULL;
    void *ostr2 = NULL;
    int  osize1 = size*sizeof(mtext);
    int  osize2 = -1;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, str,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, fmt,  FALSE);

    /* init output buffer in case of OCI failure */
 
    str[0] = 0;

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    ostr1 = OCI_GetInputMetaString(str, &osize1);
    ostr2 = OCI_GetInputMetaString(fmt, &osize2);

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeToText((dvoid *) OCILib.env, tmsp->err,
                           tmsp->handle, (OraText *) ostr2, 
                           (ub1) osize2, (ub1) precision, 
                           (OraText *) NULL, (size_t) 0, 
                           (ub4*) &osize1, (OraText *) ostr1)
                                     
    )

    OCI_GetOutputMetaString(ostr1, str, &osize1);

    OCI_ReleaseMetaString(ostr1);
    OCI_ReleaseMetaString(ostr2);

    /* set null string terminator */

    str[osize1/sizeof(mtext)] = 0;

#else

    OCI_NOT_USED(ostr1);
    OCI_NOT_USED(ostr2);
    OCI_NOT_USED(osize1);
    OCI_NOT_USED(osize2);
    OCI_NOT_USED(precision);

#endif

   OCI_RESULT(res);

   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetDate
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampGetDate(OCI_Timestamp *tmsp, int *year, int *month,
                                     int *day)
{
    boolean res = TRUE;
    sb2 yr = 0;
    ub1 mt = 0;
    ub1 dy = 0;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);

    OCI_CHECK_PTR(OCI_IPC_INT, year,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, month, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, day,   FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

    *year  = 0;
    *month = 0;
    *day   = 0;

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeGetDate((dvoid *) OCILib.env, tmsp->err, tmsp->handle,
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

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampGetTime(OCI_Timestamp *tmsp, int *hour, int *min,
                                    int *sec, int *fsec)
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

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeGetTime((dvoid *) OCILib.env, tmsp->err, tmsp->handle,
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

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetDateTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampGetDateTime(OCI_Timestamp *tmsp, int *year, 
                                         int *month, int *day, int *hour, 
                                         int *min, int *sec, int *fsec)
{
    return (OCI_TimestampGetDate(tmsp, year, month, day) &&
            OCI_TimestampGetTime(tmsp, hour, min, sec, fsec));
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetTimeZoneName
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampGetTimeZoneName(OCI_Timestamp *tmsp, int size,
                                             mtext *str)
{
    boolean res = TRUE;
    void *ostr  = NULL;
    int osize   = size*sizeof(mtext);

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_STRING, str, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    ostr = OCI_GetInputMetaString(str, &osize);

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeGetTimeZoneName((dvoid *) OCILib.env, tmsp->err, tmsp->handle,
                                   (ub1*) ostr, (ub4*) &osize)
    )

    OCI_GetOutputMetaString(ostr, str, &osize);

    OCI_ReleaseMetaString(ostr);

    /* set null string terminator */

    str[osize/sizeof(mtext)] = 0;

#else

    OCI_NOT_USED(str);
    OCI_NOT_USED(size);
    OCI_NOT_USED(ostr);
    OCI_NOT_USED(osize);

#endif

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampGetTimeZoneOffset
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampGetTimeZoneOffset(OCI_Timestamp *tmsp,
                                              int *hour, int *min)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, hour, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INT, min, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeGetTimeZoneOffset((dvoid *) OCILib.env, tmsp->err,
                                     tmsp->handle, (sb1*) hour, (sb1*) min)
    )

#else

    OCI_NOT_USED(hour);
    OCI_NOT_USED(min);

#endif

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampIntervalAdd
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampIntervalAdd(OCI_Timestamp *tmsp, OCI_Interval *itv)
{
    boolean res  = TRUE;
    OCI_Timestamp *tmp = NULL;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INTERVAL, itv,  FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* OCIDateTimeIntervalAdd() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if ((res == TRUE) && (tmsp->type != OCI_TIMESTAMP_TZ))    
    {
        tmp = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        res = OCI_TimestampConvert(tmp, tmsp);
    }
    else
        tmp = tmsp;

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeIntervalAdd((dvoid *) OCILib.env, tmp->err, tmp->handle,
                               itv->handle, tmp->handle)
    )

    /* converting back */

    if ((res == TRUE) && (tmsp->type != OCI_TIMESTAMP_TZ))    
    {
        res = OCI_TimestampConvert(tmsp, tmp);

        OCI_TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp);

#endif

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampIntervalSub
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampIntervalSub(OCI_Timestamp *tmsp,
                                         OCI_Interval *itv)
{
    boolean res  = TRUE;
    OCI_Timestamp *tmp = NULL;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INTERVAL, itv,  FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* OCIDateTimeIntervalSub() fails if timestamps is not OCI_TIMESTAMP_TZ */

    if ((res == TRUE) && (tmsp->type != OCI_TIMESTAMP_TZ))    
    {
        tmp = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        res = OCI_TimestampConvert(tmp, tmsp);
    }
    else
        tmp = tmsp;

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeIntervalSub((dvoid *) OCILib.env, tmp->err, tmp->handle, 
                               itv->handle, tmp->handle)
    )

    /* converting back */

    if ((res == TRUE) && (tmsp->type != OCI_TIMESTAMP_TZ))    
    {
        res = OCI_TimestampConvert(tmsp, tmp);

        OCI_TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp);

#endif

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampSubtract
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampSubtract(OCI_Timestamp *tmsp, OCI_Timestamp *tmsp2,
                                     OCI_Interval *itv)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp,  FALSE);
    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp2, FALSE);
    OCI_CHECK_PTR(OCI_IPC_INTERVAL, itv,   FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeSubtract((dvoid *) OCILib.env, tmsp->err, tmsp->handle,
                             tmsp2->handle, itv->handle)
    )

#endif

   OCI_RESULT(res);

   return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampSysTimeStamp
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampSysTimeStamp(OCI_Timestamp *tmsp)
{
    boolean res = TRUE;
    OCI_Timestamp *tmp  = NULL;
    OCIDateTime *handle = NULL;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);
 
    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

#if OCI_VERSION_COMPILE >= OCI_9

    /* Filling a timestamp handle of type OCI_TIMESTAMP with 
       OCIDateTimeSysTimeStamp() can lead later to an error ORA-01483 when
       binding the given timestamp to some SQL Statement (Oracle BUG).
       The only way to avoid that is to pass to OCIDateTimeSysTimeStamp() 
       a timestamp handle of type OCI_TIMESTAMP_TZ and convert it back to
       OCI_TIMESTAMP if needed
    */

    if ((res == TRUE) && (tmsp->type == OCI_TIMESTAMP))    
    {
        tmp = OCI_TimestampCreate(tmsp->con, OCI_TIMESTAMP_TZ);

        handle = tmp->handle;
    }
    else
        handle = tmsp->handle;

    OCI_CALL4
    (
        res, tmsp->err, tmsp->con,
        
        OCIDateTimeSysTimeStamp((dvoid *) OCILib.env, tmsp->err, handle)
    )

    if ((res == TRUE) && (tmsp->type == OCI_TIMESTAMP))    
    {
        res = OCI_TimestampConvert(tmsp, tmp);

        OCI_TimestampFree(tmp);
    }

#else

    OCI_NOT_USED(tmp);

#endif

    OCI_RESULT(res);

    return res;
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampToCTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampToCTime(OCI_Timestamp *tmsp, struct tm *ptm,
                                     time_t *pt)
{
    boolean res = TRUE;
    time_t time = -1;
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

        if (ptm != NULL)
            memcpy(ptm, &t, sizeof(t));

        if (pt != NULL)
            *pt = time;
    }

    OCI_RESULT(res);

    return (time != -1);
}

/* ------------------------------------------------------------------------ *
 * OCI_TimestampFromCTime
 * ------------------------------------------------------------------------ */

boolean OCI_API OCI_TimestampFromCTime(OCI_Timestamp *tmsp, struct tm *ptm,
                                       time_t t)
{
    boolean res = TRUE;

    OCI_CHECK_PTR(OCI_IPC_TIMESTAMP, tmsp, FALSE);

    OCI_CHECK_TIMESTAMP_ENABLED(tmsp->con, FALSE);

    if (ptm == NULL && t == 0)
        return FALSE;

    if (ptm == NULL)
        ptm = localtime(&t);

    res =  OCI_TimestampConstruct(tmsp, 
                                  ptm->tm_year + 1900,
                                  ptm->tm_mon  + 1,
                                  ptm->tm_mday,
                                  ptm->tm_hour,
                                  ptm->tm_min,
                                  ptm->tm_sec,
                                  (int) 0,
                                  (const mtext *) NULL);

    OCI_RESULT(res);

    return res;
}
