/*
   +----------------------------------------------------------------------+
   |                                                                      |
   |                     OCILIB - C Driver for Oracle                     |
   |                                                                      |
   |                      (C Wrapper for Oracle OCI)                      |
   |                                                                      |
   |                         DEMO HEADER FILE                             |
   |                                                                      |
   +----------------------------------------------------------------------+
   |                      Website : http://www.ocilib.net                 |
   +----------------------------------------------------------------------+
   |               Copyright (c) 2007-2010 Vincent ROGIER                 |
   +----------------------------------------------------------------------+
   | This library is free software; you can redistribute it and/or        |
   | modify it under the terms of the GNU Lesser General Public           |
   | License as published by the Free Software Foundation; either         |
   | version 2 of the License, or (at your option) any later version.     |
   |                                                                      |
   | This library is distributed in the hope that it will be useful,      |
   | but WITHOUT ANY WARRANTY; without even the implied warranty of       |
   | MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU    |
   | Lesser General Public License for more details.                      |
   |                                                                      |
   | You should have received a copy of the GNU Lesser General Public     |
   | License along with this library; if not, write to the Free           |
   | Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.   |
   +----------------------------------------------------------------------+
   |          Author: Vincent ROGIER <vince.rogier@gmail.com>             |
   +----------------------------------------------------------------------+
*/

/* ------------------------------------------------------------------------ *
 *
 * THIS DEMO RUNS ON ALL PLATFOMS AND OCILIB BUILDS.
 *
 * SO, WE NEED A LITTLE EFFORT TO KEEP THE SAME SOURCE CODE FOR :
 *
 *    - MS WINDOWS AND UNIXES
 *    - ANSI / MIXED / WIDE BUILDS
 *
 * SOME DEFINES FOLLOW IN ORDER TO HANDLE ALL THOSE STUFF
 *
 * ------------------------------------------------------------------------ */

/* ------------------------------------------------------------------------ *
 * COMMON DEFINES
 * ------------------------------------------------------------------------ */

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

/* ------------------------------------------------------------------------ *
 * PlATFORMS
 * ------------------------------------------------------------------------ */

#ifdef _WINDOWS

  #include "windows.h"

#endif

/* ------------------------------------------------------------------------ *
 * INCLUDES
 * ------------------------------------------------------------------------ */

#include "ocilib.h"

/* ------------------------------------------------------------------------ *
 * PLATFORMS
 * ------------------------------------------------------------------------ */

#ifdef _MSC_VER

  #if defined(OCI_CHARSET_WIDE)
      #pragma comment(lib, "ocilibw.lib")
  #elif defined(OCI_CHARSET_MIXED)
      #pragma comment(lib, "ocilibm.lib")
  #elif defined(OCI_CHARSET_ANSI)
      #pragma comment(lib, "ociliba.lib")
  #endif

#endif

#ifdef _WINDOWS
   #define EXE_NAME   "ocilib_demo.exe"
#else
   #define EXE_NAME   "ocilib_demo"
#endif

/* ------------------------------------------------------------------------ *
 * STRING PRINTING
 * ------------------------------------------------------------------------ */

#define print_text(x)       printf(x)
#define print_frmt(f, x)    printf(f, x)

#if defined(OCI_CHARSET_WIDE)

  #define print_mt  wprintf
  #define print_dt  wprintf
  #define sprint_mt swprintf
  #define sprint_dt swprintf

#elif defined(OCI_CHARSET_MIXED)

  #define print_mt  printf
  #define print_dt  wprintf
  #define sprint_mt ocisprintf
  #define sprint_dt swprintf

#else

  #define print_mt  printf
  #define print_dt  printf
  #define sprint_mt ocisprintf
  #define sprint_dt ocisprintf

#endif

/* print userdata text */

#if !defined(OCI_CHARSET_ANSI) && !defined(_WINDOWS)

#define print_dstr(x)   printf("%ls", (wchar_t*) x)

#else

  #define print_dstr(x)   print_dt(DT("%s"), x)

#endif

/* print metadata text */

#if defined(OCI_CHARSET_WIDE) && !defined(_WINDOWS)

  #define print_mstr(x)   printf("%ls", x)

#else

  #define print_mstr(x)   print_mt(MT("%s"), x)

#endif

/* ------------------------------------------------------------------------ *
 * PROGRAM ARGUMENTS
 * ------------------------------------------------------------------------ */

#if defined(OCI_CHARSET_WIDE)

  #if defined(_MSC_VER)

    #define mtmain          wmain
    #define mtarg           mtext
    #define print_args(x)   print_mt(x)
    #define GET_ARG(s, i)   mtsncat(s, argv[i], sizeof(s))

  #else

    #define mtmain          main
    #define mtarg           char
    #define print_args(x)   print_mstr(x)
    #define GET_ARG(s, i)   mbstowcs(s, argv[i], sizeof(s))

  #endif

#else

  #define mtmain          main
  #define mtarg           char
  #define print_args(x)   print_text(x)
  #define GET_ARG(s, i)   strncat(s, argv[i], sizeof(s))

#endif


