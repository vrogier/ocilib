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

#include "interval.h"

#include "array.h"
#include "macros.h"
#include "strings.h"

#if OCI_VERSION_COMPILE >= OCI_9_0
static const unsigned int IntervalTypeValues[] = { OCI_INTERVAL_YM, OCI_INTERVAL_DS };
#endif

/* --------------------------------------------------------------------------------------------- *
 * IntervalInit
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * IntervalInitialize
(
    OCI_Connection *con,
    OCI_Interval   *itv,
    OCIInterval    *buffer,
    ub4             type
)
{
    DECLARE_CTX(TRUE)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_9_0
 
    ALLOC_DATA(OCI_IPC_INTERVAL, itv, 1);

    if (STATUS)
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
                STATUS = MemoryAllocDescriptor((dvoid  *)itv->env, (dvoid **)(void *)&itv->handle, (ub4)ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, itv->type));

                itv->hstate = OCI_OBJECT_ALLOCATED;
            }
        }
        else
        {
            itv->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }

    /* check for failure */

    if (!STATUS && itv)
    {
        IntervalFree(itv);
        itv = NULL;
    }

#else

    OCI_NOT_USED(con)
    OCI_NOT_USED(type)
    OCI_NOT_USED(buffer)

#endif

    return itv;
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Interval * IntervalCreate
(
    OCI_Connection *con,
    unsigned int    type
)
{
    CALL_ENTER(OCI_Interval*, NULL)
    CHECK_INITIALIZED()
    CHECK_INTERVAL_ENABLED(con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_9_0
    CHECK_ENUM_VALUE(con, NULL, type, IntervalTypeValues, OTEXT("Interval type"));
#endif

    RETVAL = IntervalInitialize(con, NULL, NULL, type);
    STATUS = (NULL != RETVAL);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalFree
 * --------------------------------------------------------------------------------------------- */

boolean IntervalFree
(
    OCI_Interval *itv
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_INTERVAL_ENABLED(itv->con)
    CHECK_OBJECT_FETCHED(itv);
    CTX_SET_FROM_OBJ(itv)

    if (OCI_OBJECT_ALLOCATED == itv->hstate)
    {
        MemoryFreeDescriptor((dvoid *)itv->handle, ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, itv->type));
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != itv->hstate)
    {
        FREE(itv)
    }

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Interval ** IntervalCreateArray
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    CALL_ENTER(OCI_Interval **, NULL)
    CHECK_INTERVAL_ENABLED(con)
    CTX_SET_FROM_CON(con)

#if OCI_VERSION_COMPILE >= OCI_9_0
    CHECK_ENUM_VALUE(con, NULL, type, IntervalTypeValues, OTEXT("Interval type"))
#endif

#if OCI_VERSION_COMPILE >= OCI_9_0

    arr = ArrayCreate(con, nbelem, OCI_CDT_INTERVAL, type,
                      sizeof(OCIInterval *), sizeof(OCI_Interval), 
                      ExternalSubTypeToHandleType(OCI_CDT_INTERVAL, type), NULL);

    STATUS = (NULL != arr);

    if (STATUS)
    {
        RETVAL = (OCI_Interval **) arr->tab_obj;
    }

#else

    OCI_NOT_USED(arr)
    OCI_NOT_USED(type)
    OCI_NOT_USED(nbelem)

#endif

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalArrayFree
 * --------------------------------------------------------------------------------------------- */

boolean IntervalFreeArray
(
    OCI_Interval **itvs
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_ARRAY, itvs)

    RETVAL = STATUS = ArrayFreeFromHandles((void **)itvs);

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalGetType
 * --------------------------------------------------------------------------------------------- */

unsigned int IntervalGetType
(
    OCI_Interval *itv
)
{
    GET_PROP(unsigned int, OCI_UNKNOWN, OCI_IPC_INTERVAL, itv, type, itv->con, NULL, itv->err)
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalAssign
 * --------------------------------------------------------------------------------------------- */

boolean IntervalAssign
(
    OCI_Interval *itv,
    OCI_Interval *itv_src
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INTERVAL, itv_src)
    CTX_SET_FROM_OBJ(itv)

 #if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIIntervalAssign((dvoid *) itv->env, itv->err, itv_src->handle, itv->handle))

 #endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalCheck
 * --------------------------------------------------------------------------------------------- */

int IntervalCheck
(
    OCI_Interval *itv
)
{
    ub4 value = (ub4) OCI_ERROR;
    
    CALL_ENTER(int, value)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CTX_SET_FROM_OBJ(itv)

 #if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIIntervalCheck((dvoid *) itv->env, itv->err, itv->handle, &value))

 #endif

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalCompare
 * --------------------------------------------------------------------------------------------- */

int IntervalCompare
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    sword value = OCI_ERROR;
    
    CALL_ENTER(int, value)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INTERVAL, itv2)
    CTX_SET_FROM_OBJ(itv)

 #if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIIntervalCompare((dvoid *) itv->env, itv->err, itv->handle, itv2->handle, &value))

 #endif

    RETVAL = value;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalFromString
 * --------------------------------------------------------------------------------------------- */

boolean IntervalFromString
(
    OCI_Interval *itv,
    const otext * str
)
{
    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_STRING, str)
    CTX_SET_FROM_OBJ(itv)

    dbstr = StringGetDBString(str, &dbsize);

 #if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIIntervalFromText((dvoid *) itv->env, itv->err, (OraText *) dbstr, (size_t) dbsize, itv->handle))

    StringReleaseDBString(dbstr);

#endif

    RETVAL = STATUS;
    
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalToString
 * --------------------------------------------------------------------------------------------- */

boolean IntervalToString
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

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_STRING, str)
    CTX_SET_FROM_OBJ(itv)

    /* initialize output buffer in case of OCI failure */

    str[0] = 0;

    dbstr = StringGetDBString(str, &dbsize);

    len = (size_t) dbsize;

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC
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

    StringCopyDBStringToNativeString(dbstr, str, dbcharcount(dbsize));
    StringReleaseDBString(dbstr);

    /* set null string terminator */

    str[dbcharcount(dbsize)] = 0;


    RETVAL = STATUS;
    
    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalFromTimeZone
 * --------------------------------------------------------------------------------------------- */

boolean IntervalFromTimeZone
(
    OCI_Interval *itv,
    const otext * str
)
{
    dbtext *dbstr  = NULL;
    int     dbsize = -1;

    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_STRING, str)
    CTX_SET_FROM_OBJ(itv)

    dbstr = StringGetDBString(str, &dbsize);

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIIntervalFromTZ((dvoid *) itv->env, itv->err, (CONST OraText *) dbstr, (size_t) dbsize, itv->handle))

#endif

    StringReleaseDBString(dbstr);

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalGetDaySecond
 * --------------------------------------------------------------------------------------------- */

boolean IntervalGetDaySecond
(
    OCI_Interval *itv,
    int          *day,
    int          *hour,
    int          *min,
    int          *sec,
    int          *fsec
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INT, hour)
    CHECK_PTR(OCI_IPC_INT, min)
    CHECK_PTR(OCI_IPC_INT, sec)
    CHECK_PTR(OCI_IPC_INT, fsec)
    CTX_SET_FROM_OBJ(itv)

    *day  = 0;
    *hour = 0;
    *min  = 0;
    *sec  = 0;
    *fsec = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC
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

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalGetYearMonth
 * --------------------------------------------------------------------------------------------- */

boolean IntervalGetYearMonth
(
    OCI_Interval *itv,
    int          *year,
    int          *month
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INT, year)
    CHECK_PTR(OCI_IPC_INT, month)
    CTX_SET_FROM_OBJ(itv)

    *year  = 0;
    *month = 0;

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIIntervalGetYearMonth((dvoid *) itv->env, itv->err, (sb4 *) year, (sb4 *) month, itv->handle))

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalSetDaySecond
 * --------------------------------------------------------------------------------------------- */

