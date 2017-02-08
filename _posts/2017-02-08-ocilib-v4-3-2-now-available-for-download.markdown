---
author: admin
comments: false
date: 2017-02-08 22:35:00+01:00
layout: post
slug: ocilib-v4-3-2-now-available-for-download
title: OCILIB v4.3.2 now available for download !
categories:
- News
- Updates
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.3.2 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>

### Fixes (C API)
    
    - Issue #69: Fixed OCI_StatementGetConnection() that was returning erroneous values (regression introduced in v4.3.0)
    - Issue #70: Fixed Subscriptions issues when OCILIB charset is OCI_CHARSET_WIDE (Unicode builds)
    - Issue #71: Fixed regresson introduced in v4.3.0 about binding big_uint variables (only unsigned 64bit integers)
    - Issue #72: Fixed OCI_LobErase() return value that was wrong since v4.3.0 (1 on success and 0 on failure) instead of the number of elements erased from the LOB
    - Issue #75: Fixed OCI_ElemIsNull() that was marking the call as failed is the value was not NULL (regression introduced in v4.3.0)
    - Issue #78: Fixed usage of OCI_SetBindAllocation() when switching bind allocation mode that may result in memory leaks
    - Issue #79: Fixed issues in implicit conversion in OCI_GetString()

### Changes and Enhancements
	
    - Issue #73: Activate support for N' substitution (nchar replacement in SQL statement strings) when source code is unicode (Only for Oracle Client >= 10gR2)
    - Issue #74: Added support for OCI_Number in format calls	
    - Issue #79: Added support for OCI_Statement (cursors) in OCI_GetString()
			
### Miscellaneous
	
    - Removed outdated unmaintained Visual Studio 2005, 2008 an 2010 projects for building OCILIB DLLs and C/C++ tests apps
    - Keeping visual Studio 2013 projects
    - Added Visual Studio 2015 projects
    - Prebuilt Windows OCILIB dlls are now compiled using Visual Studio 2015
		

<br/>

Vincent

