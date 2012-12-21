# Copyright (C) 2007 Vincent Rogier <vince_rogier@yahoo.fr>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

builtin(include, acocilib.m4)

AC_INIT()

AC_CONFIG_SRCDIR([src/library.c])
AC_CONFIG_AUX_DIR(config)
AM_CONFIG_HEADER(config.h)
AM_INIT_AUTOMAKE(ocilib, `cat VERSION`)

enable_dlopen=yes

AC_PROG_CC()
AC_PROG_INSTALL()
AC_PROG_LIBTOOL()
AC_PROG_RANLIB()

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

