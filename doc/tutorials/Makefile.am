# Copyright (c) 2007-2026 Vincent ROGIER <vince.rogier@ocilib.net>
#
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without
# modifications, as long as this notice is preserved.
#
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AUTOMAKE_OPTIONS = foreign
CLEANFILES = *~

# generated tutorial site -> share/doc/ocilib/tutorials/
docdir = $(prefix)/share/doc/$(PACKAGE)/tutorials
doc_DATA = @OCILIB_TUTORIALS@

tutassetsdir = $(docdir)/assets
tutassets_DATA = @OCILIB_TUTORIALS_ASSETS@

tutsqldir = $(tutassetsdir)/sql
tutsql_DATA = @OCILIB_TUTORIALS_SQL@

EXTRA_DIST = html assets
