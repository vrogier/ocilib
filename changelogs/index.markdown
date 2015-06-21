---
author: admin
comments: true
date: 2008-08-16 21:08:33+00:00
layout: page
slug: changelogs
title: Changes
wordpress_id: 87
---

Here is the list of OCILIB changelogs. 
 <br/>
Click on a change log entry to see the full details.
<br/>
<br/>

_[2015-05-05 . version 4.1.0](/changelogs/ocilib-changelog-v4.1.0.txt)_
    
  * C API  : Major library code update  
  * C API  : Some enhancements   
  * C API  : Various Bug fixes
  * C++ API: Major enhancements
  * C++ API: Various bug fixes

_[2015-01-13 . version 4.0.1](/changelogs/ocilib-changelog-v4.0.1.txt)_
  * C API : Fixed Object API with complex nested types
  * C API : New error OCI_ERR_ARG_INVALID_VALUE
  * C API : Miscellaneous Fixes
  * C++ API : Some Enums definitions where promoted to Flags
  * C++ API : Fixed binding strings
  * C++ API : Fixed possible race conditions in multithreaded applications
  * C++ API : Miscellaneous enhancements

_[2014-12-10 . version 4.0.0](/changelogs/ocilib-changelog-v4.0.0.txt)_
  * Introducing new C++ API
  * Added Support for some Oracle 12cR1 new features
  * Enhanced Collections support
  * Refactored Oracle Pooling support
  * Enhanced string conversions
  * Miscellaneous new methods
  * Miscellaneous changes
  * Miscellaneous fixes

_[2013-03-04 . version 3.12.1](/changelogs/ocilib-changelog-v3.12.1.txt)_
  * Fixed Unicode support (OCI_CHARSET_WIDE and OCI_CHARSET_MIXED)
  * Miscellaneous fixes
_[2013-02-07 . version 3.12.0](/changelogs/ocilib-changelog-v3.12.0.txt)_
  * Added support for lob prefetching introduced in 11gR1
  * Enhanced and fixed Direct Path API
  * Fixed Oracle Streams AQ (Advanced Queues) asynchrounous notifications
  
    
  * Fixed numeric/ string conversions
  * Miscellaneous fixes

_[2012-12-19 . version 3.11.1](/changelogs/ocilib-changelog-v3.11.1.txt)_
  * Fixed broken support for Oracle Database Change notifications

_[2012-12-12 . version 3.11.0](/changelogs/ocilib-changelog-v3.11.0.txt)_
  * Extended Oracle Streams AQ (Advanced Queues) support
  * Fixed And Enhanced support of BINARY FLOAT and BINARY DOUBLE types
  * Miscellaneous changes
  * Miscellaneous fixes

_[2012-08-08 . version 3.10.0](/changelogs/ocilib-changelog-v3.10.0.txt)_
  * Added support for native C float datatype

_[2012-06-29 . version 3.9.4](/changelogs/ocilib-changelog-v3.9.4.txt)_
  * Miscellaneous fixes 
  * Miscellaneous changes 

_[2011-12-05 . version 3.9.3](/changelogs/ocilib-changelog-v3.9.3.txt)_
  * Miscellaneous fixes 
  * Miscellaneous internal changes 
  * Documentation update

_[2011-07-13 . version 3.9.2](/changelogs/ocilib-changelog-v3.9.2.txt)_
  * Miscellaneous fixes (Detection of the ‘long long’ type on unix/linux, repaired support of Oracle client versions < 10g broken by version 3.9.0)

_[2011-07-08 . version 3.9.1](/changelogs/ocilib-changelog-v3.9.1.txt)_
  * Miscellaneous fixes (string binds, objects null attribute, "returning into" SQL clause)

_[2011-04-27 . version 3.9.0](/changelogs/ocilib-changelog-v3.9.0.txt)_
  * Added support for Oracle XA connectivity (X/Open Distributed Transaction Processing XA interface)
  * Added support for TAF (Transparent Application Failover) and HA (High availability) 
     
    
  * Added support for statement cache
  * Added support for Connection attributes introduced in Oracle 10gR2
  * Miscellaneous new API calls and tuning
  * Miscellaneous changes
  * Miscellaneous fixes

_[2010-12-13 . version 3.8.1](/changelogs/ocilib-changelog-v3.8.1.txt)_
  * Miscellaneous changes
  * Miscellaneous fixes

_[2010-10-24 . version 3.8.0](/changelogs/ocilib-changelog-v3.8.0.txt)_
  * Added support for Oracle Streams AQ (Advanced Queues)
  * Added support for SQL parsing only
  * Miscellaneous changes
  * Miscellaneous fixes

_[2010-07-26 . version 3.7.0](/changelogs/ocilib-changelog-v3.7.0.txt)_
  * Added support for Session Pools
  * Extended fetch interface
  * Improved Execute/Fetch performance
  * Extended Array interface
  * Miscellaneous changes
  * Miscellaneous fixes

