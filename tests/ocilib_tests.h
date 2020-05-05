#pragma once

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include "gtest/gtest.h"

#include <array>
#include <vector>
#include <string>

#ifdef _UNICODE
    #define OCI_CHARSET_WIDE
    #define TO_STRING std::to_wstring
    using ostring = std::wstring;
#else
    #define OCI_CHARSET_ANSI
    #define TO_STRING std::to_string
    using ostring = std::string;
#endif

#define OCI_API __stdcall
#include "../include/ocilib.h"

#define DBS OTEXT("")
#define USR OTEXT("usr")
#define PWD OTEXT("pwd")
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
