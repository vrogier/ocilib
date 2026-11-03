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

#include "number.h"

#include "array.h"
#include "environment.h"
#include "macros.h"
#include "stringutils.h"

typedef struct MagicNumber
{
    unsigned char number[3];
    const otext  *name;
} MagicNumber;

static const MagicNumber MagicNumbers[] =
{
    { { 2, 255, 101    }, OTEXT("~")  },
    { { 1, 0,   0      }, OTEXT("-~") }
};

#define MAGIC_NUMBER_COUNT 2

#define NUMBER_OPERATION(func)                                              \
                                                                            \
        ENTER_FUNC(OCI_Boolean, FALSE, OCI_IPC_NUMBER, number);             \
                                                                            \
        OCINumber src_num = { {0} };                                        \
        CHECK_PTR(OCI_IPC_NUMBER, number);                                  \
                                                                            \
        CHECK(OcilibNumberTranslateValue(number->env, value, type,          \
                                         &src_num, OCI_NUM_NUMBER));        \
                                                                            \
        CHECK_OCI(number->env->err, func, number->env->err, number->handle, \
                  &src_num, number->handle);                                \
                                                                            \
        SET_SUCCESS();                                                      \
                                                                            \
        EXIT_FUNC();                                                        \


/* --------------------------------------------------------------------------------------------- *
* OcilibGetNumericTypeSize
* --------------------------------------------------------------------------------------------- */

static uword OcilibGetNumericTypeSize
(
    OCI_NumericType type
)
{
    uword size = 0;

    if (type & OCI_NUM_SHORT)
    {
        size = sizeof(OCI_Int16);
    }
    else if (type & OCI_NUM_INT)
    {
        size = sizeof(OCI_Int32);
    }
    else if (type & OCI_NUM_BIGINT)
    {
        size = sizeof(OCI_Int64);
    }
    else if (type & OCI_NUM_FLOAT)
    {
        size = sizeof(OCI_Float);
    }
    else if (type & OCI_NUM_DOUBLE)
    {
        size = sizeof(OCI_Double);
    }
    else if (type & OCI_NUM_NUMBER)
    {
        size = sizeof(OCINumber);
    }

    return size;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberTranslateValue
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberTranslateValue
(
    OCI_Environment *env,
    void            *in_value,
    uword            in_type,
    void            *out_value,
    uword            out_type
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    )

    /* translate numeric values:
        - signed/OCI_UInt32egers (OCI_Int16, OCI_Int32, OCI_Int64) to OCI_Double, OCI_Float and OCINumber
        - OCI_Double, OCI_Float to signed/OCI_UInt32egers and OCINumber
        - OCINumber to signed/OCI_UInt32egers, OCI_Double, and OCI_Float
    */

    const uword in_size = OcilibGetNumericTypeSize(in_type);
    const uword out_size = OcilibGetNumericTypeSize(out_type);
    const uword in_sign  = (in_type & OCI_NUM_UNSIGNED) ? OCI_NUMBER_UNSIGNED : OCI_NUMBER_SIGNED;
    const uword out_sign = (out_type & OCI_NUM_UNSIGNED) ? OCI_NUMBER_UNSIGNED : OCI_NUMBER_SIGNED;

    OCINumber tmp;
    memset(&tmp, 0, sizeof(tmp));

    OCIError* err = env ? env->err : NULL;

    CHECK_PTR(OCI_IPC_VOID, in_value);
    CHECK_PTR(OCI_IPC_VOID, out_value);

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
            /* OCINumber to OCI_Double / OCI_Float */

            CHECK_OCI
            (
                err,
                OCINumberToReal,
                err, in_value,
                out_size, out_value
            );
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
            );
        }
    }
    else if (in_type & OCI_NUM_DOUBLE || in_type & OCI_NUM_FLOAT)
    {
        if (out_type == OCI_NUM_NUMBER)
        {
            /* OCI_Double / OCI_Float to OCINumber */

            CHECK_OCI
            (
                err,
                OCINumberFromReal,
                err, in_value, in_size,
                (OCINumber *)out_value
            );
        }
        else if (out_type & OCI_NUM_DOUBLE || out_type & OCI_NUM_FLOAT)
        {
            /* OCI_Double to OCI_Float and OCI_Float to OCI_Double */

#if OCI_VERSION_COMPILE >= OCI_10_1

            if (env && env->version_runtime >= OCI_10_1)
            {
                if (in_type & OCI_NUM_FLOAT && (out_type & OCI_NUM_DOUBLE))
                {
                    *((OCI_Double *)out_value) = (OCI_Double) *((OCI_Float *)in_value);
                }
                else if (in_type & OCI_NUM_DOUBLE && (out_type & OCI_NUM_FLOAT))
                {
                    *((OCI_Float *)out_value) = (OCI_Float) *((OCI_Double *)in_value);
                }
            }

#endif

        }
        else
        {
            /* OCI_Double / OCI_Float to integers */

            CHECK_OCI
            (
                err,
                OCINumberFromReal,
                err, in_value, in_size,
                (OCINumber *)&tmp
            );

            CHECK_OCI
            (
                err,
                OCINumberToInt,
                err, &tmp, out_size,
                out_sign, out_value
            );
        }
    }
    else
    {
        CHECK_OCI
        (
            err, OCINumberFromInt,
            err, in_value, in_size,
            in_sign, (OCINumber *)&tmp
        );

        if (out_type == OCI_NUM_NUMBER)
        {
            /* integers to OCINumber */

            memcpy(out_value, &tmp, sizeof(tmp));
        }
        else if (out_type & OCI_NUM_DOUBLE || out_type & OCI_NUM_FLOAT)
        {
            /* integers to OCI_Double / OCI_Float */

            CHECK_OCI
            (
                err, OCINumberToReal,
                err, &tmp,
                out_size, out_value
            );
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
            );
        }
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberFromStringInternal
 * --------------------------------------------------------------------------------------------- */

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

