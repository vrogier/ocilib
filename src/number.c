/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
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


typedef struct MagicNumber
{
    unsigned char number[3];
    otext* name;
} MagicNumber;

static MagicNumber MagicNumbers[] = 
{
    { { 2, 255, 101 }, OTEXT("~") },
    { { 1,   0,   0 }, OTEXT("-~") }
};

#define OCI_MAGIC_NUMBER_COUNT 2


/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberGet
 * --------------------------------------------------------------------------------------------- */

boolean OCI_NumberGet
(
    OCI_Connection *con,
    void           *number,
    uword           size,
    uword           type,
    int             sqlcode,
    void           *out_value
)
{
    boolean res = TRUE;

    OCI_CHECK(NULL == con, FALSE)
    OCI_CHECK(NULL == number, FALSE)
    OCI_CHECK(NULL == out_value, FALSE)

#if OCI_VERSION_COMPILE < OCI_10_1

    OCI_NOT_USED(sqlcode)

#endif

    if (OCI_NUM_NUMBER == type)
    {
        memcpy(out_value, number, size);
    }
    else if (type & OCI_NUM_DOUBLE || type & OCI_NUM_FLOAT)
    {

    #if OCI_VERSION_COMPILE >= OCI_10_1

        if ((OCILib.version_runtime >= OCI_10_1) && ((sqlcode != SQLT_VNU)))
        {
            if (((type & OCI_NUM_DOUBLE) && (SQLT_BDOUBLE == sqlcode)) ||
                ((type & OCI_NUM_FLOAT ) && (SQLT_BFLOAT  == sqlcode)))
            {
                memcpy(out_value, number, size);
            }
            else if (type & OCI_NUM_DOUBLE && (SQLT_BFLOAT == sqlcode))
            {
                *((double *) out_value) = (double) *((float *) number);
            }
            else if (type & OCI_NUM_FLOAT && (SQLT_BDOUBLE == sqlcode))
            {
                 *((float *) out_value) = (float) *((double *) number);
            }
        }
        else

    #endif

        {
            OCI_CALL2
            (
                res, con,

                OCINumberToReal(con->err, (OCINumber *) number, size, out_value)
            )
        }
    }
    else
    {
        uword sign = (type & OCI_NUM_UNSIGNED) ? OCI_NUMBER_UNSIGNED : OCI_NUMBER_SIGNED;

        OCI_CALL2
        (
            res, con,

            OCINumberToInt(con->err, (OCINumber *) number, size, sign, out_value)
        )
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberSet
 * --------------------------------------------------------------------------------------------- */

boolean OCI_NumberSet
(
    OCI_Connection *con,
    void           *number,
    uword           size,
    uword           type,
    int             sqlcode,
    void           *in_value
)
{
    boolean res = TRUE;

    OCI_CHECK(NULL == con, FALSE)
    OCI_CHECK(NULL == number, FALSE)
    OCI_CHECK(NULL == in_value, FALSE)

#if OCI_VERSION_COMPILE < OCI_10_1

    OCI_NOT_USED(sqlcode)

#endif

    if (type & OCI_NUM_NUMBER)
    {
        memcpy(number, in_value, sizeof(OCINumber));
    }
    else if (type & OCI_NUM_DOUBLE || type & OCI_NUM_FLOAT)
    {

    #if OCI_VERSION_COMPILE >= OCI_10_1

        if ((OCILib.version_runtime >= OCI_10_1) && ((sqlcode != SQLT_VNU)))
        {
            if (((type & OCI_NUM_DOUBLE) && (SQLT_BDOUBLE == sqlcode)) ||
                ((type & OCI_NUM_FLOAT ) && (SQLT_BFLOAT  == sqlcode)))
            {
                memcpy(number, in_value, size);
            }
            else if (type & OCI_NUM_DOUBLE && SQLT_BFLOAT == sqlcode)
            {
                *((double *) number) = (double) *((float *) in_value);
            }
            else if (type & OCI_NUM_FLOAT && SQLT_BDOUBLE == sqlcode)
            {
                 *((float *) number) = (float) *((double *) in_value);
            }
        }
        else

    #endif

        {
            OCI_CALL2
            (
                res, con,

                OCINumberFromReal(con->err, in_value, size, (OCINumber *) number)
            )
        }
    }
    else
    {
        uword sign = (type & OCI_NUM_UNSIGNED) ? OCI_NUMBER_UNSIGNED : OCI_NUMBER_SIGNED;

        OCI_CALL2
        (
            res, con,

            OCINumberFromInt(con->err, in_value, size, sign, (OCINumber *) number)
        )
    }

    return res;
}


/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberFromString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_NumberFromString
(
    OCI_Connection *con,
    void           *out_value,
    uword           size,
    uword           type,
    int             sqlcode,
    const otext    *in_value,
    const otext   * fmt
)
{
    boolean res  = TRUE;
    boolean done = FALSE;

    /* For binary types, perform a C based conversion */

    if (type & OCI_NUM_DOUBLE || type & OCI_NUM_FLOAT || (SQLT_VNU != sqlcode))
    {

#if OCI_VERSION_COMPILE >= OCI_12_1

        if ((OCILib.version_runtime >= OCI_12_1) && ((SQLT_VNU != sqlcode)))
        {
            if (type & OCI_NUM_SHORT)
            {
                res = (osscanf(in_value, OCI_STRING_FORMAT_NUM_SHORT, (short *)out_value) == 1);
                done = TRUE;
            }

            if (type & OCI_NUM_INT)
            {
                res = (osscanf(in_value, OCI_STRING_FORMAT_NUM_INT, (int *)out_value) == 1);
                done = TRUE;
            }
        }

#endif

    #if OCI_VERSION_COMPILE >= OCI_10_1

        if (!done && OCILib.version_runtime >= OCI_10_1)
        {
            fmt = OCI_GetFormat(con, type & OCI_NUM_DOUBLE ? OCI_FMT_BINARY_DOUBLE : OCI_FMT_BINARY_FLOAT);

            if (type & OCI_NUM_DOUBLE)
            {
                res = (osscanf(in_value, fmt, (double *)out_value) == 1);
            }
            else if (type & OCI_NUM_FLOAT)
            {
                res = (osscanf(in_value, fmt, (float *)out_value) == 1);
            }

            done = TRUE;
        }

    #endif

    }

    /* use OCINumber conversion if not processed yet */

    if (!done)
    {
        int i;

        for (i = 0; i < OCI_MAGIC_NUMBER_COUNT; i++)
        {
            MagicNumber *mag_num = &MagicNumbers[i];

            if (ostrcmp(in_value, mag_num->name) == 0)
            {
                memset(out_value, 0, sizeof(OCINumber));
                memcpy(out_value, mag_num->number, sizeof(mag_num->number));
                res = done = TRUE;
                break;
            }
        }

        if (!done)
        {
            dbtext *dbstr1  = NULL;
            dbtext *dbstr2  = NULL;
            int     dbsize1 = -1;
            int     dbsize2 = -1;
            OCINumber number;

            if (!fmt)
            {
                fmt = OCI_GetFormat(con, OCI_FMT_NUMERIC);
            }

            dbstr1 = OCI_StringGetOracleString(in_value, &dbsize1);
            dbstr2 = OCI_StringGetOracleString(fmt, &dbsize2);

            memset(&number, 0, sizeof(number));

            OCI_CALL2
            (
                res, con,

                OCINumberFromText(con->err, (oratext *) dbstr1, (ub4) dbsize1, (oratext *) dbstr2,
                                    (ub4) dbsize2, (oratext *) NULL,  (ub4) 0, (OCINumber *) &number)
            )

            OCI_StringReleaseOracleString(dbstr2);
            OCI_StringReleaseOracleString(dbstr1);

            res = res && OCI_NumberGet(con, (void *) &number, size, type, sqlcode, out_value);
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberToString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_NumberToString
(
    OCI_Connection *con,
    void           *number,
    unsigned int    type,
    int             sqlcode,
    otext          *out_value,
    int             out_value_size,
    const otext   * fmt
)
{
    boolean res  = TRUE;
    boolean done = FALSE;

    out_value[0] = 0;

    /* For binary types, perform a C based conversion */

    if (type & OCI_NUM_DOUBLE || type & OCI_NUM_FLOAT || (SQLT_VNU != sqlcode))
    {

    #if OCI_VERSION_COMPILE >= OCI_12_1

        if ((OCILib.version_runtime >= OCI_12_1) && ((SQLT_VNU != sqlcode)))
        {
            if (type & OCI_NUM_SHORT)
            {
                out_value_size = osprintf(out_value, out_value_size, OCI_STRING_FORMAT_NUM_SHORT, *((short *)number));
                done = TRUE;
            }

            if (type & OCI_NUM_INT)
            {
                out_value_size = osprintf(out_value, out_value_size, OCI_STRING_FORMAT_NUM_INT, *((int *)number));
                done = TRUE;
            }
        }

    #endif

    #if OCI_VERSION_COMPILE >= OCI_10_1

        if (!done && (OCILib.version_runtime >= OCI_10_1) && ((SQLT_VNU != sqlcode)))
        {
            if (!fmt)
            {
                fmt = OCI_GetFormat(con, type & OCI_NUM_DOUBLE ? OCI_FMT_BINARY_DOUBLE : OCI_FMT_BINARY_FLOAT);
            }

            if (type & OCI_NUM_DOUBLE && (SQLT_BDOUBLE == sqlcode))
            {
                out_value_size = osprintf(out_value, out_value_size, fmt, *((double *)number));
            }
            else if (type & OCI_NUM_FLOAT && (SQLT_BFLOAT == sqlcode))
            {
                out_value_size = osprintf(out_value, out_value_size, fmt, *((float *)number));
            }

            done = TRUE;

            if ((out_value_size) > 0)
            {
                while (out_value[out_value_size-1] == OTEXT('0'))
                {
                    out_value[out_value_size-1] = 0;
                }

                out_value--;
            }
        }

    #else

        OCI_NOT_USED(sqlcode)

    #endif

    }

    /* use OCINumber conversion if not processed yet */

    if (!done)
    {
        int i;

        for (i = 0; i < OCI_MAGIC_NUMBER_COUNT; i++)
        {
            MagicNumber *mag_num = &MagicNumbers[i];

            if (memcmp(number, mag_num->number, sizeof(mag_num->number)) == 0)
            {
                ostrcpy(out_value, mag_num->name);
                res = done = TRUE;
                break;
            }
        }

        if (!done)
        {
            dbtext *dbstr1 = NULL;
            dbtext *dbstr2 = NULL;
            int     dbsize1 = out_value_size * (int) sizeof(otext);
            int     dbsize2 = -1;

            if (!fmt)
            {
                fmt = OCI_GetFormat(con, OCI_FMT_NUMERIC);
            }

            dbstr1 = OCI_StringGetOracleString(out_value, &dbsize1);
            dbstr2 = OCI_StringGetOracleString(fmt, &dbsize2);

            OCI_CALL2
            (
                res, con,

                OCINumberToText(con->err, (OCINumber *)number, (oratext *)dbstr2,
                (ub4)dbsize2, (oratext *)NULL, (ub4)0,
                (ub4 *)&dbsize1, (oratext *)dbstr1)
            )

            OCI_StringCopyOracleStringToNativeString(dbstr1, out_value, dbcharcount(dbsize1));
            OCI_StringReleaseOracleString(dbstr2);
            OCI_StringReleaseOracleString(dbstr1);

            out_value_size = (dbsize1 / (int) sizeof(dbtext));
        }
    }

    /* do we need to suppress last '.' or ',' from integers */

    if ((--out_value_size) >= 0)
    {
        if ((out_value[out_value_size] == OTEXT('.')) ||
            (out_value[out_value_size] == OTEXT(',')))
        {
            out_value[out_value_size] = 0;
        }
    }

    return res;
}

OCI_Number * OCI_NumberInit
(
    OCI_Connection  *con,
    OCI_Number     **pnumber,
    OCINumber        *buffer
)
{
    OCI_Number *number = NULL;
    boolean res = FALSE;

    OCI_CHECK(NULL == pnumber, NULL);

    if (!*pnumber)
    {
        *pnumber = (OCI_Number *)OCI_MemAlloc(OCI_IPC_NUMBER, sizeof(*number), (size_t) 1, TRUE);
    }

    if (*pnumber)
    {
        res = TRUE;

        number = *pnumber;

        number->con = con;
        number->handle = buffer;

        /* get the right error handle */

        number->err = con ? con->err : OCILib.err;
        number->env = con ? con->env : OCILib.env;

        /* allocate buffer if needed */

        if (!number->handle || (OCI_OBJECT_ALLOCATED_ARRAY == number->hstate))
        {
            if (OCI_OBJECT_ALLOCATED_ARRAY != number->hstate)
            {
                number->hstate = OCI_OBJECT_ALLOCATED;
                number->handle = (OCINumber *)OCI_MemAlloc(OCI_IPC_NUMBER, sizeof(*number->handle), (size_t)1, TRUE);

                res = (NULL != number->handle);
            }
        }
        else
        {
            number->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }

    /* check for failure */

    if (!res && number)
    {
        OCI_NumberFree(number);
        *pnumber = number = NULL;
    }

    return number;
}


OCI_Number * OCI_API OCI_NumberCreate
(
    OCI_Connection *con
)
{
    OCI_LIB_CALL_ENTER(OCI_Number*, NULL)

    OCI_CHECK_INITIALIZED()

    call_retval = OCI_NumberInit(con, &call_retval, NULL);
    call_status = (NULL != call_retval);

    OCI_LIB_CALL_EXIT()
}

boolean OCI_API OCI_NumberFree
(
    OCI_Number *number
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_NUMBER, number)
    OCI_CHECK_OBJECT_FETCHED(number);

    if (OCI_OBJECT_ALLOCATED == number->hstate)
    {
        OCI_FREE(number->handle)
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != number->hstate)
    {
        OCI_FREE(number)
    }

    call_retval = call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

OCI_Number ** OCI_API OCI_NumberArrayCreate
(
    OCI_Connection *con,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_LIB_CALL_ENTER(OCI_Number **, NULL)

    OCI_CHECK_INITIALIZED()

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_NUMERIC, OCI_NUM_NUMBER, sizeof(OCINumber), sizeof(OCI_Number), 0, NULL);

    if (arr)
    {
        call_retval = (OCI_Number **)arr->tab_obj;
        call_status = TRUE;
    }

    OCI_LIB_CALL_EXIT()
}

OCI_EXPORT boolean OCI_API OCI_NumberArrayFree
(
    OCI_Number **numbmers
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_ARRAY, numbmers)

    call_retval = call_status = OCI_ArrayFreeFromHandles((void **)numbmers);

    OCI_LIB_CALL_EXIT()

}

boolean OCI_API OCI_NumberAssign
(
    OCI_Number *number,
    OCI_Number *number_src
)
{
    OCI_LIB_CALL_ENTER(boolean, FALSE)

    OCI_CHECK_PTR(OCI_IPC_NUMBER, number)
    OCI_CHECK_PTR(OCI_IPC_NUMBER, number_src)

    call_status = TRUE;

    OCI_CALL4
    (
        call_status, number->err, number->con,

        OCINumberAssign(number->err, number_src->handle, number->handle)
    )

    call_retval = call_status;

    OCI_LIB_CALL_EXIT()
}


boolean OCI_API OCI_NumberToText
(
    OCI_Number  *number,
    const otext *fmt,
    int          size,
    otext       *str
)
{
    OCI_LIB_CALL_ENTER(boolean, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_NUMBER, number)

    call_retval = OCI_NumberToString(number->con, number->handle, OCI_NUM_NUMBER, SQLT_VNU, str, size, fmt);
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}

boolean OCI_API OCI_NumberFromText
(
    OCI_Number  *number,
    const otext *str,
    const otext *fmt
)
{
    OCI_LIB_CALL_ENTER(boolean, OCI_UNKNOWN)

    OCI_CHECK_PTR(OCI_IPC_NUMBER, number)

    call_retval = OCI_NumberFromString(number->con, number->handle, sizeof(OCINumber), OCI_NUM_NUMBER, SQLT_VNU, str, fmt);
    call_status = TRUE;

    OCI_LIB_CALL_EXIT()
}
