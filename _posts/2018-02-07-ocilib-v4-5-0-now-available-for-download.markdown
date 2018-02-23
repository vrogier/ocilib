---
author: admin
comments: false
date: 2018-02-07 10:00:00+01:00
layout: post
slug: ocilib-v4-5-0-now-available-for-download
title: OCILIB v4.5.0 now available for download !
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.5.0 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>
### Enhancements (C and C++ API)

- Extended support for non final UTDs
    * Added OCI_TypeInfoGetSuperType() and ocilib::TypeInfo::GetSuperType()
    * Added OCI_TypeInfoIsFinalType() and ocilib::TypeInfo::IsFinalType()
    * Added C and C++ demos about super types and sub types    

### Fixes (C API)
    
- Issue #132, #133, #135: Fixed segfault while accessing to derived UTDs members (due to wrong members offset computation)
- Issue #134: OCI_ObjectToText() and OCI_CollToText() were mangling conversion errors leading OCI_GetLastError() to not report conversion errors         
- Issue #136: OCI_RegisterSubscription() was not correctly setting port number if provided
- Issue #137: BINARY_DOUBLE and BINARY_FLOAT values could not be set or retrieved from OCI_Elem and OCI_Object
- Issue #139: OCI_ErrorGetInternalCode() may report wrong OCI_ERR_* value

### Miscellaneous

- Refactored internal numeric types conversions
- Removed usage of deprecated symbols
- Some code cleanup

<br/>

Vincent

