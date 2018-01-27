/*
 * OCILIB - C Driver for Oracle (C Wrapper for Oracle OCI)
 *
 * Website: http://www.ocilib.net
 *
 * Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
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

/* --------------------------------------------------------------------------------------------- *
 *
 * THIS DEMO RUNS ON ALL PLATFOMS AND OCILIB BUILDS.
 *
 * SO, WE NEED A LITTLE EFFORT TO KEEP THE SAME SOURCE CODE FOR :
 *
 *    - MS WINDOWS AND UNIXES
 *    - ANSI / WIDE BUILDS
 *
 * SOME DEFINES FOLLOW IN ORDER TO HANDLE ALL THOSE STUFF
 *
 * --------------------------------------------------------------------------------------------- */

/* --------------------------------------------------------------------------------------------- *
 * COMMON DEFINES
 * --------------------------------------------------------------------------------------------- */

#define ARG_DB     1
#define ARG_USER   2
#define ARG_PWD    3
#define ARG_HOME   4

#define ARG_COUNT  5

#define SIZE_STR   260
#define SIZE_BUF   2048
#define SIZE_TAB   5

#define SIZE_ARRAY 100
#define NB_LOAD    10
#define SIZE_COL1  20
#define SIZE_COL2  30
#define SIZE_COL3  8
#define NUM_COLS   3

#define ARRAY_COUNT(t) (sizeof(t)/sizeof(t[0]))

typedef struct test_t
{
        void (*proc)(void); /* test procedure */
        int    execute;     /* do execute the procedure ? */
}test_t;

/* --------------------------------------------------------------------------------------------- *
 * INCLUDES
 * --------------------------------------------------------------------------------------------- */

#include "ocilib.h"

/* --------------------------------------------------------------------------------------------- *
 * PLATFORMS
 * --------------------------------------------------------------------------------------------- */

#ifdef _MSC_VER

  #if defined(OCI_CHARSET_WIDE)
      #pragma comment(lib, "ocilibw.lib")
  #elif defined(OCI_CHARSET_ANSI)
      #pragma comment(lib, "ociliba.lib")
  #endif

#endif

#ifndef OCI_SHARED_LIB
    #if defined(_WINDOWS)
        #define OCI_SHARED_LIB                   "oci.dll"
    #elif defined(__APPLE__)
        #define OCI_SHARED_LIB                   "libclntsh.dylib"
    #elif defined(__hppa)
        #define OCI_SHARED_LIB                   "libclntsh.sl"
    #else
        #define OCI_SHARED_LIB                   "libclntsh.so"
    #endif
#endif

/* --------------------------------------------------------------------------------------------- *
 * STRING PRINTING
 * --------------------------------------------------------------------------------------------- */

#define print_frmt(f, x)    printf(f, x)
#define print_text(x)       printf(x)

#if defined(OCI_CHARSET_WIDE)
    #ifdef _WINDOWS
        #define print_ostr(x)   wprintf(OTEXT("%s"), x)
#else
        #define print_ostr(x)   printf("%ls", x)
#endif
#else
  #define print_ostr(x)   printf(OTEXT("%s"), x)
#endif

/* --------------------------------------------------------------------------------------------- *
 * PROGRAM ARGUMENTS
 * --------------------------------------------------------------------------------------------- */

#if defined(OCI_CHARSET_WIDE)

  #if defined(_MSC_VER)

    #define omain           wmain
    #define oarg            otext
    #define print_args(x)   wprintf(x)
    #define GET_ARG(s, i)   ostrncat(s, argv[i], sizeof(s))

  #else

    #define omain           main
    #define oarg            char
    #define print_args(x)   printf(x)
    #define GET_ARG(s, i)   mbstowcs(s, argv[i], sizeof(s))

  #endif

#else

  #define omain           main
  #define oarg            char
  #define print_args(x)   printf(x)
  #define GET_ARG(s, i)   strncat(s, argv[i], sizeof(s)-1)

#endif


