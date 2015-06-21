---
author: admin
comments: true
date: 2010-10-16 17:19:44+00:00
layout: post
slug: oracle-advanced-queues-administration-in-ocilib-v3-8-0
title: Oracle Advanced Queues Administration in OCILIB v3.8.0
wordpress_id: 570
categories:
- Code
- News
tags:
- 3.8.0
- administration
- advanded queues
- OCILIB
---

Hi,


Here is a demonstration application showing Oracle Advanced Queues Administration using OCILIB v.3.8.0.

As mentioned in my previous post, Oracle AQ administration is now available in C/C++ with OCILIB :)


    
    
    
    #include "ocilib.h"
    
    int main(void)
    {
        OCI_Connection *cn;
    
        if (!OCI_Initialize(ocilib_err_handler, NULL, OCI_ENV_DEFAULT))
            return EXIT_FAILURE;
    
        /* connect to db */
        cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    
        /* create queue table "TEST_QUEUE_TABLE" */
        OCI_QueueTableCreate(cn, "TEST_QUEUE_TABLE", "MY_MESSAGE", 
                             NULL, NULL, TRUE, 1, NULL, 
                             0, 0, NULL);
                             
         /* alter queue table "TEST_QUEUE_TABLE" - add comment on table */
        OCI_QueueTableAlter(cn, "TEST_QUEUE_TABLE", "MY_COMMENT", 0, 0);
        
        /* purge messages in  queue table "TEST_QUEUE_TABLE" */   
        OCI_QueueTablePurge(cn, "TEST_QUEUE_TABLE", NULL, FALSE, 0);
        
        /* migrate queue table "TEST_QUEUE_TABLE" to Oracle 10.1 version */   
        OCI_QueueTableMigrate(cn, "TEST_QUEUE_TABLE", "10.1");
        
        /* create queue "TEST_QUEUE" using queue table "TEST_QUEUE_TABLE" */
        OCI_QueueCreate(cn, "TEST_QUEUE",  "TEST_QUEUE_TABLE", 
                        0, 0, 0, 0, FALSE, "MY_COMMENT");   
        
        /* alter queue "TEST_QUEUE" - modify comment */
         OCI_QueueAlter(cn, "TEST_QUEUE", 0, 0, 0, "MY_NEW_COMMENT");   
       
         /* start queue "TEST_QUEUE" */  
        OCI_QueueStart(cn, "TEST_QUEUE", TRUE, TRUE);
        
        /* stop queue "TEST_QUEUE" */
        OCI_QueueStop(cn, "TEST_QUEUE", TRUE, TRUE, FALSE);
        
        /* drop queue "TEST_QUEUE" */
        OCI_QueueDrop(cn, "TEST_QUEUE");
        
        /* drop queue table "TEST_QUEUE_TABLE" */
        OCI_QueueTableDrop(cn, "TEST_QUEUE_TABLE", TRUE);
        
        /* disconnect from db */
        OCI_ConnectionFree(cn);
        
        OCI_Cleanup();
    
        return EXIT_SUCCESS;
    }
    
    







