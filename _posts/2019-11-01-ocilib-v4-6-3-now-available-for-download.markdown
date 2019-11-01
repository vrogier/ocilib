---
author: admin
comments: false
date: 2019-11-01 13:00:00+01:00
layout: post
slug: ocilib-v4-6-3-now-available-for-download
title: OCILIB v4.6.3 now available for download !
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.6.3 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>
### Fixes (C API)
	
        - Issue #207: Updated Oracle command codes that were not accurate
        - Issue #213: Updated some version checks that were testing only for server version instead of testing both client and server versions
        - Issue #216: Re-calling OCI_BindArraySetSize() with a smaller value than previous assignment led to segfaults when statement has non scalar type binds
        - Issue #220: Implicit conversion Clob issue in OCI_GetString() when using UTF8 encoding
	
### Miscellaneous (C API)
	
	    - Issue #212: Added possible error codes for OCI_Initialize() in the documentation
		- Issue #215: Added a workaround to correctly retrieve environment variables set at runtime using SetEnvironmentVariable() on Windows 

<br/>

Vincent

