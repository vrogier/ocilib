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

typedef struct MagicNumber
{
    unsigned char number[3];
    const otext  *name;
} MagicNumber;

static const MagicNumber MagicNumbers[] =
{
    { { 2, 255, 101 }, OTEXT("~") },
    { { 1,   0,   0 }, OTEXT("-~") }
};

#define OCI_MAGIC_NUMBER_COUNT 2


#define OCI_NUMBER_OPERATION(func)                                                  \
                                                                                    \
    OCINumber src_num = { {0} };                                                    \
                                                                                    \
    OCI_CALL_ENTER(boolean, FALSE)                                                  \
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number)                                      \
    OCI_CALL_CONTEXT_SET_FROM_OBJ(number)                                           \
                                                                                    \
    OCI_STATUS = OCI_TranslateNumericValue(number->con, value, type,                \
                                           &src_num, OCI_NUM_NUMBER);               \
                                                                                    \
    OCI_EXEC(func(number->err, number->handle, &src_num, number->handle))           \
                                                                                    \
    OCI_RETVAL = OCI_STATUS;                                                        \
    OCI_CALL_EXIT()                                                                 \


/* --------------------------------------------------------------------------------------------- *
* OCI_GetNumericTypeSize
* --------------------------------------------------------------------------------------------- */

