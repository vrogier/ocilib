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

  demo_sources_files=`(cd ${srcdir:-.}/demo && ls -1 *.c *.h *.cpp *.sql)`
  OCILIB_DEMO_SOURCES=`echo $demo_sources_files | sed 's,$, ,g'`

  tests_files=`(cd ${srcdir:-.}/tests && ls -1 *.h *.cpp)`
  OCILIB_TESTS=`echo $tests_files | sed 's,$, ,g'`

  AC_SUBST(OCILIB_PREFIX)
  AC_SUBST(OCILIB_DOCS)
  AC_SUBST(OCILIB_DEMO_SOURCES)
  AC_SUBST(OCILIB_TESTS)
])

dnl ------------------------------------------------------------------------
dnl OCILIB_CHECK_CFLAG(flag, shell-variable-to-append-to)
dnl ------------------------------------------------------------------------
dnl Probe whether the selected C compiler accepts `flag'. The probe itself is
dnl compiled with -Werror so that compilers which merely *warn* about unknown
dnl -W options (gcc and clang both do) are correctly reported as rejecting it.
dnl On success the flag is appended to the named shell variable.
dnl

AC_DEFUN([OCILIB_CHECK_CFLAG],
[
  AC_MSG_CHECKING([whether $CC accepts $1])

  ocilib_check_saved_cflags="$CFLAGS"
  CFLAGS="-Werror $1"

  AC_COMPILE_IFELSE([AC_LANG_PROGRAM([])],
    [
      AC_MSG_RESULT([yes])
      CFLAGS="$ocilib_check_saved_cflags"
      $2="$$2 $1"
    ],
    [
      AC_MSG_RESULT([no])
      CFLAGS="$ocilib_check_saved_cflags"
    ]
  )
])

dnl ------------------------------------------------------------------------
dnl OCILIB_COMPILER_FLAGS()
dnl ------------------------------------------------------------------------
dnl

AC_DEFUN([OCILIB_COMPILER_FLAGS],
[
  OCILIB_EXTRA_FLAGS=""

  OCILIB_CHECK_CFLAG([-fvisibility=hidden], [OCILIB_EXTRA_FLAGS])

  AC_MSG_CHECKING([for OCILIB compiler specific flags])
  AC_MSG_RESULT([$OCILIB_EXTRA_FLAGS])

  AC_SUBST(OCILIB_EXTRA_FLAGS)
 
])

dnl ------------------------------------------------------------------------
dnl OCILIB_WARNINGS()
dnl ------------------------------------------------------------------------
dnl Diagnostics are organised in two tiers:
dnl
dnl   --enable-warnings (default: yes)
dnl       High-signal diagnostics that the tree is expected to stay clean of.
dnl       This is the set CI gates on. It includes -Wconversion and
dnl       -Wsign-conversion: this codebase is dense with narrowing casts
dnl       (size_t -> unsigned int, size_t -> int) and unsigned subtraction,
dnl       which is precisely the defect class worth catching, so they are
dnl       on by default despite the initial backlog they expose.
dnl
dnl   --enable-warnings=paranoid
dnl       Adds further audit-only diagnostics that are either noisy on system
dnl       and OCI headers or fire on constructs that are already correct.
dnl
dnl   --enable-werror (default: no)
dnl       Turn the selected diagnostics into errors. Intended for CI only;
dnl       never enable it in a build meant for distribution, because a newer
dnl       compiler inventing a new warning would break downstream builds.
dnl

AC_DEFUN([OCILIB_WARNINGS],
[
  AC_ARG_ENABLE(warnings,
  [  --enable-warnings=(no|yes|paranoid)
                          compiler diagnostics level (default: yes)],
  [ ac_ocilib_warnings=$enableval ], [ ac_ocilib_warnings="yes" ])

  AC_ARG_ENABLE(werror,
  [  --enable-werror         treat compiler warnings as errors (CI only)],
  [ ac_ocilib_werror=$enableval ], [ ac_ocilib_werror="no" ])

  OCILIB_WARN_FLAGS=""

  if test x"$ac_ocilib_warnings" != x"no"; then

    dnl -- baseline ------------------------------------------------------
    OCILIB_CHECK_CFLAG([-Wall],                    [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wextra],                  [OCILIB_WARN_FLAGS])

    dnl -- memory / bounds: catches the strncpy truncation and unchecked
    dnl    size arithmetic classes of defect --------------------------------
    OCILIB_CHECK_CFLAG([-Warray-bounds],           [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wstringop-overflow],      [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wstringop-truncation],    [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Walloca],                 [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wvla],                    [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wpointer-arith],          [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wcast-align],             [OCILIB_WARN_FLAGS])

    dnl -- narrowing and sign-changing conversions -----------------------
    dnl    The highest-value pair on this codebase: they flag the truncating
    dnl    casts (size_t -> unsigned int) and the unsigned subtractions that
    dnl    underflow, e.g. `length - offset' in src/stringutils.c.
    dnl    (-Wfloat-conversion is implied by -Wconversion, and -Wsign-compare
    dnl    by -Wextra, so neither is listed separately.)
    OCILIB_CHECK_CFLAG([-Wconversion],             [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wsign-conversion],        [OCILIB_WARN_FLAGS])

    dnl -- format strings ------------------------------------------------
    OCILIB_CHECK_CFLAG([-Wformat=2],               [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wformat-security],        [OCILIB_WARN_FLAGS])

    dnl -- declaration hygiene: -Wmissing-variable-declarations flags
    dnl    internal globals that should have been static or hidden --------
    OCILIB_CHECK_CFLAG([-Wstrict-prototypes],      [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wold-style-definition],   [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wmissing-prototypes],     [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wmissing-variable-declarations], [OCILIB_WARN_FLAGS])

    dnl -- general correctness -------------------------------------------
    OCILIB_CHECK_CFLAG([-Wshadow],                 [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wundef],                  [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Wwrite-strings],          [OCILIB_WARN_FLAGS])
    OCILIB_CHECK_CFLAG([-Winit-self],              [OCILIB_WARN_FLAGS])

    if test x"$ac_ocilib_warnings" = x"paranoid"; then
      OCILIB_CHECK_CFLAG([-Wcast-qual],            [OCILIB_WARN_FLAGS])
      OCILIB_CHECK_CFLAG([-Wdouble-promotion],     [OCILIB_WARN_FLAGS])
      OCILIB_CHECK_CFLAG([-Wnull-dereference],     [OCILIB_WARN_FLAGS])
      OCILIB_CHECK_CFLAG([-Wduplicated-cond],      [OCILIB_WARN_FLAGS])
      OCILIB_CHECK_CFLAG([-Wduplicated-branches],  [OCILIB_WARN_FLAGS])
      OCILIB_CHECK_CFLAG([-Wlogical-op],           [OCILIB_WARN_FLAGS])
      dnl These two fire on system/OCI headers and on switches that already
      dnl have a default:, so they are audit-only rather than CI-gating.
      OCILIB_CHECK_CFLAG([-Wredundant-decls],      [OCILIB_WARN_FLAGS])
      OCILIB_CHECK_CFLAG([-Wswitch-enum],          [OCILIB_WARN_FLAGS])
    fi

    dnl OCI headers themselves are not warning-clean and are outside our
    dnl control, so keep a couple of unavoidable ones quiet.
    OCILIB_CHECK_CFLAG([-Wno-unused-parameter],    [OCILIB_WARN_FLAGS])

    if test x"$ac_ocilib_werror" = x"yes"; then
      OCILIB_CHECK_CFLAG([-Werror],                [OCILIB_WARN_FLAGS])
    fi

  fi

  AC_MSG_CHECKING([for OCILIB warning flags])
  AC_MSG_RESULT([$ac_ocilib_warnings (werror: $ac_ocilib_werror)])

  AC_SUBST(OCILIB_WARN_FLAGS)
])

