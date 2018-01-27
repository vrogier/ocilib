# Copyright (c) 2007-2018 Vincent ROGIER <vince.rogier@ocilib.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

ACLOCAL_AMFLAGS = -I m4
EXTRA_DIST = doc/html reconf configure VERSION acocilib.m4
SUBDIRS = doc/html src demo
DISTCLEANFILES = *~ \#*\#
MAINTAINERCLEANFILES = \
Makefile Makefile.in config.cache config.log config.h.in \
libtool acinclude.m4 aclocal.m4 acocilib.m4 configure stamp-h.in

pkgconfigdir = $(libdir)/pkgconfig
pkgconfig_DATA = ocilib.pc
