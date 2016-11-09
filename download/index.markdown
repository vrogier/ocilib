---
author: admin
comments: true
date: 2016-11-09 22:35:00+01:00
layout: page
slug: download
title: Download
wordpress_id: 68
---

* Get the lastest package from GitHub :[version 4.3.0 (2016-11-09)]({{site.projecturl}}/releases/)

## Installation

### Windows platforms

{% highlight bash %}
- unzip the archive
- copy ocilib\include\ocilib.h to any place located in your include's path
- copy ocilib\lib32|64\ocilib[x].lib to any place located in your libraries path
- copy ocilib\lib32|64\ocilib[x].dll to any place located in your windows path
{% endhighlight %}

### GNU (Unix / Linux) platforms

{% highlight bash %}
untar the archive	
  
> cd ocilib-x.y.z
> ./configure
> make
> make install (you might need to `su` to make install)
{% endhighlight %}

Make sure Oracle and OCILIB libraries paths are defined in your shared library environment variable
You need to provide extra configure parameters when using Instant Clients - [see Installation section]({{site.baseurl}}/doc/html/group___ocilib_c_api_installation.html))

### Compatibilities
	
  * C API is pure ISO C99 and should compile with any C90 compliant compiler	
  * C++ API is pure ISO C++03 and should compile with any C++03 compliant compiler
  * Validated platforms : Windows, Linux, HP/UX, Solaris, AIX, Mac OS X, OpenVMS
  * Validated C compilers : Microsoft C compiler, GCC, MinGW, IMB XLC, LabView, various CCs
  * Validated C++ compilers : Microsoft C++ compiler, G++, Clang
  * Validated Oracle versions : 8i, 9i, 10g, 11g