boolean IntervalSetDaySecond
(
    OCI_Interval *itv,
    int           day,
    int           hour,
    int           min,
    int           sec,
    int           fsec
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CTX_SET_FROM_OBJ(itv)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC
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

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalSetYearMonth
 * --------------------------------------------------------------------------------------------- */

boolean IntervalSetYearMonth
(
    OCI_Interval *itv,
    int           year,
    int           month
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CTX_SET_FROM_OBJ(itv)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIIntervalSetYearMonth((dvoid *) itv->env, itv->err, (sb4) year, (sb4) month, itv->handle))

#else

    OCI_NOT_USED(year)
    OCI_NOT_USED(month)

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalAdd
 * --------------------------------------------------------------------------------------------- */

boolean IntervalAdd
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INTERVAL, itv2)
    CTX_SET_FROM_OBJ(itv)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIIntervalAdd((dvoid *) itv->env, itv->err, itv->handle, itv2->handle, itv->handle))

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * IntervalSubtract
 * --------------------------------------------------------------------------------------------- */

boolean IntervalSubtract
(
    OCI_Interval *itv,
    OCI_Interval *itv2
)
{
    CALL_ENTER(boolean, FALSE)
    CHECK_PTR(OCI_IPC_INTERVAL, itv)
    CHECK_PTR(OCI_IPC_INTERVAL, itv2)
    CTX_SET_FROM_OBJ(itv)

#if OCI_VERSION_COMPILE >= OCI_9_0

    EXEC(OCIIntervalSubtract((dvoid *) itv->env, itv->err, itv->handle, itv2->handle, itv->handle))

#endif

    RETVAL = STATUS;

    CALL_EXIT()
}
