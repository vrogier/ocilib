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

#ifndef OCILIB_INTERVAL_H_INCLUDED
#define OCILIB_INTERVAL_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Interval * OcilibIntervalInitialize
(
    OCI_Environment *env,
    OCI_Connection  *con,
    OCI_Interval    *itv,
    OCIInterval     *buffer,
    ub4              type
);

OCI_SYM_LOCAL OCI_Interval * OcilibIntervalCreate
(
    OCI_Environment *env,
    OCI_IntervalType type
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalFree
(
    OCI_Interval *itv
);

OCI_SYM_LOCAL OCI_Interval ** OcilibIntervalCreateArray
(
    OCI_Environment *env,
    OCI_IntervalType type,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalFreeArray
(
    OCI_Interval **itvs
);

OCI_SYM_LOCAL OCI_IntervalType OcilibIntervalGetType
(
    OCI_Interval *itv
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalAssign
(
    OCI_Interval *itv,
    OCI_Interval *itv_src
);

OCI_SYM_LOCAL OCI_Int32 OcilibIntervalCheck
(
    OCI_Interval *itv
);

OCI_SYM_LOCAL OCI_Int32 OcilibIntervalCompare
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalFromString
(
    OCI_Interval *itv,
    const otext * str
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalToString
(
    OCI_Interval *itv,
    OCI_Int32           leading_prec,
    OCI_Int32           fraction_prec,
    OCI_Int32           size,
    otext        *str
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalFromTimeZone
(
    OCI_Interval *itv,
    const otext * str
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalGetDaySecond
(
    OCI_Interval *itv,
    OCI_Int32          *day,
    OCI_Int32          *hour,
    OCI_Int32          *min,
    OCI_Int32          *sec,
    OCI_Int32          *fsec
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalGetYearMonth
(
    OCI_Interval *itv,
    OCI_Int32          *year,
    OCI_Int32          *month
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalSetDaySecond
(
    OCI_Interval *itv,
    OCI_Int32           day,
    OCI_Int32           hour,
    OCI_Int32           min,
    OCI_Int32           sec,
    OCI_Int32           fsec
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalSetYearMonth
(
    OCI_Interval *itv,
    OCI_Int32           year,
    OCI_Int32           month
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalAdd
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

OCI_SYM_LOCAL OCI_Boolean OcilibIntervalSubtract
(
    OCI_Interval *itv,
    OCI_Interval *itv2
);

#endif /* OCILIB_INTERVAL_H_INCLUDED */