_[2010-05-18 . version 3.6.0](/changelogs/ocilib-changelog-v3.6.0.txt)_
  * Improved Array interface
   
    
  * Added proper support for UTF8 strings
  * Exented String support
  * Extended LOB API
  * Modified native CHARSET modes
  * Fixed Multiple loads in Direct path mode
  * Miscellaneous changes
  * Miscellaneous fixes

_[2010-02-03 . version 3.5.1](/changelogs/ocilib-changelog-v3.5.1.txt)_
  * Miscelleanous fixes (see changelog)

_[2009-12-21 . version 3.5.0](/changelogs/ocilib-changelog-v3.5.0.txt)_
  * Added support for Oracle Database Change notification / Continuous Query Notification
  * Added support for Oracle warning (grace period notification, SQL truncation, etc ...)
  * Major Object API internal update, rewrite and improvement
  * Extended Lob Support
  * New various functions 
  * Miscelleanous fixes (see changelog)
  * Miscelleanous modifications (see changelog)

_[2009-30-07 . version 3.4.0](/changelogs/ocilib-changelog-v3.4.0.txt)_

  
  * Added support for Oracle 11g remote instance management

  
  * Miscelleanous fixes (see changelog)

  
  * Miscelleanous modifications (see changelog)

_[2009-30-06 . version 3.3.0](/changelogs/ocilib-changelog-v3.3.0.txt)_

  
  * Added SQL command and verb retrieving

  
  * Added support for batched errors for Array DML

  
  * Extended Lob Support

  
  * Extended Collection API

  
  * Modified and extended Bind API

  
  * Extended OCI_ImmediateXXX() API

  
  * Extended OCI_XXXFmt() API

  
  * Miscelleanous fixes (see changelog)
  * Miscelleanous modifications (see changelog)

_[2009-20-04 . version 3.2.0](/changelogs/ocilib-changelog-v3.2.0.txt)_
  * support  for direct path loading
  * extended binding capabilities (rebinding, update of array size, ...)
  * extended objects API
  * modified Database objects describe API
  * static libraries builds for GCC ports on MS Windows added
  * documentation updated
  * Miscellaneous internal modifications
  * Miscelleanous fixes (see changelog)
  * Miscelleanous modifications (see changelog)

_[2009-27-01 . version 3.1.0](/changelogs/ocilib-changelog-v3.1.0.txt)_
  * Added support for Oracle REF datatype
  * Added some tracing features
  * Extended Object API
  * Extended Collection elements API
  * Extended Bind informations
  * Fixed Unicode support for Unixes platforms
 
  
  * Miscellaneous public modifications
  * Miscellaneous internal modifications
  * Miscelleanous fixes (see changelog)
  * Miscelleanous modifications (see changelog)

_[2008-17-10 . version 3.0.1](/changelogs/ocilib-changelog-v3.0.1.txt)_
  * Fixed few memory leaks introduced in v3.0.0 (see changelog)
  * Fixed ocilib sources compilation with MSVC6++

_[2008-13-10 . version 3.0.0](/changelogs/ocilib-changelog-v3.0.0.txt)_
  * Added support for scrollable cursors
  * Added Support for Oracle Collections (VARRAYs and Nested Tables)
  * Major library rewrite
  * Extended Error handling
  * New thread contextual error handling
  * New functions
 
  
  * Extended implicit datatypes conversions
  * Extended documentation and demos
  * Miscelleanous fixes (see changelog)
  * Miscelleanous modifications (see changelog)

_[2008-24-07 . version 2.5.1](/changelogs/ocilib-changelog-v2.5.1.txt)_
  * Fixed runtime loading of som     OCI symbols (broken since v2.5.0)
  * Fixed runtime loading of OCI shared library on Unix-like platforms(broken since v2.5.0)
  * Fixed connection pooling emulation for Oracle 8i
  * Fixed Unicode strings binding to PL/SQL tables of char/varchar
  * see changelog for details

_[2008-21-07 . version 2.5.0]({{site.url}}/changelogs/ocilib-changelog-v2.5.0.txt)_
  * Added support for PL/SQL tables (PL/SQL arrays)
  * Extended Date, timestamp and interval support
  * Extended OCI_Connection handle attributes
  * Added Support for retreiving server output
  * Added Support for OCI Threadkeys
  * Extended runtime loading of Oracle shared libs on various Unix-like systems
  * Added Support for Oracle Instant Client on Unix-like systems
  * Extended OCILIB documentation
  * Added more demo source code
  * Miscelleanous modifications (see changelog)
_[2008-24-04 . version 2.4.0]({{site.url}}/changelogs/ocilib-changelog-v2.4.0.txt)_
  * Added support for 16bits and 64bits integers
  * Extended error handling
  * Added prebuilt binaries for MS Windows 64bits
  * Miscelleanous modifications (see changelog)
  * Miscelleanous fixes (see changelog)
