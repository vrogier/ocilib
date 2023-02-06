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

#ifndef OCILIB_DATE_H_INCLUDED
#define OCILIB_DATE_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Date * OcilibDateInitialize
(
    OCI_Connection *con,
    OCI_Date       *date,
    OCIDate        *buffer,
    boolean         allocate,
    boolean         ansi
);

OCI_SYM_LOCAL OCI_Date * OcilibDateCreate
(
    OCI_Connection *con
);

OCI_SYM_LOCAL boolean OcilibDateFree
(
    OCI_Date* date
);

OCI_SYM_LOCAL OCI_Date ** OcilibDateCreateArray
(
    OCI_Connection *con,
    unsigned int    nbelem
);

OCI_SYM_LOCAL boolean OcilibDateFreeArray
(
    OCI_Date** dates
);

OCI_SYM_LOCAL boolean OcilibDateAddDays
(
    OCI_Date* date,
    int       nb
);

OCI_SYM_LOCAL boolean OcilibDateAddMonths
(
    OCI_Date* date,
    int       nb
);

OCI_SYM_LOCAL boolean OcilibDateAssign
(
    OCI_Date* date,
    OCI_Date* date_src
);

OCI_SYM_LOCAL int OcilibDateCheck
(
    OCI_Date* date
);

OCI_SYM_LOCAL int OcilibDateCompare
(
    OCI_Date* date,
    OCI_Date* date2
);

OCI_SYM_LOCAL int OcilibDateDaysBetween
(
    OCI_Date* date,
    OCI_Date* date2
);

OCI_SYM_LOCAL boolean OcilibDateFromString
(
    OCI_Date   * date,
    const otext* str,
    const otext* fmt
);

OCI_SYM_LOCAL boolean OcilibDateToString
(
    OCI_Date   * date,
    const otext* fmt,
    int          size,
    otext      * str
);

OCI_SYM_LOCAL boolean OcilibDateGetDate
(
    OCI_Date* date,
    int     * year,
    int     * month,
    int     * day
);

OCI_SYM_LOCAL boolean OcilibDateGetTime
(
    OCI_Date* date,
    int     * hour,
    int     * min,
    int     * sec
);

OCI_SYM_LOCAL boolean OcilibDateGetDateTime
(
    OCI_Date* date,
    int     * year,
    int     * month,
    int     * day,
    int     * hour,
    int     * min,
    int     * sec
);

OCI_SYM_LOCAL boolean OcilibDateLastDay
(
    OCI_Date* date
);

OCI_SYM_LOCAL boolean OcilibDateNextDay
(
    OCI_Date   * date,
    const otext* day
);

OCI_SYM_LOCAL boolean OcilibDateSetDate
(
    OCI_Date* date,
    int       year,
    int       month,
    int       day
);

OCI_SYM_LOCAL boolean OcilibDateSetTime
(
    OCI_Date* date,
    int       hour,
    int       min,
    int       sec
);

OCI_SYM_LOCAL boolean OcilibDateSetDateTime
(
    OCI_Date* date,
    int       year,
    int       month,
    int       day,
    int       hour,
    int       min,
    int       sec
);

OCI_SYM_LOCAL boolean OcilibDateSysDate
(
    OCI_Date* date
);

OCI_SYM_LOCAL boolean OcilibDateZoneToZone
(
    OCI_Date   * date,
    const otext* zone1,
    const otext* zone2
);

OCI_SYM_LOCAL boolean OcilibDateToCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t   * pt
);

OCI_SYM_LOCAL boolean OcilibDateFromCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t     t
);

#endif /* OCILIB_DATE_H_INCLUDED */
