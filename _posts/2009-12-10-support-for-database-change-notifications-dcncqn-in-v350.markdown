---
author: admin
comments: true
date: 2009-12-10 09:09:42+00:00
layout: post
slug: support-for-database-change-notifications-dcncqn-in-v350
title: Support for Database Change Notifications (DCN/CQN) in v3.5.0 !
wordpress_id: 357
categories:
- Code
- News
- Updates
tags:
- 3.5.0
- cache
- change
- CQN
- DCN
- notification
- query
---

Hi all,

Final version of OCILIB v3.5.0 supports Oracle Database Change Notifications (or Continuous Query Notification).

It allows a program to be notified by Oracle servers of any DML / DDL actions on the underlying objects part of given tables or queries.

Rowids of modified rows can also be retrieved.

This feature is really interesting in application that caches data or need to be notified on some data sources changes.

Here is a example program that illustrates the OCILIB implementation of ORACLE DCN/CQN :

**[demo code edited on 2009-12-11 with final approved API]**





    
    
    #include "ocilib.h"
    
    #ifdef _WINDOWS
      #define sleep(x) Sleep(x*1000)
    #endif
    
    #define wait_for_events() sleep(5)
    
    void event_handler(OCI_Event *event);
    void error_handler(OCI_Error *err);
    
    int main(void)
    {
        OCI_Connection   *con;
        OCI_Subscription *sub;
        OCI_Statement    *st;
     
        printf("=> Initializing OCILIB in event mode...\n\n");
    
        if (!OCI_Initialize(error_handler, NULL, OCI_ENV_EVENTS))
            return EXIT_FAILURE;
    
        printf("=> Connecting to winrest@winrest...\n\n");
    
        con = OCI_ConnectionCreate("winrest", "winrest", "xxx", OCI_SESSION_DEFAULT);
        
        OCI_SetAutoCommit(con, TRUE);
     
        printf("=> Creating statement...\n\n");
    
        st  = OCI_StatementCreate(con);
    
        printf("=> Creating tables...\n\n");
    
        OCI_ExecuteStmt(st, "create table table1(code number)");
        OCI_ExecuteStmt(st, "create table table2(str varchar2(10))");
    
        printf("=> Registering subscription...\n\n");
    
        sub = OCI_SubscriptionRegister(con, "sub-00", OCI_CNT_ALL, event_handler, 5468, 0);
    
        printf("=> Adding queries to be notified...\n\n");
    
        OCI_Prepare(st, "select * from table1");
        OCI_SubscriptionAddStatement(sub, st);
    
        OCI_Prepare(st, "select * from table2");
        OCI_SubscriptionAddStatement(sub, st);
    
        printf("=> Executing some DDL operation...\n\n");
    
        OCI_ExecuteStmt(st, "alter table table1 add price number");
    
        wait_for_events();
     
        printf("=> Executing some DML operation...\n\n");
    
        OCI_ExecuteStmt(st, "insert into table1 values(1, 10.5)");
        OCI_ExecuteStmt(st, "insert into table2 values('shoes')");
    
        OCI_ExecuteStmt(st, "update table1 set price = 13.5 where code = 1");
        OCI_ExecuteStmt(st, "delete from table2 ");
    
        wait_for_events();
    
        printf("=> Droping tables...\n\n");
    
        OCI_ExecuteStmt(st, "drop table table1");
        OCI_ExecuteStmt(st, "drop table table2");
    
        wait_for_events();
    
        printf("=> Disconnecting from DB...\n\n");
    
        OCI_ConnectionFree(con);
    
        printf("=> Stopping the remote database...\n\n");
    
        OCI_DatabaseShutdown("winrest", "sys", "xxx",   
                             OCI_SESSION_SYSDBA,
                             OCI_DB_SDM_FULL,
                             OCI_DB_SDF_IMMEDIATE);
    
        wait_for_events();;
    
        printf("=> Starting the remote database...\n\n");
    
        OCI_DatabaseStartup("winrest", "sys", "xxx",   
                             OCI_SESSION_SYSDBA,
                             OCI_DB_SPM_FULL,
                             OCI_DB_SPF_FORCE,
                             NULL);
    
        wait_for_events();
    
        printf("=> Unregistering subscription...\n\n");
    
        OCI_SubscriptionUnregister(sub);
    
        printf("=> Cleaning up OCILIB resources...\n\n");
    
        OCI_Cleanup();
    
        printf("=> Done...\n\n");
    
        return EXIT_SUCCESS;
    }
    
    void error_handler(OCI_Error *err)
    {
        int         err_type = OCI_ErrorGetType(err);
        const char *err_msg  = OCI_ErrorGetString(err);
    
        printf("** %s - %s\n", err_type == OCI_ERR_WARNING ? "Warning" : "Error", err_msg);
    }
    
    void event_handler(OCI_Event *event)
    {
        unsigned int type     = OCI_EventGetType(event);
        unsigned int op       = OCI_EventGetOperation(event);
        OCI_Subscription *sub = OCI_EventGetSubscription(event);
    
        printf("** Notification      : %s\n\n", OCI_SubscriptionGetName(sub));
        printf("...... Database      : %s\n",   OCI_EventGetDatabase(event));
    
        switch (type)
        {
            case OCI_ENT_STARTUP:
                printf("...... Event         : Startup\n");
                break;
            case OCI_ENT_SHUTDOWN:
                printf("...... Event         : Shutdown\n");
                break;
            case OCI_ENT_SHUTDOWN_ANY:
                printf("...... Event         : Shutdown any\n");
                break;
            case OCI_ENT_DROP_DATABASE:
                printf("...... Event         : drop database\n");
                break;
            case OCI_ENT_DEREGISTER:
                printf("...... Event         : deregister\n");
                break;
             case OCI_ENT_OBJECT_CHANGED:
                
                printf("...... Event         : object changed\n");
                printf("........... Object   : %s\n", OCI_EventGetObject(event));
          
                switch (op)
                {
                    case OCI_ONT_INSERT:
                        printf("........... Action   : insert\n");
                        break;
                    case OCI_ONT_UPDATE:
                        printf("........... Action   : update\n");
                        break;
                    case OCI_ONT_DELETE:
                        printf("........... Action   : delete\n");
                        break;
                    case OCI_ONT_ALTER:
                        printf("........... Action   : alter\n");
                        break;
                    case OCI_ONT_DROP:
                        printf("........... Action   : drop\n");
                        break;
                }
                        
                if (op < OCI_ONT_ALTER)
                    printf("........... Rowid    : %s\n",  OCI_EventGetRowid(event));
            
                break;
        }
        
        printf("\n");
    }
    
    



