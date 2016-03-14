/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2016 Vincent ROGIER <vince.rogier@ocilib.net>
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

#include "ocilib_internal.h"

/* ********************************************************************************************* *
 *                            STRINGS MESSAGES
 * ********************************************************************************************* */

static otext * OCILib_TypeNames[OCI_IPC_COUNT] =
{
    OTEXT("Oracle memory"),

    OTEXT("boolean pointer"),
    OTEXT("generic pointer"),
    OTEXT("short pointer"),
    OTEXT("int pointer"),
    OTEXT("big_int pointer"),
    OTEXT("double pointer"),
    OTEXT("float pointer"),
    OTEXT("string pointer"),
    OTEXT("function callback"),

    OTEXT("Error handle"),
    OTEXT("TypeInfo handle"),
    OTEXT("Connection handle"),
    OTEXT("Pool handle"),
    OTEXT("Transaction handle"),
    OTEXT("Statement handle"),
    OTEXT("Resultset handle"),
    OTEXT("Column handle"),
    OTEXT("Date handle"),
    OTEXT("Timestamp handle"),
    OTEXT("Interval handle"),
    OTEXT("Lob handle"),
    OTEXT("File handle"),
    OTEXT("Long handle"),
    OTEXT("Object handle"),
    OTEXT("Collection handle"),
    OTEXT("Collection iterator handle"),
    OTEXT("Collection element handle"),
    OTEXT("Hash Table handle"),
    OTEXT("Thread handle"),
    OTEXT("Mutex handle"),
    OTEXT("Bind handle"),
    OTEXT("Ref handle"),
    OTEXT("Direct Path handle"),
    OTEXT("Subscription handle"),
    OTEXT("Event handle"),
    OTEXT("Array handle"),
    OTEXT("Message handle"),
    OTEXT("Enqueue handle"),
    OTEXT("Dequeue handle"),
    OTEXT("Agent handle"),

    OTEXT("Internal list handle"),
    OTEXT("Internal list item handle"),
    OTEXT("Internal array of bind handles"),
    OTEXT("Internal define handle"),
    OTEXT("Internal array of define handles"),
    OTEXT("Internal hash entry handle"),
    OTEXT("Internal array of hash entry handles"),
    OTEXT("Internal hash value handle"),
    OTEXT("Internal thread key handle"),
    OTEXT("Internal Oracle date handle"),
    OTEXT("Internal C time structure"),
    OTEXT("Internal array of resultset handles"),
    OTEXT("Internal array of PL/SQL sizes integers"),
    OTEXT("Internal array of PL/SQL return codes integers"),
    OTEXT("Internal server output handle"),
    OTEXT("Internal array of indicator integers"),
    OTEXT("Internal array of buffer length integers"),
    OTEXT("Internal array of data buffers"),
    OTEXT("Internal Long handle data buffer"),
    OTEXT("Internal trace info structure"),
    OTEXT("Internal array of direct path columns"),
    OTEXT("Internal array of batch error objects")
};

#if defined(OCI_CHARSET_WIDE) && !defined(_MSC_VER)

static otext * OCILib_ErrorMsg[OCI_ERR_COUNT] =
{
    OTEXT("No error"),
    OTEXT("OCILIB has not been initialized"),
    OTEXT("Cannot load OCI shared library (%ls)"),
    OTEXT("Cannot load OCI symbols from shared library"),
    OTEXT("OCILIB has not been initialized in multi threaded mode"),
    OTEXT("Memory allocation failure (type %ls, size : %d)"),
    OTEXT("Feature not available (%ls) "),
    OTEXT("A null %ls has been provided"),
    OTEXT("Oracle data type (sql code %d) not supported for this operation "),
    OTEXT("Unknown identifier %c while parsing SQL"),
    OTEXT("Unknown argument %d while retrieving data"),
    OTEXT("Index %d out of bounds"),
    OTEXT("Found %d non freed %ls"),
    OTEXT("Maximum number of binds (%d) already reached"),
    OTEXT("Object attribute '%ls' not found"),
    OTEXT("The integer parameter value must be at least %d"),
    OTEXT("Elements are not compatible"),
    OTEXT("The statement must be %ls to perform this operation"),
    OTEXT("The statement is not scrollable"),
    OTEXT("Name or position '%ls' already binded to the statement"),
    OTEXT("Invalid new size for bind arrays (initial %d, current %d, new %d)"),
    OTEXT("Column '%ls' not find in table '%ls'"),
    OTEXT("Unable to perform this operation on a %ls direct path process"),
    OTEXT("Cannot create OCI environment"),
    OTEXT("Name or position '%ls' previously binded with different data type"),
    OTEXT("Object '%ls' type does not match the requested object type"),
    OTEXT("Item '%ls' (type %d)  not found"),
    OTEXT("Argument '%ls' : Invalid value %d")
};

