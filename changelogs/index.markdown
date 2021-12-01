---
author: admin
comments: true
date: 2021-03-09 21:00:00+01:00
layout: page
slug: changelogs
title: Changelogs
---

Here is the list of OCILIB changelogs. 
 <br/>
 <br/>
 
Click on a change log entry to see the full details.

<br/>
<br/>


_[2021-11-30 . version 4.7.4]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.7.4.txt)_
 
   * Fixes (C API)

        - Issue 291: OCI_DirPathGetErrorRow() always return 0 since v4.7.0
        - Issue 284: OCI_MsgGetID() returns a NULL ID after being queued with OCI_DequeuePut()
        - Issue 288: OCI_GetString returns null on implicit conversion of a RAW value when it's longer than 64 bytes
		- Issue 283: OCI_DequeueGet() returns null pointer when message has empty payload
		- Issue 282: PL/SQL Server output wrong internal buffer size on some oracle versions
		- Issue 281: ORA-00931 in OCI_TypeInfoGet() when using SYS.RAW type

    * Fixes (C++ API)
    
		- Issue 272: AddressSanitizer reports mismatched new[] vs delete
		- Issue 278: ocilib::Object causes Segmentation fault
	
   * Miscellaneous
    
		- Updated compilers for MS Windows prebuilt binaries
            * VS2022 17.0.1 for 32/64 bit DLLs 
            * VC runtime (statically linked) updated from vc142 to vc143

<br/>
<br/>
_[2020-11-10 . version 4.7.2]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.7.2.txt)_
  
    *  Fixes (C API)
    
        - Issue 247: When using a OCI_Timestamp object created with OCI_TIMESTAMP_TZ flags, some OCILIB timestamps methods might raise an OCI_ERR_NULL_POINTER error
        - Issue 248: For Oracle client version < 11g, version 4.7.0 broke the ability to fetch data from resultset containing specific datatypes
        - Fixed some Coverity code analysis findings (hypothetical issues never encountered)
   
    *  Miscellaneous

        - Updated C API test suite  
            * Re-Ran successfully test suite with OCILIB compiled in 8i, 9i, 10g, 11g, 12c, 18c modes (ANSI and Unicode)
    
<br/>
<br/>

_[2020-10-28 . version 4.7.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.7.1.txt)_

  * Enhancements (C API)
    
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
         
    * Fixes (C API)
    
        - Issue 242:  Fixed strings.h conflict with recent GCC versions
        - Issue 243 : Fixed compiling ocilib directly from repository on non Windows systems (while using distribution archive was OK)
        - Issue 245 : Fixed ocilib shared library symbols visibility (Linux/Unix)
 
    * Changes (C API)
    
        - Removed useless old macros for mapping lib C string function to ansi or wide char versions (ostrXXX)
        - Removed symbols ocisprintf(), ocistrdup(), ocistrcasecmp(), ociwcsdup(), ociwcscasecmp() from OCILIB shared library
  
    * Miscellaneous
  
        - Updated compilers for MS Windows prebuilt binaries
            * VS2019 16.7.6 for 32/64 bit DLLs
 <br/>
<br/>
           