OCI_Boolean OcilibNumberFromStringInternal
(
    OCI_Environment *env,
    void            *out_value,
    uword            type,
    const otext     *in_value,
    const otext    * fmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_Boolean done = FALSE;

    OCIError *err = env ? env->err : NULL;

    dbtext* dbstr1 = NULL;
    dbtext* dbstr2 = NULL;

    OCI_Int32 dbsize1 = -1;
    OCI_Int32 dbsize2 = -1;

    CHECK_PTR(OCI_IPC_VOID, in_value);
    CHECK_PTR(OCI_IPC_VOID, out_value);

    /* For binary types, perform a C based conversion */

    if (type != OCI_NUM_NUMBER)
    {
        if (type & OCI_NUM_SHORT)
        {
            CHECK(osscanf(in_value, OCI_STRING_FORMAT_NUM_SHORT, (OCI_Int16 *)out_value) == 1);
            done = TRUE;
        }
        else if (type & OCI_NUM_INT)
        {
            CHECK(osscanf(in_value, OCI_STRING_FORMAT_NUM_INT, (OCI_Int32 *)out_value) == 1);
            done = TRUE;
        }

#if OCI_VERSION_COMPILE >= OCI_10_1

        if (!done && env && env->version_runtime >= OCI_10_1)
        {
            const otext *tmp_fmt = fmt;

            if (NULL == tmp_fmt)
            {
                tmp_fmt = OcilibEnvironmentGetFormat(env, type & OCI_NUM_DOUBLE ? OCI_FMT_BINARY_DOUBLE : OCI_FMT_BINARY_FLOAT);
                CHECK_NULL(tmp_fmt);
            }

            if (type & OCI_NUM_DOUBLE)
            {
                CHECK(osscanf(in_value, tmp_fmt, (OCI_Double *)out_value) == 1);
                done = TRUE;
            }
            else if (type & OCI_NUM_FLOAT)
            {
                CHECK(osscanf(in_value, tmp_fmt, (OCI_Float *)out_value) == 1);
                done = TRUE;
            }
        }

#endif

    }

    /* use OCINumber conversion if not processed yet */

    if (!done)
    {
        for (OCI_Int32 i = 0; i < MAGIC_NUMBER_COUNT; i++)
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
                fmt = OcilibEnvironmentGetFormat(env, OCI_FMT_NUMERIC);
                CHECK_NULL(fmt);
            }

            dbstr1 = OcilibStringGetDBString(env, in_value, &dbsize1);
            dbstr2 = OcilibStringGetDBString(env, fmt, &dbsize2);

            memset(&number, 0, sizeof(number));

            CHECK_OCI
            (
                err,
                OCINumberFromText,
                err, (oratext *) dbstr1, (ub4) dbsize1, (oratext *) dbstr2,
                (ub4) dbsize2, (oratext *) NULL,  (ub4) 0, (OCINumber *) &number
            );

            CHECK(OcilibNumberTranslateValue(env, &number, OCI_NUM_NUMBER, out_value, type));
        }
    }

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(env, dbstr1);
        OcilibStringReleaseDBString(env, dbstr2);
    );
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberToStringInternal
 * --------------------------------------------------------------------------------------------- */

