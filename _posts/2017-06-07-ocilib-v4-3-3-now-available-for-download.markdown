---
author: admin
comments: false
date: 2017-06-07 22:35:00+01:00
layout: post
slug: ocilib-v4-3-3-now-available-for-download
title: OCILIB v4.3.3 now available for download !
categories:
- News
- Updates
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.3.3 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>

### Fixes (C API)

- Issue #81: Fixed OCI_GetInt() on string columns 
    * Wrong implicit string to integer conversion (returning 0 whatever string value) occurred under following conditions: 
        - Column is string based
        - Oracle client < 12gR1
        - Using OCI_GetInt() or OCI_GetUnsignedInt()
- Issue #86: Fixed wrong error information when statement execution failed involving 64bit integer binds
    * It occurred under following conditions:     
        - Binding 64bit integer host variables with OCI_BDM_OUT flag set (default if OCI_BindSetDirection() has not been called)
        - Statement execution failed on the server for any reasons (server side error, constraint violation, ..) 
    * Real server side error information was not reported by OCI_GetLastError() or error callback
    * Instead an error "OCI-22060: argument [2] is an invalid or uninitialized number" was reported
- Issue #88: Fixed OCI_GetString() on BLOB columns with blob size > 1024 bytes
    * Filling the resulting string with BLOB data led to a buffer overflow 
- Issue #89: Fixed Segfault occurring in OCI_Execute()
    * The issue was a wrong offset computation when trying to update user strings after execution.
    * It occurred under following conditions:     
        - OCI_CHARSET_WIDE (Unicode) ocilib build 
        - Unix/Linux (wchar_t implemented as 32bit integers)
        - Binding host string variable with OCI_BDM_OUT flag set (default if OCI_BindSetDirection() has not been called)

### Fixes (C++ API)

- Issue #63: (Re)Added cstddef header in ocilib_core.hpp as on some platforms/compiler versions, it was reported as needed (including iterator header was not enough)

### Enhancements (C API)

- Issue #84: Add support for numeric type in OCI_ColumnGetSubType() with limitations as real numeric type cannot really be identified


		

<br/>

Vincent