_[2020-07-16 . version 4.7.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.7.0.txt)_

    * Enhancements (C API)
    
        - Library internal design changes (preparing v5.0.0)
            * OCILIB code base has been layered in 2 distinct API layers:
                - Internal API that has now its own separate design
                    - v4.7.0 internal API, while matching the public one in this version, has most of its methods renamed
                    - It will evolve differently, preparing new public APIs for future versions
                - Public APIs exposing features to clients
                    - v4.7.0 is exposing the same public API as previous versions
                    - v5.0.0 will expose a different API
                    - Both will use same internal API
                - Contextual error management enhancements
                    - As internal APIs cannot be called from outside the library, last error management has been simplified and improved    
                - Enhancements on internal methods implementation:
                    - Since v3.0.0 (back in 2008), OCILIB methods implementations are standardized to follow the same single exit point pattern
                    - Standardized entry point checks and exit points are based on extentive macros usage
                    - But the control flow was handled by a status variable leading to continuous and nested checks
                    - With 4.7.0, no more "if (STATUS) {...}" in the code.
                    - Instead, in case of errors, code directly jumps to exit points that can have cleanup code sections.
                    - See it as try {...} finally{...}  
                    - Thus, code is now very sequential, much easier to read, with no nested error management/branching.
                    
        - Public headers reorganization:
            * Broke up ocilib.h content into multiple headers located in /include/ocilibc/
            * Broke up C++ headers content into multiple headers located in /include/ocilibcpp/            
    
        - Enhanced Error handing:
            * Updated OCI_Cleanup(): if there are any unfreed allocated bytes by the library itself (e.g. OCILIB user objects not freed)
                - it raises an error of type OCI_ERR_UNFREED_BYTES (with the amount of bytes in the error message)
                - it completes cleanup and returns FALSE
            * Added method OCI_ErrorGetLocation() to return the method name where the exception occured
            * Updated method OCI_ErrorGetString() to return a string using the following format "Error occured at {Location}: {Description}"
      
    * Fixes (C API)
    
        - Issue 239: Fetching nclobs as strings return value in hexa format truncated values when nsl_lang is UTF8
        - Issue 238: Fetching clobs as strings return truncated values when nsl_lang is UTF8
        - Issue 236: Memory issue with OCI_BAM_INTERNAL and OCI_CDT_NUMERIC        
        - Fixed OCI_EnqueuePut() and OCI_DequeueGet() in OCI_CHARSET_WIDE charset mode (added a workaround for a known oracle client UTF16 bug)
        - Fixed a minor memory leak when using implicit resultsets     
   
    * Fixes (C++ API)
    
        - Issue 230: Memory leaks while rebinding vectors
    
    * Miscellaneous
    
        - Extended C API Test suite that has now a much wider coverage
<br/>
<br/> 

_[2020-04-18 . version 4.6.4]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.6.4.txt)_

    * Fixes (C API)
        - Issue #222: Allow OCI_SESSION_SYSDBA for session pools
        - Issue #225: Fixed some PL/SQL tables binding issues when statement is an anonymous PL/SQL block
		- Issue #226: When NOT using OCI_ENV_CONTEXT environment flag, OCI_CollToText() andOCI_ObjectToText() failed if previous OCILIB call failed
        - Issue #227: Enhanced performance when fetching Clobs as strings (for Oracle 11g and above)

    * Miscellaneous (C API)
		- Some performance improvements when converting Objects and Collections to strings
		- Some performance improvements when calling OCI_GetString() on non strings columns
	
	* Miscellaneous (C++ API)	
		- Enhanced Number class in C++ API
<br/>
<br/>
  
_[2019-11-01 . version 4.6.3]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.6.3.txt)_

  * Some C API Bug fixes 

_[2019-02-25 . version 4.6.2]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.6.2.txt)_

  * Some C API and C++ API Bug fixes 

_[2019-02-17 . version 4.6.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.6.1.txt)_

  * Some C API and C++ API Bug fixes 

_[2018-11-21 . version 4.6.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.6.0.txt)_

  * Enhanced Oracle 18c support     
  * Added network timeout management support        
  * Some C API and C++ API Bug fixes 
  * Some C API and C++ changes
  
_[2018-09-13 . version 4.5.2]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.5.2.txt)_

  * Some C API and C++ API Bug fixes 

_[2018-02-21 . version 4.5.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.5.1.txt)_

  * Some C API and C++ API enhancements and Bug fixes 

_[2018-02-07 . version 4.5.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.5.0.txt)_

  * Some C API and C++ API enhancements and Bug fixes 

_[2017-11-29 . version 4.4.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.4.1.txt)_

  * Some C API and C++ API enhancements and Bug fixes 

