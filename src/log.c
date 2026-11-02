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

#include <ctype.h>
#include <limits.h>
#include <stdarg.h>
#include <stdio.h>
#include <stdlib.h>
#include <string.h>
#include <time.h>

#ifndef _WINDOWS
  #include <sys/time.h>
#endif

#if defined(OCI_CHARSET_WIDE) && (WCHAR_MAX == 0xFFFF)
#define OCILIB_LOG_UTF16 1
#else
#define OCILIB_LOG_UTF16 0
#endif

/* Native (otext) formatted output helpers. In ANSI builds otext is char,
   in wide builds otext is wchar_t, so we map to the matching CRT functions. */

#ifdef OCI_CHARSET_ANSI
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

#define OCILIB_LOG_COUNT(array) ((OCI_Int32) (sizeof(array) / sizeof(otext)) - 1)

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
        case OCI_LOG_ERROR: return "ERROR";
        case OCI_LOG_WARN:  return "WARN";
        case OCI_LOG_INFO:  return "INFO";
        case OCI_LOG_TRACE: return "TRACE";
        case OCI_LOG_DEBUG: return "DEBUG";
        case OCI_LOG_NONE: return "NONE";
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
 * OcilibLogAnsiToNative
 * --------------------------------------------------------------------------------------------- */

