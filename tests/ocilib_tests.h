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
    using ostring = std::wstring;
    #define osprintf wsprintf
    #define ostrlen wcslen
#else
    #define TO_STRING std::to_string
    using ostring = std::string;
    #define osprintf sprintf
    #define ostrlen strlen
#endif

#ifdef _MSC_VER
#  define OCI_API __stdcall
#endif

#include "../include/ocilib.h"

#define DBSNAME OTEXT("FREE")
#define DBS OTEXT("192.168.0.11:1522/FREEPDB1")
#define USR OTEXT("usr")
#define PWD OTEXT("pwd")
#define SYS_USR OTEXT("sys")
#define SYS_PWD OTEXT("pwd")
#define HOME OTEXT("")
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

void ExecDML(ostring dml);
ostring ToUpper(ostring str);

OCI_Environment * GetEnvironment(POCI_ERROR err_handler);
ocilib::Environment& GetEnvironmentCpp();

