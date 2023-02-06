---
author: admin
comments: false
date: 2023-02-06 20:00:00+01:00
layout: post
slug: ocilib-v4-7-5-now-available-for-download
title: OCILIB v4.7.5 now available for download !
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.7.5 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>
### Changes (C API)
	
    - OCI_Cleanup() now returns FALSE if OCI_Initialize() has not been called
    - OCI_GetSQLVerb() handles some new verbs added with Oracle 21c
    - OCI_Execute() and related methods return now FALSE and report an OCI_Error for PL/SQL statements raising PL/SQL no_data_found exceptions
        * This revert behavior introduced in v4.3.1 for addressing issue #68
        * Motivation is to re-align on SQL*Plus behavior
    - OCI_GetOCIRuntimeVersion() and OCI_GetOCICompileVersion() returned integer values on 6 digits instead of 4 digits
        * Handles now  minor version using 2 digits (e.g. Oracle OCI version 19.16.0)
        * Use macros OCI_VER_MAJ, OCI_VER_MIN, OCI_VER_REV and OCI_VER_MAKE to do computations on versions
    - Very old Oracle OCI 8.0.x is now supported 
        - Up to now, OCILIB supported all Oracle versions from 8.1
        - Oracle 8.0 is now also supported for very old legacy systems (as Oracle 8.0 was released in mid 90's)
        - As OCI multi-threaded APIs were only available from 8.1, passing OCI_ENV_THREADED to OCI_Initialize() with Oracle 8.0 will generate an error:
            - OCILIB Error code : OCI_ERR_NOT_AVAILABLE 
            - OCILIB Error message : "Feature not available (Oracle 8.1 multihreading support)"		
			
### Fixes (C API)

    - Issue 297: Library compilation fails with MSVC 2022 version 17.2 (error C2220)
    - Issue 298: OCI_DirPathLoad() can never returns in case of internal load errors (infinite loop)
    - Issue 308: OCI_Execute() raises error OCI-22060 when binding host big_int variables with OCI_BDM_OUT in case the SQL or PL/SQL engine sets a NULL value
    - Issue 309: OCI_Execute() does not raise exceptions when executing PL/SQL statement raising no data was found PL/SQL exception
    - Issue 313: OCI_GetString() returns NULL for CLOB columns is the CLOB row value was filled with empty_clob()
    - Issue 314: OCI_BindSetCharsetForm() is not accessible since v4.7.0
    - ISsue 316: OCI_DequeueGet() returns messages with OCI_Object payloads that can have NULL properties while being NOT NULL when queued
    - ISsue 317: OCI_SubscriptionRegister() generates a segfault on failure

### Fixes (C++ API)
    
    - Issue 299: Performance issues with C++ API in multi-threaded environments
    - Issue 306: Compile error under GCC when accessing an ocilib::Collection item
	
### Miscellaneous
    
    - Updated compilers for MS Windows prebuilt binaries to VS2022 17.4.4
    - Internal code changes / refactoring
    - Some documentation updates
    - Added more tests in tests suite 
    - Fixed ocilib C demo makefile : compilation could failed on some Linux environments
 
<br/>

Vincent

