---
author: admin
comments: false
date: 2013-03-05 19:31:38+00:00
layout: post
slug: ocilib-v3-12-1-now-available-for-download
title: OCILIB v3.12.1 now available for download !
wordpress_id: 753
categories:
- News
- Updates
---

Hello all,

  

OCILIB v3.12.1 is now [_available for download_](http://orclib.sourceforge.net/download/) ! 




    
  * Fixed Unicode support (OCI_CHARSET_WIDE and OCI_CHARSET_MIXED)

    
  * Miscellaneous fixes
 
 

See the [_complete v3.12.1 change log here_](http://orclib.sourceforge.net/public/changelogs/ocilib-changelog-v3.12.1.txt)

I apologize for releases 3.10.x and 3.11.x that have introduced issues on stable features !!

This happened mainly because of corrupted test suites that should have reported errors. Since I have fix my test suite. But to be honest, they do not convert the full OCILIB API. Each new feature is tested upon its initial release but not necessary in later releases. 

I did not start unit tests in early release of OCILIB. And building now a full unit test suite for OCILIB is a huge task and i can't find the time to work on it as I work on OCILIB on my spare time.

If anybody is interested in helping me building a complete unit test suite for OCILIB, don't hesitate to contact me :)

Get the release and enjoy it !

Vincent

