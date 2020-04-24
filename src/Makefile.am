# Copyright (c) 2007-2020 Vincent ROGIER <vince.rogier@ocilib.net>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

AM_CPPFLAGS= @ORACLE_INCLUDES@ -I$(top_srcdir)/include

lib_LTLIBRARIES= libocilib.la

libocilib_la_LIBADD= @ORACLE_LIBADD@ 
libocilib_la_SOURCES=   \
    agent.c             \
    array.c             \
    bind.c              \
    callback.c          \
    collection.c        \
    column.c            \
    connection.c        \
    date.c              \
    define.c            \
    dequeue.c           \
    dirpath.c           \
    element.c           \
    enqueue.c           \
    error.c             \
    event.c             \
    exception.c         \
    file.c              \
    format.c            \
    handle.c            \
    hash.c              \
    interval.c          \
    iterator.c          \
    library.c           \
    list.c              \
    lob.c               \
    long.c              \
    memory.c            \
    message.c           \
    mutex.c             \
    number.c            \
    object.c            \
    ocilib.c            \
    pool.c              \
    queue.c             \
    reference.c         \
    resultset.c         \
    statement.c         \
    strings.c           \
    subscription.c      \
    thread.c            \
    threadkey.c         \
    timestamp.c         \
    transaction.c       \
    typeinfo.c

libocilib_la_CFLAGS= -D@OCILIB_IMPORT@ -D@OCILIB_CHARSET@ @ORACLE_LIBNAME@ 
libocilib_la_LDFLAGS= @OCILIB_LD_FLAG@  -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

include_HEADERS=                            \
    $(top_srcdir)/include/ocilib.h          \
    $(top_srcdir)/include/ocilib_api.h      \
    $(top_srcdir)/include/ocilib_compat.h   \
    $(top_srcdir)/include/ocilib_config.h   \
    $(top_srcdir)/include/ocilib_types.h    \
    $(top_srcdir)/include/ocilib.hpp        \
    $(top_srcdir)/include/ocilib_core.hpp   \
    $(top_srcdir)/include/ocilib_impl.hpp
    
noinst_HEADERS=     \
	agent.h         \
    array.h         \
    bind.h          \
    callback.h      \
    checks.h        \
    collection.h    \
    column.h        \
    connection.h    \
    date.h          \
    define.h        \
    defs.h          \
    dequeue.h       \
    dirpath.h       \
    element.h       \
    enqueue.h       \
    error.h         \
    event.h         \
    exception.h     \
    file.h          \
    format.h        \
    handle.h        \
    hash.h          \
    import.h        \
    interval.h      \
    iterator.h      \
    library.h       \
    list.h          \
    loader.h        \
    lob.h           \
    long.h          \
    macro.h         \
    memory.h        \
    message.h       \
    mutex.h         \
    number.h        \
    object.h        \
    pool.h          \
    queue.h         \
    reference.h     \
    resultset.h     \
    statement.h     \
    strings.h       \
    subscription.h  \
    thread.h        \
    threadkey.h     \
    timestamp.h     \
    transaction.h   \
    typeinfo.h      \
    types.h         \
	oci/types.h     \
	oci/defs.h      \
	oci/api.h 

CLEANFILES = *~ 