uword OCI_GetNumericTypeSize
(
    unsigned int type
)
{
    uword size = 0;

    if (type & OCI_NUM_SHORT)
    {
        size = sizeof(short);
    }
    else if (type & OCI_NUM_INT)
    {
        size = sizeof(int);
    }
    else if (type & OCI_NUM_BIGINT)
    {
        size = sizeof(big_int);
    }
    else if (type & OCI_NUM_FLOAT)
    {
        size = sizeof(float);
    }
    else if (type & OCI_NUM_DOUBLE)
    {
        size = sizeof(double);
    }
    else if (type & OCI_NUM_NUMBER)
    {
        size = sizeof(OCINumber);
    }
    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_TranslateNumericValue
 * --------------------------------------------------------------------------------------------- */

boolean OCI_TranslateNumericValue
(
    OCI_Connection *con,
    void           *in_value,
    uword           in_type,
    void           *out_value,
    uword           out_type
)
{
    /* translate numeric values:
        - signed/unsigned integers (short, int, big_int) to double, float and OCINumber
        - double, float to signed/unsigned integers and OCINumber
        - OCINumber to signed/unsigned integers, double, and float
    */

    const uword in_size  = OCI_GetNumericTypeSize(in_type);
    const uword out_size = OCI_GetNumericTypeSize(out_type);
    const uword in_sign  = (in_type & OCI_NUM_UNSIGNED) ? OCI_NUMBER_UNSIGNED : OCI_NUMBER_SIGNED;
    const uword out_sign = (out_type & OCI_NUM_UNSIGNED) ? OCI_NUMBER_UNSIGNED : OCI_NUMBER_SIGNED;

    OCINumber tmp;
    memset(&tmp, 0, sizeof(tmp));

    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == in_value, FALSE)
    OCI_CHECK(NULL == out_value, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    if (in_type == out_type)
    {
        /* same type, no conversions needed. Just copy memory.
           We could have used assignments but it would require explicit casting using a switch statement
        */
        memcpy(out_value, in_value, out_size);
    }
    else if (OCI_NUM_NUMBER == in_type)
    {
        if (out_type & OCI_NUM_DOUBLE || out_type & OCI_NUM_FLOAT)
        {
            /* OCINumber to double / float */

            OCI_EXEC(OCINumberToReal(ctx->oci_err, in_value, out_size, out_value))
        }
        else
        {
            /* OCINumber to integers */

            OCI_EXEC(OCINumberToInt(ctx->oci_err, in_value, out_size, out_sign, out_value))
        }
    }
    else if (in_type & OCI_NUM_DOUBLE || in_type & OCI_NUM_FLOAT)
    {
        if (out_type == OCI_NUM_NUMBER)
        {
            /* double / float to OCINumber */

            OCI_EXEC(OCINumberFromReal(ctx->oci_err, in_value, in_size, (OCINumber *)out_value))
        }
        else  if (out_type & OCI_NUM_DOUBLE || out_type & OCI_NUM_FLOAT)
        {
            /* double to float and float to double */

        #if OCI_VERSION_COMPILE >= OCI_10_1

            if (OCILib.version_runtime >= OCI_10_1)
            {
                if (in_type & OCI_NUM_FLOAT && (out_type & OCI_NUM_DOUBLE))
                {
                    *((double *)out_value) = (double) *((float *)in_value);
                }
                else if (in_type & OCI_NUM_DOUBLE && (out_type & OCI_NUM_FLOAT))
                {
                    *((float *)out_value) = (float) *((double *)in_value);
                }
            }

        #endif     

        }
        else
        {
            /* double / float to integers */

            OCI_EXEC(OCINumberFromReal(ctx->oci_err, in_value, in_size, (OCINumber *)&tmp))
            OCI_EXEC(OCINumberToInt(ctx->oci_err, &tmp, out_size, out_sign, out_value))
        }
    }
    else
    {
        OCI_EXEC(OCINumberFromInt(ctx->oci_err, in_value, in_size, in_sign, (OCINumber *)&tmp))
            
        if (out_type == OCI_NUM_NUMBER)
        {
            /* integers to OCINumber */

            memcpy(out_value, &tmp, sizeof(tmp));
        }
        else if (out_type & OCI_NUM_DOUBLE || out_type & OCI_NUM_FLOAT)
        {
            /* integers to double / float */

            OCI_EXEC(OCINumberToReal(ctx->oci_err, &tmp, out_size, out_value))
        }
        else
        {    
            /* only for conversions between integers with different sizes or sign */

            OCI_EXEC(OCINumberToInt(ctx->oci_err, &tmp, out_size, out_sign, out_value))
        }
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberFromString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_NumberFromString
(
    OCI_Connection *con,
    void           *out_value,
    uword           type,
    const otext    *in_value,
    const otext   * fmt
)
{
    boolean done = FALSE;

    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == out_value, FALSE)
    OCI_CHECK(NULL == in_value, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    /* For binary types, perform a C based conversion */

    if (type != OCI_NUM_NUMBER)
    {
        if (type & OCI_NUM_SHORT)
        {
            OCI_STATUS = (osscanf(in_value, OCI_STRING_FORMAT_NUM_SHORT, (short *)out_value) == 1);
            done = TRUE;
        }
        else if (type & OCI_NUM_INT)
        {
            OCI_STATUS = (osscanf(in_value, OCI_STRING_FORMAT_NUM_INT, (int *)out_value) == 1);
            done = TRUE;
        }

    #if OCI_VERSION_COMPILE >= OCI_10_1

        if (!done && OCILib.version_runtime >= OCI_10_1)
        {
            const otext *tmp_fmt = fmt;

            if (!tmp_fmt)
            {
                tmp_fmt = OCI_GetFormat(con, type & OCI_NUM_DOUBLE ? OCI_FMT_BINARY_DOUBLE : OCI_FMT_BINARY_FLOAT);
            }

            if (type & OCI_NUM_DOUBLE)
            {
                OCI_STATUS = (osscanf(in_value, tmp_fmt, (double *)out_value) == 1);
                done = TRUE;
            }
            else if (type & OCI_NUM_FLOAT)
            {
                OCI_STATUS = (osscanf(in_value, tmp_fmt, (float *)out_value) == 1);
                done = TRUE;
            }
        }

    #endif

    }

    /* use OCINumber conversion if not processed yet */

    if (!done)
    {
        for (int i = 0; i < OCI_MAGIC_NUMBER_COUNT; i++)
        {
            const MagicNumber *mag_num = &MagicNumbers[i];

            if (ostrcmp(in_value, mag_num->name) == 0)
            {
                memset(out_value, 0, sizeof(OCINumber));
                memcpy(out_value, mag_num->number, mag_num->number[0] + 1);
                OCI_STATUS = done = TRUE;
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

            OCI_EXEC
            (
                OCINumberFromText(ctx->oci_err, (oratext *) dbstr1, (ub4) dbsize1, (oratext *) dbstr2,
                                   (ub4) dbsize2, (oratext *) NULL,  (ub4) 0, (OCINumber *) &number)
            )

            OCI_StringReleaseOracleString(dbstr2);
            OCI_StringReleaseOracleString(dbstr1);

            OCI_STATUS = OCI_STATUS && OCI_TranslateNumericValue(con, &number, OCI_NUM_NUMBER, out_value, type);
        }
    }

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberToString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_NumberToString
(
    OCI_Connection *con,
    void           *number,
    unsigned int    type,
    otext          *out_value,
    int             out_value_size,
    const otext   * fmt
)
{
    boolean   done = FALSE;

    OCI_CALL_DECLARE_CONTEXT(TRUE)

    OCI_CHECK(NULL == out_value, FALSE)
    OCI_CHECK(NULL == number, FALSE)

    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    out_value[0] = 0;

    /* For binary types, perform a C based conversion */

    if (type != OCI_NUM_NUMBER)
    {
        if (type & OCI_NUM_SHORT)
        {
            out_value_size = osprintf(out_value, out_value_size, OCI_STRING_FORMAT_NUM_SHORT, *((short *)number));
            done = TRUE;
        }
        else if (type & OCI_NUM_INT)
        {
            out_value_size = osprintf(out_value, out_value_size, OCI_STRING_FORMAT_NUM_INT, *((int *)number));
            done = TRUE;
        }

    #if OCI_VERSION_COMPILE >= OCI_10_1

        if (!done && (OCILib.version_runtime >= OCI_10_1))
        {
            const otext *tmp_fmt = fmt;

            if (!tmp_fmt)
            {
                tmp_fmt = OCI_GetFormat(con, type & OCI_NUM_DOUBLE ? OCI_FMT_BINARY_DOUBLE : OCI_FMT_BINARY_FLOAT);
            }

            if (type & OCI_NUM_DOUBLE)
            {
                out_value_size = osprintf(out_value, out_value_size, tmp_fmt, *((double *)number));
                done = TRUE;
            }
            else if (type & OCI_NUM_FLOAT)
            {
                out_value_size = osprintf(out_value, out_value_size, tmp_fmt, *((float *)number));
                done = TRUE;
            }

            if (done)
            {
                if ((out_value_size) > 0)
                {
                    while (out_value[out_value_size - 1] == OTEXT('0'))
                    {
                        out_value[out_value_size - 1] = 0;
                    }

                    out_value--;
                }
            }
        }

    #endif

    }

    /* use OCINumber conversion if not processed yet */

    if (!done)
    {
        for (int i = 0; i < OCI_MAGIC_NUMBER_COUNT; i++)
        {
            const MagicNumber *mag_num = &MagicNumbers[i];

            if (memcmp(number, mag_num->number, mag_num->number[0] + 1) == 0)
            {
                ostrcpy(out_value, mag_num->name);
                OCI_STATUS = done = TRUE;
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

            OCI_EXEC
            (
                OCINumberToText(ctx->oci_err, (OCINumber *)number, (oratext *)dbstr2,
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

    return OCI_STATUS;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberInit
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OCI_NumberInit
(
    OCI_Connection  *con,
    OCI_Number      *number,
    OCINumber       *buffer
)
{
    OCI_CALL_DECLARE_CONTEXT(TRUE)
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_ALLOCATE_DATA(OCI_IPC_NUMBER, number, 1);

    if (OCI_STATUS)
    {
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
                OCI_ALLOCATE_DATA(OCI_IPC_ARRAY, number->handle, 1)
            }
        }
        else
        {
            number->hstate = OCI_OBJECT_FETCHED_CLEAN;
        }
    }

    /* check for failure */

    if (!OCI_STATUS && number)
    {
        OCI_NumberFree(number);
        number = NULL;
    }

    return number;
}

/* ********************************************************************************************* *
 *                            PUBLIC FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OCI_API OCI_NumberCreate
(
    OCI_Connection *con
)
{
    OCI_CALL_ENTER(OCI_Number*, NULL)
    OCI_CALL_CHECK_INITIALIZED()
    OCI_CALL_CONTEXT_SET_FROM_CONN(con)

    OCI_RETVAL = OCI_NumberInit(con, NULL, NULL);
    OCI_STATUS = (NULL != OCI_RETVAL);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberAssign
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberFree
(
    OCI_Number *number
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number)
    OCI_CALL_CHECK_OBJECT_FETCHED(number);
    OCI_CALL_CONTEXT_SET_FROM_OBJ(number)

    if (OCI_OBJECT_ALLOCATED == number->hstate)
    {
        OCI_FREE(number->handle)
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != number->hstate)
    {
        OCI_FREE(number)
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberArrayCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Number ** OCI_API OCI_NumberArrayCreate
(
    OCI_Connection *con,
    unsigned int    nbelem
)
{
    OCI_Array *arr = NULL;

    OCI_CALL_ENTER(OCI_Number **, NULL)
    OCI_CALL_CHECK_INITIALIZED()

    arr = OCI_ArrayCreate(con, nbelem, OCI_CDT_NUMERIC, OCI_NUM_NUMBER, sizeof(OCINumber), sizeof(OCI_Number), 0, NULL);
    OCI_STATUS = (NULL != arr);

    if (OCI_STATUS)
    {
        OCI_RETVAL = (OCI_Number **)arr->tab_obj;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberArrayFree
 * --------------------------------------------------------------------------------------------- */

OCI_EXPORT boolean OCI_API OCI_NumberArrayFree
(
    OCI_Number **numbmers
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_ARRAY, numbmers)

    OCI_RETVAL = OCI_STATUS = OCI_ArrayFreeFromHandles((void **)numbmers);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberAssign
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberAssign
(
    OCI_Number *number,
    OCI_Number *number_src
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number_src)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(number)

    OCI_EXEC(OCINumberAssign(number->err, number_src->handle, number->handle))

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberToText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberToText
(
    OCI_Number  *number,
    const otext *fmt,
    int          size,
    otext       *str
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(number)

    OCI_RETVAL = OCI_NumberToString(number->con, number->handle, OCI_NUM_NUMBER, str, size, fmt);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberFromText
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberFromText
(
    OCI_Number  *number,
    const otext *str,
    const otext *fmt
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(number)

    OCI_RETVAL = OCI_NumberFromString(number->con, number->handle, OCI_NUM_NUMBER, str, fmt);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberGetContent
 * --------------------------------------------------------------------------------------------- */

unsigned char * OCI_API OCI_NumberGetContent
(
    OCI_Number *number
)
{
    OCI_CALL_ENTER(unsigned char *, NULL)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(number)

    if (number->handle)
    {
        OCI_RETVAL = number->handle->OCINumberPart;
    }

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberSetContent
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberSetContent
(
    OCI_Number     *number,
    unsigned char  *content
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number)
    OCI_CALL_CHECK_PTR(OCI_IPC_VOID, content)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(number)

    if (number->handle)
    {
        memcpy(number->handle->OCINumberPart, content, sizeof(number->handle->OCINumberPart));
    }

    OCI_RETVAL = OCI_STATUS;

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberSetValue
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberSetValue
(
    OCI_Number     *number,
    unsigned int    type,
    void           *value
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(number)

    OCI_RETVAL = OCI_STATUS = OCI_TranslateNumericValue(number->con, value, type, number->handle, OCI_NUM_NUMBER);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberGetValue
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberGetValue
(
    OCI_Number     *number,
    unsigned int    type,
    void           *value
)
{
    OCI_CALL_ENTER(boolean, FALSE)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number)
    OCI_CALL_CONTEXT_SET_FROM_OBJ(number)

    OCI_RETVAL = OCI_STATUS = OCI_TranslateNumericValue(number->con, number->handle, OCI_NUM_NUMBER, value, type);

    OCI_CALL_EXIT()
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberAdd
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberAdd
(
    OCI_Number     *number,
    unsigned int    type,
    void           *value
)
{
    OCI_NUMBER_OPERATION(OCINumberAdd)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberSub
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberSub
(
    OCI_Number     *number,
    unsigned int    type,
    void           *value
)
{
    OCI_NUMBER_OPERATION(OCINumberSub)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberMultiply
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberMultiply
(
    OCI_Number     *number,
    unsigned int    type,
    void           *value
)
{
    OCI_NUMBER_OPERATION(OCINumberMul)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberDivide
 * --------------------------------------------------------------------------------------------- */

boolean OCI_API OCI_NumberDivide
(
    OCI_Number     *number,
    unsigned int    type,
    void           *value
)
{
    OCI_NUMBER_OPERATION(OCINumberDiv)
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberCompare
 * --------------------------------------------------------------------------------------------- */

int OCI_API OCI_NumberCompare
(
    OCI_Number *number1,
    OCI_Number *number2
)
{
    sword value = OCI_ERROR;

    OCI_CALL_ENTER(int, value)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number1)
    OCI_CALL_CHECK_PTR(OCI_IPC_NUMBER, number2)
    OCI_CALL_CONTEXT_SET_FROM_CONN(number1->con)

    OCI_EXEC(OCINumberCmp(number1->err, number1->handle, number2->handle, &value))

    OCI_RETVAL = (int) value;

    OCI_CALL_EXIT()
}



