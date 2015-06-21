---
author: admin
comments: true
date: 2008-07-24 09:38:59+00:00
layout: post
slug: ocilib-251-patch-now-available
title: OCILIB 2.5.1 patch now available
wordpress_id: 34
categories:
- Updates
tags:
- Code
- patch
- release
---

A version 2.5.1 is now available !

This release is a patch for the 2.5.0 that came out few days agos with some broken functionnalities.

This updates contains 5 fixes, most of them related to dynamic runtime loading of OCI shared library.

Here is the list :



	
  * Fixed runtime loading of some OCI symbols (broken since v2.5.0)

	
  * Fixed runtime loading of OCI shared library on Unix-like platforms(broken since v2.5.0)

	
  * Fixed connection pooling emulation for Oracle 8i

	
  * Fixed Unicode strings binding to PL/SQL tables of char/varchar

	
  * Fixed Unicode internal buffer expansion for Unix-like platforms (where wchar_t is 4 bytes) that could mess up the data for string  arrays binds


OCILIB releases are always internally tested :

	
  * with Oracle 8i, 9i (R1&R2), 10gR2, 11gR1

	
  * on MS Windows Vista, Linux OpenSuse 10

	
  * All Configurations : ansi, mixed, unicode with dynamic linkage to OCI at compile time and dynamic loading at runtime (explicit dynamic loading of OCI library)


OCILIB is also tested by users before a release goes out.

For v2.5.0, tests were done on the wrong packages and few bugs had not been detected.

Sorry for the inconvience !

v2.5.1 corrects now all bugs introduced by v2.5.0 and passed all tests.

Have a nice day !
