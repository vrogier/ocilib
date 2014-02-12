/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2014 Vincent ROGIER <vince.rogier@ocilib.net>            |
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
 * $Id: number.c, Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

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

    OCI_CHECK(con       == NULL, FALSE);
    OCI_CHECK(number    == NULL, FALSE);
    OCI_CHECK(out_value == NULL, FALSE);

#if OCI_VERSION_COMPILE < OCI_10_1

    OCI_NOT_USED(sqlcode);

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

    OCI_CHECK(con       == NULL, FALSE);
    OCI_CHECK(number    == NULL, FALSE);
    OCI_CHECK(in_value  == NULL, FALSE);

#if OCI_VERSION_COMPILE < OCI_10_1

    OCI_NOT_USED(sqlcode);

#endif

    if (type & OCI_NUM_DOUBLE || type & OCI_NUM_FLOAT)
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

    /* For binary types, perfom a C based conversion */

    if (type & OCI_NUM_DOUBLE || type & OCI_NUM_FLOAT)
    {

    #if OCI_VERSION_COMPILE >= OCI_10_1

        if (OCILib.version_runtime >= OCI_10_1)
        {   
            if (type & OCI_NUM_DOUBLE)
            {
                res = (osscanf(in_value,  OCI_STRING_FORMAT_NUM_BIN, out_value) == 1);
            }
            else if (type & OCI_NUM_FLOAT)
            {
                double tmp_value = 0.0;
                
                res = (osscanf(in_value, OCI_STRING_FORMAT_NUM_BIN, &tmp_value) == 1);

                *((float *) out_value) = (float) tmp_value;
            }

            done = TRUE;
        }

    #endif
    
    }

    /* use OCINumber conversion if not processed yet */

    if (!done)
    {   
        dbtext *dbstr1  = NULL;
        dbtext *dbstr2  = NULL;
        int     dbsize1 = -1;
        int     dbsize2 = -1;
        OCINumber number;
        
        if (!fmt)
        {
            fmt = OCI_GetDefaultFormatNumeric(con);
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

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_NumberToString
 * --------------------------------------------------------------------------------------------- */

boolean OCI_NumberToString
(
    OCI_Connection *con,
    void           *number,
    uword           type,
    int             sqlcode,
    otext          *out_value,
    int             out_value_size,
    const otext   * fmt
)
{
    boolean res  = TRUE;
    boolean done = FALSE;

    out_value[0] = 0;

    /* For binary types, perfom a C based conversion */

    if (type & OCI_NUM_DOUBLE || type & OCI_NUM_FLOAT)
    {

    #if OCI_VERSION_COMPILE >= OCI_10_1

        if ((OCILib.version_runtime >= OCI_10_1) && ((SQLT_VNU != sqlcode)))
        {
            if (!fmt)
            {
                fmt = OCI_STRING_FORMAT_NUM_BIN;
            }

            if (type & OCI_NUM_DOUBLE && (SQLT_BDOUBLE == sqlcode))
            {
                out_value_size = osprintf(out_value, out_value_size, OCI_STRING_FORMAT_NUM_BIN,  *((double *) number));
            }
            else if (type & OCI_NUM_FLOAT && (SQLT_BFLOAT == sqlcode))
            {
                 out_value_size = osprintf(out_value, out_value_size, OCI_STRING_FORMAT_NUM_BIN,  *((float *) number));
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
    
        OCI_NOT_USED(sqlcode);

    #endif
    
    }

    /* use OCINumber conversion if not processed yet */

    if (!done)
    {   
        dbtext *dbstr1  = NULL;
        dbtext *dbstr2  = NULL;
        int     dbsize1 = out_value_size * (int) sizeof(otext);
        int     dbsize2 = -1;
        
        if (!fmt)
        {
            fmt = OCI_GetDefaultFormatNumeric(con);
        }

        dbstr1 = OCI_StringGetOracleString(out_value, &dbsize1);
        dbstr2 = OCI_StringGetOracleString(fmt, &dbsize2);

        OCI_CALL2
        (
            res, con,

            OCINumberToText(con->err, (OCINumber *) number,  (oratext *) dbstr2,
                            (ub4) dbsize2, (oratext *) NULL,  (ub4) 0,
                            (ub4 *) &dbsize1, (oratext *) dbstr1)
        )

        OCI_StringCopyOracleStringToNativeString(dbstr1, out_value, dbcharcount(dbsize1));
        OCI_StringReleaseOracleString(dbstr2);
        OCI_StringReleaseOracleString(dbstr1);

        out_value_size = (dbsize1 / (int) sizeof(otext));
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
