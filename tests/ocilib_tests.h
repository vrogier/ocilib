#pragma once

#define _SILENCE_TR1_NAMESPACE_DEPRECATION_WARNING
#include "gtest/gtest.h"

#ifdef FALSE //_UNICODE
#define OCI_CHARSET_WIDE
using ostring = std::wstring;
#else
#define OCI_CHARSET_ANSI
using ostring = std::string; 
#endif

#define OCI_API __stdcall
#include "../include/ocilib.h"

#define DBS OTEXT("db")
#define USR OTEXT("usr")
#define PWD OTEXT("pwd")
#define PWD_WRONG OTEXT("pwd_wrong")
#define ARRAY_SIZE 10
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


#ifdef _MSC_VER

#if defined(OCI_CHARSET_WIDE)
#pragma comment(lib, "../lib64/ocilibw.lib")
#elif defined(OCI_CHARSET_ANSI)
#pragma comment(lib, "../lib64/ociliba.lib")
#endif
#endif

#ifndef _WINDOWS
#include <unistd.h>
#define Sleep(x) usleep(x * 1000);
#endif

