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

OCI_Date * DateInit
(
    OCI_Connection *con,
    OCI_Date       *date,
    OCIDate        *buffer,
    boolean         allocate,
    boolean         ansi
);


OCI_Date * DateCreate
(
    OCI_Connection *con
);

boolean DateFree
(
    OCI_Date* date
);


OCI_Date ** DateArrayCreate
(
    OCI_Connection *con,
    unsigned int    nbelem
);

boolean DateArrayFree
(
    OCI_Date** dates
);


boolean DateAddDays
(
    OCI_Date* date,
    int       nb
);

boolean DateAddMonths
(
    OCI_Date* date,
    int       nb
);


boolean DateAssign
(
    OCI_Date* date,
    OCI_Date* date_src
);

int DateCheck
(
    OCI_Date* date
);

int DateCompare
(
    OCI_Date* date,
    OCI_Date* date2
);

int DateDaysBetween
(
    OCI_Date* date,
    OCI_Date* date2
);

boolean DateFromText
(
    OCI_Date* date,
    const otext* str,
    const otext* fmt
);

boolean DateGetDate
(
    OCI_Date* date,
    int* year,
    int* month,
    int* day
);

boolean DateGetTime
(
    OCI_Date* date,
    int* hour,
    int* min,
    int* sec
);

boolean DateGetDateTime
(
    OCI_Date* date,
    int* year,
    int* month,
    int* day,
    int* hour,
    int* min,
    int* sec
);

boolean DateLastDay
(
    OCI_Date* date
);

boolean DateNextDay
(
    OCI_Date* date,
    const otext* day
);

boolean DateSetDate
(
    OCI_Date* date,
    int       year,
    int       month,
    int       day
);

boolean DateSetTime
(
    OCI_Date* date,
    int       hour,
    int       min,
    int       sec
);

boolean DateSetDateTime
(
    OCI_Date* date,
    int       year,
    int       month,
    int       day,
    int       hour,
    int       min,
    int       sec
);

boolean DateSysDate
(
    OCI_Date* date
);

boolean DateToText
(
    OCI_Date* date,
    const otext* fmt,
    int          size,
    otext* str
);

boolean DateZoneToZone
(
    OCI_Date* date,
    const otext* zone1,
    const otext* zone2
);

boolean DateToCTime
(
    OCI_Date* date,
    struct tm* ptm,
    time_t* pt
);

boolean DateFromCTime
(
    OCI_Date* date,
    struct tm* ptm,
    time_t     t
);

#endif /* OCILIB_DATE_H_INCLUDED */
