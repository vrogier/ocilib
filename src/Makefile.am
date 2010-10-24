# Copyright (C) 2007 Vincent Rogier <vince_rogier@yahoo.fr>
#  
# This file is free software; as a special exception the author gives
# unlimited permission to copy and/or distribute it, with or without 
# modifications, as long as this notice is preserved.
# 
# This program is distributed in the hope that it will be useful, but
# WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
# implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

INCLUDES= @ORACLE_INCLUDES@ -I$(top_srcdir)/include

lib_LTLIBRARIES= libocilib.la

libocilib_la_LIBADD= @ORACLE_LIBADD@ 
libocilib_la_SOURCES=           \
	array.c         \
	bind.c          \
	callback.c     	\
	connection.c   	\
	define.c   	\
	exception.c  	\
	handle.c    	\
	iterator.c  	\
	lob.c     	\
	mutex.c   	\
	resultset.c  	\
	string.c     	\
	timestamp.c  	\
	collection.c  	\
	pool.c    	\
	element.c  	\
	file.c       	\
	hash.c      	\
	library.c   	\
	long.c    	\
	number.c  	\
	typeinfo.c     	\
	thread.c     	\
	transaction.c 	\
	column.c      	\
	date.c        	\
	error.c    	\
	format.c     	\
	interval.c  	\
	list.c      	\
	memory.c  	\
	object.c  	\
	statement.c  	\
	ref.c           \
	threadkey.c     \
	dirpath.c       \
	event.c         \
	subscription.c  \
	agent.c         \
	dequeue.c       \
	enqueue.c       \
	msg.c           \
	queue.c

libocilib_la_CFLAGS= -D@OCILIB_IMPORT@ -D@OCILIB_CHARSET@ @ORACLE_LIBNAME@ 
libocilib_la_LDFLAGS= @OCILIB_LD_FLAG@  -version-info $(LT_CURRENT):$(LT_REVISION):$(LT_AGE)

include_HEADERS= $(top_srcdir)/include/ocilib.h
noinst_HEADERS=  \
	ocilib_internal.h \
	ocilib_types.h \
	ocilib_checks.h \
	ocilib_defs.h \
	oci_types.h \
	oci_import.h \
	oci_loader.h \
	oci_defs.h \
	oci_api.h 

CLEANFILES = *~ 