_[2017-10-25 . version 4.4.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.4.0.txt)_

  * Some C API enhancements and Bug fixes 

_[2017-06-07 . version 4.3.3]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.3.3.txt)_

  * Some C API Bug fixes
  
_[2017-02-08 . version 4.3.2]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.3.2.txt)_

  * Some C API Bug fixes 
 
_[2017-01-25 . version 4.3.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.3.1.txt)_

  * Some C API Bug fixes
  * Some C++ API Bug fixes

_[2016-11-09 . version 4.3.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.3.0.txt)_

  * Added NUMBER SQL type native wrapping 
  * Various Bug fixes
  * Major internal refactoring

_[2016-03-21 . version 4.2.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.2.1.txt)_

  * Re-licensed OCILIB under Apache License Version 2.0 instead of LGPL 2.1
  * Various Bug fixes
  * Some minor changes

_[2015-09-21 . version 4.2.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.2.0.txt)_
  * Implemented Oracle 12c extended support for PL/SQL types
  * Added Memory usage information
  * Added Session Max Cursor information
  * Miscellaneous enhancements
  * Various Bug fixes

_[2015-05-05 . version 4.1.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.1.0.txt)_
  * C API : Major library code update
  * C API : Some enhancements
  * C API : Various Bug fixes
  * C++ API: Major enhancements
  * C++ API: Various bug fixes

_[2015-01-13 . version 4.0.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.0.1.txt)_
  * C API : Fixed Object API with complex nested types
  * C API : New error OCI_ERR_ARG_INVALID_VALUE
  * C API : Miscellaneous Fixes
  * C++ API : Some Enums definitions where promoted to Flags
  * C++ API : Fixed binding strings
  * C++ API : Fixed possible race conditions in multithreaded applications
  * C++ API : Miscellaneous enhancements

_[2014-12-10 . version 4.0.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v4.0.0.txt)_
  * Introducing new C++ API
  * Added Support for some Oracle 12cR1 new features
  * Enhanced Collections support
  * Refactored Oracle Pooling support
  * Enhanced string conversions
  * Miscellaneous new methods
  * Miscellaneous changes
  * Miscellaneous fixes

_[2013-03-04 . version 3.12.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.12.1.txt)_
  * Fixed Unicode support (OCI_CHARSET_WIDE and OCI_CHARSET_MIXED)
  * Miscellaneous fixes
_[2013-02-07 . version 3.12.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.12.0.txt)_
  * Added support for lob prefetching introduced in 11gR1
  * Enhanced and fixed Direct Path API
  * Fixed Oracle Streams AQ (Advanced Queues) asynchrounous notifications
  
    
  * Fixed numeric/ string conversions
  * Miscellaneous fixes

_[2012-12-19 . version 3.11.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.11.1.txt)_
  * Fixed broken support for Oracle Database Change notifications

_[2012-12-12 . version 3.11.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.11.0.txt)_
  * Extended Oracle Streams AQ (Advanced Queues) support
  * Fixed And Enhanced support of BINARY FLOAT and BINARY DOUBLE types
  * Miscellaneous changes
  * Miscellaneous fixes

_[2012-08-08 . version 3.10.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.10.0.txt)_
  * Added support for native C float datatype

_[2012-06-29 . version 3.9.4]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.9.4.txt)_
  * Miscellaneous fixes 
  * Miscellaneous changes 

_[2011-12-05 . version 3.9.3]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.9.3.txt)_
  * Miscellaneous fixes 
  * Miscellaneous internal changes 
  * Documentation update

_[2011-07-13 . version 3.9.2]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.9.2.txt)_
  * Miscellaneous fixes (Detection of the ‘long long’ type on unix/linux, repaired support of Oracle client versions < 10g broken by version 3.9.0)

_[2011-07-08 . version 3.9.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.9.1.txt)_
  * Miscellaneous fixes (string binds, objects null attribute, "returning into" SQL clause)

