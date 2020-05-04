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

#include "number.h"

#include "array.h"
#include "environment.h"
#include "macros.h"
#include "strings.h"

typedef struct MagicNumber
{
    unsigned char number[3];
    const otext  *name;
} MagicNumber;

static const MagicNumber MagicNumbers[] =
{
    { { 2, 255, 101       }, OTEXT("~")  },
    { { 1, 0,   0         }, OTEXT("-~") }
};

#define MAGIC_NUMBER_COUNT 2

#define NUMBER_OPERATION(func)                                \
                                                              \
    ENTER_FUNC(boolean, FALSE, OCI_IPC_NUMBER, number)        \
                                                              \
    OCINumber src_num = { {0} };                              \
    CHECK_PTR(OCI_IPC_NUMBER, number)                         \
                                                              \
    CHECK(NumberTranslateValue(number->con, value, type,      \
                               &src_num, OCI_NUM_NUMBER))     \
                                                              \
    CHECK_OCI(number->err, func, number->err, number->handle, \
              &src_num, number->handle)                       \
                                                              \
    SET_SUCCESS()                                             \
                                                              \
    EXIT_FUNC()                                               \


/* --------------------------------------------------------------------------------------------- *
* GetNumericTypeSize
* --------------------------------------------------------------------------------------------- */

uword GetNumericTypeSize
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
 * NumberTranslateValue
 * --------------------------------------------------------------------------------------------- */

