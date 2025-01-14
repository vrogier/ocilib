---
author: admin
comments: true
date: 2024-01-15 10:00:00+01:00
layout: page
slug: download
title: Download
---

**Latest version is {{site.ocilib_current_release_version}} ({{site.ocilib_current_release_date}}).**
<br/>
<br/>
You can get OCILIB from the following sources:
<br/>
* [Windows, Linux/Unix distribution archives from GitHub]({{site.projecturl}}/releases/)
* [Windows Nuget package from nuget.org](https://www.nuget.org/packages/ocilib/)

## Installation

### Windows platforms

Since version 4.6.4, OCILIB is also available as a nuget packages available on nuget.org
Thus, in your VS project, add a dependency to the package "ocilib".
Your project will be automatically configured to reference ocilib headers and link ocilib shared librairies.
 
Otherwise, you can still process the old way:

{% highlight bash %}
- unzip the archive
- copy ocilib\include\ocilib.h to any place located in your includes path
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
  * Validated Oracle versions : 8, 9, 10, 11, 12, 18, 19, 21
  
Note: OCILIB is comptabile with:
  * Any existing Oracle version from 8i
  * Any future Oracle version
 