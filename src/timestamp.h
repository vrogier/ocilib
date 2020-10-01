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

#ifndef OCILIB_TIMESTAMP_H_INCLUDED
#define OCILIB_TIMESTAMP_INCLUDED

#include "types.h"

OCI_Timestamp * OcilibTimestampInitialize
(
    OCI_Connection *con,
    OCI_Timestamp  *tmsp,
    OCIDateTime    *buffer,
    ub4             type
);

OCI_Timestamp * OcilibTimestampCreate
(
    OCI_Connection *con,
    unsigned int    type
);

boolean OcilibTimestampFree
(
    OCI_Timestamp *tmsp
);

OCI_Timestamp ** OcilibTimestampCreateArray
(
    OCI_Connection* con,
    unsigned int    type,
    unsigned int    nbelem
);

boolean OcilibTimestampFreeArray
(
    OCI_Timestamp **tmsps
);

unsigned int OcilibTimestampGetType
(
    OCI_Timestamp *tmsp
);

boolean OcilibTimestampAssign
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

int OcilibTimestampCheck
(
    OCI_Timestamp *tmsp
);

int OcilibTimestampCompare
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2
);

boolean OcilibTimestampConstruct
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

boolean OcilibTimestampConvert
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

boolean OcilibTimestampFromString
(
    OCI_Timestamp *tmsp,
    const otext   *str,
    const otext   *fmt
);

boolean OcilibTimestampToString
(
    OCI_Timestamp *tmsp,
    const otext   *fmt,
    int            size,
    otext         *str,
    int            precision
);

boolean OcilibTimestampGetDate
(
    OCI_Timestamp *tmsp,
    int           *year,
    int           *month,
    int           *day
);

boolean OcilibTimestampGetTime
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min,
    int           *sec,
    int           *fsec
);

boolean OcilibTimestampGetDateTime
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

boolean OcilibTimestampGetTimeZoneName
(
    OCI_Timestamp *tmsp,
    int            size,
    otext         *str
);

boolean OcilibTimestampGetTimeZoneOffset
(
    OCI_Timestamp *tmsp,
    int           *hour,
    int           *min
);

boolean OcilibTimestampIntervalAdd
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
);

boolean OcilibTimestampIntervalSub
(
    OCI_Timestamp* tmsp,
    OCI_Interval  *itv
);

boolean OcilibTimestampSubtract
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2,
    OCI_Interval  *itv
);

boolean OcilibTimestampSysTimestamp
(
    OCI_Timestamp *tmsp
);

boolean OcilibTimestampToCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t        *pt
);

boolean OcilibTimestampFromCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t         t
);

#endif /* OCILIB_TIMESTAMP_H_INCLUDED */
