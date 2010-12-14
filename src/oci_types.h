/*
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                               OCILIB - C Driver for Oracle                              |
    |                                                                                         |
    |                                (C Wrapper for Oracle OCI)                               |
    |                                                                                         |
    |                              Website : http://www.ocilib.net                            |
    |                                                                                         |
    |             Copyright (c) 2007-2010 Vincent ROGIER <vince.rogier@ocilib.net>            |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             This library is free software; you can redistribute it and/or               |
    |             modify it under the terms of the GNU Lesser General Public                  |
    |             License as published by the Free Software Foundation; either                |
    |             version 2 of the License, or (at your option) any later version.            |
    |                                                                                         |
    |             This library is distributed in the hope that it will be useful,             |
    |             but WITHOUT ANY WARRANTY; without even the implied warranty of              |
    |             MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the GNU           |
    |             Lesser General Public License for more details.                             |
    |                                                                                         |
    |             You should have received a copy of the GNU Lesser General Public            |
    |             License along with this library; if not, write to the Free                  |
    |             Software Foundation, Inc., 675 Mass Ave, Cambridge, MA 02139, USA.          |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+

    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |                                       IMPORTANT NOTICE                                  |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
    |                                                                                         |
    |             THIS FILE CONTAINS CONSTANTS AND STRUCTURES DECLARATIONS THAT WERE          |
    |             PICKED UP FROM ORACLE PUBLIC HEADER FILES.                                  |
    |                                                                                         |
    |             SO THE CONTENT OF THIS FILE IS UNDER ORACLE COPYRIGHT AND THE               |
    |             DECLARATIONS REPRODUCED HERE ARE ORIGINALLY WRITTEN BY ORACLE               |
    |             CORPORATION.                                                                |
    |                                                                                         |
    |             THE AUTHOR OF OCILIB LIBRARY HAS NOT WRITTEN THE CONTENT OF THIS            |
    |             FILE AND HAS PARTIALLY INTEGRATED SOME ORACLE OCI DEFINITONS TO             |
    |             ALLOW COMPILATION OF THIS OPEN SOURCE LIBRARY WITHOUT HAVING                |
    |             ORACLE PRODUCTS INSTALLED ON DEVELOPMENT ENVIRONMENTS                       |
    |                                                                                         |
    +-----------------------------------------------------------------------------------------+
*/

/* --------------------------------------------------------------------------------------------- *
 * $Id: oci_types.h, v 3.8.1 2010-12-13 00:00 Vincent Rogier $
 * --------------------------------------------------------------------------------------------- */

#ifndef OCILIB_OCI_TYPES_H_INCLUDED
#define OCILIB_OCI_TYPES_H_INCLUDED

# define ORATYPES
# define SX_ORACLE
# define SX3_ORACLE

#ifndef ORASTDDEF
  # include <stddef.h>
  # define ORASTDDEF
#endif

#ifndef ORALIMITS
  # include <limits.h>
  # define ORALIMITS
#endif

#ifndef TRUE
  # define TRUE  1
  # define FALSE 0
#endif

#ifndef lint
typedef unsigned char ub1;
typedef   signed char sb1;
#else
  #define ub1 unsigned char
  #define sb1 signed char
#endif

#define UB1MAXVAL ((ub1)UCHAR_MAX)
#define UB1MINVAL ((ub1)        0)
#define SB1MAXVAL ((sb1)SCHAR_MAX)
#define SB1MINVAL ((sb1)SCHAR_MIN)
#define MINUB1MAXVAL ((ub1)  255)
#define MAXUB1MINVAL ((ub1)    0)
#define MINSB1MAXVAL ((sb1)  127)
#define MAXSB1MINVAL ((sb1) -127)

#ifndef lint
typedef unsigned short ub2;
typedef   signed short sb2;
#else
  #define ub2  unsigned short
  #define sb2  signed short
#endif

