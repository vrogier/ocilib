---
author: admin
comments: false
date: 2018-09-13 10:00:00+01:00
layout: post
slug: ocilib-v4-5-2-now-available-for-download
title: OCILIB v4.5.2 now available for download !
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.5.2 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>
### Fixes (C API)
    
        - Issue #152: Wrong error reporting when calling OCI_Execute() on a non prepared statement
        - Issue #153: OCI_GetSqlErrorPos() not working since v4.3.0
        - Issue #158: OCI_IntervalCompare() always returns 0 (equality)
        - Issue #159 (workaround for oracle Bug): OCI_TimestampAssign() returns ORA-00932 when timestamp type is OCI_DTYPE_TIMESTAMP_LTZ
        - Issue #161: Current offset not updated when calling OCI_LobTruncate()
        - Issue #162: OCI_RegisterObject() and OCI_RegisterRef() failure
        - Issue #164: OCI_RefSetNull() fails if already null or if its underlying object has not been pinned
        - OCI_IntervalArrayCreate() and OCI_TimestampArrayCreate() did not allow to create arrays if the connection handle was null

### Fixes (C++ API)
    
        - Issue #155: ocilib::Date::operator inverted operator >= and <=    

### Miscellaneous

        - Updated compilers for MS Windows pre built binaries
            * VS2017 15.8.3 for 32/64 bit DLLs
            * Code::Blocks 17.12 (TDM-GCC 5.1.0) for 32 bit MinGW static libs
        - Added Google Test suite for the C API 
            * Work In Progress
            * VS2017 (only) project using Google test nugets

<br/>

I am currently working on version 4.6.0 for supporting Oracle 18 new features :)

Vincent

