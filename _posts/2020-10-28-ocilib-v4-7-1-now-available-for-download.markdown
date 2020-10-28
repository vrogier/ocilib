---
author: admin
comments: false
date: 2020-10-28 10:00:00+01:00
layout: post
slug: ocilib-v4-7-1-now-available-for-download
title: OCILIB v4.7.1 now available for download !
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.7.1 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>
**This release contains no code changes. 
It enhances library symbol visibility with GCC and compilation issues with recent GCC versions**

### Enhancements (C API)
    
        - OCILIB internal and public symbols management
            * For static library:
                - All private symbols that can have local scope are now declared static
                - All private non static symbols are now prefixed with "Ocilib" to avoid collisions 
            * For shared library:
                - No change for Windows platform as __declspec(dllimport) and  __declspec(dllexport) were already handled
                - When using GNUC compatible compiler:
                    - all public  symbols are now declared with __attribute__ ((visibility ("default")))
                    - all private symbols are now delcared with __attribute__ ((visibility ("hidden")))
                    - Configure script will automatically add the compiler flag "-fvisibility=hidden"
        
        - OCILIB compiles now without any warnings when using GNUC with flags -Wall -Wextra -pedantic
         
### Fixes (C API)
    
        - Issue 242:  Fixed strings.h conflict with recent GCC versions
        - Issue 243 : Fixed compiling ocilib directly from repository on non Windows systems (while using distribution archive was OK)
        - Issue 245 : Fixed ocilib shared library symbols visibility (Linux/Unix)
 
### Changes (C API)
    
        - Removed useless old macros for mapping lib C string function to ansi or wide char versions (ostrXXX)
        - Removed symbols ocisprintf(), ocistrdup(), ocistrcasecmp(), ociwcsdup(), ociwcscasecmp() from OCILIB shared library

### Miscellaneous
  
        - Updated compilers for MS Windows prebuilt binaries
            * VS2019 16.7.6 for 32/64 bit DLLs
<br/>

Vincent