#define UB2MAXVAL ((ub2)USHRT_MAX)
#define UB2MINVAL ((ub2)        0)
#define SB2MAXVAL ((sb2) SHRT_MAX)
#define SB2MINVAL ((sb2) SHRT_MIN)
#define MINUB2MAXVAL ((ub2) 65535)
#define MAXUB2MINVAL ((ub2)     0)
#define MINSB2MAXVAL ((sb2) 32767)
#define MAXSB2MINVAL ((sb2)-32767)

#ifndef lint
typedef unsigned int ub4;
typedef   signed int sb4;
#else
  #define eb4 int
  #define ub4 unsigned int
  #define sb4 signed int
#endif

#define UB4MAXVAL ((ub4)UINT_MAX)
#define UB4MINVAL ((ub4)        0)
#define SB4MAXVAL ((sb4) INT_MAX)
#define SB4MINVAL ((sb4) INT_MIN)
#define MINUB4MAXVAL ((ub4) 4294967295)
#define MAXUB4MINVAL ((ub4)          0)
#define MINSB4MAXVAL ((sb4) 2147483647)
#define MAXSB4MINVAL ((sb4)-2147483647)

#define ORAXB8_DEFINED
#ifndef lint
  #ifndef __GNUC__
typedef unsigned _int64 oraub8;
typedef signed _int64 orasb8;
  #else
typedef unsigned long long oraub8;
typedef signed long long orasb8;
  #endif
typedef oraub8 ub8;
typedef orasb8 sb8;
#else
  # define ub8 oraub8
  # define sb8 orasb8
  # define oraub8 unsigned _int64
  # define orasb8 signed _int64
#endif /* !lint */

#define ORAUB8MINVAL    ((oraub8)0)
#define ORAUB8MAXVAL    ((oraub8)18446744073709551615)
#define ORASB8MINVAL    ((orasb8)-9223372036854775808)
#define ORASB8MAXVAL    ((orasb8) 9223372036854775807)

#define MAXORAUB8MINVAL ((oraub8)0)
#define MINORAUB8MAXVAL ((oraub8)18446744073709551615)
#define MAXORASB8MINVAL ((orasb8)-9223372036854775807)
#define MINORASB8MAXVAL ((orasb8) 9223372036854775807)

#define UB1BITS          CHAR_BIT
#define UB1MASK          ((1 << ((uword)CHAR_BIT)) - 1)

#ifdef lint
  # define oratext unsigned char
#else
typedef  unsigned char oratext;
#endif

#ifndef lint
typedef          char eb1;
typedef          short eb2;
typedef          int eb4;
#else
  # define         eb1      char
  # define         eb2      short
  # define         eb4      int
#endif

#define EB1MAXVAL      ((eb1)SCHAR_MAX)
#define EB1MINVAL      ((eb1)        0)
#define MINEB1MAXVAL   ((eb1)  127)
#define MAXEB1MINVAL   ((eb1)    0)
#define EB2MAXVAL      ((eb2) SHRT_MAX)
#define EB2MINVAL      ((eb2)        0)
#define MINEB2MAXVAL   ((eb2) 32767)
#define MAXEB2MINVAL   ((eb2)     0)
#define EB4MAXVAL      ((eb4) INT_MAX)
#define EB4MINVAL      ((eb4)        0)
#define MINEB4MAXVAL   ((eb4) 2147483647)
#define MAXEB4MINVAL   ((eb4)          0)

#ifndef lint
typedef         sb1 b1;
#else
  #define         b1 sb1
#endif
#define  B1MAXVAL  SB1MAXVAL
#define  B1MINVAL  SB1MINVAL

#ifndef lint
typedef         sb2 b2;
#else
  #define         b2 sb2
#endif
#define  B2MAXVAL  SB2MAXVAL
#define  B2MINVAL  SB2MINVAL

#ifndef lint
typedef         sb4 b4;
#else
  #define         b4 sb4
#endif
# define  B4MAXVAL  SB4MAXVAL
# define  B4MINVAL  SB4MINVAL

