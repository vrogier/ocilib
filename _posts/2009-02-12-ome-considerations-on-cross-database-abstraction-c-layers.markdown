---
author: admin
comments: true
date: 2009-02-12 21:04:57+00:00
layout: post
slug: ome-considerations-on-cross-database-abstraction-c-layers
title: Some considerations about cross-database C layers
wordpress_id: 267
categories:
- News
tags:
- cross
- database
- layer
---

Recently, i've tried to find and look at some "generic database layers" available in C.

I was interesting to see :



	
  * How the public APIs of these libraries were designed

	
  * the supported features (datatypes, backends specific features, ...)


I've looked at the code of a dozen of these C cross-database libraries (libsdb, openDBX, libdbi, ...) that claim to support Oracle...

These libraries look good, the concept is sexy ... but when i see the implementations of the underlying Oracle layers, things start to get bad :

	
  * most of the time, only strings or integer C scalar type are supported

	
  * no support for a significant part of Oracle datatypes (collections, blobs, refs, user object types, etc..)

	
  * no support for binding host variables

	
  * no support for unicode

	
  * no support for PL/SQL

	
  * no control at all of .... anything ... in terms of memory, server roundtrips, tuning, etc..


Well, these cross-platform layer can be appealing when dealing with simple tasks like logging data in databases. But it comes to make proper applications based on Oracle Database, they become inadequate.

That's why I strongly beleive that specific C drivers like OCILIB are justified and are necessary when it comes to efficiency, speed and support for DB vendor specificities.

C is still used for server side critical database processing applications. Thus, C DB layers must be efficient and powerful to get the best of the database.

Specific, optimized and full featured C DB drivers, like OCILIB for Oracle, are the right choice for writing applications that require processing, logic, speed and coverage for DB features.

Have a nice evening !

ps : btw, i started to write an Oracle libdbi driver on top of OCILIB ...
