---
author: admin
comments: true
date: 2008-08-09 17:34:37+00:00
layout: post
slug: ocilib-next-release-features
title: OCILIB Next Release features
wordpress_id: 47
categories:
- News
tags:
- features
- ideas
- version
---

I'm currently working on a list of new features for the next release of OCILIB.

Initially, I was focused on extending object API support, thread contextual error handing and Events support.

OCILIB already has some support for objects but not for collections and references.
Implementing theses features is a lot of work and i'll reserve it for the branch version v3.x.

So, for branch v2.x, at the moment, i'm thinking to focus on :




	
  * Session Pools (>=10g)

	
  * Events (>=8i)

	
  * Implicit fetch of rowid (>=11g)

	
  * Statement cache (=>9i)

	
  * Scrollable cursors (>= 9i)

	
  * Lob tuning (prefetch size,  chunck size, internal array allocation) (>=11g)



If you have any comments, ideas for OCILIB new functionalities, just let me know !