#if !defined(LUSEMFC)
  # ifdef lint
    #  define text unsigned char
  # else
typedef oratext text;
  # endif
#endif

#ifdef lint
  # define OraText unsigned char
#else
typedef oratext OraText;
#endif

#ifndef lint
typedef          int eword;
typedef unsigned int uword;
typedef   signed int sword;
#else
  #define eword int
  #define uword unsigned int
  #define sword signed int
#endif

#define  EWORDMAXVAL  ((eword) INT_MAX)
#define  EWORDMINVAL  ((eword)       0)
#define  UWORDMAXVAL  ((uword)UINT_MAX)
#define  UWORDMINVAL  ((uword)       0)
#define  SWORDMAXVAL  ((sword) INT_MAX)
#define  SWORDMINVAL  ((sword) INT_MIN)
#define  MINEWORDMAXVAL  ((eword)  2147483647)
#define  MAXEWORDMINVAL  ((eword)      0)
#define  MINUWORDMAXVAL  ((uword)  4294967295)
#define  MAXUWORDMINVAL  ((uword)           0)
#define  MINSWORDMAXVAL  ((sword)  2147483647)
#define  MAXSWORDMINVAL  ((sword) -2147483647)

#ifdef _WIN64

  #ifndef lint
typedef unsigned _int64 ubig_ora;
typedef   signed _int64 sbig_ora;
  #else
    #define ubig_ora unsigned _int64
    #define sbig_ora signed _int64
  #endif

  #define UBIG_ORAMAXVAL ((ubig_ora)_UI64_MAX)
  #define UBIG_ORAMINVAL ((ubig_ora)        0)
  #define SBIG_ORAMAXVAL ((sbig_ora) _I64_MAX)
  #define SBIG_ORAMINVAL ((sbig_ora) _I64_MIN)
  #define MINUBIG_ORAMAXVAL ((ubig_ora) 4294967295)
  #define MAXUBIG_ORAMINVAL ((ubig_ora)          0)
  #define MINSBIG_ORAMAXVAL ((sbig_ora) 2147483647)
  #define MAXSBIG_ORAMINVAL ((sbig_ora)-2147483647)

#else

  #ifndef lint
typedef unsigned long ubig_ora;
typedef   signed long sbig_ora;
  #else
    #define ubig_ora unsigned long
    #define sbig_ora signed long
  #endif

  #define UBIG_ORAMAXVAL ((ubig_ora)ULONG_MAX)
  #define UBIG_ORAMINVAL ((ubig_ora)        0)
  #define SBIG_ORAMAXVAL ((sbig_ora) LONG_MAX)
  #define SBIG_ORAMINVAL ((sbig_ora) LONG_MIN)
  #define MINUBIG_ORAMAXVAL ((ubig_ora) 4294967295)
  #define MAXUBIG_ORAMINVAL ((ubig_ora)          0)
  #define MINSBIG_ORAMAXVAL ((sbig_ora) 2147483647)
  #define MAXSBIG_ORAMINVAL ((sbig_ora)-2147483647)

#endif  /* _WIN64 */

#define UBIGORABITS      (UB1BITS * sizeof(ubig_ora))

#undef CONST
#define CONST const

#define dvoid void

typedef void (*lgenfp_t)( void );

#ifndef ORASYS_TYPES
  # include <sys/types.h>
  # define ORASYS_TYPES
#endif

#ifndef boolean
  # define boolean int
#endif

#ifdef sparc
  # define SIZE_TMAXVAL SB4MAXVAL
#else
  # define SIZE_TMAXVAL UB4MAXVAL
#endif

#define MINSIZE_TMAXVAL (size_t)4294967295

#if !defined(MOTIF) && !defined(LISPL)  && !defined(__cplusplus) && !defined(LUSEMFC)
typedef  oratext *string;
#endif

#ifndef lint
typedef unsigned short utext;
#else
  #define utext  unsigned short
#endif

#endif /* OCILIB_OCI_TYPES_H_INCLUDED */