_[2011-04-27 . version 3.9.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.9.0.txt)_
  * Added support for Oracle XA connectivity (X/Open Distributed Transaction Processing XA interface)
  * Added support for TAF (Transparent Application Failover) and HA (High availability) 
     
    
  * Added support for statement cache
  * Added support for Connection attributes introduced in Oracle 10gR2
  * Miscellaneous new API calls and tuning
  * Miscellaneous changes
  * Miscellaneous fixes

_[2010-12-13 . version 3.8.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.8.1.txt)_
  * Miscellaneous changes
  * Miscellaneous fixes

_[2010-10-24 . version 3.8.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.8.0.txt)_
  * Added support for Oracle Streams AQ (Advanced Queues)
  * Added support for SQL parsing only
  * Miscellaneous changes
  * Miscellaneous fixes

_[2010-07-26 . version 3.7.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.7.0.txt)_
  * Added support for Session Pools
  * Extended fetch interface
  * Improved Execute/Fetch performance
  * Extended Array interface
  * Miscellaneous changes
  * Miscellaneous fixes

_[2010-05-18 . version 3.6.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.6.0.txt)_
  * Improved Array interface
   
    
  * Added proper support for UTF8 strings
  * Exented String support
  * Extended LOB API
  * Modified native CHARSET modes
  * Fixed Multiple loads in Direct path mode
  * Miscellaneous changes
  * Miscellaneous fixes

_[2010-02-03 . version 3.5.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.5.1.txt)_
  * Miscelleanous fixes (see changelog)

_[2009-12-21 . version 3.5.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.5.0.txt)_
  * Added support for Oracle Database Change notification / Continuous Query Notification
  * Added support for Oracle warning (grace period notification, SQL truncation, etc ...)
  * Major Object API internal update, rewrite and improvement
  * Extended Lob Support
  * New various functions 
  * Miscelleanous fixes (see changelog)
  * Miscelleanous modifications (see changelog)

_[2009-30-07 . version 3.4.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.4.0.txt)_

  
  * Added support for Oracle 11g remote instance management

  
  * Miscelleanous fixes (see changelog)

  
  * Miscelleanous modifications (see changelog)

_[2009-30-06 . version 3.3.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.3.0.txt)_

  
  * Added SQL command and verb retrieving

  
  * Added support for batched errors for Array DML

  
  * Extended Lob Support

  
  * Extended Collection API

  
  * Modified and extended Bind API

  
  * Extended OCI_ImmediateXXX() API

  
  * Extended OCI_XXXFmt() API

  
  * Miscelleanous fixes (see changelog)
  * Miscelleanous modifications (see changelog)

_[2009-20-04 . version 3.2.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.2.0.txt)_
  * support  for direct path loading
  * extended binding capabilities (rebinding, update of array size, ...)
  * extended objects API
  * modified Database objects describe API
  * static libraries builds for GCC ports on MS Windows added
  * documentation updated
  * Miscellaneous internal modifications
  * Miscelleanous fixes (see changelog)
  * Miscelleanous modifications (see changelog)

_[2009-27-01 . version 3.1.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.1.0.txt)_
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

_[2008-17-10 . version 3.0.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.0.1.txt)_
  * Fixed few memory leaks introduced in v3.0.0 (see changelog)
  * Fixed ocilib sources compilation with MSVC6++

_[2008-13-10 . version 3.0.0]({{ site.baseurl }}/changelogs/ocilib-changelog-v3.0.0.txt)_
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

_[2008-24-07 . version 2.5.1]({{ site.baseurl }}/changelogs/ocilib-changelog-v2.5.1.txt)_
  * Fixed runtime loading of som     OCI symbols (broken since v2.5.0)
  * Fixed runtime loading of OCI shared library on Unix-like platforms(broken since v2.5.0)
  * Fixed connection pooling emulation for Oracle 8i
  * Fixed Unicode strings binding to PL/SQL tables of char/varchar
  * see changelog for details