And the outpout is :


    
    
    
    => Initializing OCILIB in event mode...
    
    => Connecting to winrest@winrest...
    
    => Creating statement...
    
    => Creating tables...
    
    => Registering subscription...
    
    => Adding queries to be notified...
    
    => Executing some DDL operation...
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : object changed
    ........... Object   : WINREST.TABLE1
    ........... Action   : alter
    
    => Executing some DML operation...
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : object changed
    ........... Object   : WINREST.TABLE1
    ........... Action   : insert
    ........... Rowid    : AAADJMAAEAAAw49AAA
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : object changed
    ........... Object   : WINREST.TABLE2
    ........... Action   : insert
    ........... Rowid    : AAADJNAAEAAAw5FAAA
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : object changed
    ........... Object   : WINREST.TABLE1
    ........... Action   : update
    ........... Rowid    : AAADJMAAEAAAw49AAA
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : object changed
    ........... Object   : WINREST.TABLE2
    ........... Action   : delete
    ........... Rowid    : AAADJNAAEAAAw5FAAA
    
    => Droping tables...
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : object changed
    ........... Object   : WINREST.TABLE1
    ........... Action   : alter
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : object changed
    ........... Object   : WINREST.TABLE1
    ........... Action   : drop
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : object changed
    ........... Object   : WINREST.TABLE2
    ........... Action   : alter
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : object changed
    ........... Object   : WINREST.TABLE2
    ........... Action   : drop
    
    => Disconnecting from DB...
    
    => Stopping the remote database...
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : Shutdown
    
    => Starting the remote database...
    
    ** Notification      : sub-00
    
    ...... Database      : WINREST
    ...... Event         : Startup
    
    => Unregistering subscription...
    
    => Cleaning up OCILIB resources...
    
    => Done...
    
    




Version OCILIB 3.5.0 is still in validation phase and should be released soon !


