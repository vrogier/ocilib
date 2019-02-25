---
author: admin
comments: false
date: 2019-02-17 20:00:00+01:00
layout: post
slug: ocilib-v4-6-1-now-available-for-download
title: OCILIB v4.6.1 now available for download !
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.6.1 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>
### Fixes (C API)

        - Issue #188: Compile error with Oracle client 12.1 and option OCI_IMPORT_LINKAGE
        - Issue #196: Warning 4996
        - Issue #198: Compile Error with Oracle Client 12.2 and option OCI_ATTR_CALL_TIMEOUT
        - Issue #199: Compile warnings on Oracle Client 12.2
                
### Fixes (C++ API)
    
        - Issue #182: Cannot bind a std::vector as a PL/SQL Table (v4.6.0 fixed the issue only for 1st element of the vector)
        - Issue #189: In/out binded vector<ostring> not updated after statement execution
        - Issue #191: Reassigning SAME C handle to C++ a object leads to loose the content of the C handle
        - Issue #192: Vectors of non scalar types binded as pure OUT binds are not updated after execution if vector element do not have their value set 
        - Issue #200: Compiler warning 4018 with 4.6.0
 
### Miscellaneous:
  
        - Updated compilers for MS Windows prebuilt binaries
            * VS2017 15.9.7 for 32/64 bit DLLs
        - Prebuilt static libraries built with TDM-GCC (Code::Blocks projects) are not shipped anymore
            * Recent TDM- GCC versions produces executables that links statically against winpthreads
            * This implies requirements and restrictions that motivated to remove prebuilt libs from ocilib releases
            * Users can compile them using the provided Code::Blocks project
  

<br/>

Vincent

