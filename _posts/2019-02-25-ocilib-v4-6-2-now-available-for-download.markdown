---
author: admin
comments: false
date: 2019-02-25 20:00:00+01:00
layout: post
slug: ocilib-v4-6-2-now-available-for-download
title: OCILIB v4.6.2 now available for download !
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.6.2 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>
### Fixes (C API)
	
       - Fixed compile and run time Oracle version detection when Oracle version is >= 18.1
	   - Fixed OCI_SetTimeout() that was setting all timeouts with input value
	   - Fixed OCI_GetTimeout() that was returning OCI_NTO_CALL or OCI_NTO_RECEIVE value whatever requested timeout
	   - Added macro OCI_VER_MAKE to make an ocilib compliant oracle version from major, minor and revision versions (e.g. OCI_VER_MAKE(18,3,0) equals OCI_18_3)
	   - Updated Oracle versions constants
	   
### Fixes (C++ API)
	
       - Issue #197: Using C++ interface in multiple DLLs

<br/>

Vincent

