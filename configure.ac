# Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

builtin(include, acocilib.m4)

AC_INIT(ocilib, m4_esyscmd([tr -d '\n' < VERSION]))

m4_pattern_allow([LT_MAJOR])

AC_CONFIG_SRCDIR([src/library.c])
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AC_CONFIG_MACRO_DIR([m4])
AM_INIT_AUTOMAKE()

LT_INIT()

AC_PROG_CC()
AC_PROG_CC_STDC()
AC_PROG_INSTALL()
AC_LIBTOOL_DLOPEN()
AC_PROG_LIBTOOL()

OCILIB_PATH()
OCILIB_VERSION()
OCILIB_OPTIONS()
OCILIB_ORACLE()

AC_CONFIG_FILES([
   README
   ocilib.pc
   Makefile
   src/Makefile
   doc/html/Makefile
   demo/Makefile
   demo/Makefile_demo
 ])

AC_OUTPUT()