_[2008-21-07 . version 2.5.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v2.5.0.txt)_
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
_[2008-24-04 . version 2.4.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v2.4.0.txt)_
  * Added support for 16bits and 64bits integers
  * Extended error handling
  * Added prebuilt binaries for MS Windows 64bits
  * Miscelleanous modifications (see changelog)
  * Miscelleanous fixes (see changelog)
_[2008-30-03 . version 2.3.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v2.3.0.txt)_
  * Added support for connection pooling
  * Added support for portable threads and mutexes
  * Extended Lob support
  * Public interface modification (see changelog)
  * Misc. fixes (see changelog)
_[2008-19-03 . version 2.2.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v2.2.0.txt)_
  * Added support for cancelling long calls
  * Extended statement tuning
  * Added password modification
  * Misc. modifications (see changelog)
  * Misc. fixes (see changelog)
_[2008-04-03 . version 2.1.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v2.1.0.txt)_
  * Added public retrieval of internal OCI handles
  * Extended Hash Tables support
  * Added support for Lobs larger > 4 Go
  * Extended Oracle versions support
  * Modified Error handling
  * Misc. fixes (see changelog)
_[2008-02-16 . version 2.0.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v2.0.0.txt)_
  * Added Object support
  * Added Unicode support for Unixes
  * Added National charform support
  * Added SYSDBA/SYSOPER session support
  * Extended Metadata information support
  * Modified OCI_Long and OCI_Lob API
  * Various modifications (see changelog)
  * Various fixed bugs (see changelog)
_[2008-01-28 . version 1.6.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.6.0.txt)_
  * "Returning into" SQL Clause support
  * Added misc. fonctions (see changelog)
  * Modifications (see changelog)
  * Fixed bugs (see changelog)
_[2008-01-10 . version 1.5.4]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.5.4.txt)_
  * Fixed bugs (see changelog)
  * Code refactoring (see changelog)
  * Updated demo applications (see changelog)
_[2008-01-04 . version 1.5.3]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.5.3.txt)_
  * Fixed Array binding (see changelog)
_[2008-01-04 . version 1.5.2]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.5.2.txt)_
  * Fixed fetch on  empty resultsets (see changelog)
_[2008-01-04 . version 1.5.1]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.5.1.txt)_
  * Fixed 2 bugs (see changelog)
_[2008-01-04 . version 1.5.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.5.0.txt)_
  * Added proper RAW support
  * Added Array interface support for fast bulk operations
  * Extended interval and timestamp support
  * Miscellanious internal modifications and fixes (see changelog)
_[2007-12-30 . version 1.4.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.4.0.txt)_
  * Added Interval datatype support
  * Added Timestamp datatype support
  * Added Hash tables support
  * Added OCI version detection
  * Added OCILIB C source compatibility for C++
  * Resulset values can be retreived now by column names
  * Miscellanious internal fixes (see changelog)
  * Miscellanious internal modifications (see changelog)
_[2007-12-18 . version 1.3.1]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.3.1.txt)_
  * Fixed sourceforge bug #1853369
_[2007-12-10 . version 1.3.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.3.0.txt)_
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
_[2007-12-01 . version 1.2.1]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.2.1.txt)_
  * Updated Documentation (regenerated from source without macro expansion)
  * Fixed sourceforge bug #1841998
_[2007-11-25 . version 1.2.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.2.0.txt)_
  * Added complete html documentation
  * Embedded source documentatiobn (throught Doxygen)
  * Support for global transactions
  * Interface modfications (see changelog)
_[2007-11-12. version 1.1.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.1.0.txt)_
  * Modified the library comes with 2 archvives : GNU et Win32
  * Added use of GNU install tools
  * Added options for building the library
  * Modified interfaces and some functions calls
  * Fixed unicode support
_[2007-10-19 . version 1.0.0]({{site.url}}ocilib/changelogs/ocilib-changelog-v1.0.0.txt)_
  * First public release
