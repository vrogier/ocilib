---
author: admin
comments: true
date: 2010-05-14 10:59:46+00:00
layout: post
slug: ocilib-v360-coming-on-june-thuesday-18th
title: OCILIB v3.6.0 coming on June, Thuesday 18th !
wordpress_id: 471
categories:
- News
- Updates
tags:
- array
- charset
- interface
- mixed
- speed
- UTF8
---

Hello all !

OCILIB v3.6.0 will be finally available by next tuesday (2010-06-18) on sourceforge.net.

The SVN repository has been updated today with the v3.6.0 sources.
Packages will be added for download on June, 18th.

This version is a major features updates and fixes.

Here is the changelog of the 3.6.0 :


    
    
    2010-05-18 Version 3.6.0 Vincent Rogier vince.rogier@ocilib.net
    
        * Improved Array interface
        
          - Arrays of non scalar datatypes (based on handles) previously had to be initialized element by element and could be time consuming
          - It is now possible to allocate arrays in one step and thus improve performance (especially with Oracle 11g), internal OCILIB stuctures and OCI handles are allocated in one step
          - Added OCI_DateArrayCreate()
          - Added OCI_DateArrayFree()
          - Added OCI_TimestampArrayCreate()
          - Added OCI_TimestampArrayFree()
          - Added OCI_IntervalArrayCreate()
          - Added OCI_IntervalArrayFree()
          - Added OCI_LobArrayCreate()
          - Added OCI_LobArrayFree()
          - Added OCI_FileArrayCreate()
          - Added OCI_FileArrayFree()
          - Added OCI_ObjectArrayCreate()
          - Added OCI_ObjectArrayFree()
          - Added OCI_RefArrayCreate()
          - Added OCI_RefArrayFree()
          - Added OCI_CollArrayCreate()
          - Added OCI_CollArrayFree()
          
        * Added proper support for UTF8 strings
    
          - Use OCI_CHARSET_ANSI mode
          - Set the environment variable NLS_LANG to value that contains the substring 'UTF8'
          - Depending on the database charset :
        
              - UTF8 : UTF8 strings are used for any text based binds or retrieved columns values
              - Other charsets : UTF8 strings can be used only with national strings (NCHAR/NVARCHAR/NCLOB)
    
        * Exented String support
    
          - Added OCI_BindSetCharsetForm() to enable the use of national charset on binded strings
    
        * Extended LOB API
    
          - It is now possible to specify for CLOB/NCLOB buffer lengths in characters and/or bytes for R/W operations
          - Added OCI_LobRead2()
          - Added OCI_LobWrite2()
          - Added OCI_LobAppend2()
    
        * Modified native CHARSET modes
    
          - OCI_CHARSET_UNICODE replaced by OCI_CHARSET_WIDE 
          - OCI_CHAR_UNICODE replaced by OCI_CHAR_WIDE
          - OCI_XXX_UNICODE symbols are still supported for backward compatibility
       
        * Fixed Multiple loads in Direct path mode
    
          - Fixed the API to enable multiple load
          - Updated direct path demo
    
        * Miscellaneous changes
    
          - Added OCI_HandleGetSubscription()
          - Modified OCI_Initialize() : renamed parameter 'home' to 'lib_path' 
          - OCI_GetString() : when column base type is not string based, the implicit conversion uses now now dynamic memory allocation to return full data content instead of truncating to OCI_BUFFER_SIZE limit    
          - Various documentation updates
          - Modified OCI_LobRead() (and thus OCI_LobRead2()) to set a trailing null character
          - Replaced few left direct malloc() calls by OCI_MemAlloc()
    
        * Miscellaneous fixes
    
          - fixed prototype of OCI_ConnPoolGetlGetNoWait() => OCI_ConnPoolGetGetNoWait()
          - fixed internal raw size retrieving in OCI_ObjectGetRaw() and OCI_ElemSetRaw()
          - fixed OCI_ObjectSetString() in OCI_CHARSET_MIXED mode that could cause a segfault
          - fixed internal string conversions in OCI_CHARSET_MIXED mode on Unix systems that could cause a segfault
          - fixed various problems of RAW and LONG handling in OCI_MIXED_CHARSET mode
          - fixed OCI_ServerGetOutput() when statements are reprepared
          - fixed computation of objects members offsets (in the internal object opaque structure) which caused misaligned pointer and lead to segfault depending on the members datatype
          - fixed OCI_xxxFmt() functions that were badly formatting the ouput sql when more than one DATE parameter was present
          - fixed OCI_BindArrayOfXXXX() documentation
          - fixed OCI_BindArraySetSize() when called many times with a new size > initial size
          - fixed endianness problem on big endian platforms when a string buffer had to be converted
          - fixed internal lengths used in implicit conversions in OCI_GetString() for some datatypes
          - Improved long long support detection
          - fixed national charset attribute detection for NCLOB binds
          - Fixed memory leak appeared in v3.5.0 when deallocating internal fetched LOBs
    
        * Modified developer and support contacts
    
          - use the following contact address for any information/question/help about OCILIB :
    
              - support and help         : support@ocilib.net
              - contact and information  : vince.rogier@ocilib.net 
    
    
    



Have a nice week end !
