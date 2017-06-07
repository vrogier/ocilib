---
author: admin
comments: true
date: 2017-06-07 22:35:00+01:00
layout: page
slug: home
title: Home
---

Welcome to **OCILIB - C and C++ Driver for Oracle** website !

OCILIB is an **open source** and **cross platform** Oracle **Driver** that delivers efficient access to **Oracle databases**.

The OCILIB library  :

  * offers a rich, full featured and easy to use API
  * runs on all Oracle platforms
  * is written in pure ISO C99 code with native ISO C Unicode support
  * provides also a C++ API written in standard C++03
  * Enables high productivity
  * encapsulates OCI (Oracle Call Interface)
  * is the most complete available OCI wrapper

**OCILIB** is developed by [Vincent Rogier]({{site.baseurl}}/about/). Get the latest package, install and enjoy it !
  
## Latest version

The latest version is [version 4.3.3 (2017-06-07)]({{site.projecturl}}/releases/)
<br/>
<br/>
[![Build Status](https://img.shields.io/travis/vrogier/ocilib/master.svg)](https://travis-ci.org/vrogier/ocilib)
[![Code Analysis](https://img.shields.io/coverity/scan/8019.svg)](https://scan.coverity.com/projects/vrogier-ocilib)
<br/>
<br/>
Github downloads: <img alt="Github downloads" src="https://img.shields.io/github/downloads/vrogier/ocilib/total.svg"/>
<br/>
<br/>
SF.NET downloads: <img alt="SF.NET downloads" src="https://img.shields.io/sourceforge/dt/orclib.svg"/>
<br/>
 
## Recent Posts

{% for post in site.posts limit:5 site.recent_posts %}
  *  [{% if site.titlecase %}{{ post.title | titlecase }}{% else %}{{ post.title }}{% endif %}]({{ site.baseurl }}{{ post.url }})
{% endfor %}
 
## Testimonies
  
>"OCILIB code is truly a really nice piece of code"
><br/>  
>Pete Finnigan, CEO and Founder of PeteFinnigan.com Limited
>Specialists in database security.

>"About Ocilib, i am working in Telecom Italia S.p.A. and we are using your Ocilib library compiled on HP Itanium System in our Interconnection System in Brazil. In a process to extract daily traffic data from partitioned oracle tables, about 30.000.000 records for each day. This process wrote to use dynamic sql 4 Method had a big performance problem and i have sponsored your ocilib and we have improved +60% performance. I am using your ocilib also in a windows desktop application on XP system, with good results. Thanks Vincent."
><br/>  
>Giovanni Palleschi
>Software Engeener at Telecom Italia S.p.A
      
>"I have been asked to replay 90 minutes of captured SQL code back into a database. The oracle database is one of the largest in the southern hemisphere apparently [....] 90 minutes of captured data is about 2 terabytes of information [...] The replay is all happening on a single client Solaris 64 bit machine [...] From a parent controlling process I spawn 1300 client processes that each connect to the database[...]"
>all calls seems to happen very, very quickly and that, in my opinion, a significant reason the project is succeeding is due to your library and your commitment to quality code.
>Thank you! Well Done! Bravo!
>The system I have written has been designed to really hammer the database and your code stands up very well.
><br/>   
>James D'Arcy
>CommonwealthBank

