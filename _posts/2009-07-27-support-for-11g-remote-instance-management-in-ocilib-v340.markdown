---
author: admin
comments: true
date: 2009-07-27 10:26:57+00:00
layout: post
slug: support-for-11g-remote-instance-management-in-ocilib-v340
title: Support for 11g remote Instance management in OCILIB v3.4.0
wordpress_id: 329
categories:
- Code
- News
- Updates
tags:
- 11g
- 3.4.0
- instance
- remote
- shutdown
- startup
---

Hello,

OCILIB v3.4.0 is coming soon :)

This release fixes 3 bugs and adds support for Oracle 11g remote Instance startup/shutdown.

Here is a sample app that startups a remote instance and then shuts it down :
  



    
    
    #include "ocilib.h"
    
    int main(int argc, char **argv)
    { 
        OCI_Initialize(error, NULL,  OCI_ENV_DEFAULT);
    
        /* start remote instance */
    
        OCI_DatabaseStartup("db", 
                            "sys_user", 
                            "sys_pwd",                
                            OCI_SESSION_SYSDBA, 
                            OCI_DB_SPM_FULL,
                            OCI_DB_SPF_DEFAULT,
                            NULL);
    
    
         /* shutdown remote instance */
    
        OCI_DatabaseShutdown("db", 
                            "sys_user", 
                            "sys_pwd", 
                             OCI_SESSION_SYSDBA,
                             OCI_DB_SDM_FULL,
                             OCI_DB_SDF_DEFAULT);
    
        OCI_Cleanup();
        
        return EXIT_SUCCESS;
    }   
    



**Isn't cool ?**

Have a nice day..

