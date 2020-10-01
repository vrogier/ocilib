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

#ifndef OCILIB_INTERVAL_H_INCLUDED
#define OCILIB_INTERVAL_H_INCLUDED

#include "types.h"

OCI_Interval * OcilibIntervalInitialize
(
    OCI_Connection *con,
    OCI_Interval   *itv,
    OCIInterval    *buffer,
    ub4             type
);

OCI_Interval * OcilibIntervalCreate
(
    OCI_Connection *con,
    unsigned int    type
);

boolean OcilibIntervalFree
(
    OCI_Interval *itv
);

OCI_Interval ** OcilibIntervalCreateArray
(
    OCI_Connection *con,
    unsigned int    type,
    unsigned int    nbelem
);

boolean OcilibIntervalFreeArray
(
    OCI_Interval **itvs
);

unsigned int OcilibIntervalGetType
(
    OCI_Interval *itv
);

boolean OcilibIntervalAssign
(
    OCI_Interval *itv,
    OCI_Interval *itv_src
);

int OcilibIntervalCheck
(
    OCI_Interval *itv
);

int OcilibIntervalCompare
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

boolean OcilibIntervalFromString
(
    OCI_Interval *itv,
    const otext * str
);

boolean OcilibIntervalToString
(
    OCI_Interval *itv,
    int           leading_prec,
    int           fraction_prec,
    int           size,
    otext        *str
);

boolean OcilibIntervalFromTimeZone
(
    OCI_Interval *itv,
    const otext * str
);

boolean OcilibIntervalGetDaySecond
(
    OCI_Interval *itv,
    int          *day,
    int          *hour,
    int          *min,
    int          *sec,
    int          *fsec
);

boolean OcilibIntervalGetYearMonth
(
    OCI_Interval *itv,
    int          *year,
    int          *month
);

boolean OcilibIntervalSetDaySecond
(
    OCI_Interval *itv,
    int           day,
    int           hour,
    int           min,
    int           sec,
    int           fsec
);

boolean OcilibIntervalSetYearMonth
(
    OCI_Interval *itv,
    int           year,
    int           month
);

boolean OcilibIntervalAdd
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

boolean OcilibIntervalSubtract
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

#endif /* OCILIB_INTERVAL_H_INCLUDED */
