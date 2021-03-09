---
author: admin
comments: false
date: 2021-03-09 22:00:00+01:00
layout: post
slug: ocilib-v4-7-3-now-available-for-download
title: OCILIB v4.7.3 now available for download !
---

Dear all,
<br/>
<br/>
<b>OCILIB v4.7.3 is now [_available for download_]({{site.projecturl}}/releases/) !</b>
<br/>
<br/>
Get the release and enjoy it !
<br/>
<br/>
###  Fixes (C API)
    
        - Issue 251: OCI_ErrorGetMessage() returns invalid messages when OCILIB is build with OCI_CHARSET_WIDE on linux/unix platforms
        - Issue 252: Incorrect OCILIB client driver layer namein V$SESSION_CONNECT_INFO and GV$SESSION_CONNECT_INFO when OCILIB is build with OCI_CHARSET_WIDE on linux/unix platforms
        - Issue 253: Error occured at OcilibLobFree: ORA-01403: no data found
        - Issue 258: OCI_SubscriptionRegister() returns always NULL since v4.7.0
        - Issue 264: Fetched UROWIDs (IOT Table ROWIDs) values are truncated
        - Issue 268: OCI_DirPathSetColumn() forwards parameter 'maxsize' as unsigned short to Oracle client leading to data truncation

 ###  Fixes (C++ API)  

        - Issue 250: Exception handling when using OCI_CHARSET_WIDE
        - Issue 262: Using correct delete[] in ManagedBuffer destructor

###  Miscellaneous

        - Updated compilers for MS Windows prebuilt binaries
            * VS2019 16.9.0 for 32/64 bit DLLs 
            * VC runtime (statically linked) updated from vc141_xp to vc142
            * Note: Windows XP is not supported anymore due to the runtime update
        - Reduced some ocilib structures size by removing some paddings (especially in 64bits)
        - Added a makefile to build the C++ demo (Makefile_demo_cpp) that is also now installed with the C demo Makefile
        - OCILIB Test suite is now ported to Linux/Unix platforms
            * Test suite is now provided in ocilib-x.y.z-gnu.tar.gz distribution archives and installed by 'make install' among documentation and demo (typically in /usr/local/share/doc/ocilib/tests)
            * To build the test suite:
                - Modify the db credentials in ocilib_tests.h
                - Use Makefile_tests (it depends on google test that must be installed prior building the test suite)

<br/>

Vincent

