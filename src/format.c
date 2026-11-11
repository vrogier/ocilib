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

#include "format.h"

#include "exception.h"
#include "interval.h"
#include "json.h"
#include "macros.h"
#include "number.h"
#include "reference.h"
#include "stringutils.h"
#include "timestamp.h"
#include "vector.h"

/* --------------------------------------------------------------------------------------------- *
 * OcilibParseSqlFmt
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibFormatParseSql
(
    OCI_Statement *stmt,
    otext         *buf,
    const otext   *format,
    va_list       *pargs
)
{
    ENTER_FUNC
    (
        /* returns */ OCI_Int32, 0,
        /* context */ OCI_IPC_STATEMENT, stmt
    );

    OCI_Int32    size  = 0;
    OCI_Int32    len   = 0;
    OCI_Boolean  quote = FALSE;
    otext       *pb    = buf;
    const otext *pf    = format;

    CHECK_PTR(OCI_IPC_STATEMENT, stmt);
    CHECK_PTR(OCI_IPC_STRING,    format);

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
            case OTEXT('%'):
            {
                /* escaped percent: the literal '%' was already emitted above */

                break;
            }
            case OTEXT('s'):
            case OTEXT('m'):
            {
                const otext *str = (const otext *) va_arg(*pargs, const otext *);

                if (IS_STRING_VALID(str))
                {
                    len = (OCI_Int32) ostrlen(str);

                    if (quote && (*pf != OTEXT('m')))
                    {
                        if (buf)
                        {
                            *pb =  OTEXT('\'');
                            ostrcpy(pb + (OCI_Size) 1, str);
                            *(pb + (OCI_Size) (len + 1)) = OTEXT('\'');
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
                        otext     str_ff[12];
                        OCI_Int32 yy = 0, mm = 0, dd = 0, hh = 0, mi = 0, ss = 0, ff = 0;

                        CHECK(OcilibTimestampGetDateTime(tmsp, &yy, &mm, &dd,  &hh, &mi, &ss, &ff));

                        if (ff > 0)
                        {
                            OcilibStringFormat(str_ff, (OCI_Int32) osizeof(str_ff)- 1, OTEXT("%i"), ff);
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
                    CHECK(OcilibIntervalToString(itv, 3, 3, (OCI_Int32) osizeof(temp)- 1, temp));

                    len = (OCI_Int32) ostrlen(temp);

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

                len = (OCI_Int32)OcilibStringFormat(temp, (OCI_Int32) osizeof(temp) - 1,
                                                    OTEXT("%i"), va_arg(*pargs, OCI_Int32));

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

                len = (OCI_Int32)OcilibStringFormat(temp, (OCI_Int32)  osizeof(temp) - 1,
                                                    OTEXT("%u"), va_arg(*pargs, OCI_UInt32));

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
                    len = (OCI_Int32)OcilibStringFormat(temp, (OCI_Int32) osizeof(temp) - 1,
                                                        OTEXT("%lld"), va_arg(*pargs, OCI_Int64));
                }
                else if (OTEXT('u') == *pf)
                {
                    len = (OCI_Int32)OcilibStringFormat(temp, (OCI_Int32) osizeof(temp) - 1,
                                                        OTEXT("%llu"), va_arg(*pargs, OCI_UInt64));
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

                /* OCI_Int16 OCI_Int32 must be passed as OCI_Int32 to va_args */

                if (OTEXT('i') == *pf)
                {
                    len = (OCI_Int32)OcilibStringFormat(temp, (OCI_Int32) osizeof(temp) - 1,
                                                        OTEXT("%hd"), va_arg(*pargs, OCI_Int32));
                }
                else if (OTEXT('u') == *pf)
                {
                    len = (OCI_Int32)OcilibStringFormat(temp, (OCI_Int32) osizeof(temp) - 1,
                                                        OTEXT("%hu"), va_arg(*pargs, OCI_UInt32));
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

                len = (OCI_Int32)OcilibStringFormat(temp, (OCI_Int32) osizeof(temp) - 1,
                                                    OTEXT("%lf"), va_arg(*pargs, OCI_Double));

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

                CHECK(OcilibNumberToString(va_arg(*pargs, OCI_Number*), NULL, 128, temp));
                len = (OCI_Int32) ostrlen(temp);

                if (buf && (len > 0))
                {
                    ostrcpy(pb, temp);
                }

                break;
            }
            case OTEXT('r'):
            {
                otext temp[128];

                OCI_Reference *ref = (OCI_Reference *) va_arg(*pargs, OCI_Reference *);

                temp[0] = 0;

                if (ref)
                {
                    CHECK(OcilibReferenceToText( ref, (OCI_UInt32) osizeof(temp) - 1, temp));

                    len = (OCI_Int32) ostrlen(temp);

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
            case OTEXT('j'):
            {
                OCI_Json *json = (OCI_Json *) va_arg(*pargs, OCI_Json *);

                if (json)
                {
                    /* the document is emitted as a quoted JSON text literal */

                    OCI_UInt32 json_size = 0;

                    CHECK(OcilibJsonToText(json, NULL, &json_size, 0));

                    len = (OCI_Int32) json_size + 2;

                    if (buf)
                    {
                        OCI_UInt32 out_size = json_size;

                        pb[0] = OTEXT('\'');

                        CHECK(OcilibJsonToText(json, pb + 1, &out_size, 0));

                        pb[out_size + 1] = OTEXT('\'');

                        len = (OCI_Int32) out_size + 2;
                    }
                }
                else
                {
                    len = OCI_SIZE_NULL;

                    if (buf)
                    {
                        ostrcpy(pb, OCI_STRING_NULL);
                    }
                }

                break;
            }
            case OTEXT('V'):
            {
                OCI_Vector *vect = (OCI_Vector *) va_arg(*pargs, OCI_Vector *);

                if (vect)
                {
                    /* the vector is emitted as a quoted '[...]' text literal */

                    OCI_UInt32 vect_size = 0;

                    CHECK(OcilibVectorToString(vect, &vect_size, NULL));

                    len = (OCI_Int32) vect_size + 2;

                    if (buf)
                    {
                        OCI_UInt32 out_size = vect_size;

                        pb[0] = OTEXT('\'');

                        CHECK(OcilibVectorToString(vect, &out_size, pb + 1));

                        pb[out_size + 1] = OTEXT('\'');

                        len = (OCI_Int32) out_size + 2;
                    }
                }
                else
                {
                    len = OCI_SIZE_NULL;

                    if (buf)
                    {
                        ostrcpy(pb, OCI_STRING_NULL);
                    }
                }

                break;
            }
            default:
            {
                THROW(OcilibExceptionParsingToken, *pf);

                break;
            }
        }

        if (buf)
        {
            pb += (OCI_Size) len;
        }

        size += len;
    }

    if (buf)
    {
        *pb = 0;
    }

    SET_RETVAL(size);

    EXIT_FUNC();
}
