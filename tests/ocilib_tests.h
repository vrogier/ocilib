#pragma once

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING

#include "gtest/gtest.h"

#include <array>
#include <vector>
#include <string>
#include <algorithm>
#include <atomic>

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

#define DBSNAME OTEXT("DB21C")
#define DBS OTEXT("DB21C_DEV")
#define USR OTEXT("usr")
#define PWD OTEXT("pwd")
#define SYS_USR OTEXT("sys")
#define SYS_PWD OTEXT("sys")
#define HOME OTEXT("")
#define PWD_WRONG OTEXT("pwd_wrong")
#define ARRAY_SIZE 10
#define STRING_SIZE 20
#define NLS_LANGUAGE_SUNDAY_NAME OTEXT("Dimanche")

#include "mutex"

using Mutex = std::recursive_mutex;
using Guard = std::lock_guard<Mutex>;

struct Error
{
    int LibCode;
    int OCICode;
};

using Errors = std::vector<Error>;

struct Context
{
    Errors Errs;
    Mutex Lock;
};


#ifndef _WINDOWS
#include <unistd.h>
#define Sleep(x) usleep(x * 1000);
#endif

void ExecDML(ostring dml);

inline ostring ToUpper(ostring str)
{
    std::for_each(std::begin(str), std::end(str), [](otext& c) { c = static_cast<otext>(::towupper(c)); });

    return str;
}
