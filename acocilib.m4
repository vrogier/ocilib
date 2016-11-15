dnl Copyright (C) 2007-2015 Vincent Rogier <vince.rogier@gmail.com>
dnl  
dnl This file is free software; as a special exception the author gives
dnl unlimited permission to copy and/or distribute it, with or without 
dnl modifications, as long as this notice is preserved.
dnl 
dnl This program is distributed in the hope that it will be useful, but
dnl WITHOUT ANY WARRANTY, to the extent permitted by law; without even the
dnl implied warranty of MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.

dnl acocilib.m4

dnl ------------------------------------------------------------------------
dnl Find a file (or one of more files in a list of dirs)
dnl ------------------------------------------------------------------------
dnl

AC_DEFUN([AC_FIND_FILE],
[
  $3=NO
  for i in $2;do
    for j in $1;  do
      if test -r "$i/$j"; then
        $3=$i
        break 2
      fi
    done
  done
]) 

dnl ------------------------------------------------------------------------
dnl OCILIB_PATH()
dnl ------------------------------------------------------------------------
dnl

AC_DEFUN([OCILIB_PATH],
[
  if test x"$prefix" = x"NONE"; then
    OCILIB_PREFIX=$ac_default_prefix
  else
    OCILIB_PREFIX=$prefix
  fi

  AC_MSG_CHECKING([for OCILIB install path])
  AC_SUBST(OCILIB_IMPORT)
  AC_MSG_RESULT([$OCILIB_PREFIX])

  doc_files=`(cd ${srcdir:-.}/doc/html && ls -1 *.html *.css *.png)`
  OCILIB_DOCS=`echo $doc_files | sed 's,$, ,g'`

  demo_sources_files=`(cd ${srcdir:-.}/demo/ && ls -1 *.c *.h *.cpp *.sql)`
  OCILIB_DEMO_SOURCES=`echo $demo_sources_files | sed 's,$, ,g'`

  AC_SUBST(OCILIB_PREFIX)
  AC_SUBST(OCILIB_DOCS)
  AC_SUBST(OCILIB_DEMO_SOURCES)

])

dnl ------------------------------------------------------------------------
dnl OCILIB_VERSION()
dnl ------------------------------------------------------------------------
dnl

AC_DEFUN([OCILIB_VERSION],
[
AC_MSG_CHECKING([for OCILIB version])

  OCILIB_VER=`cat VERSION`

  LT_MAJOR=`echo $OCILIB_VER | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\1/'`
  LT_AGE=`echo $OCILIB_VER | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\2/'`
  LT_REVISION=`echo $OCILIB_VER | sed 's/\([[0-9]]*\).\([[0-9]]*\).\([[0-9]]*\)/\3/'`
  LT_CURRENT=`expr $LT_MAJOR + $LT_AGE`

  AC_MSG_RESULT([v$OCILIB_VER ])

  AC_SUBST(LT_RELEASE)
  AC_SUBST(LT_CURRENT)
  AC_SUBST(LT_REVISION)
  AC_SUBST(LT_AGE)
])

dnl ------------------------------------------------------------------------
dnl OCILIB_OPTIONS()
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([OCILIB_OPTIONS],
[
  AC_MSG_CHECKING([for OCILIB options])

  AC_ARG_WITH(oracle_import,
  [  --with-oracle-import=(linkage|runtime)],
  [  ac_oracle_import=$withval ], [ ac_oracle_import="linkage" ])

  AC_ARG_WITH(oracle_charset,
  [  --with-oracle-charset=(ansi|wide)],
  [  ac_oracle_charset=$withval ], [ ac_oracle_charset="ansi" ])

  OCILIB_IMPORT="OCI_IMPORT_LINKAGE"
  OCILIB_CHARSET="OCI_CHARSET_ANSI"

  if test x"$ac_oracle_import" = x"linkage"; then
    OCILIB_IMPORT="OCI_IMPORT_LINKAGE"
  elif test x"$ac_oracle_import" = x"runtime" ; then 
    OCILIB_IMPORT="OCI_IMPORT_RUNTIME"
  else
    AC_MSG_ERROR([Invalid argument ($ac_oracle_import) for --with-oracle-import.])
  fi

  if test x"$ac_oracle_charset" = x"ansi"; then
    OCILIB_CHARSET="OCI_CHARSET_ANSI"
  elif test x"$ac_oracle_charset" = x"unicode" ; then 
    OCILIB_CHARSET="OCI_CHARSET_WIDE"
  elif test x"$ac_oracle_charset" = x"wide" ; then 
    OCILIB_CHARSET="OCI_CHARSET_WIDE"
  else
    AC_MSG_ERROR([Invalid argument ($ac_oracle_charset) for --with-oracle-charset.])
  fi

  AC_MSG_RESULT([import: $ac_oracle_import, charset = $ac_oracle_charset ])

  AC_SUBST(OCILIB_IMPORT)
  AC_SUBST(OCILIB_CHARSET)
])


