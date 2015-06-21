---
author: admin
comments: true
date: 2010-12-14 22:29:29+00:00
layout: post
slug: ocilib-version-3-8-1-coming-on-december-the-16th
title: OCILIB version 3.8.1 coming on December the 16th !
wordpress_id: 594
categories:
- News
- Updates
---

Hi,

OCILIB version 3.8.1 will be finally  released in 2 days, on December the 16th !

**It only contains bug fixes. **

The SVN is up to date with this version.

Here is the current changelog :


    
    
    2010-12-13 Version 3.8.1 Vincent Rogier vince.rogier@ocilib.net
    
        * Miscellaneous fixes
    
          - Fixed internal computation of OCI_Object attributes null indicator offsets
          - Fixed OCI_Elem handle initialization by OCI_CollGetAt() and OCI_CollGetAt2()
          - Fixed OCI_Ping() : OCI symbol OCIPing() was not dynamically loaded if OCI_IMPORT_RUNTIME was used (default for precompiled MS Windows Dlls)
          - Fixed OCI_ConnectionCreate() : in case of an unsuccessfull attempt to create a connection, an OCI internal handle was not freed since v3.7.0 (-> memory leak)
          - Fixed OCI_LongWrite()   + OCI_CHARSET_WIDE charset mode : internal length passed to internal OCI calls was expressed in chars instead of bytes
          - Fixed OCI_TypeInfoGet() + OCI_TYF_TYPE : an Oracle error was raised when passing as type name a builtin system type like "SYS.RAW"
          - Fixed OCI_GetLastError() that could return NULL when errors occured in OCI_FetchXXX() calls (although the global error handler was correctly fired) 
          - Fixed OCI_DequeueGet() : a segfault happened if the queue payload was of type RAW 
          - Fixed OCI_DequeueFree() : internal structure member that hold the value set by OCI_DequeueSetConsumer() was not freed (memory leak)
          - Fixed OCI_MsgFree() : internal message ID allocated at enqueue time by OCI_EnqueuePut() was not freed (memory leak)
          - Fixed OCI_IterFree() : internal OCI_Elem handle was not freed for local collections resulting a small memory leak
          - Fixed OCI_EnqueuePut() and OCI_DequeueGet() : possible memory leak on Unix platforms + OCI_CHARSET_WIDE/OCI_CHARSET_MIXED charset mode
          - Fixed OCI_DequeueFree() : Internal OCI_Msg handle deallocation forgot to deallocate internal message ID resulting a small memory leak
          - Fixed OCI_SetPassword() and OCI_SetUserPassword() : in OCI_CHARSET_WIDE and OCI_CHARSET_MIXED builds, theses functions failed to change the password
          - Fixed OCI_LobRead2() and OCI_LobWrite2() that could block when using UTF8 through NLS_LANG environment variable
          - Fixed OCI_GetStruct() : structure padding was not handled properly
          - Fixed internal allocation of ROWID and UROWID internal buffers when using UTF8 through NLS_LANG environment variable
    
        * Miscellaneous changes
    
          - Added Exception type OCI_ERR_REBIND_BAD_DATATYPE if a rebinding call attempts to bind a datatype different than the initial one
          - Updated documentation about datatypes for rebinding
          - Added support for numeric subtypes in OCI_BindGetSubtype() + documentation update
          - Manual update of source code formatted with Uncrustify (wrong indentation of witch case and some variable initialization)
          - Pre built MS Windows 32/64bits Dlls are now built using MS Visual Studio 2010 Express (instead of MS Visual Studio 2008 Professional)
          - A MS Visual Studio 2010 Express solution and project has been added to the Windows package to rebuild the Dlls
    
    



