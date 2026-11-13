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

#include "log.h"

#include "stringutils.h"

#include "threading.h"

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _WINDOWS
  #include <sys/time.h>
  #include <pthread.h>
#endif

/* Native (otext) formatted output helpers. In MBCS builds otext is char,
   in wide builds otext is wchar_t, so we map to the matching CRT functions. */

#ifdef OCI_CHARSET_MBCS
#define OcilibLogSnprintf   snprintf
#define OcilibLogVsnprintf  vsnprintf
#define OcilibLogFputs      fputs
#else
#define OcilibLogSnprintf   swprintf
#define OcilibLogVsnprintf  vswprintf
#define OcilibLogFputs      fputws
#endif

#define OCILIB_LOG_BUFFER_SIZE  4096
#define OCILIB_LOG_PREFIX_SIZE   512
#define OCILIB_LOG_LINE_SIZE    (OCILIB_LOG_BUFFER_SIZE + OCILIB_LOG_PREFIX_SIZE)
#define OCILIB_LOG_UTF8_SIZE    (OCILIB_LOG_LINE_SIZE * 4)

#define OCILIB_LOG_COUNT(array) ((OCI_Size) (sizeof(array) / sizeof(otext)) - 1)

static OCI_Logger OcilibLogger;

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogIsEnabled
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLogIsEnabled
(
    OCI_LogCategory category,
    OCI_LogLevel    level
)
{
    /* Unlocked fast-path check. This is a benign race: the worst outcome is one
       extra or one missing line around a concurrent level change or cleanup. */

    return (OcilibLogger.initialized &&
        (OcilibLogger.categories & category) != 0 &&
        OcilibLogger.level >= level);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogGetLevelName
 * --------------------------------------------------------------------------------------------- */

const char * OcilibLogGetLevelName
(
    OCI_LogLevel level
)
{
    switch (level)
    {
        case OCI_LOG_ERROR: return "ERR";
        case OCI_LOG_WARN:  return "WRN";
        case OCI_LOG_INFO:  return "INF";
        case OCI_LOG_TRACE: return "TRC";
        case OCI_LOG_DEBUG: return "DBG";
        case OCI_LOG_NONE: return "NIL";
        default:            return "?";
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogGetCategoryName
 * --------------------------------------------------------------------------------------------- */

const char * OcilibLogGetCategoryName
(
    OCI_LogCategory category
)
{
    switch (category)
    {
        case OCI_LOG_CAT_LIB: return "LIB";
        case OCI_LOG_CAT_MEM: return "MEM";
        case OCI_LOG_CAT_OCI: return "OCI";
        case OCI_LOG_CAT_ALL: return "ALL";
        default:              return "?";
    }
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogStrEquals
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLogStrEquals
(
    const char *value,
    const char *name
)
{
    if (!value || !name)
    {
        return FALSE;
    }

    while (*value && *name && toupper((unsigned char) *value) == toupper((unsigned char) *name))
    {
        value++;
        name++;
    }

    return *value == 0 && *name == 0;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogGetTime
 * --------------------------------------------------------------------------------------------- */

static void OcilibLogGetTime
(
    OCI_Int32 *year,
    OCI_Int32 *month,
    OCI_Int32 *day,
    OCI_Int32 *hour,
    OCI_Int32 *min,
    OCI_Int32 *sec,
    OCI_Int32 *msec
)
{
#ifdef _WINDOWS

    SYSTEMTIME st;

    GetLocalTime(&st);

    *year  = (OCI_Int32) st.wYear;
    *month = (OCI_Int32) st.wMonth;
    *day   = (OCI_Int32) st.wDay;
    *hour  = (OCI_Int32) st.wHour;
    *min   = (OCI_Int32) st.wMinute;
    *sec   = (OCI_Int32) st.wSecond;
    *msec  = (OCI_Int32) st.wMilliseconds;

#else

    struct timeval tv;
    struct tm     *tm = NULL;

    *msec = 0;

    if (gettimeofday(&tv, NULL) == 0)
    {
        tm    = localtime(&tv.tv_sec);
        *msec = (OCI_Int32) (tv.tv_usec / 1000);
    }
    else
    {
        time_t now = time(NULL);
        tm = localtime(&now);
    }

    if (tm)
    {
        *year  = tm->tm_year + 1900;
        *month = tm->tm_mon + 1;
        *day   = tm->tm_mday;
        *hour  = tm->tm_hour;
        *min   = tm->tm_min;
        *sec   = tm->tm_sec;
    }
    else
    {
        *year = *month = *day = *hour = *min = *sec = 0;
    }

#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogOpenFile
 * --------------------------------------------------------------------------------------------- */

static FILE * OcilibLogOpenFile
(
    const otext *path
)
{
#if defined(OCI_CHARSET_MBCS)

    return fopen(path, "a");

#elif defined(_WINDOWS)

    return _wfopen(path, OTEXT("a"));

#else

    /* POSIX fopen() takes a MBCS path: convert with wcstombs() */

    char path_mb[OCI_SIZE_BUFFER * MB_LEN_MAX + 1];

    if (wcstombs(path_mb, path, sizeof(path_mb) - 1) == (OCI_Size) -1)
    {
        return NULL;
    }

    path_mb[sizeof(path_mb) - 1] = 0;

    return fopen(path_mb, "a");

#endif
}

/* The logger is process-global state shared by every OCI_Environment, so its
 * lock must be valid before the first environment exists and must stay valid
 * until after the last one is gone. It is therefore created exactly once via
 * call_once() and deliberately never destroyed: destroying it in
 * OcilibLogCleanup() would leave a concurrent OcilibLogWrite() locking freed
 * memory. One never-freed mtx_t is a fair price for that guarantee.
 *
 * The same lock guards both the sink (file/handler) and the logger state
 * (ref_count/level/categories), so initialize and cleanup are serialised
 * against each other and against writers.
 */

static mtx_t     logger_mutex;
static once_flag logger_mutex_once = ONCE_FLAG_INIT;

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogMutexCreate
 * --------------------------------------------------------------------------------------------- */

static void OcilibLogMutexCreate(void)
{
    mtx_init(&logger_mutex, mtx_plain);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogLock
 * --------------------------------------------------------------------------------------------- */

static void OcilibLogLock(void)
{
    call_once(&logger_mutex_once, OcilibLogMutexCreate);

    mtx_lock(&logger_mutex);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogUnlock
 * --------------------------------------------------------------------------------------------- */

static void OcilibLogUnlock(void)
{
    mtx_unlock(&logger_mutex);
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogInitializeUnlocked
 * --------------------------------------------------------------------------------------------- */

static OCI_Boolean OcilibLogInitializeUnlocked(void)
{
    if (OcilibLogger.ref_count > 0 || OcilibLogger.initialized)
    {
        OcilibLogger.ref_count++;
        return TRUE;
    }

    OcilibLogger.level          = OCI_LOG_NONE;
    OcilibLogger.categories     = OCI_LOG_CAT_ALL;
    OcilibLogger.file           = NULL;
    OcilibLogger.file_owned     = FALSE;
    OcilibLogger.initialized    = FALSE;
    OcilibLogger.output_path[0] = 0;
    OcilibLogger.ref_count      = 0;

    const char *level_str  = getenv("OCILIB_LOG");
    const char *cat_str    = getenv("OCILIB_LOG_CATEGORIES");
    const char *output_str = getenv("OCILIB_LOG_OUTPUT");

    if (!level_str || level_str[0] == 0)
    {
        OcilibLogger.ref_count = 1;
        return TRUE;
    }

    if      (OcilibLogStrEquals(level_str, "ERROR")) OcilibLogger.level = OCI_LOG_ERROR;
    else if (OcilibLogStrEquals(level_str, "WARN"))  OcilibLogger.level = OCI_LOG_WARN;
    else if (OcilibLogStrEquals(level_str, "INFO"))  OcilibLogger.level = OCI_LOG_INFO;
    else if (OcilibLogStrEquals(level_str, "TRACE")) OcilibLogger.level = OCI_LOG_TRACE;
    else if (OcilibLogStrEquals(level_str, "DEBUG")) OcilibLogger.level = OCI_LOG_DEBUG;

    if (OcilibLogger.level == OCI_LOG_NONE)
    {
        OcilibLogger.ref_count = 1;
        return TRUE;
    }

    if (cat_str)
    {
        char  cat_buf[128];
        char *token, *next;

        snprintf(cat_buf, sizeof(cat_buf), "%s", cat_str);
        OcilibLogger.categories = 0;

        for (token = cat_buf; token && *token; token = next)
        {
            next = strchr(token, ',');
            if (next) { *next = 0; next++; }
            while (*token == ' ') token++;
            if      (OcilibLogStrEquals(token, "LIB")) OcilibLogger.categories |= OCI_LOG_CAT_LIB;
            else if (OcilibLogStrEquals(token, "MEM")) OcilibLogger.categories |= OCI_LOG_CAT_MEM;
            else if (OcilibLogStrEquals(token, "OCI")) OcilibLogger.categories |= OCI_LOG_CAT_OCI;
            else if (OcilibLogStrEquals(token, "ALL")) OcilibLogger.categories |= OCI_LOG_CAT_ALL;
        }
    }

    if (output_str && output_str[0])
    {
        OcilibStringMbcsToNative(output_str, OcilibLogger.output_path, OCI_SIZE_BUFFER);

        if (OcilibLogStrEquals(output_str, "STDERR"))
        {
            OcilibLogger.file = stderr;
        }
        else if (OcilibLogStrEquals(output_str, "STDOUT"))
        {
            OcilibLogger.file = stdout;
        }
        else
        {
            OcilibLogger.file       = OcilibLogOpenFile(OcilibLogger.output_path);
            OcilibLogger.file_owned = (OcilibLogger.file != NULL);
        }
    }
    else
    {
        OcilibLogger.file = stderr;
    }

    OcilibLogger.initialized = TRUE;
    OcilibLogger.ref_count   = 1;

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogInitialize
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLogInitialize(void)
{
    OCI_Boolean res = FALSE;

    OcilibLogLock();

    res = OcilibLogInitializeUnlocked();

    OcilibLogUnlock();

    return res;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogCleanup
 * --------------------------------------------------------------------------------------------- */

OCI_Boolean OcilibLogCleanup(void)
{
    OcilibLogLock();

    if (OcilibLogger.ref_count > 0)
    {
        OcilibLogger.ref_count--;
    }

    if (OcilibLogger.ref_count == 0 && OcilibLogger.initialized)
    {
        if (OcilibLogger.file && OcilibLogger.file_owned)
        {
            fclose(OcilibLogger.file);
        }

        OcilibLogger.file       = NULL;
        OcilibLogger.file_owned = FALSE;

        /* logger_mutex is intentionally not destroyed: see its declaration */

        OcilibLogger.initialized = FALSE;
    }

    OcilibLogUnlock();

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogWrite
 * --------------------------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogGetThreadId
 * --------------------------------------------------------------------------------------------- */

static OCI_UInt64 OcilibLogGetThreadId
(
    void
)
{
#if defined(_WIN32)
    /* the numeric OS thread id — the same one shown by debuggers/tools */
    return (OCI_UInt64) GetCurrentThreadId();
#else
    return (OCI_UInt64) (OCI_Size) pthread_self();
#endif
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogWrite
 * --------------------------------------------------------------------------------------------- */

void OcilibLogWrite
(
    OCI_LogCategory category,
    OCI_LogLevel    level,
    const char     *function,
    const otext    *format,
    ...
)
{
    va_list args;

    otext prefix[OCILIB_LOG_PREFIX_SIZE];
    otext message[OCILIB_LOG_BUFFER_SIZE];
    otext line[OCILIB_LOG_LINE_SIZE];

    otext level_text[16];
    otext cat_text[16];
    otext func_text[256];

    OCI_Int32 year = 0, month = 0, day = 0;
    OCI_Int32 hour = 0, min = 0, sec = 0, msec = 0;
    OCI_Int32 chars;

    /* Unlocked fast-path gate.
       Everything that touches the sink below is done under the lock. */

    if (!OcilibLogIsEnabled(category, level))
    {
        return;
    }

    OcilibLogGetTime(&year, &month, &day, &hour, &min, &sec, &msec);

    OcilibStringMbcsToNative(OcilibLogGetLevelName(level),        level_text, OCILIB_LOG_COUNT(level_text));
    OcilibStringMbcsToNative(OcilibLogGetCategoryName(category),  cat_text,   OCILIB_LOG_COUNT(cat_text));
    OcilibStringMbcsToNative(function ? function : "",            func_text,  OCILIB_LOG_COUNT(func_text));

    OcilibLogSnprintf(prefix, OCILIB_LOG_COUNT(prefix),
                      OTEXT("%04d-%02d-%02d %02d:%02d:%02d.%03d [%s] [%s] [%llu] %s: "),
                      year, month, day, hour, min, sec, msec,
                      level_text, cat_text,
                      OcilibLogGetThreadId(), func_text);

    va_start(args, format);
    chars = OcilibLogVsnprintf(message, OCILIB_LOG_COUNT(message), format, args);
    va_end(args);

    if (chars < 0)
    {
        message[0] = 0;
    }

    chars = OcilibLogSnprintf(line, OCILIB_LOG_COUNT(line), OTEXT("%s%s\n"), prefix, message);

    if (chars < 0)
    {
        line[0] = 0;
    }

    OcilibLogLock();

    FILE *file = OcilibLogger.file ? OcilibLogger.file : stderr;

    OcilibLogFputs(line, file);
    fflush(file);

    OcilibLogUnlock();
}
