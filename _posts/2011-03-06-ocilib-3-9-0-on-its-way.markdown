---
author: admin
comments: true
date: 2011-03-06 18:35:31+00:00
layout: post
slug: ocilib-3-9-0-on-its-way
title: OCILIB 3.9.0 on its way...
wordpress_id: 616
categories:
- News
tags:
- bind
- describe
- failover
- high availability
- taf
- xa
---

Hi all,

I'm still working on finishing the version 3.9.0 of OCILIB.  
All implementation is done, still some work to do on the documentation !

Here is the summary of this version :





  * Added support Oracle XA 


  * Added support for statement cache 


  * Added support for TAF (Transparent Application Failover) and HA (High Availability)  


  * Added support for new connection attributes introduced in Oracle 10gR2 


  * Improved bind tuning 


  * Miscellaneous new calls 


  * Miscellaneous changes 


  * Miscellaneous fixes 



Here is the changelog of this version

    
    
    2011-03-XX Version 3.9.0 Vincent Rogier vince.rogier@ocilib.net
    
        * Added support Oracle XA
    
          - Oracle XA intregration in OCLIB is transparent by using OCI_ConnectionCreate() and requires just :
          
             - an XA db name for parameter "db"
             - an extra flag OCI_SESSION_XA for parameter "mode"
             - no user or password
             
          - See the OCI_ConnnectionCreate() documentation for the XA open string needed by OCILIB to pass to the TMP 
    
        * Added support for statement cache
    
          - Statement cache is now automatically enabled when using an Oracle client >= 9.2
          - Added statement cache size customization when using an Oracle client >= 10.1 :
            
            - Added OCI_SetStatementCacheSize()
            - Added OCI_GetStatementCacheSize()
            - Added OCI_PoolSetStatementCacheSize()
            - Added OCI_PoolGetStatementCacheSize()
    
        * Added support for TAF (Transparent Application Failover) and HA (High Availability) 
        
          - The following handler has been added when using an Oracle client >= 10.2
    
            - Added OCI_IsTAFCapable()
            - Added OCI_SetTAFHandler()
            - Added OCI_SetHAHandler()
    
        * Added support for Connection attributes introduced in Oracle 10gR2
    
          - Added OCI_GetDDBName()
          - Added OCI_GetInstanceName()
          - Added OCI_GetServiceName()
          - Added OCI_GetServerName()
          - Added OCI_GetDomainName()
          - Added OCI_GetInstanceStartTime()
    
        * Improved bind tuning. Now it is possible to set the bind direction mode (in, out, in/out) for better performances
        
          - Added OCI_BindSetDirection()
          - Added OCI_BindGetDirection()
    
        * Miscellaneous new calls :
    
          -  Added OCI_Describe() to enable the retrieving of an OCI_Resultset object for "select" statement without executing the SQL
    
        * Miscellaneous changes :
    
          - Re-enforced checks on OCI_Statement handle related methods that checks the statement state (prepared, executed, parsed,...) that now can throw more exceptions
          - Manual update of source code formatted with Uncrustify (wrong indentation of preprocessor directives)
    
        * Miscellaneous fixes :
    
          - Fixed OCI_ImmediateFmt() : the method returned FALSE even on success
          - Fixed 11Gr2 compile detection    
          - Fixed OCI_PoolGetConnection() : OCILIB tried to set the driver name attribute (appeared in 10gR2) on the session that is not allowed in 11gR2
          - Fixed OCI_SetFormatDate() : fixed macros parameters declaration
          - Fixed OCI_GetColumnNullable() returned value
          - Fixed OCI_Parse() wrong internal mode
          - Fixed OCI_ConnectionCreate() : unfreed handles not freed if connection failed
          - Fixed OCI_SetPrefetchSize(), OCI_SetPrefetchMemory() : the given value was not really passed to Oracle
          - Fixed OCI_TypeInfoGet() called internally for fetch using object from different schema
          - Fixed OCI_StringGetFromAttrHandle() allocation of insufficient buffer in Unicode mode
          - Fixed OCI_ObjectSetxxx() could return true if attribute not found
          - Fixed OCI_Parse() : the method was internally doing a describe operation instead of just a parse (which gave same results from an end user view)
    
    
