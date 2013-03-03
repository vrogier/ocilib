/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2013 Vincent ROGIER <vince.rogier@ocilib.net>            |
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

    if (type == OCI_NUM_NUMBER)
    {
        memcpy(out_value, number, size);
    }
    else if (type & OCI_NUM_DOUBLE || type & OCI_NUM_FLOAT)
    {

    #if OCI_VERSION_COMPILE >= OCI_10_1

        if ((OCILib.version_runtime >= OCI_10_1) && ((sqlcode != SQLT_VNU)))
        {
            if (((type & OCI_NUM_DOUBLE) && (sqlcode == SQLT_BDOUBLE)) ||
                ((type & OCI_NUM_FLOAT ) && (sqlcode == SQLT_BFLOAT )))
            {
                memcpy(out_value, number, size);
            }
            else if (type & OCI_NUM_DOUBLE && sqlcode == SQLT_BFLOAT)
            {
                *((double *) out_value) = (double) *((float *) number);
            }
            else if (type & OCI_NUM_FLOAT && sqlcode == SQLT_BDOUBLE)
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
        uword sign = OCI_NUMBER_SIGNED;

        if (type & OCI_NUM_UNSIGNED)
        {
            sign = OCI_NUMBER_UNSIGNED;
        }

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
            if (((type & OCI_NUM_DOUBLE) && (sqlcode == SQLT_BDOUBLE)) ||
                ((type & OCI_NUM_FLOAT ) && (sqlcode == SQLT_BFLOAT )))
            {
                memcpy(number, in_value, size);
            }
            else if (type & OCI_NUM_DOUBLE && sqlcode == SQLT_BFLOAT)
            {
                *((double *) number) = (double) *((float *) in_value);
            }
            else if (type & OCI_NUM_FLOAT && sqlcode == SQLT_BDOUBLE)
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
        uword sign = OCI_NUMBER_SIGNED;

        if (type & OCI_NUM_UNSIGNED)
        {
            sign = OCI_NUMBER_UNSIGNED;
        }

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
    const dtext    *in_value,
    const mtext   * fmt
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
            void *ostr1  = NULL;
            int   osize1 = -1;

            fmt = OCI_STRING_FORMAT_NUM_BIN;            
 
            ostr1 = OCI_GetInputString((void *) fmt, &osize1, sizeof(mtext), sizeof(dtext));

            if (type & OCI_NUM_DOUBLE)
            {
                res = (dtscanf(in_value, (dtext *) ostr1, out_value) == 1);
            }
            else if (type & OCI_NUM_FLOAT)
            {
                double tmp_value = 0.0;
                
                res = (dtscanf(in_value, (dtext *) ostr1, &tmp_value) == 1);

                *((float *) out_value) = (float) tmp_value;
            }

            done = TRUE;

            if (fmt != ostr1)
            {
                OCI_ReleaseMetaString(ostr1);
            }
        }

    #endif
    
    }

    /* use OCINumber conversion if not processed yet */

    if (done == FALSE)
    {   
        void *ostr1 = NULL;
        void *ostr2 = NULL;
        int osize1  = -1;
        int osize2  = -1;
        OCINumber number;
        
        if (fmt == NULL)
        {
            fmt = OCI_GetDefaultFormatNumeric(con);
        }

        ostr1 = OCI_GetInputString((void *) in_value, &osize1, sizeof(dtext), sizeof(omtext));
        ostr2 = OCI_GetInputMetaString(fmt, &osize2);

        memset(&number, 0, sizeof(number));

        OCI_CALL2
        (
            res, con,

            OCINumberFromText(con->err, (oratext *) ostr1, (ub4) osize1, (oratext *) ostr2,
                                (ub4) osize2, (oratext *) NULL,  (ub4) 0, (OCINumber *) &number)
        )
 
        OCI_ReleaseMetaString(ostr2);

        if (in_value != ostr1)
        {
            OCI_ReleaseMetaString(ostr1);
        }

        res = res && OCI_NumberGet(con, &number, size, type, sqlcode, out_value);
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
    dtext          *out_value,
    int             out_value_size,
    const mtext   * fmt
)
{
    boolean res  = TRUE;
    boolean done = FALSE;

    out_value[0] = 0;

    /* For binary types, perfom a C based conversion */

    if (type & OCI_NUM_DOUBLE || type & OCI_NUM_FLOAT)
    {

    #if OCI_VERSION_COMPILE >= OCI_10_1

        if ((OCILib.version_runtime >= OCI_10_1) && ((sqlcode != SQLT_VNU)))
        {
            void *ostr1  = NULL;
            int   osize1 = -1;

            if (fmt == NULL)
            {
                fmt = OCI_STRING_FORMAT_NUM_BIN;
            }
 
            ostr1 = OCI_GetInputString((void *) fmt, &osize1, sizeof(mtext), sizeof(dtext));

            if (type & OCI_NUM_DOUBLE && sqlcode == SQLT_BDOUBLE)
            {
                out_value_size = dtsprintf(out_value, out_value_size, (dtext *) ostr1,  *((double *) number));
            }
            else if (type & OCI_NUM_FLOAT && sqlcode == SQLT_BFLOAT)
            {
                 out_value_size = dtsprintf(out_value, out_value_size, (dtext *) ostr1,  *((float *) number));
            }

            done = TRUE;

            if (fmt != ostr1)
            {
                OCI_ReleaseMetaString(ostr1);
            }

            if ((out_value_size) > 0)
            {
                while (out_value[out_value_size-1] == DT('0'))
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

    if (done == FALSE)
    {   
        void *ostr1 = NULL;
        void *ostr2 = NULL;
        int osize1  = out_value_size * (int) sizeof(dtext);
        int osize2  = -1;
        
        if (fmt == NULL)
        {
            fmt = OCI_GetDefaultFormatNumeric(con);
        }

        ostr1 = OCI_GetInputString(out_value, &osize1, sizeof(dtext), sizeof(omtext));
        ostr2 = OCI_GetInputMetaString(fmt, &osize2);

        OCI_CALL2
        (
            res, con,

            OCINumberToText(con->err, (OCINumber *) number,  (oratext *) ostr2,
                            (ub4) osize2, (oratext *) NULL,  (ub4) 0,
                            (ub4 *) &osize1, (oratext *) ostr1)
        )

        OCI_GetOutputString(ostr1, out_value, &osize1, sizeof(omtext), sizeof(dtext));
        OCI_ReleaseMetaString(ostr2);

        if (out_value != ostr1)
        {
            OCI_ReleaseMetaString(ostr1);
        }

        out_value_size = (osize1 / (int) sizeof(dtext));
    }  

    /* do we need to suppress last '.' or ',' from integers */

    if ((--out_value_size) >= 0)
    {
        if ((out_value[out_value_size] == DT('.')) ||
            (out_value[out_value_size] == DT(',')))
        {
            out_value[out_value_size] = 0;
        }
    }

    return res;
}
