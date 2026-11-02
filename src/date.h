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

#ifndef OCILIB_DATE_H_INCLUDED
#define OCILIB_DATE_H_INCLUDED

#include "types.h"

OCI_SYM_LOCAL OCI_Date * OcilibDateInitialize
(
    OCI_Environment *env,
    OCI_Date        *date,
    OCIDate         *buffer,
    OCI_Boolean      allocate,
    OCI_Boolean      ansi
);

OCI_SYM_LOCAL OCI_Date * OcilibDateCreate
(
    OCI_Environment *env
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateFree
(
    OCI_Date* date
);

OCI_SYM_LOCAL OCI_Date ** OcilibDateCreateArray
(
    OCI_Environment *env,
    OCI_UInt32     nbelem
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateFreeArray
(
    OCI_Date** dates
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateAddDays
(
    OCI_Date* date,
    OCI_Int32       nb
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateAddMonths
(
    OCI_Date* date,
    OCI_Int32       nb
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateAssign
(
    OCI_Date* date,
    OCI_Date* date_src
);

OCI_SYM_LOCAL OCI_Int32 OcilibDateCheck
(
    OCI_Date* date
);

OCI_SYM_LOCAL OCI_Int32 OcilibDateCompare
(
    OCI_Date* date,
    OCI_Date* date2
);

OCI_SYM_LOCAL OCI_Int32 OcilibDateDaysBetween
(
    OCI_Date* date,
    OCI_Date* date2
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateFromString
(
    OCI_Date   * date,
    const otext* str,
    const otext* fmt
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateToString
(
    OCI_Date   * date,
    const otext* fmt,
    OCI_Int32          size,
    otext      * str
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateGetDate
(
    OCI_Date* date,
    OCI_Int32     * year,
    OCI_Int32     * month,
    OCI_Int32     * day
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateGetTime
(
    OCI_Date* date,
    OCI_Int32     * hour,
    OCI_Int32     * min,
    OCI_Int32     * sec
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateGetDateTime
(
    OCI_Date* date,
    OCI_Int32     * year,
    OCI_Int32     * month,
    OCI_Int32     * day,
    OCI_Int32     * hour,
    OCI_Int32     * min,
    OCI_Int32     * sec
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateLastDay
(
    OCI_Date* date
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateNextDay
(
    OCI_Date   * date,
    const otext* day
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateSetDate
(
    OCI_Date* date,
    OCI_Int32       year,
    OCI_Int32       month,
    OCI_Int32       day
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateSetTime
(
    OCI_Date* date,
    OCI_Int32       hour,
    OCI_Int32       min,
    OCI_Int32       sec
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateSetDateTime
(
    OCI_Date* date,
    OCI_Int32       year,
    OCI_Int32       month,
    OCI_Int32       day,
    OCI_Int32       hour,
    OCI_Int32       min,
    OCI_Int32       sec
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateSysDate
(
    OCI_Date* date
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateZoneToZone
(
    OCI_Date   * date,
    const otext* zone1,
    const otext* zone2
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateToCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t   * pt
);

OCI_SYM_LOCAL OCI_Boolean OcilibDateFromCTime
(
    OCI_Date * date,
    struct tm* ptm,
    time_t     t
);

#endif /* OCILIB_DATE_H_INCLUDED */
