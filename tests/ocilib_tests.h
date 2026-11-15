#pragma once

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <atomic>

#define OCILIBPP_DEBUG_MEMORY_ENABLED
#define OCILIBPP_DEBUG_MEMORY_THROW

#include <ocilib.hpp>

#ifdef OCI_CHARSET_WIDE
    #define TO_STRING std::to_wstring
    using String = std::wstring;
    #define osprintf wsprintf
    #define ostrlen wcslen
#else
    #define TO_STRING std::to_string
    using String = std::string;
    #define osprintf sprintf
    #define ostrlen strlen
#endif

#ifdef _MSC_VER
#  define OCI_API __stdcall
#endif

#include "../include/ocilib.h"

/* Test configuration — loaded once at startup in main.cpp from these
   environment variables (all required; OCI_TEST_HOME may be set empty):
     OCI_TEST_DB       connection string  (e.g. localhost:1523/FREEPDB1)
     OCI_TEST_USR      application user
     OCI_TEST_PWD      application user password
     OCI_TEST_SYS_USR  sysdba user
     OCI_TEST_SYS_PWD  sysdba password
     OCI_TEST_DBSNAME  database/service name as reported by events
     OCI_TEST_HOME     Oracle client library dir ('' = PATH lookup) */
extern const otext* DBSNAME;
extern const otext* DBS;
extern const otext* USR;
extern const otext* PWD;
extern const otext* SYS_USR;
extern const otext* SYS_PWD;
extern const otext* HOME;
#define PWD_WRONG OTEXT("pwd_wrong")
#define ARRAY_SIZE 10
#define STRING_SIZE 20

#include "mutex"

using Mutex = std::recursive_mutex;
using Guard = std::lock_guard<::Mutex>;

struct Error
{
    OCI_ErrorCode Code;
    OCI_ErrorCategory Category;
};

using Errors = std::vector<Error>;

struct Context
{
    Errors Errs;
    ::Mutex Lock;
};


#ifndef _WINDOWS
#include <unistd.h>
#define Sleep(x) usleep(x * 1000);
#endif

void ExecDML(String dml);
String ToUpper(String str);

OCI_Environment * GetEnvironment(POCI_ERROR err_handler);
OCI_Environment * GetSpecificEnvironment(POCI_ERROR err_handler, OCI_UInt32 version);
ocilib::Environment& GetEnvironmentCpp();

