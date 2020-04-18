---
author: admin
comments: false
date: 2020-04-18 12:45:00+02:00
layout: post
slug: ocilib-v4-6-4-now-available-for-download
title: OCILIB v4.6.4 now available for download !
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.6.4 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>
### Fixes (C API)

	- Issue #222: Allow OCI_SESSION_SYSDBA for session pools
	- Issue #225: Fixed some PL/SQL tables binding issues when statement is an anonymous PL/SQL block
	- Issue #226: When NOT using OCI_ENV_CONTEXT environment flag, OCI_CollToText() and OCI_ObjectToText() failed if previous OCILIB call failed
	- Issue #227: Enhanced performance when fetching Clobs as strings (for Oracle 11g and above)

### Miscellaneous (C API)

	- Some performance improvements when converting Objects and Collections to strings
	- Some performance improvements when calling OCI_GetString() on non strings columns
	- Minor changes in code for addressing some code analysis hints (clang, resharper)

### Miscellaneous (C++ API)

	- Enhanced Number class in C++ API:
		* Restrict arithmetic operations on supported types only
		* Arithmetic operations are also now supported between Number objects
	- Minor changes in code for addressing code analysis hints (clang, resharper)
	- Updated C++ demo application

### Miscellaneous

	- Updated documentation
	- Updated all files Copyright to current year (2020)
	- Updated compilers for MS Windows prebuilt binaries
		* VS2019 16.5.3 for 32/64 bit DLLs	
		* Still targeting vc141_xp toolset for backward compatibility
<br/>

Vincent

