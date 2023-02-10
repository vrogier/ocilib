/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2023 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "format.h"

#include "exception.h"
#include "interval.h"
#include "macros.h"
#include "number.h"
#include "reference.h"
#include "stringutils.h"
#include "timestamp.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibParseSqlFmt
 * --------------------------------------------------------------------------------------------- */

int OcilibFormatParseSql
(
    OCI_Statement *stmt,
    otext         *buf,
    const otext   *format,
    va_list       *pargs
)
{
    ENTER_FUNC
    (
        /* returns */ int, 0,
        /* context */ OCI_IPC_STATEMENT, stmt
    )

    int size = 0;
    int          len   = 0;
    boolean      quote = FALSE;
    otext       *pb    = buf;
    const otext *pf    = format;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt)
    CHECK_PTR(OCI_IPC_STRING,    format)

    for (; *pf; pf++)
    {
        if (*pf != OTEXT('%'))
        {
            if (buf)
            {
                *(pb++) = *pf;
            }

            size++;
            continue;
        }
        else
        {
            quote = TRUE;
            len   = 0;

            if (OTEXT('%') == *(++pf))
            {
                if (buf)
                {
                    *pb = *pf;
                }

                quote = FALSE;
                len   = 1;
            }
        }

        switch (*pf)
        {
            case OTEXT('s'):
            case OTEXT('m'):
            {
                const otext *str = (const otext *) va_arg(*pargs, const otext *);

                if (IS_STRING_VALID(str))
                {
                    len = (int) ostrlen(str);

                    if (quote && (*pf != OTEXT('m')))
                    {
                        if (buf)
                        {
                            *pb =  OTEXT('\'');
                            ostrcpy(pb + (size_t) 1, str);
                            *(pb + (size_t) (len + 1)) = OTEXT('\'');
                        }

                        len += 2;
                    }
                    else if (buf)
                    {
                        ostrcpy(pb, str);
                    }
                }
                else
                {
                    if (*pf != OTEXT('m'))
                    {
                        len = OCI_SIZE_NULL;

                        if (buf)
                        {
                            ostrcpy(pb, OCI_STRING_NULL);
                        }
                    }
                }
                break;
            }
            case OTEXT('t'):
            {
                OCI_Date *date = (OCI_Date *) va_arg(*pargs, OCI_Date *);

                if (buf)
                {
                    if (date)
                    {
                        len = OcilibStringFormat
                        (
                            pb, OCI_SIZE_DATE,
                           OTEXT("to_date('%02i%02i%04i%02i%02i%02i',")
                           OTEXT("'DDMMYYYYHH24MISS')"),
                           date->handle->OCIDateDD,
                           date->handle->OCIDateMM,
                           date->handle->OCIDateYYYY,
                           date->handle->OCIDateTime.OCITimeHH,
                           date->handle->OCIDateTime.OCITimeMI,
                           date->handle->OCIDateTime.OCITimeSS
                        );
                    }
                    else
                    {
                        ostrcpy(pb, OCI_STRING_NULL);
                        len = OCI_SIZE_NULL;
                    }
                }
                else
                {
                    len = date ? OCI_SIZE_DATE : OCI_SIZE_NULL;
                }

                break;
            }
            case OTEXT('p'):
            {
                OCI_Timestamp *tmsp = (OCI_Timestamp *) va_arg(*pargs, OCI_Timestamp *);

                if (buf)
                {
                    if (tmsp)
                    {
                        otext str_ff[12];
                        int   yy = 0, mm = 0, dd = 0, hh = 0, mi = 0, ss = 0, ff = 0;

                        CHECK(OcilibTimestampGetDateTime(tmsp, &yy, &mm, &dd,  &hh, &mi, &ss, &ff))

                        if (ff > 0)
                        {
                            OcilibStringFormat(str_ff, (int) osizeof(str_ff)- 1, OTEXT("%i"), ff);
                        }
                        else
                        {
                            ostrcpy(str_ff, OTEXT("00"));
                        }

                        str_ff[2] = 0;

                        len = OcilibStringFormat
                        (
                            pb, 
                            OCI_SIZE_TIMESTAMP,
                        #if defined(OCI_CHARSET_WIDE) && !defined(_WINDOWS)
                            OTEXT("to_timestamp('%02i%02i%04i%02i%02i%02i%ls', 'DDMMYYYYHH24MISSFF')"),
                        #else
                            OTEXT("to_timestamp('%02i%02i%04i%02i%02i%02i%s', 'DDMMYYYYHH24MISSFF')"),
                        #endif                            
                            dd, mm, yy, hh, mi, ss, str_ff
                        );
                    }
                    else
                    {
                        ostrcpy(pb, OCI_STRING_NULL);
                        len = OCI_SIZE_NULL;
                    }
                }
                else
                {
                    len = tmsp ? OCI_SIZE_TIMESTAMP : OCI_SIZE_NULL;
                }

                break;
            }
            case OTEXT('v'):
            {
                otext temp[128];

                OCI_Interval *itv = (OCI_Interval *) va_arg(*pargs, OCI_Interval *);

                temp[0] = 0;

                if (itv)
                {
                    CHECK(OcilibIntervalToString(itv, 3, 3, (int) osizeof(temp)- 1, temp))

                    len = (int) ostrlen(temp);

                    if ((buf) && (len > 0))
                    {
                        ostrcpy(pb, temp);
                    }
                }
                else
                {
                    len = OCI_SIZE_NULL;

                    if (buf && (len > 0))
                    {
                        ostrcpy(pb, OCI_STRING_NULL);
                    }
                }

                break;
            }
            case OTEXT('i'):
            {
                otext temp[64];

                temp[0] = 0;

                len = (int)OcilibStringFormat(temp, (int) osizeof(temp) - 1, 
                                              OTEXT("%i"), va_arg(*pargs, int));

                if (buf && (len > 0))
                {
                    ostrcpy(pb, temp);
                }

                break;
            }
            case OTEXT('u'):
            {
                otext temp[64];

                temp[0] = 0;

                len = (int)OcilibStringFormat(temp, (int)  osizeof(temp) - 1, 
                                              OTEXT("%u"), va_arg(*pargs, unsigned int));

                if (buf && (len > 0))
                {
                    ostrcpy(pb, temp);
                }

                break;
            }
            case OTEXT('l'):
            {
                otext temp[64];

                temp[0] = 0;

                pf++;

                if (OTEXT('i') == *pf)
                {
                    len = (int)OcilibStringFormat(temp, (int) osizeof(temp) - 1,
                                                  OTEXT("%lld"), va_arg(*pargs, big_int));
                }
                else if (OTEXT('u') == *pf)
                {
                    len = (int)OcilibStringFormat(temp, (int) osizeof(temp) - 1, 
                                                  OTEXT("%llu"), va_arg(*pargs, big_uint));
                }
                else
                {
                    len = 0;
                }

                if (buf && (len > 0))
                {
                    ostrcpy(pb, temp);
                }

                break;
            }
            case OTEXT('h'):
            {
                otext temp[128];

                temp[0] = 0;

                pf++;

                /* short int must be passed as int to va_args */

                if (OTEXT('i') == *pf)
                {
                    len = (int)OcilibStringFormat(temp, (int) osizeof(temp) - 1, 
                                                  OTEXT("%hd"), va_arg(*pargs, int));
                }
                else if (OTEXT('u') == *pf)
                {
                    len = (int)OcilibStringFormat(temp, (int) osizeof(temp) - 1,
                                                  OTEXT("%hu"), va_arg(*pargs, unsigned int));
                }
                else
                {
                    len = 0;
                }

                if (buf && (len > 0))
                {
                    ostrcpy(pb, temp);
                }

                break;
            }
            case OTEXT('g'):
            {
                otext temp[128];

                temp[0] = 0;

                len = (int)OcilibStringFormat(temp, (int) osizeof(temp) - 1, 
                                              OTEXT("%lf"), va_arg(*pargs, double));

                if (buf && (len > 0))
                {
                    ostrcpy(pb, temp);
                }

                break;
            }
            case OTEXT('n'):
            {
                otext temp[128];

                temp[0] = 0;

                CHECK(OcilibNumberToString(va_arg(*pargs, OCI_Number*), NULL, 128, temp))
                len = (int) ostrlen(temp);

                if (buf && (len > 0))
                {
                    ostrcpy(pb, temp);
                }

                break;
            }
            case OTEXT('r'):
            {
                otext temp[128];

                OCI_Ref *ref = (OCI_Ref *) va_arg(*pargs, OCI_Ref *);

                temp[0] = 0;

                if (ref)
                {
                    CHECK(OcilibReferenceToString( ref, (unsigned int) osizeof(temp) - 1, temp))

                    len = (int) ostrlen(temp);

                    if (buf && (len > 0))
                    {
                        ostrcpy(pb, temp);
                    }
                }
                else
                {
                    len = OCI_SIZE_NULL;

                    if (buf && (len > 0))
                    {
                        ostrcpy(pb, OCI_STRING_NULL);
                    }
                }

                break;
            }
            default:
            {
                THROW(OcilibExceptionParsingToken, *pf)

                break;
            }
        }

        if (buf)
        {
            pb += (size_t) len;
        }

        size += len;
    }

    if (buf)
    {
        *pb = 0;
    }

    SET_RETVAL(size)

    EXIT_FUNC()
}
