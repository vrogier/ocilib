---
author: admin
comments: true
date: 2010-11-09 21:48:10+00:00
layout: post
slug: ocilib-version-3-8-1-about-to-be-released
title: OCILIB version 3.8.1 about to be released !
wordpress_id: 590
categories:
- News
- Updates
---

Hi,

A new release v3.8.1 is about to be released.
It only contains bug fixes. 
THe SVN is up to date with this version and packages are ready and will be uploaded for download at some point this week.

Here is the current changelog :


    
    
    2010-11-10 Version 3.8.1 Vincent Rogier vince.rogier@ocilib.net
    
        * Fixed UTF8 support using NLS_LANG environment variable set to any UTF8 values
    
          - Writing/reading lob using OCI_LobRead2() and OCI_LobWrite2()
          - Allocating internal memory for ROWID and UROWID
    
        * Miscellaneous fixes
    
          - Fixed internal computation of OCI_Object attributes null indicator offsets
          - Fixed OCI_Elem handle initialization by OCI_CollGetAt() and OCI_CollGetAt2()
          - Fixed OCI_Ping() : OCI symbol OCIPing() was not dynamically loaded if OCI_IMPORT_RUNTIME was used (default for precompiled MS Windows Dlls)
          - Fixed memory leak : in case of an unsuccessfull attempt to create a connection, an OCI internal handle was not freed since v3.7.0
          - Fixed OCI_LongWrite() + OCI_CHARSET_WIDE charset mode : internal length passed to internal OCI calls was expressed in chars instead of bytes
    
        * Miscellaneous changes
    
          - Added support for numeric subtypes in OCI_BindGetSubtype() + documentation update
          - Manual update of source code formatted with Uncrustify (wrong indentation of witch case and some variable initialization)
          - Pre built MS Windows 32/64bits Dlls are now built using MS Visual Studio 2010 Express (instead of MS Visual Studio 2008 Professional)
          - A MS Visual Studio 2010 Express solution and project has been added to the Windows package to rebuild the Dlls
    
    



