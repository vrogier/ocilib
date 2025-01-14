/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2025 Vincent ROGIER <vince.rogier@ocilib.net>
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

#ifndef OCILIB_TIMESTAMP_H_INCLUDED
#define OCILIB_TIMESTAMP_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Timestamp * OcilibTimestampInitialize
(
    OCI_Connection *con,
    OCI_Timestamp  *tmsp,
    OCIDateTime    *buffer,
    ub4             type
);

OCI_SYM_LOCAL OCI_Timestamp * OcilibTimestampCreate
(
    OCI_Connection *con,
    unsigned int    type
);

OCI_SYM_LOCAL boolean OcilibTimestampFree
(
    OCI_Timestamp *tmsp
);

OCI_SYM_LOCAL OCI_Timestamp ** OcilibTimestampCreateArray
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    nbelem
);

OCI_SYM_LOCAL boolean OcilibTimestampFreeArray
(
    OCI_Timestamp **tmsps
);

OCI_SYM_LOCAL unsigned int OcilibTimestampGetType
(
    OCI_Timestamp *tmsp
);

OCI_SYM_LOCAL boolean OcilibTimestampAssign
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

OCI_SYM_LOCAL int OcilibTimestampCheck
(
    OCI_Timestamp *tmsp
);

OCI_SYM_LOCAL int OcilibTimestampCompare
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2
);

OCI_SYM_LOCAL boolean OcilibTimestampConstruct
(
    OCI_Timestamp *tmsp,
    int            year,
    int            month,
    int            day,
    int            hour,
    int            min,
    int            sec,
    int            fsec,
    const otext   *time_zone
);

OCI_SYM_LOCAL boolean OcilibTimestampConvert
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

OCI_SYM_LOCAL boolean OcilibTimestampFromString
(
    OCI_Timestamp *tmsp,
    const otext   *str,
    const otext   *fmt
);

OCI_SYM_LOCAL boolean OcilibTimestampToString
(
    OCI_Timestamp *tmsp,
    const otext   *fmt,
    int            size,
    otext         *str,
    int            precision
);

OCI_SYM_LOCAL boolean OcilibTimestampGetDate
(
    OCI_Timestamp *tmsp,
    int           *year,
    int           *month,
    int           *day
);

OCI_SYM_LOCAL boolean OcilibTimestampGetTime
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min,
    int           *sec,
    int           *fsec
);

OCI_SYM_LOCAL boolean OcilibTimestampGetDateTime
(
    OCI_Timestamp *tmsp,
    int           *year,
    int           *month,
    int           *day,
    int           *hour,
    int           *min,
    int           *sec,
    int           *fsec
);

OCI_SYM_LOCAL boolean OcilibTimestampGetTimeZoneName
(
    OCI_Timestamp *tmsp,
    int            size,
    otext         *str
);

OCI_SYM_LOCAL boolean OcilibTimestampGetTimeZoneOffset
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min
);

OCI_SYM_LOCAL boolean OcilibTimestampIntervalAdd
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
);

OCI_SYM_LOCAL boolean OcilibTimestampIntervalSub
(
    OCI_Timestamp* tmsp,
    OCI_Interval  *itv
);

OCI_SYM_LOCAL boolean OcilibTimestampSubtract
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2,
    OCI_Interval  *itv
);

OCI_SYM_LOCAL boolean OcilibTimestampSysTimestamp
(
    OCI_Timestamp *tmsp
);

OCI_SYM_LOCAL boolean OcilibTimestampToCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t        *pt
);

OCI_SYM_LOCAL boolean OcilibTimestampFromCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t         t
);

#endif /* OCILIB_TIMESTAMP_H_INCLUDED */
