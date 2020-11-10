#pragma once

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include "gtest/gtest.h"

#include <array>
#include <vector>
#include <string>

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

#define OCI_API __stdcall
#include "../include/ocilib.h"

#define DBS OTEXT("localhost:1521/db19c")
#define USR OTEXT("usr")
#define PWD OTEXT("pwd")
#define SYS_USR OTEXT("sys")
#define SYS_PWD OTEXT("sys")
#define HOME OTEXT("")
#define PWD_WRONG OTEXT("pwd_wrong")
#define ARRAY_SIZE 10
#define STRING_SIZE 20
#define NLS_LANGUAGE_SUNDAY_NAME OTEXT("Sunday")

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