_[2008-30-03 . version 2.3.0]({{site.url}}/changelogs/ocilib-changelog-v2.3.0.txt)_
  * Added support for connection pooling
  * Added support for portable threads and mutexes
  * Extended Lob support
  * Public interface modification (see changelog)
  * Misc. fixes (see changelog)
_[2008-19-03 . version 2.2.0]({{site.url}}/changelogs/ocilib-changelog-v2.2.0.txt)_
  * Added support for cancelling long calls
  * Extended statement tuning
  * Added password modification
  * Misc. modifications (see changelog)
  * Misc. fixes (see changelog)
_[2008-04-03 . version 2.1.0]({{site.url}}/changelogs/ocilib-changelog-v2.1.0.txt)_
  * Added public retrieval of internal OCI handles
  * Extended Hash Tables support
  * Added support for Lobs larger > 4 Go
  * Extended Oracle versions support
  * Modified Error handling
  * Misc. fixes (see changelog)
_[2008-02-16 . version 2.0.0]({{site.url}}/changelogs/ocilib-changelog-v2.0.0.txt)_
  * Added Object support
  * Added Unicode support for Unixes
  * Added National charform support
  * Added SYSDBA/SYSOPER session support
  * Extended Metadata information support
  * Modified OCI_Long and OCI_Lob API
  * Various modifications (see changelog)
  * Various fixed bugs (see changelog)
_[2008-01-28 . version 1.6.0]({{site.url}}/changelogs/ocilib-changelog-v1.6.0.txt)_
  * "Returning into" SQL Clause support
  * Added misc. fonctions (see changelog)
  * Modifications (see changelog)
  * Fixed bugs (see changelog)
_[2008-01-10 . version 1.5.4]({{site.url}}/changelogs/ocilib-changelog-v1.5.4.txt)_
  * Fixed bugs (see changelog)
  * Code refactoring (see changelog)
  * Updated demo applications (see changelog)
_[2008-01-04 . version 1.5.3]({{site.url}}/changelogs/ocilib-changelog-v1.5.3.txt)_
  * Fixed Array binding (see changelog)
_[2008-01-04 . version 1.5.2]({{site.url}}/changelogs/ocilib-changelog-v1.5.2.txt)_
  * Fixed fetch on  empty resultsets (see changelog)
_[2008-01-04 . version 1.5.1]({{site.url}}/changelogs/ocilib-changelog-v1.5.1.txt)_
  * Fixed 2 bugs (see changelog)
_[2008-01-04 . version 1.5.0]({{site.url}}/changelogs/ocilib-changelog-v1.5.0.txt)_
  * Added proper RAW support
  * Added Array interface support for fast bulk operations
  * Extended interval and timestamp support
  * Miscellanious internal modifications and fixes (see changelog)
_[2007-12-30 . version 1.4.0]({{site.url}}/changelogs/ocilib-changelog-v1.4.0.txt)_
  * Added Interval datatype support
  * Added Timestamp datatype support
  * Added Hash tables support
  * Added OCI version detection
  * Added OCILIB C source compatibility for C++
  * Resulset values can be retreived now by column names
  * Miscellanious internal fixes (see changelog)
  * Miscellanious internal modifications (see changelog)
_[2007-12-18 . version 1.3.1]({{site.url}}/changelogs/ocilib-changelog-v1.3.1.txt)_
  * Fixed sourceforge bug #1853369
_[2007-12-10 . version 1.3.0]({{site.url}}/changelogs/ocilib-changelog-v1.3.0.txt)_
  * Added Full BFILE support
  * Extended Lob support
  * Added Prefetch support
  * Added Support for describing database objects (currenly only table support)
  * Modified and extended Column info API
  * Extended support for formatted fonctions
  * Fixed OCILIB Objects binding
  * Miscellanious internal fixes (see changelog)
  * Miscellanious internal modifications (see changelog)
  * Perfomances improvement
  * Updated Documentation
_[2007-12-01 . version 1.2.1]({{site.url}}/changelogs/ocilib-changelog-v1.2.1.txt)_
  * Updated Documentation (regenerated from source without macro expansion)
  * Fixed sourceforge bug #1841998
_[2007-11-25 . version 1.2.0]({{site.url}}/changelogs/ocilib-changelog-v1.2.0.txt)_
  * Added complete html documentation
  * Embedded source documentatiobn (throught Doxygen)
  * Support for global transactions
  * Interface modfications (see changelog)
_[2007-11-12. version 1.1.0]({{site.url}}/changelogs/ocilib-changelog-v1.1.0.txt)_
  * Modified the library comes with 2 archvives : GNU et Win32
  * Added use of GNU install tools
  * Added options for building the library
  * Modified interfaces and some functions calls
  * Fixed unicode support
_[2007-10-19 . version 1.0.0]({{site.url}}/changelogs/ocilib-changelog-v1.0.0.txt)_
  * First public release
