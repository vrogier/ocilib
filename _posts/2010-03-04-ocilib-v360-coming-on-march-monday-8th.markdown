---
author: admin
comments: true
date: 2010-03-04 21:45:58+00:00
layout: post
slug: ocilib-v360-coming-on-march-monday-8th
title: OCILIB v3.6.0 coming on March, Monday 8th !
wordpress_id: 463
categories:
- News
tags:
- 3.6.0
- OCILIB
- string handling
- Updates
---

Hello all !

OCILIB v3.6.0 will be available by next monday (2010-03-08) on sourceforge.net.

The SVN repository will be updated this weekend with the v3.6.0 sources.
Packages will be added for download on March, 8th.

This version is a minor features updates and fixes.

Here is the changelog of the 3.6.0 :


    
    
    2010-03-08 Version 3.6.0 Vincent Rogier vince.rogier@ocilib.net
    
        * Extended String handling
    
          - It's now possible to set how NULL strings are returned to application (NULL or "")
        
            - Added OCI_SetNullStringMode()
            - Added OCI_GetNullStringMode()
    
          - It's now possible to set how string lengths are compute (bytes or characters)
        
            - Added OCI_SetStringLengthMode()
            - Added OCI_GetStringLengthMode()
    
        * Miscellaneous changes
    
          - Added OCI_HandleGetSubscription()
          - Modified OCI_Initialize() : remaned parameter 'home' to 'lib_path' 
          - Various documentation updates
    
        * Miscellaneous fixes
    
          - fixed prototype of OCI_ConnPoolGetlGetNoWait() => OCI_ConnPoolGetGetNoWait()
          - fixed various problems of RAW and LONG handling in OCI_MIXED_CHARSET mode
          - fixed OCI_GetServerOuput() when statements are reprepared
          - fixed computation of objects members offsets (in the internal object opaque structure) which caused mmisaligned pointer and lead to segfault depending on the members datatype
          - fixed OCI_xxxFmt() functions that were badly formatting the ouput sql when more than one DATE parameter was present
          - fixed OCI_BindArrayOfXXXX() documentation
          - fixed OCI_BindArraySetSize() when called many times with a new size > initial size
          - fixed endianness problem on big endian platforms when a string buffer had to be converted
          - fixed internal lengths used in implicit conversions in OCI_GetString() for some datatypes
    
        * Modified developer and support contacts
    
          - use the following contact address for any information/question/help about OCILIB :
    
              - support and help         : support@ocilib.net
              - contact and information  : vince.rogier@ocilib.net 
    
    



Have a nice week end !
