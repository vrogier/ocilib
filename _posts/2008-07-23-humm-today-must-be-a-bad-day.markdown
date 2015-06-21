---
author: admin
comments: true
date: 2008-07-23 07:13:15+00:00
layout: post
slug: humm-today-must-be-a-bad-day
title: Humm... today must be a bad day !
wordpress_id: 30
categories:
- News
- Updates
tags:
- bug
- runtime loading
---

I've found that in the OCILIB 2.5.0 releases build with the option OCI_IMPORT_LINKAGE (runtime loading of oracle shared lib, default option for the prebuilt Windows OCILIB Dlls), some symbols where not loaded anymore !

It affects multithreaded applications. The OCI functions related to mutexes were not loaded (the magic of cut'n paste...)

The following code disappeared between 2.4.0 and 2.5.0 in ocilib.c, OCI_Initialize():


    
       
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexInit", OCIThreadMutexInit,
                   OCITHREADMUTEXINIT);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexDestroy", OCIThreadMutexDestroy,
                   OCITHREADMUTEXDESTROY);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexAcquire", OCIThreadMutexAcquire,
                   OCITHREADMUTEXACQUIRE);
        LIB_SYMBOL(OCILib.lib_handle, "OCIThreadMutexRelease", OCIThreadMutexRelease,
                   OCITHREADMUTEXRELEASE);
    


So, i'll make a 2.5.1 release later today !