static void OcilibLogAnsiToNative
(
    const char *src,
    otext      *dst,
    OCI_Int32         max_chars
)
{
    OCI_Int32 i = 0;

    if (src && dst)
    {
        while (i < max_chars && src[i])
        {
            dst[i] = (otext) (unsigned char) src[i];
            i++;
        }

        dst[i] = 0;
    }
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
 * OcilibLogCodepointToUTF8
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibLogCodepointToUTF8
(
    OCI_UInt32 code,
    char        *dst,
    OCI_Int32          dst_size
)
{
    if (code < 0x80)
    {
        if (dst_size < 1) return 0;
        dst[0] = (char) code;
        return 1;
    }

    if (code < 0x800)
    {
        if (dst_size < 2) return 0;
        dst[0] = (char) (0xC0 | (code >> 6));
        dst[1] = (char) (0x80 | (code & 0x3F));
        return 2;
    }

    if (code < 0x10000)
    {
        if (dst_size < 3) return 0;
        dst[0] = (char) (0xE0 | (code >> 12));
        dst[1] = (char) (0x80 | ((code >> 6) & 0x3F));
        dst[2] = (char) (0x80 | (code & 0x3F));
        return 3;
    }

    if (code < 0x110000)
    {
        if (dst_size < 4) return 0;
        dst[0] = (char) (0xF0 | (code >> 18));
        dst[1] = (char) (0x80 | ((code >> 12) & 0x3F));
        dst[2] = (char) (0x80 | ((code >> 6) & 0x3F));
        dst[3] = (char) (0x80 | (code & 0x3F));
        return 4;
    }

    return 0;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogNativeToUTF8
 * --------------------------------------------------------------------------------------------- */

OCI_Int32 OcilibLogNativeToUTF8
(
    const otext *src,
    char        *dst,
    OCI_Int32          dst_size
)
{
    if (!src || !dst || dst_size <= 0)
    {
        return 0;
    }

#ifdef OCI_CHARSET_ANSI

    {
        OCI_Int32 len = (OCI_Int32) strlen(src);

        if (len > dst_size - 1)
        {
            len = dst_size - 1;
        }

        memcpy(dst, src, (OCI_Size) len);
        dst[len] = 0;

        return len;
    }

#else

    {
        OCI_Int32 i   = 0;
        OCI_Int32 pos = 0;

        while (src[i] != 0 && pos + 4 < dst_size)
        {
            OCI_UInt32 code;

  #if OCILIB_LOG_UTF16
            code = (OCI_UInt32) (OCI_UInt16) src[i];

            if (code >= 0xD800 && code <= 0xDBFF)
            {
                const OCI_UInt32 low = (OCI_UInt32) (OCI_UInt16) src[i + 1];

                if (low >= 0xDC00 && low <= 0xDFFF)
                {
                    code = 0x10000 + ((code - 0xD800) << 10) + (low - 0xDC00);
                    i++;
                }
            }
  #else
            code = (OCI_UInt32) src[i];
  #endif

            {
                const OCI_Int32 n = OcilibLogCodepointToUTF8(code, dst + pos, dst_size - pos);

                if (n == 0)
                {
                    break;
                }

                pos += n;
            }
            i++;
        }

        dst[pos] = 0;

        return pos;
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
#if defined(OCI_CHARSET_ANSI)

    return fopen(path, "a");

#elif defined(_WINDOWS)

    return _wfopen(path, OTEXT("a"));

#else

    /* POSIX fopen() takes a multibyte path: convert with wcstombs() */

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
    OcilibLogger.handler        = NULL;
    OcilibLogger.output_path[0] = 0;
    OcilibLogger.ref_count      = 0;

    {
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
            if      (OcilibLogStrEquals(cat_str, "LIB")) OcilibLogger.categories = OCI_LOG_CAT_LIB;
            else if (OcilibLogStrEquals(cat_str, "OCI")) OcilibLogger.categories = OCI_LOG_CAT_OCI;
        }

        if (output_str && output_str[0])
        {
            OcilibLogAnsiToNative(output_str, OcilibLogger.output_path, OCI_SIZE_BUFFER);

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
 * OcilibLogSetHandler
 * --------------------------------------------------------------------------------------------- */

void OcilibLogSetHandler
(
    POCI_LOG_HANDLER handler
)
{
    OcilibLogLock();

    OcilibLogger.handler = handler;

    OcilibLogUnlock();
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
        OcilibLogger.handler    = NULL;

        /* logger_mutex is intentionally not destroyed: see its declaration */

        OcilibLogger.initialized = FALSE;
    }

    OcilibLogUnlock();

    return TRUE;
}

/* --------------------------------------------------------------------------------------------- *
 * OcilibLogWrite
 * --------------------------------------------------------------------------------------------- */

void OcilibLogWrite
(
    OCI_Environment *env,
    OCI_LogCategory  category,
    OCI_LogLevel     level,
    const char      *function,
    const otext     *format,
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

    POCI_LOG_HANDLER handler = NULL;

    /* Unlocked fast-path gate.
       Everything that touches the sink below is done under the lock. */

    if (!OcilibLogIsEnabled(category, level))
    {
        return;
    }

    OcilibLogGetTime(&year, &month, &day, &hour, &min, &sec, &msec);

    OcilibLogAnsiToNative(OcilibLogGetLevelName(level),        level_text, OCILIB_LOG_COUNT(level_text));
    OcilibLogAnsiToNative(OcilibLogGetCategoryName(category),  cat_text,   OCILIB_LOG_COUNT(cat_text));
    OcilibLogAnsiToNative(function ? function : "",            func_text,  OCILIB_LOG_COUNT(func_text));

    OcilibLogSnprintf(prefix, OCILIB_LOG_COUNT(prefix),
                      OTEXT("%04d-%02d-%02d %02d:%02d:%02d.%03d [%s] [%s] %s: "),
                      year, month, day, hour, min, sec, msec,
                      level_text, cat_text, func_text);

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

    {
        FILE *file = OcilibLogger.file ? OcilibLogger.file : stderr;

        OcilibLogFputs(line, file);
        fflush(file);
    }

    /* Snapshot the handler under the lock, then invoke it *outside* the lock.
       logger_mutex is mtx_plain (non-recursive), so calling back into user code
       while holding it would deadlock the moment that code calls any OCILIB
       function that logs. The trade-off is that concurrent handler invocations
       are no longer serialised: a user handler must be thread safe. */

    handler = OcilibLogger.handler;

    OcilibLogUnlock();

    if (handler)
    {
#ifdef OCI_CHARSET_ANSI

        handler(env, level, category, line);

#else

        char utf8[OCILIB_LOG_UTF8_SIZE];

        OcilibLogNativeToUTF8(line, utf8, (OCI_Int32) sizeof(utf8) - 1);
        handler(env, level, category, utf8);

#endif
    }
}
