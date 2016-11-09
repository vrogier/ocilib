---
author: admin
comments: false
date: 2016-09-11 22:35:00+01:00
layout: post
slug: ocilib-v4-3-0-now-available-for-download
title: OCILIB v4.3.0 now available for download !
categories:
- News
- Updates
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.3.0 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>

### New Features
    
    * Added NUMBER SQL type native wrapping          
            - Optional type as NUMBER type can still be fetched/binded using regular C native numeric types
            - New OCI_Number type can be used when precision matters or for dealing with special values such as positive and negative infinity
            - C API : 
                    Added OCI_NUM_NUMBER numeric type
                    Added OCI_Number type
                    Added OCI_NumberCreate()
                    Added OCI_NumberFree()
                    Added OCI_NumberArrayCreate()
                    Added OCI_NumberArrayFree()
                    Added OCI_NumberAssign()
                    Added OCI_NumberGetContent()
                    Added OCI_NumberSetContent()
                    Added OCI_NumberSetValue()
                    Added OCI_NumberGetValue()
                    Added OCI_NumberAdd()
                    Added OCI_NumberSub()
                    Added OCI_NumberMultiply()
                    Added OCI_NumberDivide()
                    Added OCI_NumberCompare()
                    Added OCI_GetNumber()
                    Added OCI_GetNumber2()
                    Added OCI_BindNumber()
                    Added OCI_BindArrayOfNumbers()
                    Added OCI_RegisterNumber()
                    Added OCI_NumberFromText()
                    Added OCI_NumberToText()
                    Added OCI_ElemGetNumber()
                    Added OCI_ElemSetNumber()
                    Added OCI_ObjectGetNumber()
                    Added OCI_ObjectSetNumber()                
            - C++ API: 
                    Added ocilib::Number type encapsulating the C API OCI_Number type
                    This new type comes with all possible operator overloads for better usability

### Fixes:

    * OCI_ObjectGetString(): Fixed usage of internal temporary buffers
    * OCI_FileRead(): Return value was not the correct one
    * OCI_PoolGetConnection() : was returning NULL connections for session pool using external credentials
    * OCI_StringGetFromType() : Fixed segfault when converting CLOBs (with length > to 512) to UTF8 strings in OCI_GetString() if an UTF8 charset is used
    * Fix indicators usage for collections : OCI_IsNull() always returned FALSE even if fetched collection was NULL
    * Fix compilation under C compilers not supporting C99 feature allowing declaring variables anywhere in blocks

     
### Miscellaneous

    * C API: Added OCI_GetDataSize() and OCI_GetDataSize2()
    * C API: Major internal code re-factoring       
            - Code factorization
            - Replaced verbose macros with more compact ones
            - Code base reduced by few thousands lines of code
            - Fixed some error handling logic
    * C++ API: 
            - Rewrote iterators for ocilib::Collection<> 
            - Collection Iterators are now random iterators allowing Collections being used with STL algorithms such as std::sort()
            - Added Collection const_iterators
            - Addressed various code analysis hints
            - Added C++ compiler capabilities detection (C+11)
    * Updated samples in folder /demo
            - dded required DDL and DML for all samples
            - Updated some samples


<br/>

Vincent

