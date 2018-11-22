---
author: admin
comments: false
date: 2018-11-21 10:00:00+01:00
layout: post
slug: ocilib-v4-6-0-now-available-for-download
title: OCILIB v4.6.0 now available for download !
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.6.0 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>
### Changes 

        - Enhanced Oracle 18c support     
            * Fixed Oracle minor and revision versions computation when Oracle version >= 18c (due to breaking in OCI API)
            * Added new version constants
            * Extended Column property flags
            * Added the ability to retrieve the SQL_ID of an SQL statement
            * Added the ability to find out if a CLOB/NCLOB/BLOB is remote
            * Added network timeout management support         
         - C++ API:
            * Added Environment::GetCharMaxSize()

### Fixes (C API)

        - Fixed compilation with OCI_IMPORT_LINKAGE when compile time OCI version is OCI_8_1 or OCI_9_1 or OCI_9_2 (e.g. on Unix based system with very old Oracle clients)   
    
### Fixes (C++ API)
    
        - Issue #170: How to write a clob using utf-8 encoding charset
        - Issue #181: Problem with utf-8 in LobNationalCharacter
        - Issue #182: Cannot bind a std::vector as a PL/SQL Table 
 
### Miscellaneous:

        - Issue #180 : Added workaround for OCI bug related to prefetching with Oracle 9i clients
 
        - Updated compilers for MS Windows pre built binaries
            * VS2017 15.9.1 for 32/64 bit DLLs
            * Code::Blocks 17.12 (TDM-GCC 5.1.0) for 32 bit MinGW static libs

<br/>

Vincent

