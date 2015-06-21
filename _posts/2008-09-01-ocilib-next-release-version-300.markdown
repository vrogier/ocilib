---
author: admin
comments: true
date: 2008-09-01 07:45:51+00:00
layout: post
slug: ocilib-next-release-version-300
title: 'OCILIB next release : version 3.0.0'
wordpress_id: 133
categories:
- News
tags:
- release
- Updates
---

OCILIB v3.0.0 is coming soon !

We swicthed from v2.5.1 to v3.0.0 because this new version will add some important features :





  * Support for Oracle scrollable cursors : OCI_Resultset object can now be fetched in both direction and the current position can be set to any absolute or relative index


  * Full support for Oracle collections (VARRAYs and Nested tables)


  * Extension of internal implicit datatypes conversion : what ever is the real type of an item (column in resultset, object attribute or collection element), OCILIB will return the data in the form of the requested datatype if the conversion is possible


  * New memory checks for user unfreed handles, objects and descriptors


  * Fixed connection pools internal memory management
  * 

  * A lot of miscelleanous changes were made


  * A lot of miscelleanous fixes were made



Version 3.0.0 will released by mid september.




