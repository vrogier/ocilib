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

#ifndef OCILIB_TIMESTAMP_H_INCLUDED
#define OCILIB_TIMESTAMP_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Timestamp * OcilibTimestampInitialize
(
    OCI_Environment *env,
    OCI_Connection  *con,
    OCI_Timestamp   *tmsp,
    OCIDateTime     *buffer,
    ub4              type
);

OCI_SYM_LOCAL OCI_Timestamp * OcilibTimestampCreate
(
    OCI_Environment  *env,
    OCI_TimestampType type
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampFree
(
    OCI_Timestamp *tmsp
);

OCI_SYM_LOCAL OCI_Timestamp ** OcilibTimestampCreateArray
(
    OCI_Environment  *env,
    OCI_TimestampType type,
    OCI_UInt32        nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampFreeArray
(
    OCI_Timestamp **tmsps
);

OCI_SYM_LOCAL OCI_TimestampType OcilibTimestampGetType
(
    OCI_Timestamp *tmsp
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampAssign
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

OCI_SYM_LOCAL OCI_Int32 OcilibTimestampCheck
(
    OCI_Timestamp *tmsp
);

OCI_SYM_LOCAL OCI_Int32 OcilibTimestampCompare
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampConstruct
(
    OCI_Timestamp *tmsp,
    OCI_Int32      year,
    OCI_Int32      month,
    OCI_Int32      day,
    OCI_Int32      hour,
    OCI_Int32      min,
    OCI_Int32      sec,
    OCI_Int32      fsec,
    const otext   *time_zone
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampConvert
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampFromString
(
    OCI_Timestamp *tmsp,
    const otext   *str,
    const otext   *fmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampToString
(
    OCI_Timestamp *tmsp,
    const otext   *fmt,
    OCI_Int32      size,
    otext         *str,
    OCI_Int32      precision
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampGetDate
(
    OCI_Timestamp *tmsp,
    OCI_Int32     *year,
    OCI_Int32     *month,
    OCI_Int32     *day
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampGetTime
(
    OCI_Timestamp *tmsp,
    OCI_Int32     *hour,
    OCI_Int32     *min,
    OCI_Int32     *sec,
    OCI_Int32     *fsec
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampGetDateTime
(
    OCI_Timestamp *tmsp,
    OCI_Int32     *year,
    OCI_Int32     *month,
    OCI_Int32     *day,
    OCI_Int32     *hour,
    OCI_Int32     *min,
    OCI_Int32     *sec,
    OCI_Int32     *fsec
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampGetTimeZoneName
(
    OCI_Timestamp *tmsp,
    OCI_Int32      size,
    otext         *str
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampGetTimeZoneOffset
(
    OCI_Timestamp *tmsp,
    OCI_Int32     *hour,
    OCI_Int32     *min
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampIntervalAdd
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampIntervalSub
(
    OCI_Timestamp* tmsp,
    OCI_Interval  *itv
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampSubtract
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2,
    OCI_Interval  *itv
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampSysTimestamp
(
    OCI_Timestamp *tmsp
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampToCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t        *pt
);

OCI_SYM_LOCAL OCI_Boolean OcilibTimestampFromCTime
(
    OCI_Timestamp *tmsp,
    struct tm     *ptm,
    time_t         t
);

#endif /* OCILIB_TIMESTAMP_H_INCLUDED */