dnl ------------------------------------------------------------------------
dnl OCILIB_SANITIZERS()
dnl ------------------------------------------------------------------------
dnl --enable-sanitizers[=list] builds the library with the given sanitizers
dnl (default list: address,undefined). Sanitizer flags must reach both the
dnl compile and the link step, so two substitutions are exported.
dnl
dnl Note: address and thread are mutually exclusive; ask for one or the other.
dnl

AC_DEFUN([OCILIB_SANITIZERS],
[
  AC_ARG_ENABLE(sanitizers,
  [  --enable-sanitizers[[=LIST]]
                          build with sanitizers (default LIST:
                          address,undefined)],
  [ ac_ocilib_sanitizers=$enableval ], [ ac_ocilib_sanitizers="no" ])

  OCILIB_SAN_CFLAGS=""
  OCILIB_SAN_LDFLAGS=""

  if test x"$ac_ocilib_sanitizers" = x"yes"; then
    ac_ocilib_sanitizers="address,undefined"
  fi

  if test x"$ac_ocilib_sanitizers" != x"no"; then

    ocilib_san_flag="-fsanitize=$ac_ocilib_sanitizers"

    dnl A compile-only probe would succeed even when the sanitizer runtime
    dnl library is missing, so this must be a *link* test with the flag in
    dnl both CFLAGS and LDFLAGS. -Werror is deliberately not used here: some
    dnl compilers warn about unused linker arguments during the probe.
    AC_MSG_CHECKING([whether $CC supports $ocilib_san_flag])

    ocilib_san_saved_cflags="$CFLAGS"
    ocilib_san_saved_ldflags="$LDFLAGS"
    CFLAGS="$CFLAGS $ocilib_san_flag"
    LDFLAGS="$LDFLAGS $ocilib_san_flag"

    AC_LINK_IFELSE([AC_LANG_PROGRAM([], [])],
      [ ocilib_san_works=yes ],
      [ ocilib_san_works=no  ]
    )

    CFLAGS="$ocilib_san_saved_cflags"
    LDFLAGS="$ocilib_san_saved_ldflags"

    AC_MSG_RESULT([$ocilib_san_works])

    if test x"$ocilib_san_works" != x"yes"; then
      AC_MSG_ERROR([$CC cannot build and link with $ocilib_san_flag.
    Check that the matching sanitizer runtime is installed
    (e.g. libasan / libubsan for GCC, compiler-rt for Clang).])
    fi

    OCILIB_SAN_CFLAGS="$ocilib_san_flag"
    OCILIB_SAN_LDFLAGS="$ocilib_san_flag"

    dnl Keep frame pointers and line info so sanitizer reports are readable.
    OCILIB_CHECK_CFLAG([-fno-omit-frame-pointer],       [OCILIB_SAN_CFLAGS])
    OCILIB_CHECK_CFLAG([-fno-optimize-sibling-calls],   [OCILIB_SAN_CFLAGS])
    OCILIB_CHECK_CFLAG([-g],                            [OCILIB_SAN_CFLAGS])
  fi

  AC_MSG_CHECKING([for OCILIB sanitizers])
  AC_MSG_RESULT([$ac_ocilib_sanitizers])

  AC_SUBST(OCILIB_SAN_CFLAGS)
  AC_SUBST(OCILIB_SAN_LDFLAGS)
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