#else

static otext * OCILib_ErrorMsg[OCI_ERR_COUNT] =
{
    OTEXT("No error"),
    OTEXT("OCILIB has not been initialized"),
    OTEXT("Cannot load OCI shared library (%s)"),
    OTEXT("Cannot load OCI symbols from shared library"),
    OTEXT("OCILIB has not been initialized in multi threaded mode"),
    OTEXT("Memory allocation failure (type %s, size : %d)"),
    OTEXT("Feature not available (%s) "),
    OTEXT("A null %s has been provided"),
    OTEXT("Oracle data type (sql code %d) not supported for this operation "),
    OTEXT("Unknown identifier %c while parsing SQL : "),
    OTEXT("Unknown argument %d while retrieving data"),
    OTEXT("Index %d out of bounds"),
    OTEXT("Found %d non freed %s"),
    OTEXT("Maximum number of binds (%d) already reached"),
    OTEXT("Object attribute '%s' not found"),
    OTEXT("The integer parameter value must be at least %d"),
    OTEXT("Elements are not compatible"),
    OTEXT("The statement must be %s to perform this operation"),
    OTEXT("The statement is not scrollable"),
    OTEXT("Name or position '%s' already binded to the statement"),
    OTEXT("Invalid new size for bind arrays (initial %d, current %d, new %d)"),
    OTEXT("Column '%s' not find in table '%s'"),
    OTEXT("Unable to perform this operation on a %s direct path process"),
    OTEXT("Cannot create OCI environment"),
    OTEXT("Name or position '%s' previously binded with different datatype"),
    OTEXT("Object '%s' type does not match the requested object type"),
    OTEXT("Item '%s' (type %d)  not found"),
    OTEXT("Argument '%s' : Invalid value %d")
};

#endif

static otext * OCILib_OraFeatures[OCI_FEATURE_COUNT] =
{
    OTEXT("Oracle 9.0 support for Unicode data"),
    OTEXT("Oracle 9.0 Timestamps and Intervals"),
    OTEXT("Oracle 9.2 Direct path date caching"),
    OTEXT("Oracle 9.2 Statement caching"),
    OTEXT("Oracle 10g R1 LOBs size extensions"),
    OTEXT("Oracle 10g R2 Database change notification"),
    OTEXT("Oracle 10g R2 remote database startup/shutdown"),
    OTEXT("Oracle 10g R2 High Availability"),
    OTEXT("Oracle XA Connections"),
    OTEXT("Oracle 12c R1 PL/SQL extended support")
};

typedef struct OCI_StmtStateTable
{
    int    state;
    otext *name;
} OCI_StmtStateTable;

static OCI_StmtStateTable OCILib_StmtStates[OCI_STMT_STATES_COUNT] =
{
    { OCI_STMT_CLOSED,    OTEXT("closed")        },
    { OCI_STMT_PARSED,    OTEXT("parsed")        },
    { OCI_STMT_PREPARED,  OTEXT("prepared")      },
    { OCI_STMT_DESCRIBED, OTEXT("described")     },
    { OCI_STMT_EXECUTED,  OTEXT("executed")      }
};

static otext * OCILib_DirPathStates[OCI_DPS_COUNT] =
{
    OTEXT("non prepared"),
    OTEXT("prepared"),
    OTEXT("converted"),
    OTEXT("terminated")
};

static otext * OCILib_HandleNames[OCI_HDLE_COUNT] =
{
    OTEXT("OCI handle"),
    OTEXT("OCI descriptors"),
    OTEXT("OCI Object handles")
};

/* ********************************************************************************************* *
 *                             PRIVATE FUNCTIONS
 * ********************************************************************************************* */

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionGetError
 * --------------------------------------------------------------------------------------------- */