#if defined(__GNUC__)
#pragma GCC diagnostic push
#pragma GCC diagnostic ignored "-Wformat-nonliteral"
#endif

OCI_Boolean OcilibNumberToStringInternal
(
    OCI_Environment *env,
    void            *number,
    OCI_NumericType  type,
    otext           *out_value,
    OCI_Int32        out_value_size,
    const otext    * fmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    dbtext  * dbstr1  = NULL;
    dbtext  * dbstr2  = NULL;
    OCI_Int32 dbsize1 = out_value_size * (OCI_Int32)sizeof(otext);
    OCI_Int32 dbsize2 = -1;

    OCIError* err = env ? env->err : NULL;

    OCI_Boolean done = FALSE;

    CHECK_PTR(OCI_IPC_VOID, out_value);
    CHECK_PTR(OCI_IPC_VOID, number);

    out_value[0] = 0;

    /* For binary types, perform a C based conversion */

    if (type != OCI_NUM_NUMBER)
    {
        if (type & OCI_NUM_SHORT)
        {
            out_value_size = OcilibStringFormat(out_value, out_value_size,
                                                OCI_STRING_FORMAT_NUM_SHORT,
                                                *((OCI_Int16 *)number));

            done = TRUE;
        }
        else if (type & OCI_NUM_INT)
        {
            out_value_size = OcilibStringFormat(out_value, out_value_size,
                                                OCI_STRING_FORMAT_NUM_INT,
                                                *((OCI_Int32 *)number));

            done = TRUE;
        }

#if OCI_VERSION_COMPILE >= OCI_10_1

        if (!done && env && (env->version_runtime >= OCI_10_1))
        {
            const otext *tmp_fmt = fmt;

            if (NULL == tmp_fmt)
            {
                tmp_fmt = OcilibEnvironmentGetFormat(env, type & OCI_NUM_DOUBLE ? OCI_FMT_BINARY_DOUBLE : OCI_FMT_BINARY_FLOAT);
                CHECK_NULL(tmp_fmt);
            }

            if (type & OCI_NUM_DOUBLE)
            {
                out_value_size = OcilibStringFormat(out_value, out_value_size, tmp_fmt, *((OCI_Double *)number));

                done = TRUE;
            }
            else if (type & OCI_NUM_FLOAT)
            {
                out_value_size = OcilibStringFormat(out_value, out_value_size, tmp_fmt, *((OCI_Float *)number));

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
        for (OCI_Int32 i = 0; i < MAGIC_NUMBER_COUNT; i++)
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
                fmt = OcilibEnvironmentGetFormat(env, OCI_FMT_NUMERIC);
                CHECK_NULL(fmt);
            }

            dbstr1 = OcilibStringGetDBString(env, out_value, &dbsize1);
            dbstr2 = OcilibStringGetDBString(env, fmt, &dbsize2);

            CHECK_OCI
            (
                err,
                OCINumberToText,
                err, (OCINumber *)number, (oratext *)dbstr2,
                (ub4)dbsize2, (oratext *)NULL, (ub4)0,
                (ub4 *)&dbsize1, (oratext *)dbstr1
            );

            OcilibStringCopyDBStringToNativeString(env, dbstr1, out_value, dbcharcount(dbsize1));

            out_value_size = (dbsize1 / (OCI_Int32) sizeof(dbtext));
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

    SET_SUCCESS();

    CLEANUP_AND_EXIT_FUNC
    (
        OcilibStringReleaseDBString(env, dbstr2);
        OcilibStringReleaseDBString(env, dbstr1);
    );
}

#if defined(__GNUC__)
#pragma GCC diagnostic pop
#endif

/* --------------------------------------------------------------------------------------------- *
 *Ocilib NumberInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OcilibNumberInitialize
(
    OCI_Environment *env,
    OCI_Number      *number,
    OCINumber       *buffer
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Number*, number,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    ALLOC_HANDLE(OCI_IPC_NUMBER, number);
    number->handle = buffer;

    /* allocate buffer if needed */

    if (NULL == number->handle || (OCI_OBJECT_ALLOCATED_ARRAY == number->hstate))
    {
        if (OCI_OBJECT_ALLOCATED_ARRAY != number->hstate)
        {
            number->hstate = OCI_OBJECT_ALLOCATED;
            ALLOC_DATA(OCI_IPC_ARRAY, number->handle, 1);
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
            OcilibNumberFree(number);
            number = NULL;
        }

        SET_RETVAL(number);
    );
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberCreate
 * --------------------------------------------------------------------------------------------- */

OCI_Number * OcilibNumberCreate
(
    OCI_Environment *env
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Number*, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);

    SET_RETVAL(OcilibNumberInitialize(env, NULL, NULL));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberFree
(
    OCI_Number *number
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    );

    CHECK_PTR(OCI_IPC_NUMBER, number);
    CHECK_OBJECT_FETCHED(number);

    if (OCI_OBJECT_ALLOCATED == number->hstate)
    {
        FREE(number->handle);
    }

    if (OCI_OBJECT_ALLOCATED_ARRAY != number->hstate)
    {
        OcilibErrorResetSource(NULL, number);

        FREE(number);
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberCreateArray
 * --------------------------------------------------------------------------------------------- */

OCI_Number ** OcilibNumberCreateArray
(
    OCI_Environment *env,
    OCI_UInt32       nbelem
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Number**, NULL,
        /* context */ OCI_IPC_ENVIRONMENT, env
    );

    OCI_Array *arr = NULL;

    CHECK_PTR(OCI_IPC_ENVIRONMENT, env);

    arr = OcilibArrayCreate(env, NULL, nbelem, OCI_CDT_NUMERIC, OCI_NUM_NUMBER,
                            sizeof(OCINumber), sizeof(OCI_Number), 0, NULL);

    CHECK_NULL(arr);

    SET_RETVAL((OCI_Number**)arr->tab_obj);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberFreeArray
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberFreeArray
(
    OCI_Number **nummers
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_VOID, NULL
    );

    CHECK_PTR(OCI_IPC_ARRAY, nummers);

    SET_RETVAL(OcilibArrayFreeFromHandles(((OCI_Number *) nummers[0])->env, (void**)nummers));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberAssign
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberAssign
(
    OCI_Number *number,
    OCI_Number *number_src
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    );

    CHECK_PTR(OCI_IPC_NUMBER, number);
    CHECK_PTR(OCI_IPC_NUMBER, number_src);

    CHECK_OCI
    (
        number->env->err,
        OCINumberAssign,
        number->env->err, number_src->handle,
        number->handle
    );

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberToString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberToString
(
    OCI_Number  *number,
    const otext *fmt,
    OCI_Int32    size,
    otext       *str
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    );

    CHECK_PTR(OCI_IPC_NUMBER, number);

    SET_RETVAL(OcilibNumberToStringInternal(number->env, number->handle,
                                            OCI_NUM_NUMBER, str, size, fmt));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberFromString
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberFromString
(
    OCI_Number  *number,
    const otext *str,
    const otext *fmt
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    );

    CHECK_PTR(OCI_IPC_NUMBER, number);

    SET_RETVAL(OcilibNumberFromStringInternal(number->env, number->handle,
                                              OCI_NUM_NUMBER, str, fmt));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberGetContent
 * --------------------------------------------------------------------------------------------- */

unsigned char * OcilibNumberGetContent
(
    OCI_Number *number
)
{
    ENTER_FUNC
    (
        /* returns */ unsigned char*, NULL,
        /* context */ OCI_IPC_NUMBER, number
    );

    CHECK_PTR(OCI_IPC_NUMBER, number);

    CHECK_NULL(number->handle);

    SET_RETVAL(number->handle->OCINumberPart);

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberSetContent
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberSetContent
(
    OCI_Number    *number,
    unsigned char *content
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    );

    CHECK_PTR(OCI_IPC_NUMBER, number);
    CHECK_PTR(OCI_IPC_VOID,   content);

    if (NULL != number->handle)
    {
        memcpy(number->handle->OCINumberPart, content, sizeof(number->handle->OCINumberPart));
    }

    SET_SUCCESS();

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberSetValue
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberSetValue
(
    OCI_Number     *number,
    OCI_NumericType type,
    void           *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    );

    CHECK_PTR(OCI_IPC_NUMBER, number);

    SET_RETVAL(OcilibNumberTranslateValue(number->env, value, type,
                                          number->handle, OCI_NUM_NUMBER));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberGetValue
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberGetValue
(
    OCI_Number     *number,
    OCI_NumericType type,
    void           *value
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Boolean, FALSE,
        /* context */ OCI_IPC_NUMBER, number
    );

    CHECK_PTR(OCI_IPC_NUMBER, number);

    SET_RETVAL(OcilibNumberTranslateValue(number->env, number->handle,
                                          OCI_NUM_NUMBER, value, type));

    EXIT_FUNC();
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberAdd
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberAdd
(
    OCI_Number     *number,
    OCI_NumericType type,
    void           *value
)
{
    NUMBER_OPERATION(OCINumberAdd);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberSub
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberSub
(
    OCI_Number     *number,
    OCI_NumericType type,
    void           *value
)
{
    NUMBER_OPERATION(OCINumberSub);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberMultiply
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberMultiply
(
    OCI_Number     *number,
    OCI_NumericType type,
    void           *value
)
{
    NUMBER_OPERATION(OCINumberMul);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberDivide
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibNumberDivide
(
    OCI_Number     *number,
    OCI_NumericType type,
    void           *value
)
{
    NUMBER_OPERATION(OCINumberDiv);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibNumberCompare
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibNumberCompare
(
    OCI_Number *number1,
    OCI_Number *number2
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, OCI_ERROR,
        /* context */ OCI_IPC_NUMBER, number1
    );

    sword value = OCI_ERROR;

    CHECK_PTR(OCI_IPC_NUMBER, number1);
    CHECK_PTR(OCI_IPC_NUMBER, number2);

    CHECK_OCI
    (
        number1->env->err,
        OCINumberCmp,
        number1->env->err, number1->handle,
        number2->handle, &value
    );

    SET_RETVAL((OCI_Int32) value);

    EXIT_FUNC();
}
