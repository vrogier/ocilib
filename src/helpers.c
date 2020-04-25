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

#include "helpers.h"

#include "collection.h"
#include "date.h"
#include "file.h"
#include "interval.h"
#include "lob.h"
#include "number.h"
#include "object.h"
#include "reference.h"
#include "timestamp.h"

/* --------------------------------------------------------------------------------------------- *
  * ExternalSubTypeToSQLType
  * --------------------------------------------------------------------------------------------- */

unsigned int ExternalSubTypeToSQLType
(
    unsigned int type,
    unsigned int subtype
)
{
    unsigned int res = OCI_UNKNOWN;

    switch (type)
    {
        case OCI_CDT_TIMESTAMP:
        {
            switch (subtype)
            {
                case OCI_TIMESTAMP:
                    res = SQLT_TIMESTAMP;
                    break;
                case OCI_TIMESTAMP_TZ:
                    res = SQLT_TIMESTAMP_TZ;
                    break;
                case OCI_TIMESTAMP_LTZ:
                    res = SQLT_TIMESTAMP_LTZ;
                    break;
            }
            break;
        }
        case OCI_CDT_INTERVAL:
        {
            switch (subtype)
            {
                case OCI_INTERVAL_YM:
                    res = SQLT_INTERVAL_YM;
                    break;
                case OCI_INTERVAL_DS:
                    res = SQLT_INTERVAL_DS;
                    break;
            }
            break;
        }
        case OCI_CDT_LOB:
        {
            switch (subtype)
            {
                case OCI_CLOB:
                case OCI_NCLOB:
                    res = SQLT_CLOB;
                    break;
                case OCI_BLOB:
                    res = SQLT_BLOB;
                    break;
            }
            break;
        }
        case OCI_CDT_FILE:
        {
            switch (subtype)
            {
                case OCI_CFILE:
                    res = SQLT_CFILE;
                    break;
                case OCI_BFILE:
                    res = SQLT_BFILE;
                    break;
            }
            break;
        }
        case OCI_CDT_LONG:
        {
            switch (subtype)
            {
                case OCI_CLONG:
                    res = SQLT_LNG;
                    break;
                case OCI_BLONG:
                    res = SQLT_LBI;
                    break;
            }
            break;
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * ExternalSubTypeToHandleType
 * --------------------------------------------------------------------------------------------- */

unsigned int ExternalSubTypeToHandleType
(
    unsigned int type,
    unsigned int subtype
)
{
    unsigned int res = OCI_UNKNOWN;

    switch (type)
    {
        case OCI_CDT_TIMESTAMP:
        {
            switch (subtype)
            {
                case OCI_TIMESTAMP:
                    res = OCI_DTYPE_TIMESTAMP;
                    break;
                case OCI_TIMESTAMP_TZ:
                    res = OCI_DTYPE_TIMESTAMP_TZ;
                    break;
                case OCI_TIMESTAMP_LTZ:
                    res = OCI_DTYPE_TIMESTAMP_LTZ;
                    break;
            }
            break;
        }
        case OCI_CDT_INTERVAL:
        {
            switch (subtype)
            {
                case OCI_INTERVAL_YM:
                    res = OCI_DTYPE_INTERVAL_YM;
                    break;
                case OCI_INTERVAL_DS:
                    res = OCI_DTYPE_INTERVAL_DS;
                    break;
            }
            break;
        }
    }

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * FreeObjectFromType
 * --------------------------------------------------------------------------------------------- */

boolean FreeObjectFromType(void* obj, unsigned int type)
{
    boolean res = FALSE;

    switch (type)
    {
        case OCI_CDT_NUMERIC:
        {
            res = NumberFree((OCI_Number*)obj);
            break;
        }
        case OCI_CDT_DATETIME:
        {
            res = DateFree((OCI_Date*)obj);
            break;
        }
        case OCI_CDT_LOB:
        {
            res = LobFree((OCI_Lob*)obj);
            break;
        }
        case OCI_CDT_FILE:
        {
            res = FileFree((OCI_File*)obj);
            break;
        }
        case OCI_CDT_OBJECT:
        {
            res = ObjectFree((OCI_Object*)obj);
            break;
        }
        case OCI_CDT_COLLECTION:
        {
            res = CollectionFree((OCI_Coll*)obj);
            break;
        }
        case OCI_CDT_TIMESTAMP:
        {
            res = TimestampFree((OCI_Timestamp*)obj);
            break;
        }
        case OCI_CDT_INTERVAL:
        {
            res = IntervalFree((OCI_Interval*)obj);
            break;
        }
        case OCI_CDT_REF:
        {
            res = ReferenceFree((OCI_Ref*)obj);
            break;
        }
    }

    return res;
}
