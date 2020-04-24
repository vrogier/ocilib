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

OCI_Timestamp * TimestampInit
(
    OCI_Connection  *con,
    OCI_Timestamp   *tmsp,
    OCIDateTime *buffer,
    ub4          type
);

OCI_Timestamp * TimestampCreate
(
    OCI_Connection  *con,
    unsigned int type
);

boolean TimestampFree
(
    OCI_Timestamp *tmsp
);

OCI_Timestamp ** TimestampArrayCreate
(
    OCI_Connection  *con,
    unsigned int type,
    unsigned int nbelem
);

boolean TimestampArrayFree
(
    OCI_Timestamp **tmsps
);

unsigned int TimestampGetType
(
    OCI_Timestamp *tmsp
);

boolean TimestampAssign
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

int TimestampCheck
(
    OCI_Timestamp *tmsp
);

int TimestampCompare
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2
);

boolean TimestampConstruct
(
    OCI_Timestamp   *tmsp,
    int          year,
    int          month,
    int          day,
    int          hour,
    int          min,
    int          sec,
    int          fsec,
    const otext *time_zone
);

boolean TimestampConvert
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp_src
);

boolean TimestampFromText
(
    OCI_Timestamp   *tmsp,
    const otext *str,
    const otext *fmt
);

boolean TimestampToText
(
    OCI_Timestamp   *tmsp,
    const otext *fmt,
    int          size,
    otext       *str,
    int          precision
);

boolean TimestampGetDate
(
    OCI_Timestamp *tmsp,
    int       *year,
    int       *month,
    int       *day
);

boolean TimestampGetTime
(
    OCI_Timestamp *tmsp,
    int       *hour,
    int       *min,
    int       *sec,
    int       *fsec
);

boolean TimestampGetDateTime
(
    OCI_Timestamp *tmsp,
    int       *year,
    int       *month,
    int       *day,
    int       *hour,
    int       *min,
    int       *sec,
    int       *fsec
);

boolean TimestampGetTimeZoneName
(
    OCI_Timestamp *tmsp,
    int        size,
    otext     *str
);

boolean TimestampGetTimeZoneOffset
(
    OCI_Timestamp *tmsp,
    int       *hour,
    int       *min
);

boolean TimestampIntervalAdd
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
);

boolean TimestampIntervalSub
(
    OCI_Timestamp *tmsp,
    OCI_Interval  *itv
);

boolean TimestampSubtract
(
    OCI_Timestamp *tmsp,
    OCI_Timestamp *tmsp2,
    OCI_Interval  *itv
);

boolean TimestampSysTimestamp
(
    OCI_Timestamp *tmsp
);

boolean TimestampToCTime
(
    OCI_Timestamp *tmsp,
    struct tm *ptm,
    time_t    *pt
);

boolean TimestampFromCTime
(
    OCI_Timestamp *tmsp,
    struct tm *ptm,
    time_t     t
);

#endif /* OCILIB_TIMESTAMP_H_INCLUDED */