OCI_Error * OCI_ExceptionGetError
(
    void
)
{
    OCI_Error *err = OCI_ErrorGet(TRUE);

    if (err)
    {
        OCI_ErrorReset(err);
    }
    return err;
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionRaise
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionRaise
(
    OCI_Error *err
)
{
    if (err)
    {
        err->active = TRUE;

        if (OCILib.error_handler)
        {
            OCILib.error_handler(err);
        }

        err->active = FALSE;
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionOCI
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionOCI
(
    OCIError       *p_err,
    OCI_Connection *con,
    OCI_Statement  *stmt,
    boolean         warning
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        int     dbsize = (int) (osizeof(err->str) - (size_t) 1);
        dbtext *dbstr  =  OCI_StringGetOracleString(err->str, &dbsize);

        err->type = (warning ? OCI_ERR_WARNING : OCI_ERR_ORACLE);
        err->con  = con;
        err->stmt = stmt;

        /* get oracle description */

        OCIErrorGet((dvoid *) p_err, (ub4) 1, (OraText *) NULL, &err->sqlcode,
                    (OraText *) dbstr, (ub4) dbsize, (ub4) OCI_HTYPE_ERROR);

        OCI_StringCopyOracleStringToNativeString(dbstr, err->str, dbcharcount(dbsize));
        OCI_StringReleaseOracleString(dbstr);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionNotInitialized
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionNotInitialized
(
    void
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_NOT_INITIALIZED;

        ostrncat(err->str,  OCILib_ErrorMsg[OCI_ERR_NOT_INITIALIZED], osizeof(err->str) - (size_t) 1);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionLoadingShareLib
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionLoadingSharedLib
(
    void
)
{
#ifdef OCI_IMPORT_RUNTIME

    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_LOADING_SHARED_LIB;

        osprintf(err->str, osizeof(err->str) - (size_t) 1,
                  OCILib_ErrorMsg[OCI_ERR_LOADING_SHARED_LIB],
                  OCI_DL_META_NAME);
    }

    OCI_ExceptionRaise(err);

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionLoadingSymbols
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionLoadingSymbols
(
    void
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_LOADING_SYMBOLS;

        ostrncat(err->str, OCILib_ErrorMsg[OCI_ERR_LOADING_SYMBOLS], osizeof(err->str) - (size_t) 1);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionNotMultithreaded
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionNotMultithreaded
(
    void
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_MULTITHREADED;

        ostrncat(err->str, OCILib_ErrorMsg[OCI_ERR_MULTITHREADED], osizeof(err->str) - (size_t) 1);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionNullPointer
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionNullPointer
(
    int type
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_NULL_POINTER;

        osprintf(err->str, osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_NULL_POINTER],
                 OCILib_TypeNames[type+1]);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionMemory
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionMemory
(
    int             type,
    size_t          nb_bytes,
    OCI_Connection *con,
    OCI_Statement  *stmt
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_MEMORY;
        err->con     = con;
        err->stmt    = stmt;

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_MEMORY],
                 OCILib_TypeNames[type+1],
                 nb_bytes);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionNotAvailable
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionNotAvailable
(
    OCI_Connection *con,
    int             feature
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_NOT_AVAILABLE;
        err->con     = con;

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_NOT_AVAILABLE],
                 OCILib_OraFeatures[feature-1]);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionDatatypeNotSupported
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionDatatypeNotSupported
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    int             code
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_DATATYPE_NOT_SUPPORTED;
        err->con     = con;
        err->stmt    = stmt;

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_DATATYPE_NOT_SUPPORTED],
                 code);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionParsingError
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionParsingToken
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    otext           token
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_PARSE_TOKEN;
        err->con     = con;
        err->stmt    = stmt;

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_PARSE_TOKEN],
                 token);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionMappingArgument
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionMappingArgument
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    int             arg
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_MAP_ARGUMENT;
        err->con     = con;
        err->stmt    = stmt;

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_MAP_ARGUMENT],
                 arg);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionOutOfBounds
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionOutOfBounds
(
    OCI_Connection *con,
    int             value
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_OUT_OF_BOUNDS;
        err->con     = con;

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_OUT_OF_BOUNDS],
                 value);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ExceptionUnfreedData
* --------------------------------------------------------------------------------------------- */

