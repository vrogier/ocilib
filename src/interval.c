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
static const unsigned int IntervalTypeValues[] = { OCI_INTERVAL_YM, OCI_INTERVAL_DS };
#endif

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 *                         OCI_Interval functions
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OCI_IntervalInit
(
    OCI_Connection *con,
    OCI_Interval   *itv,
    OCIInterval    *buffer,
    ub4             type
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_9_0
 
    OCI_ALLOCATE_DATA(OCI_IPC_INTERVAL, itv, 1);

    if (OCI_STATUS)
    {
        itv->con    = con;
        itv->handle = buffer;
        itv->type   = type;

        /* get the right error handle */

        itv->err = con ? con->err : OCILib.err;
        itv->env = con ? con->env : OCILib.env;

        /* allocate buffer if needed */

        if (!itv->handle || (OCI_OBJECT_ALLOCATED_ARRAY == itv->hstate))
        {
            if (OCI_OBJECT_ALLOCATED_ARRAY != itv->hstate)
            {
                OCI_STATUS = OCI_DescriptorAlloc((dvoid  *)itv->env, (dvoid **)(void *)&itv->handle, (ub4)OCI_ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, itv->type));

                itv->hstate = OCI_OBJECT_ALLOCATED;
            }
        }
        else
        {
            itv->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }

    /* check for failure */

    if (!OCI_STATUS && itv)
    {
        OCI_IntervalFree(itv);
        itv = NULL;
    }

#else

    OCI_NOT_USED(con)
    OCI_NOT_USED(pitv)
    OCI_NOT_USED(type)
    OCI_NOT_USED(buffer)

#endif

    return itv;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * OCI_API OCI_IntervalCreate
(
    OCI_Connection *con,
    unsigned int    type
)
{
    OCI_CALL_ENTER(OCI_Interval*, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CHECK_INTERVAL_ENABLED(con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_9_0
    OCI_CALL_CHECK_ENUM_VALUE(con, NULL, type, IntervalTypeValues, OTEXT("Interval type"));
#endif

    OCI_RETVAL = OCI_IntervalInit(con, NULL, NULL, type);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalFree
(
    OCI_Interval *itv
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_INTERVAL_ENABLED(itv->con)
    OCI_CALL_CHECK_OBJECT_FETCHED(itv);
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

    if (OCI_OBJECT_ALLOCATED == itv->hstate)
    {
        OCI_DescriptorFree((dvoid *)itv->handle, OCI_ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, itv->type));
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != itv->hstate)
    {
        OCI_FREE(itv)
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Interval ** OCI_API OCI_IntervalArrayCreate
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_CALL_ENTER(OCI_Interval **, NULL)
    OCI_CALL_CHECK_INTERVAL_ENABLED(con)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

#if OCI_VERSION_COMPILE >= OCI_9_0
    OCI_CALL_CHECK_ENUM_VALUE(con, NULL, type, IntervalTypeValues, OTEXT("Interval type"))
#endif

#if OCI_VERSION_COMPILE >= OCI_9_0

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_INTERVAL, type,
                          sizeof(OCIInterval *), sizeof(OCI_Interval), 
                          OCI_ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, type), NULL);

    OCI_STATUS = (NULL != arr);

    if (OCI_STATUS)
    {
        OCI_RETVAL = (OCI_Interval **) arr->tab_obj;
    }

#else

    OCI_NOT_USED(arr)
    OCI_NOT_USED(type)
    OCI_NOT_USED(nbelem)

#endif

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalArrayFree
(
    OCI_Interval **itvs
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ARRAY, itvs)

    OCI_RETVAL = OCI_STATUS = OCI_ArrayFreeFromHandles((void **)itvs);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int OCI_API OCI_IntervalGetType
(
    OCI_Interval *itv
)
{
    OCI_GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_INTERVAL, itv, type, itv->con, NULL, itv->err)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalAssign
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalAssign
(
    OCI_Interval *itv,
    OCI_Interval *itv_src
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv_src)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

 #if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIIntervalAssign((dvoid *) itv->env, itv->err, itv_src->handle, itv->handle))

 #endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalCheck
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_IntervalCheck
(
    OCI_Interval *itv
)
{
    ub4 value = (ub4) OCI_ERROR;
    
    OCI_CALL_ENTER(int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

 #if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIIntervalCheck((dvoid *) itv->env, itv->err, itv->handle, &value))

 #endif

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalCompare
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_IntervalCompare
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    sword value = OCI_ERROR;
    
    OCI_CALL_ENTER(int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv2)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

 #if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIIntervalCompare((dvoid *) itv->env, itv->err, itv->handle, itv2->handle, &value))

 #endif

    OCI_RETVAL = value;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalFromText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalFromText
(
    OCI_Interval *itv,
    const otext * str
)
{
    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, str)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

    dbstr = OCI_StringGetOracleString(str, &dbsize);

 #if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIIntervalFromText((dvoid *) itv->env, itv->err, (OraText *) dbstr, (size_t) dbsize, itv->handle))

    OCI_StringReleaseOracleString(dbstr);

#endif

    OCI_RETVAL = OCI_STATUS;
    
    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalToText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalToText
(
    OCI_Interval *itv,
    int           leading_prec,
    int           fraction_prec,
    int           size,
    otext        *str
)
{
    dbtext *dbstr  = NULL;
    int     dbsize = size * (int)   sizeof(otext);
    size_t  len    = 0;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, str)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    dbstr = OCI_StringGetOracleString(str, &dbsize);

    len = (size_t) dbsize;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC
    (
        OCIIntervalToText((dvoid *) itv->env, itv->err,
                          (OCIInterval *) itv->handle,
                          (ub1) leading_prec, (ub1) fraction_prec,
                          (OraText *) dbstr, (size_t) dbsize,
                          (size_t *) &len)
    )

 #else

    OCI_NOT_USED(leading_prec)
    OCI_NOT_USED(fraction_prec)

 #endif

    dbsize = (int)len;

    OCI_StringCopyOracleStringToNativeString(dbstr, str, dbcharcount(dbsize));
    OCI_StringReleaseOracleString(dbstr);

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;


    OCI_RETVAL = OCI_STATUS;
    
    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalFromTimeZone
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalFromTimeZone
(
    OCI_Interval *itv,
    const otext * str
)
{
    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_PTR(OCI_IPC_STRING, str)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

    dbstr = OCI_StringGetOracleString(str, &dbsize);

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIIntervalFromTZ((dvoid *) itv->env, itv->err, (CONST OraText *) dbstr, (size_t) dbsize, itv->handle))

#endif

    OCI_StringReleaseOracleString(dbstr);

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalGetDaySecond
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalGetDaySecond
(
    OCI_Interval *itv,
    int          *day,
    int          *hour,
    int          *min,
    int          *sec,
    int          *fsec
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, hour)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, min)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, sec)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, fsec)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

    *day  = 0;
    *hour = 0;
    *min  = 0;
    *sec  = 0;
    *fsec = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC
    (
        OCIIntervalGetDaySecond((dvoid *) itv->env, itv->err,
                                (sb4 *) day, (sb4 *) hour, (sb4 *) min,
                                (sb4 *) sec, (sb4 *) fsec, itv->handle)
    )

#else

    OCI_NOT_USED(day)
    OCI_NOT_USED(hour)
    OCI_NOT_USED(min)
    OCI_NOT_USED(sec)
    OCI_NOT_USED(fsec)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalGetYearMonth
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalGetYearMonth
(
    OCI_Interval *itv,
    int          *year,
    int          *month
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, year)
    OCI_CALL_CHECK_PTR(OCI_IPC_INT, month)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

    *year  = 0;
    *month = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIIntervalGetYearMonth((dvoid *) itv->env, itv->err, (sb4 *) year, (sb4 *) month, itv->handle))

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalSetDaySecond
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalSetDaySecond
(
    OCI_Interval *itv,
    int           day,
    int           hour,
    int           min,
    int           sec,
    int           fsec
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC
    (
        OCIIntervalSetDaySecond((dvoid *) itv->env, itv->err,
                                (sb4) day, (sb4) hour, (sb4) min,
                                (sb4) sec, (sb4) fsec, itv->handle)
    )

#else

    OCI_NOT_USED(day)
    OCI_NOT_USED(hour)
    OCI_NOT_USED(min)
    OCI_NOT_USED(sec)
    OCI_NOT_USED(fsec)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalSetYearMonth
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalSetYearMonth
(
    OCI_Interval *itv,
    int           year,
    int           month
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIIntervalSetYearMonth((dvoid *) itv->env, itv->err, (sb4) year, (sb4) month, itv->handle))

#else

    OCI_NOT_USED(year)
    OCI_NOT_USED(month)

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalAdd
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalAdd
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv2)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIIntervalAdd((dvoid *) itv->env, itv->err, itv->handle, itv2->handle, itv->handle))

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_IntervalSubtract
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_IntervalSubtract
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv)
    OCI_CALL_CHECK_PTR(OCI_IPC_INTERVAL, itv2)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(itv)

#if OCI_VERSION_COMPILE >= OCI_9_0

    OCI_EXEC(OCIIntervalSubtract((dvoid *) itv->env, itv->err, itv->handle, itv2->handle, itv->handle))

#endif

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}
