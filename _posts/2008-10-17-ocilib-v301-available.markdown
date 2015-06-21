---
author: admin
comments: true
date: 2008-10-17 14:10:35+00:00
layout: post
slug: ocilib-v301-available
title: OCILIB v3.0.1 available !
wordpress_id: 219
categories:
- News
- Updates
tags:
- bug memory leak sql
---

  

OCILIB v3.0.1 is now [_available for download_](http://orclib.sourceforge.net/download/) ! Here is the list of the main changes :



  
  * Fixed few memory leaks introduced in v3.0.0 (see changelog)

  
  * Fixed ocilib sources compilation with MSVC6++


See the [_complete v3.0.1 change log here_](http://orclib.sourceforge.net/public/changelogs/ocilib-changelog-v3.0.1.txt)

This release contains fixes some memory leaks introduced in v3.0.0. 

In v3.0.0, SQL statement strings were not freed any more. So, every time you were executing a different SQL statement, memory leak occurs !

You should download v3.0.1 if you are using v.3.0.0.

Have a nice hay.