boolean NumberTranslateValue
(
    OCI_Connection *con,
    void           *in_value,
    uword           in_type,
    void           *out_value,
    uword           out_type
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    /* translate numeric values:
        - signed/unsigned integers (short, int, big_int) to double, float and OCINumber
        - double, float to signed/unsigned integers and OCINumber
        - OCINumber to signed/unsigned integers, double, and float
    */

    const uword in_size = GetNumericTypeSize(in_type);
    const uword out_size = GetNumericTypeSize(out_type);
    const uword in_sign  = (in_type & OCI_NUM_UNSIGNED) ? OCI_NUMBER_UNSIGNED : OCI_NUMBER_SIGNED;
    const uword out_sign = (out_type & OCI_NUM_UNSIGNED) ? OCI_NUMBER_UNSIGNED : OCI_NUMBER_SIGNED;

    OCINumber tmp;
    memset(&tmp, 0, sizeof(tmp));

    OCIError* err = con ? con->err : Env.err;

    CHECK_PTR(OCI_IPC_VOID, in_value)
    CHECK_PTR(OCI_IPC_VOID, out_value)

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

            CHECK_OCI
            (
                err,
                OCINumberToReal,
                err, in_value,
                out_size, out_value
            )
        }
        else
        {
            /* OCINumber to integers */

            CHECK_OCI
            (
                err,
                OCINumberToInt,
                err, in_value, out_size,
                out_sign, out_value
            )
        }
    }
    else if (in_type & OCI_NUM_DOUBLE || in_type & OCI_NUM_FLOAT)
    {
        if (out_type == OCI_NUM_NUMBER)
        {
            /* double / float to OCINumber */

            CHECK_OCI
            (
                err,
                OCINumberFromReal,
                err, in_value, in_size,
                (OCINumber *)out_value
            )
        }
        else if (out_type & OCI_NUM_DOUBLE || out_type & OCI_NUM_FLOAT)
        {
            /* double to float and float to double */

#if OCI_VERSION_COMPILE >= OCI_10_1

            if (Env.version_runtime >= OCI_10_1)
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

            CHECK_OCI
            (
                err,
                OCINumberFromReal,
                err, in_value, in_size,
                (OCINumber *)&tmp
            )

            CHECK_OCI
            (
                err,
                OCINumberToInt,
                err, &tmp, out_size,
                out_sign, out_value
            )
        }
    }
    else
    {
        CHECK_OCI
        (
            err, OCINumberFromInt,
            err, in_value, in_size,
            in_sign, (OCINumber *)&tmp
        )

        if (out_type == OCI_NUM_NUMBER)
        {
            /* integers to OCINumber */

            memcpy(out_value, &tmp, sizeof(tmp));
        }
        else if (out_type & OCI_NUM_DOUBLE || out_type & OCI_NUM_FLOAT)
        {
            /* integers to double / float */

            CHECK_OCI
            (
                err, OCINumberToReal,
                err, &tmp,
                out_size, out_value
            )
        }
        else
        {
            /* only for conversions between integers with different sizes or sign */

            CHECK_OCI
            (
                err,
                OCINumberToInt,
                err, &tmp, out_size,
                out_sign, out_value
            )
        }
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberFromStringInternal
 * --------------------------------------------------------------------------------------------- */

boolean NumberFromStringInternal
(
    OCI_Connection *con,
    void           *out_value,
    uword           type,
    const otext    *in_value,
    const otext   * fmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    boolean done = FALSE;

    OCIError *err = con ? con->err : Env.err;

    dbtext* dbstr1 = NULL;
    dbtext* dbstr2 = NULL;

    int dbsize1 = -1;
    int dbsize2 = -1;

    CHECK_PTR(OCI_IPC_VOID, in_value)
    CHECK_PTR(OCI_IPC_VOID, out_value)

    /* For binary types, perform a C based conversion */

    if (type != OCI_NUM_NUMBER)
    {
        if (type & OCI_NUM_SHORT)
        {
            CHECK(osscanf(in_value, OCI_STRING_FORMAT_NUM_SHORT, (short *)out_value) == 1)
            done = TRUE;
        }
        else if (type & OCI_NUM_INT)
        {
            CHECK(osscanf(in_value, OCI_STRING_FORMAT_NUM_INT, (int *)out_value) == 1)
            done = TRUE;
        }

#if OCI_VERSION_COMPILE >= OCI_10_1

        if (!done && Env.version_runtime >= OCI_10_1)
        {
            const otext *tmp_fmt = fmt;

            if (NULL == tmp_fmt)
            {
                tmp_fmt = EnvironmentGetFormat(con, type & OCI_NUM_DOUBLE ? OCI_FMT_BINARY_DOUBLE : OCI_FMT_BINARY_FLOAT);
                CHECK_NULL(tmp_fmt)
            }

            if (type & OCI_NUM_DOUBLE)
            {
                CHECK(osscanf(in_value, tmp_fmt, (double *)out_value) == 1)
                done = TRUE;
            }
            else if (type & OCI_NUM_FLOAT)
            {
                CHECK(osscanf(in_value, tmp_fmt, (float *)out_value) == 1)
                done = TRUE;
            }
        }

#endif

    }

    /* use OCINumber conversion if not processed yet */

    if (!done)
    {
        for (int i = 0; i < MAGIC_NUMBER_COUNT; i++)
        {
            const MagicNumber *mag_num = &MagicNumbers[i];

            if (ostrcmp(in_value, mag_num->name) == 0)
            {
                memset(out_value, 0, sizeof(OCINumber));
                memcpy(out_value, mag_num->number, mag_num->number[0] + 1);
                done = TRUE;
                break;
            }
        }

        if (!done)
        {
            OCINumber number;

            if (NULL == fmt)
            {
                fmt = EnvironmentGetFormat(con, OCI_FMT_NUMERIC);
                CHECK_NULL(fmt)
            }

            dbstr1 = StringGetDBString(in_value, &dbsize1);
            dbstr2 = StringGetDBString(fmt, &dbsize2);

            memset(&number, 0, sizeof(number));

            CHECK_OCI
            (
                err,
                OCINumberFromText,
                err, (oratext *) dbstr1, (ub4) dbsize1, (oratext *) dbstr2,
                (ub4) dbsize2, (oratext *) NULL,  (ub4) 0, (OCINumber *) &number
            )

            CHECK(NumberTranslateValue(con, &number, OCI_NUM_NUMBER, out_value, type))
        }
    }

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr1);
        StringReleaseDBString(dbstr2);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * NumberToStringInternal
 * --------------------------------------------------------------------------------------------- */

boolean NumberToStringInternal
(
    OCI_Connection *con,
    void           *number,
    unsigned int    type,
    otext          *out_value,
    int             out_value_size,
    const otext   * fmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_CONNECTION, con
    )

    dbtext* dbstr1 = NULL;
    dbtext* dbstr2  = NULL;
    int     dbsize1 = out_value_size * (int)sizeof(otext);
    int     dbsize2 = -1;

    OCIError* err = con ? con->err : Env.err;

    boolean done = FALSE;

    CHECK_PTR(OCI_IPC_VOID, out_value)
    CHECK_PTR(OCI_IPC_VOID, number)

    out_value[0] = 0;

    /* For binary types, perform a C based conversion */

    if (type != OCI_NUM_NUMBER)
    {
        if (type & OCI_NUM_SHORT)
        {
            out_value_size = osprintf(out_value, out_value_size,
                                      OCI_STRING_FORMAT_NUM_SHORT,
                                      *((short *)number));

            done = TRUE;
        }
        else if (type & OCI_NUM_INT)
        {
            out_value_size = osprintf(out_value, out_value_size,
                                      OCI_STRING_FORMAT_NUM_INT,
                                      *((int *)number));

            done = TRUE;
        }

#if OCI_VERSION_COMPILE >= OCI_10_1

        if (!done && (Env.version_runtime >= OCI_10_1))
        {
            const otext *tmp_fmt = fmt;

            if (NULL == tmp_fmt)
            {
                tmp_fmt = EnvironmentGetFormat(con, type & OCI_NUM_DOUBLE ? OCI_FMT_BINARY_DOUBLE : OCI_FMT_BINARY_FLOAT);
                CHECK_NULL(tmp_fmt)
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
        for (int i = 0; i < MAGIC_NUMBER_COUNT; i++)
        {
            const MagicNumber *mag_num = &MagicNumbers[i];

            if (memcmp(number, mag_num->number, mag_num->number[0] + 1) == 0)
            {
                ostrcpy(out_value, mag_num->name);
                done = TRUE;
                break;
            }
        }

        if (!done)
        {
            if (NULL == fmt)
            {
                fmt = EnvironmentGetFormat(con, OCI_FMT_NUMERIC);
                CHECK_NULL(fmt)
            }

            dbstr1 = StringGetDBString(out_value, &dbsize1);
            dbstr2 = StringGetDBString(fmt, &dbsize2);

            CHECK_OCI
            (
                err,
                OCINumberToText,
                err, (OCINumber *)number, (oratext *)dbstr2,
                (ub4)dbsize2, (oratext *)NULL, (ub4)0,
                (ub4 *)&dbsize1, (oratext *)dbstr1
            )

            StringCopyDBStringToNativeString(dbstr1, out_value, dbcharcount(dbsize1));

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

    SET_SUCCESS()

    CLEANUP_AND_EXIT_FUNC
    (
        StringReleaseDBString(dbstr2);
        StringReleaseDBString(dbstr1);
    )
}

/* --------------------------------------------------------------------------------------------- *
 * NumberInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Number * NumberInitialize
(
    OCI_Connection *con,
    OCI_Number     *number,
    OCINumber      *buffer
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Number*, number,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )

    ALLOC_DATA(OCI_IPC_NUMBER, number, 1);

    number->con    = con;
    number->handle = buffer;

    /* get the right error handle */

    number->err = con ? con->err : Env.err;
    number->env = con ? con->env : Env.env;

    /* allocate buffer if needed */

    if (NULL == number->handle || (OCI_OBJECT_ALLOCATED_ARRAY == number->hstate))
    {
        if (OCI_OBJECT_ALLOCATED_ARRAY != number->hstate)
        {
            number->hstate = OCI_OBJECT_ALLOCATED;
            ALLOC_DATA(OCI_IPC_ARRAY, number->handle, 1)
        }
    }
    else
    {
        number->hstate = OCI_OBJECT_FETCHED_CLEAN;
    }

    /* check for failure */

    CLEANUP_AND_EXIT_FUNC
    (
        if (FAILURE)
        {
            NumberFree(number);
            number = NULL;
        }

        SET_RETVAL(number)
    )
}

/* --------------------------------------------------------------------------------------------- *
 * NumberCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Number * NumberCreate
(
    OCI_Connection *con
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Number*, NULL,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )

    CHECK_INITIALIZED()

    SET_RETVAL(NumberInitialize(con, NULL, NULL))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberAssign
 * --------------------------------------------------------------------------------------------- */

boolean NumberFree
(
    OCI_Number *number
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    )

    CHECK_PTR(OCI_IPC_NUMBER, number)
    CHECK_OBJECT_FETCHED(number);

    if (OCI_OBJECT_ALLOCATED == number->hstate)
    {
        FREE(number->handle)
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != number->hstate)
    {
        FREE(number)
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Number ** NumberCreateArray
(
    OCI_Connection *con,
    unsigned int    nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Number**, NULL,
        /* context */ (con ? OCI_IPC_CONNECTION : OCI_IPC_VOID), (con ? (void*)con : (void*)&Env)
    )

    OCI_Array *arr = NULL;

    CHECK_INITIALIZED()

    arr = ArrayCreate(con, nbelem, OCI_CDT_NUMERIC, OCI_NUM_NUMBER,
                      sizeof(OCINumber), sizeof(OCI_Number), 0, NULL);

    CHECK_NULL(arr)

    SET_RETVAL((OCI_Number**)arr->tab_obj)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberFreeArray
 * --------------------------------------------------------------------------------------------- */

boolean NumberFreeArray
(
    OCI_Number **nummers
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_VOID, &Env
    )

    CHECK_PTR(OCI_IPC_ARRAY, nummers)

    SET_RETVAL(ArrayFreeFromHandles((void**)nummers))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberAssign
 * --------------------------------------------------------------------------------------------- */

boolean NumberAssign
(
    OCI_Number *number,
    OCI_Number *number_src
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    )

    CHECK_PTR(OCI_IPC_NUMBER, number)
    CHECK_PTR(OCI_IPC_NUMBER, number_src)

    CHECK_OCI
    (
        number->err,
        OCINumberAssign,
        number->err, number_src->handle,
        number->handle
    )

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberToString
 * --------------------------------------------------------------------------------------------- */

boolean NumberToString
(
    OCI_Number  *number,
    const otext *fmt,
    int          size,
    otext       *str
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    )

    CHECK_PTR(OCI_IPC_NUMBER, number)

    SET_RETVAL(NumberToStringInternal(number->con, number->handle,
                                      OCI_NUM_NUMBER, str, size, fmt))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberFromString
 * --------------------------------------------------------------------------------------------- */

boolean NumberFromString
(
    OCI_Number  *number,
    const otext *str,
    const otext *fmt
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    )

    CHECK_PTR(OCI_IPC_NUMBER, number)

    SET_RETVAL(NumberFromStringInternal(number->con, number->handle,
                                        OCI_NUM_NUMBER, str, fmt))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberGetContent
 * --------------------------------------------------------------------------------------------- */

unsigned char * NumberGetContent
(
    OCI_Number *number
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned char*, NULL,
        /* context */ OCI_IPC_NUMBER, number
    )

    CHECK_PTR(OCI_IPC_NUMBER, number)

    CHECK_NULL(number->handle)

    SET_RETVAL(number->handle->OCINumberPart)

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberSetContent
 * --------------------------------------------------------------------------------------------- */

boolean NumberSetContent
(
    OCI_Number    *number,
    unsigned char *content
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    )

    CHECK_PTR(OCI_IPC_NUMBER, number)
    CHECK_PTR(OCI_IPC_VOID,   content)

    if (NULL != number->handle)
    {
        memcpy(number->handle->OCINumberPart, content, sizeof(number->handle->OCINumberPart));
    }

    SET_SUCCESS()

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberSetValue
 * --------------------------------------------------------------------------------------------- */

boolean NumberSetValue
(
    OCI_Number  *number,
    unsigned int type,
    void        *value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    )

    CHECK_PTR(OCI_IPC_NUMBER, number)

    SET_RETVAL(NumberTranslateValue(number->con, value, type,
                                    number->handle, OCI_NUM_NUMBER))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberGetValue
 * --------------------------------------------------------------------------------------------- */

boolean NumberGetValue
(
    OCI_Number  *number,
    unsigned int type,
    void        *value
)
{
    ENTER_FUNC
    (
        /* returns */ boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    )

    CHECK_PTR(OCI_IPC_NUMBER, number)

    SET_RETVAL(NumberTranslateValue(number->con, number->handle,
                                    OCI_NUM_NUMBER, value, type))

    EXIT_FUNC()
}

/* --------------------------------------------------------------------------------------------- *
 * NumberAdd
 * --------------------------------------------------------------------------------------------- */

boolean NumberAdd
(
    OCI_Number  *number,
    unsigned int type,
    void        *value
)
{
    NUMBER_OPERATION(OCINumberAdd)
}

/* --------------------------------------------------------------------------------------------- *
 * NumberSub
 * --------------------------------------------------------------------------------------------- */

boolean NumberSub
(
    OCI_Number  *number,
    unsigned int type,
    void        *value
)
{
    NUMBER_OPERATION(OCINumberSub)
}

/* --------------------------------------------------------------------------------------------- *
 * NumberMultiply
 * --------------------------------------------------------------------------------------------- */

boolean NumberMultiply
(
    OCI_Number  *number,
    unsigned int type,
    void        *value
)
{
    NUMBER_OPERATION(OCINumberMul)
}

/* --------------------------------------------------------------------------------------------- *
 * NumberDivide
 * --------------------------------------------------------------------------------------------- */

boolean NumberDivide
(
    OCI_Number  *number,
    unsigned int type,
    void        *value
)
{
    NUMBER_OPERATION(OCINumberDiv)
}

/* --------------------------------------------------------------------------------------------- *
 * NumberCompare
 * --------------------------------------------------------------------------------------------- */

int NumberCompare
(
    OCI_Number *number1,
    OCI_Number *number2
)
{
    ENTER_FUNC
    (
        /* returns */ int, OCI_ERROR,
        /* context */ OCI_IPC_NUMBER, number1
    )

    sword value = OCI_ERROR;

    CHECK_PTR(OCI_IPC_NUMBER, number1)
    CHECK_PTR(OCI_IPC_NUMBER, number2)

    CHECK_OCI
    (
        number1->err,
        OCINumberCmp,
        number1->err, number1->handle,
        number2->handle, &value
    )

    SET_RETVAL((int) value)

    EXIT_FUNC()
}
