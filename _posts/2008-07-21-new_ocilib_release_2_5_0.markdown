---
author: admin
comments: true
date: 2008-07-21 09:50:24+00:00
layout: post
slug: new_ocilib_release_2_5_0
title: 'New OCILIB release : v2.5.0'
wordpress_id: 20
categories:
- Updates
tags:
- 2.5.0
- OCILIB
- package
- release
- Updates
---



A new OCILIB release is available !

Content :



	
  * Added support for PL/SQL tables (PL/SQL arrays)  when binding C (native and OCILIB types arrays) to PL/SQL block

	
  * Extended Date, timestamp and interval support : few more functions, added conversion to ISO C time datatypes and now these types can be created independtly from database connections

	
  * Extended OCI_Connection handle attributes : alive status and association of program user data

	
  * Added Support for retreiving server output : calls to dbms_output.put() and dbms_output.put_line() from inside PL/SQL blocks, triggers, procedures, packages, .. can be now retreived by OCILIB

	
  * Added Support for OCI Threadkeys : you can now have a process wide name identifier (key) that can have hold thread specific values (a bit like a getenv() and putenv() but values are stored by thread)

	
  * Extended runtime loading of Oracle shared libs on various Unix-like systems : some exotic sharted lib names (dylib, sh, ...) are now handled by the configure script and also managed for runtime time loading of Oracle shared libs

	
  * Added Support for Oracle Instant Client on Unix-like systems : 2 options have been added to the configure script in order to handle Instant Client packages correctly (differents packages for headers and libs + paths are different from regular clients)

	
  * Extended OCILIB documentation : added an installation page for all platforms and all examples have been checked out and extended to full C program source code

	
  * Added more demo source code : now, about 25 example source code ready to compile are provided

	
  * Miscelleanous modifications (see changelog)


[Download OCILIB **v2.5.0
**](http://sourceforge.net/project/showfiles.php?group_id=140360)

[Full 2.5.0 ChangeLog](http://sourceforge.net/project/shownotes.php?group_id=140360&release_id=614448)

Validated platforms :



	
  * Windows

	
  * Linux

	
  * HP/UX

	
  * Solaris

	
  * AIX

	
  * Mac OS X

	
  * OpenVMS


Supported Oracle versions : 8i, 9i, 10g, 11g


[ OCILIB project website](http://orclib.sourceforge.net/)




Get the latest package and enjoy it ! !