void OCI_ExceptionUnfreedData
(
    int type_elem,
    int nb_elem
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_UNFREED_DATA;

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_UNFREED_DATA],
                 nb_elem, OCILib_HandleNames[type_elem-1]);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionRuntimeLoading
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionMaxBind
(
    OCI_Statement *stmt
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_MAX_BIND;
        err->stmt    = stmt;

        if (stmt)
        {
            err->con = stmt->con;
        }

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_MAX_BIND],
                 OCI_BIND_MAX);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionAttributeNotFound
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionAttributeNotFound
(
    OCI_Connection *con,
    const otext    *attr
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_ATTR_NOT_FOUND;
        err->con     = con;

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_ATTR_NOT_FOUND],
                 attr);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionMinimumValue
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionMinimumValue
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    int             min
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_MIN_VALUE;
        err->con     = con;
        err->stmt    = stmt;

        osprintf(err->str, osizeof(err->str) - (size_t) 1, OCILib_ErrorMsg[OCI_ERR_MIN_VALUE], min);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionTypeNotCompatible
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionTypeNotCompatible
(
    OCI_Connection *con
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_NOT_COMPATIBLE;
        err->con     = con;

        ostrncat(err->str, OCILib_ErrorMsg[OCI_ERR_NOT_COMPATIBLE], osizeof(err->str) - (size_t) 1);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionStatementState
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionStatementState
(
    OCI_Statement *stmt,
    int            state
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        int i, index = 0;

        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_STMT_STATE;
        err->stmt    = stmt;

        if (stmt)
        {
            err->con = stmt->con;
        }

        for(i = 0; i < OCI_STMT_STATES_COUNT; i++)
        {
            if (state == OCILib_StmtStates[i].state)
            {
                index = i;
                break;
            }
        }

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_STMT_STATE],
                 OCILib_StmtStates[index].name);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionStatementNotScrollable
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionStatementNotScrollable
(
    OCI_Statement *stmt
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_STMT_NOT_SCROLLABLE;
        err->stmt    = stmt;

        if (stmt)
        {
            err->con = stmt->con;
        }

        ostrncat(err->str, OCILib_ErrorMsg[OCI_ERR_STMT_NOT_SCROLLABLE], osizeof(err->str) - (size_t) 1);

    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionBindAlreadyUsed
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionBindAlreadyUsed
(
    OCI_Statement *stmt,
    const otext  * bind
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_BIND_ALREADY_USED;
        err->stmt    = stmt;

        if (stmt)
        {
            err->con = stmt->con;
        }

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_BIND_ALREADY_USED],
                 bind);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionBindArraySize
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionBindArraySize
(
    OCI_Statement *stmt,
    unsigned int   maxsize,
    unsigned int   cursize,
    unsigned int   newsize
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_BIND_ARRAY_SIZE;
        err->stmt    = stmt;

        if (stmt)
        {
            err->con =  stmt->con;
        }

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_BIND_ARRAY_SIZE],
                 maxsize, cursize, newsize);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionDirPathColNotFound
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionDirPathColNotFound
(
    OCI_DirPath  *dp,
    const otext * column,
    const otext  *table
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_DIRPATH_STATE;
        err->stmt    = NULL;

        if (dp)
        {
            err->con =  dp->con;
        }

        osprintf(err->str,
                  osizeof(err->str) - (size_t) 1,
                  OCILib_ErrorMsg[OCI_ERR_COLUMN_NOT_FOUND],
                  column,
                  table);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionDirPathState
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionDirPathState
(
    OCI_DirPath *dp,
    int          state
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_DIRPATH_STATE;
        err->stmt  = NULL;

        if (dp)
        {
            err->con =  dp->con;
        }

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_DIRPATH_STATE],
                 OCILib_DirPathStates[state-1]);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionOCIEnvironment
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionOCIEnvironment
(
    void
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_CREATE_OCI_ENVIRONMENT;

        ostrncat(err->str,  OCILib_ErrorMsg[OCI_ERR_CREATE_OCI_ENVIRONMENT], osizeof(err->str) - (size_t) 1);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionRebindBadDatatype
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionRebindBadDatatype
(
    OCI_Statement *stmt,
    const otext  * bind
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_REBIND_BAD_DATATYPE;
        err->stmt    = stmt;

        if (stmt)
        {
            err->con = stmt->con;
        }

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_REBIND_BAD_DATATYPE],
                 bind);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
 * OCI_ExceptionTypeInfoWrongType
 * --------------------------------------------------------------------------------------------- */

void OCI_ExceptionTypeInfoWrongType
(
    OCI_Connection *con,
    const otext  * name
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_TYPEINFO_DATATYPE;
        err->stmt    = NULL;
        err->con     = con;

        osprintf(err->str,
                 osizeof(err->str) - (size_t) 1,
                 OCILib_ErrorMsg[OCI_ERR_TYPEINFO_DATATYPE],
                 name);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ExceptionItemNotFound
* --------------------------------------------------------------------------------------------- */

void OCI_ExceptionItemNotFound
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    const otext    *name,
    unsigned int    type
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_ITEM_NOT_FOUND;
        err->stmt    = stmt;
        err->con     = con;

        osprintf(err->str,
                 osizeof(err->str) - (size_t)1,
                 OCILib_ErrorMsg[OCI_ERR_ITEM_NOT_FOUND],
                 name, type);
    }

    OCI_ExceptionRaise(err);
}

/* --------------------------------------------------------------------------------------------- *
* OCI_ExceptionArgInvalidValue
* --------------------------------------------------------------------------------------------- */

void OCI_ExceptionArgInvalidValue
(
    OCI_Connection *con,
    OCI_Statement  *stmt,
    const otext    *name,
    unsigned int    value
)
{
    OCI_Error *err = OCI_ExceptionGetError();

    if (err)
    {
        err->type    = OCI_ERR_OCILIB;
        err->libcode = OCI_ERR_ITEM_NOT_FOUND;
        err->stmt    = stmt;
        err->con     = con;

        osprintf(err->str,
                 osizeof(err->str) - (size_t)1,
                 OCILib_ErrorMsg[OCI_ERR_ARG_INVALID_VALUE],
                 name, value);
    }

    OCI_ExceptionRaise(err);
}

