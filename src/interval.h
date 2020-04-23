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

OCI_Interval * IntervalInit
(
    OCI_Connection  *con,
    OCI_Interval    *itv,
    OCIInterval *buffer,
    ub4          type
);

OCI_Interval * IntervalCreate
(
    OCI_Connection  *con,
    unsigned int type
);

boolean IntervalFree
(
    OCI_Interval *itv
);

OCI_Interval ** IntervalArrayCreate
(
    OCI_Connection  *con,
    unsigned int type,
    unsigned int nbelem
);

boolean IntervalArrayFree
(
    OCI_Interval **itvs
);

unsigned int IntervalGetType
(
    OCI_Interval *itv
);

boolean IntervalAssign
(
    OCI_Interval *itv,
    OCI_Interval *itv_src
);

int IntervalCheck
(
    OCI_Interval *itv
);

int IntervalCompare
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

boolean IntervalFromText
(
    OCI_Interval     *itv,
    const otext * str
);

boolean IntervalToText
(
    OCI_Interval *itv,
    int       leading_prec,
    int       fraction_prec,
    int       size,
    otext    *str
);

boolean IntervalFromTimeZone
(
    OCI_Interval     *itv,
    const otext * str
);

boolean IntervalGetDaySecond
(
    OCI_Interval *itv,
    int      *day,
    int      *hour,
    int      *min,
    int      *sec,
    int      *fsec
);

boolean IntervalGetYearMonth
(
    OCI_Interval *itv,
    int      *year,
    int      *month
);

boolean IntervalSetDaySecond
(
    OCI_Interval *itv,
    int       day,
    int       hour,
    int       min,
    int       sec,
    int       fsec
);

boolean IntervalSetYearMonth
(
    OCI_Interval *itv,
    int       year,
    int       month
);

boolean IntervalAdd
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

boolean IntervalSubtract
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

#endif /* OCILIB_INTERVAL_H_INCLUDED */