dnl ------------------------------------------------------------------------
dnl OCILIB_ORACLE()
dnl ------------------------------------------------------------------------
dnl
AC_DEFUN([OCILIB_ORACLE],
[
	AC_ARG_WITH(oracle_home,
	[--with-oracle-home=Location of Oracle Installation.],
	[ac_oracle_home="$withval" ], [ ac_oracle_home="NO" ])

	AC_ARG_WITH(oracle_headers_path,
	[--with-oracle-headers-path=Location of OCI public headers.],
	[ac_headers_path="$withval" ], [ ac_headers_path="NO" ])

	AC_ARG_WITH(oracle_lib_path,
	[--with-oracle-lib-path=Location of OCI shared library.],
	[ac_oracle_lib_path="$withval" ], [ ac_oracle_lib_path="NO" ])

	AC_ARG_WITH(custom_loader,
	[--with-custom-loader=Custom loader flag.],
	[ac_ocilib_ld="$withval" ], [ ac_ocilib_ld="NO" ])

	ac_lib_full_path=NO
	ac_lib_dir=""
	ac_lib_ext=""
	ac_lib_flag=""
	ac_lib_pattern=""

	# find out the shared lib platform extension
	eval ac_lib_ext=\"$shrext_cmds\"

	# build the Oracle shared lib name
	ac_lib_name=libclntsh$ac_lib_ext

	#find out the Oracle shared lib link flag
	ac_lib_pattern="expr \"x$ac_lib_name\" : 'xlib\(.*\)$ac_lib_ext'"
	ac_lib_flag=`eval $ac_lib_pattern`

	#test for linkage mode
	if test x"$OCILIB_IMPORT" = x"OCI_IMPORT_LINKAGE"; then

	  AC_MSG_CHECKING([for Oracle home])

	  #if no Oracle home path provided, get it from environment
	  if test "$ac_oracle_home" = NO; then
		ac_oracle_home=`echo $ORACLE_HOME`
		AC_MSG_RESULT(defined by environment : [$ac_oracle_home])
	  else
		AC_MSG_RESULT(defined by option : [$ac_oracle_home])
	  fi

	  # find out the Oracle public OCI headers path
	  if test "$ac_headers_path" = NO; then
		if test "$ac_oracle_home" != NO; then  
		  test_include_path=$ac_oracle_home/rdbms/public    
		  if test -d "$test_include_path"; then    
		    ac_headers_path=$test_include_path    
		  else
		    test_include_path=$ac_oracle_home/rdbms/demo
		    if test -d "$test_include_path"; then     
		      ac_headers_path=$test_include_path      
		    fi  
		  fi
	    fi
	  fi

	  # find out the Oracle shared lib path
	  if test "$ac_oracle_lib_path" = NO; then
		if test "$ac_oracle_home" != NO; then
		  if test -d $ac_oracle_home/lib32; then
			  ac_lib_dir=lib32
		  else
			if test -d $ac_oracle_home/lib; then
			  ac_lib_dir=lib
			else
			  if test -d $ac_oracle_home/lib64; then
				ac_lib_dir=lib64
			  fi
			fi
		  fi
		  ac_oracle_lib_path=$ac_oracle_home/$ac_lib_dir
		fi
	  fi

	  AC_MSG_CHECKING([for Oracle OCI headers location])
	   
	  if test "$ac_headers_path" != NO; then
		if test -d $ac_headers_path; then
		  AC_MSG_RESULT([$ac_headers_path])
		else
		  AC_MSG_ERROR([path not found ($ac_headers_path)])
		fi
	  else
		AC_MSG_ERROR([variable not set or unable to guess])
	  fi

	  AC_MSG_CHECKING([for Oracle OCI shared lib location])

	  if test "$ac_oracle_lib_path" != NO; then
		AC_MSG_RESULT([$ac_oracle_lib_path])
	  else
		AC_MSG_ERROR([variable not set])
	  fi

	  AC_MSG_CHECKING([for Oracle OCI shared lib name])

	  if test "$ac_lib_name" != NO; then
		AC_MSG_RESULT([$ac_lib_name])
	  else
		AC_MSG_ERROR([variable not set])
	  fi

	  AC_MSG_CHECKING([for OCI shared lib existence])

	  AC_FIND_FILE($ac_lib_name, $ac_oracle_lib_path, ac_lib_full_path)

	  if test "$ac_lib_full_path" != NO; then
		AC_MSG_RESULT([yes])
	  else
		AC_MSG_ERROR([file not found])
	  fi

	  ORACLE_INCLUDES="-I$ac_headers_path"
	  ORACLE_LIBADD="-L$ac_oracle_lib_path -l$ac_lib_flag"
	  ORACLE_LIBNAME=""

	  OCILIB_LD_FLAG=""  
	else
	  #check for oracle share lib name to define
	  ORACLE_LIBNAME="-DOCI_DL="$ac_lib_name""

	  AC_MSG_CHECKING([for Oracle lib define ])
	  AC_MSG_RESULT([$ORACLE_LIBNAME])

	  #check for custom ld flags
	  if test "$ac_ocilib_ld" = NO; then
		ac_ocilib_ld="-ldl"
	  fi

	  AC_MSG_CHECKING([for loader linkage flag ])
	  AC_MSG_RESULT([$ac_ocilib_ld])

	  OCILIB_LD_FLAG=$ac_ocilib_ld
	fi

	AC_SUBST(ORACLE_HOME)
	AC_SUBST(ORACLE_INCLUDES)
	AC_SUBST(ORACLE_LIBADD)
	AC_SUBST(ORACLE_LIBNAME)

	AC_SUBST(OCILIB_LD_FLAG)
])
