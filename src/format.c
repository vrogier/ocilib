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
 * $Id: format.c, v 3.2.0 2009/04/20 00:00 Vince $
 * ------------------------------------------------------------------------ */

#include "ocilib_internal.h"

/* ************************************************************************ *
 *                             PRIVATE FUNCTIONS
 * ************************************************************************ */

/* ------------------------------------------------------------------------ *
 * OCI_ParseSqlFmt
 * ------------------------------------------------------------------------ */

int OCI_ParseSqlFmt(OCI_Statement *stmt, mtext *buf, const mtext *format,
                    va_list *pargs)
{
    int size        = 0;
    int len         = 0;
    boolean quote   = FALSE;
    mtext *pb       = buf;
    const mtext *pf = format;

    OCI_CHECK(format == NULL, 0);

    for (; *pf != 0; pf++)
    {
        if (*pf != MT('%'))
        {
            if (buf != NULL) 
                *(pb++) = *pf;

            size++;
            continue;
        }
        else
        {
            quote = TRUE;
            len   = 0;

            if ( *(++pf) == MT('%'))
            {
                if (buf != NULL)
                    *pb = *pf;

                quote = FALSE;
                len   = 1;
            }
        }

        switch (*pf)
        {
            case MT('s'):
            case MT('m'):
            {
                const mtext *str = va_arg(*pargs, const mtext *);

                if (str != NULL && str[0] != 0)
                {
                    len = (int) ((str!= NULL) ? mtslen(str) : OCI_SIZE_NULL);

                    if ((quote == TRUE) && (*pf != MT('m')))
                    {
                        if (buf != NULL)
                        {
                            *pb = 39;
                            mtscpy(pb + 1, str);
                            *(pb + len + 1) = MT('\'');
                        }

                        len+=2;
                    }
                    else if (buf != NULL) 
                        mtscpy(pb, str);
                }
                else
                {
                    if (*pf != MT('m'))
                    {
                        len = OCI_SIZE_NULL;
                        
                        if (buf != NULL) 
                            mtscpy(pb, OCI_STRING_NULL);
                    }
                }
                break;
            }
            case MT('t'):
            {
                OCI_Date *date = (OCI_Date *) va_arg(*pargs, OCI_Date *);

                if (buf != NULL)
                {
                    if (date != NULL)
                    {
                        mtsprintf(pb, OCI_SIZE_DATE,
                                  MT("to_date(\'%02i%02i%04i%02i%02i%02i\',")
                                  MT("\'DDMMYYYYHH24MISS\')"),
                                  date->handle->OCIDateDD,
                                  date->handle->OCIDateMM,
                                  date->handle->OCIDateYYYY ,
                                  date->handle->OCIDateTime.OCITimeHH,
                                  date->handle->OCIDateTime.OCITimeMI,
                                  date->handle->OCIDateTime.OCITimeSS);
                    }
                    else
                    {
                        mtscpy(pb, OCI_STRING_NULL);
                    }
                }

                len = ((date != NULL) ? OCI_SIZE_DATE : OCI_SIZE_NULL);

                break;
            }
            case MT('p'):
            {
                OCI_Timestamp *tmsp = (OCI_Timestamp *) va_arg(*pargs, OCI_Timestamp *);

                if (buf != NULL)
                {
                    if (tmsp != NULL)
                    {
                        mtext str_ff[12];
                        int yy, mm, dd, hh, mi, ss, ff;

                        yy = mm = dd = mi = hh = ss = ff = 0;

                        OCI_TimestampGetDateTime(tmsp, &yy, &mm, &dd,
                                                 &hh, &mi, &ss, &ff);

                        if (ff > 0)
                            mtsprintf(str_ff, msizeof(str_ff)-1, MT("%i"), ff);
                        else
                            mtscpy(str_ff, MT("00"));

                        str_ff[2] = 0;

                        len = mtsprintf(pb, OCI_SIZE_TIMESTAMP,
                                        MT("to_timestamp(\'%02i%02i%04i%02i%02i%02i%s\',")
                                        MT("\'DDMMYYYYHH24MISSFF\')"),
                                        dd, mm, yy, hh, mi, ss, str_ff);
                    }
                    else
                    {
                        mtscpy(pb, OCI_STRING_NULL);
                    }
                }
                else
                    len = ((tmsp != NULL) ? OCI_SIZE_TIMESTAMP : OCI_SIZE_NULL);

                break;
            }
            case MT('v'):
            {
                mtext temp[128];

                OCI_Interval *itv = (OCI_Interval *) va_arg(*pargs, OCI_Interval *);

                temp[0] = 0;

                if (itv != NULL)
                {
                    OCI_IntervalToText(itv, 3, 3, msizeof(temp)-1, temp);
                    
                    len = (int) mtslen(temp);

                    if ((buf != NULL) && (len > 0)) 
                        mtscpy(pb, temp);
                }
                else
                {
                    len = OCI_SIZE_NULL;
                    
                    if ((buf != NULL) && (len > 0)) 
                        mtscpy(pb, OCI_STRING_NULL);
                }
  
                break;
            }
            case MT('i'):
            {
                mtext temp[64];
       
                temp[0] = 0;

                len = mtsprintf(temp, msizeof(temp)-1, MT("%i"),
                                va_arg(*pargs, int));

                if ((buf != NULL) && (len > 0)) 
                    mtscpy(pb, temp);

                break;
            }
            case MT('u'):
            {
                mtext temp[64];
  
                temp[0] = 0;

                len = mtsprintf(temp, msizeof(temp)-1, MT("%u"),
                                va_arg(*pargs, unsigned int));

                if ((buf != NULL) && (len > 0)) 
                    mtscpy(pb, temp);

                break;
            }
            case MT('l'):
            {
                mtext temp[64];

                temp[0] = 0;

                pf++;

                if (*pf == MT('i'))
                {
                    len = mtsprintf(temp, msizeof(temp)-1, MT("%lld"),
                                    va_arg(*pargs, big_int));
                }
                else if (*pf == MT('u'))
                {
                    len = mtsprintf(temp, msizeof(temp)-1, MT("%llu"),
                                    va_arg(*pargs, big_uint));
                }
                else
                    len = 0;

                if ((buf != NULL) && (len > 0)) 
                    mtscpy(pb, temp);

                break;
            }
            case MT('h'):
            {
                mtext temp[64];
   
                temp[0] = 0;

                pf++;

                /* short ints must be passed as int to va_args */

                if (*pf == 'i')
                {
                    len = mtsprintf(temp, msizeof(temp)-1, MT("%hd"),
                                    va_arg(*pargs, int));
                }
                else if (*pf == 'u')
                {
                    len = mtsprintf(temp, msizeof(temp)-1, MT("%hu"),
                                    va_arg(*pargs, unsigned int));
                }
                else
                    len = 0;

                if ((buf != NULL) && (len > 0)) 
                    mtscpy(pb, temp);

                break;
            }
            case MT('g'):
            {
                mtext temp[128];
      
                temp[0] = 0;

                len = mtsprintf(temp, msizeof(temp)-1, MT("%f"),
                                va_arg(*pargs, double));

                if ((buf != NULL) && (len > 0)) 
                    mtscpy(pb, temp);

                break;
            }
            default:
            {
                OCI_ExceptionParsingToken(stmt->con, stmt, *pf);

                return 0;
            }

        }

        if (buf != NULL)
            pb += len;

        size += len;
    }

    if (buf != NULL) 
        *pb = 0;

    return size;
}
