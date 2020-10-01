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

#ifndef OCILIB_DATE_H_INCLUDED
#define OCILIB_DATE_H_INCLUDED

#include "types.h"

OCI_Date * OcilibDateInitialize
(
    OCI_Connection *con,
    OCI_Date       *date,
    OCIDate        *buffer,
    boolean         allocate,
    boolean         ansi
);

OCI_Date * OcilibDateCreate
(
    OCI_Connection *con
);

boolean OcilibDateFree
(
    OCI_Date* date
);

OCI_Date ** OcilibDateCreateArray
(
    OCI_Connection *con,
    unsigned int    nbelem
);

boolean OcilibDateFreeArray
(
    OCI_Date** dates
);

boolean OcilibDateAddDays
(
    OCI_Date* date,
    int       nb
);

boolean OcilibDateAddMonths
(
    OCI_Date* date,
    int       nb
);

boolean OcilibDateAssign
(
    OCI_Date* date,
    OCI_Date* date_src
);

int OcilibDateCheck
(
    OCI_Date* date
);

int OcilibDateCompare
(
    OCI_Date* date,
    OCI_Date* date2
);

int OcilibDateDaysBetween
(
    OCI_Date* date,
    OCI_Date* date2
);

boolean OcilibDateFromString
(
    OCI_Date   * date,
    const otext* str,
    const otext* fmt
);

boolean OcilibDateToString
(
    OCI_Date   * date,
    const otext* fmt,
    int          size,
    otext      * str
);

boolean OcilibDateGetDate
(
    OCI_Date* date,
    int     * year,
    int     * month,
    int     * day
);

boolean OcilibDateGetTime
(
    OCI_Date* date,
    int     * hour,
    int     * min,
    int     * sec
);

boolean OcilibDateGetDateTime
(
    OCI_Date* date,
    int     * year,
    int     * month,
    int     * day,
    int     * hour,
    int     * min,
    int     * sec
);

boolean OcilibDateLastDay
(
    OCI_Date* date
);

boolean OcilibDateNextDay
(
    OCI_Date   * date,
    const otext* day
);

boolean OcilibDateSetDate
(
    OCI_Date* date,
    int       year,
    int       month,
    int       day
);

boolean OcilibDateSetTime
(
    OCI_Date* date,
    int       hour,
    int       min,
    int       sec
);

boolean OcilibDateSetDateTime
(
    OCI_Date* date,
    int       year,
    int       month,
    int       day,
    int       hour,
    int       min,
    int       sec
);

boolean OcilibDateSysDate
(
    OCI_Date* date
);

boolean OcilibDateZoneToZone
(
    OCI_Date   * date,
    const otext* zone1,
    const otext* zone2
);

boolean OcilibDateToCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t   * pt
);

boolean OcilibDateFromCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t     t
);

#endif /* OCILIB_DATE_H_INCLUDED */
