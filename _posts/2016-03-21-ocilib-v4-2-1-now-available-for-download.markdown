---
author: admin
comments: false
date: 2016-03-21 21:00:00+00:00
layout: post
slug: ocilib-v4-2-1-now-available-for-download
title: OCILIB v4.2.1 now available for download !
categories:
- News
- Updates
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.2.1 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>

### Licensing
    
    * Re-licensing OCILIB to Apache License Version 2.0 instead of LGPL 2.1: this is merely for overcoming the static linkage issue of the LGPL V2.1      
   
### Fixes:

    * C++ API: Fixed memory leak caused by Statement::Bind() with non scalar types when Connection objects are closed before their child Statement objects get out of scope
    * C++ API: Fixed minor compilation warnings
    * C API: Compilation was broken when targeting Oracle 8i
    * C API: PL/SQL statements starting with keyword "CALL" where not identified as PL/SQL commands and thus output bind buffers were not updated accordingly
    * C API: OCI_Execute() ignored custom fetch and prefetch values with Oracle clients >= 9.2
    * C API: OCI_ElemSetString() caused memory retention when dealing repetitively with string attributes
    * C API: OCI_Cleanup() leaked of few bytes
    * C API: OCI_BindLob() caused access violation when binding CLOB/BLOB to PL/SQL OUT parameters using OCI_BDM_OUT bind direction mode   
    * C API: OCI_DirPathSetEntry() may passed Wrong SQL type passed to OCI_NumberFromString() for some numeric columns
    * C API: OCI_GetLastError() wWas not reporting errors/warnings after the following calls: OCI_FetchPrev(), OCI_FetchNext(), OCI_FetchFirst() and OCI_FetchLast()
    * C API: OCI_MemRealloc() may manipulate dandling pointers
    * C API: Fixed some usage of OCI_BIND_CALL() macro
    * C API: OCI_ObjectGettring(): When the attribute type was not a string, the application error handler
             was called with error code OCI_ERR_ATTR_NOT_FOUND and then the method was converting the attribute to a string without problem as expected.
     
### Changes

    * Added "Environment Variables" section in the C API documentation
    * C++ API: Added silent parameter to Date::Date()
    * C API: Fetch and Prefetch values are not reset any more to default values when statement is re-prepared
    * C API: Added support for specific environment variables: Now specific documented environment variables can be set for activating
             experimental or optional behaviours mostly for implementing workaround for Oracle Bugs.
    * C API: Added experimental workaround for unsolved Oracle OCI bug #9838993 using the environment variable "OCILIB_WORKAROUND_UTF16_COLUMN_NAME"


<br/>

Vincent

