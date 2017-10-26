---
author: admin
comments: false
date: 2017-10-25 22:35:00+01:00
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
<b>OCILIB v4.4.0 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>

         
### Enhancements (C API)
              
- OCI_Object: Supporting now non final object types
    * if an OCI_Object instance real type is a sub type of its column object type, OCILIB is dynamically retrieving its real and right type instead of processing it as its base type
    * Added missing synchronized access when iterating the list of existing OCI_TypeInfo in OCI_TypeInfoGet()
- Allocation Binding mode updates
    * Added BindGetAllocationMode()
    * updated OCI_GetBindAllocation() and OCI_SetBindAllocation() documentation to properly specify their usage
- Extended 12cR2 support:
    * Increased identifier size to 128
    * Added OCI_ColumnGetCollationID
    * Added more Oracle Session Mode               
- Refactored OCI_List implementation (Internal change)
    * No more access to OCI_List fields from other compilation units (encapsulation purposes)
    * Refactored OCI_List implementation
            
### Fixes (C API)

- Issue #112: OCI_GetString behaves badly with empty LOBs
- Issue #104: XA connection open failed with ORA-12154
    * Fixed broken support for XA connections
    * Added 2 new error codes related to XA connections management (OCI_ERR_XA_ENV_FROM_STRING, OCI_ERR_XA_CONN_FROM_STRING)
- Issue #101: Wrong OCI_STRING_FORMAT_TIMESTAMP constant
 - Fix for Oracle bug #9838993 workaround
    * Version 4.2.1 introduced support for an experimental workaround for Oracle bug 9838993 (memory leak when using unicode and re-preparing/executing same statement multiple times)
    * This support was not always working as column names where sometimes not null terminated

		

<br/>

Vincent

