#define OCI_API __stdcall

#include "ocilib.h"
#include "stdio.h"

#if defined(_WINDOWS)
#define sleep(x) Sleep(x*1000)
#else
#include <unistd.h>
#endif

#define wait_for_events() sleep(2)

#if defined(OCI_CHARSET_WIDE)
#define PRINT wprintf
#else
#define PRINT printf
#endif

void err_handler(OCI_Error* err)
{
    int err_type = OCI_ErrorGetType(err);

    PRINT(OTEXT("\n"));

    if (err_type == OCI_ERR_WARNING)
    {
        PRINT(OTEXT("> WARNING : "));
    }
    else
    {
        PRINT(OTEXT("> ERROR   : "));
    }

    PRINT(OCI_ErrorGetString(err));
}

void event_handler(OCI_Event* event)
{
    unsigned int type = OCI_EventGetType(event);
    unsigned int op = OCI_EventGetOperation(event);
    OCI_Subscription* sub = OCI_EventGetSubscription(event);

    PRINT(OTEXT("** Notification      : %s\n\n"), OCI_SubscriptionGetName(sub));
    PRINT(OTEXT("...... Database      : %s\n"), OCI_EventGetDatabase(event));

    switch (type)
    {
    case OCI_ENT_STARTUP:
        PRINT(OTEXT("...... Event         : Startup\n"));
        break;
    case OCI_ENT_SHUTDOWN:
        PRINT(OTEXT("...... Event         : Shutdown\n"));
        break;
    case OCI_ENT_SHUTDOWN_ANY:
        PRINT(OTEXT("...... Event         : Shutdown any\n"));
        break;
    case OCI_ENT_DROP_DATABASE:
        PRINT(OTEXT("...... Event         : drop database\n"));
        break;
    case OCI_ENT_DEREGISTER:
        PRINT(OTEXT("...... Event         : deregister\n"));
        break;
    case OCI_ENT_OBJECT_CHANGED:

        PRINT(OTEXT("...... Event         : object changed\n"));
        PRINT(OTEXT("........... Object   : %s\n"), OCI_EventGetObject(event));

        switch (op)
        {
        case OCI_ONT_INSERT:
            PRINT(OTEXT("........... Action   : insert\n"));
            break;
        case OCI_ONT_UPDATE:
            PRINT(OTEXT("........... Action   : update\n"));
            break;
        case OCI_ONT_DELETE:
            PRINT(OTEXT("........... Action   : delete\n"));
            break;
        case OCI_ONT_ALTER:
            PRINT(OTEXT("........... Action   : alter\n"));
            break;
        case OCI_ONT_DROP:
            PRINT(OTEXT("........... Action   : drop\n"));
            break;
        }

        if (op < OCI_ONT_ALTER)
            PRINT(OTEXT("........... Rowid    : %s\n"), OCI_EventGetRowid(event));

        break;
    }

    PRINT(OTEXT("\n"));
}


int main(void)
{
    OCI_Connection* con;
    OCI_Subscription* sub;
    OCI_Statement* st;

    PRINT(OTEXT("=> Initializing OCILIB in event mode...\n\n"));
    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_EVENTS))
    {
        return EXIT_FAILURE;
    }

    PRINT(OTEXT("=> Connecting to usr@db...\n\n"));
    con = OCI_ConnectionCreate(OTEXT("db18c"), OTEXT("usr"), OTEXT("pwd"), OCI_SESSION_DEFAULT);
    OCI_SetAutoCommit(con, TRUE);

    PRINT(OTEXT("=> Creating statement...\n\n"));
    st = OCI_StatementCreate(con);

    PRINT(OTEXT("=> Creating tables...\n\n"));
    OCI_SetErrorHandler(NULL);
    OCI_ExecuteStmt(st, OTEXT("drop table table1"));
    OCI_ExecuteStmt(st, OTEXT("drop table table2"));
    OCI_SetErrorHandler(err_handler);
    OCI_ExecuteStmt(st, OTEXT("create table table1(code number)"));
    OCI_ExecuteStmt(st, OTEXT("create table table2(str varchar2(10))"));

    PRINT(OTEXT("=> Registering subscription...\n\n"));
    sub = OCI_SubscriptionRegister(con, OTEXT("sub-00"), OCI_CNT_ALL, event_handler, 0, 0);

    PRINT(OTEXT("=> Adding queries to be notified...\n\n"));
    OCI_Prepare(st, OTEXT("select * from table1"));
    OCI_SubscriptionAddStatement(sub, st);
    OCI_Prepare(st, OTEXT("select * from table2"));
    OCI_SubscriptionAddStatement(sub, st);

    PRINT(OTEXT("=> Executing some DDL operation...\n\n"));
    OCI_ExecuteStmt(st, OTEXT("alter table table1 add price number"));
    wait_for_events();

    PRINT(OTEXT("=> OCI_SubscriptionUnregister...\n\n"));
    OCI_SubscriptionUnregister(sub);
    OCI_StatementFree(st);

    PRINT(OTEXT("=> Creating statement...\n\n"));
    st = OCI_StatementCreate(con);

    PRINT(OTEXT("=> OCI_SubscriptionRegister...\n\n"));
    sub = OCI_SubscriptionRegister(con, OTEXT("sub-01"), OCI_CNT_ALL, event_handler, 0, 0);
    OCI_Prepare(st, OTEXT("select * from table1"));
    OCI_SubscriptionAddStatement(sub, st);
    OCI_Prepare(st, OTEXT("select * from table2"));
    OCI_SubscriptionAddStatement(sub, st);

    PRINT(OTEXT("=> Executing some DDL operation...\n\n"));
    OCI_ExecuteStmt(st, OTEXT("alter table table1 add price2 number"));
    wait_for_events();

    PRINT(OTEXT("=> Executing some DML operation...\n\n"));
    OCI_ExecuteStmt(st, OTEXT("insert into table1 values(1, 10.5, 21)"));
    OCI_ExecuteStmt(st, OTEXT("insert into table2 values('shoes')"));
    OCI_ExecuteStmt(st, OTEXT("update table1 set price = 13.5 where code = 1"));
    OCI_ExecuteStmt(st, OTEXT("delete from table2 "));
    wait_for_events();

    PRINT(OTEXT("=> Droping tables...\n\n"));
    OCI_ExecuteStmt(st, OTEXT("drop table table1"));
    OCI_ExecuteStmt(st, OTEXT("drop table table2"));
    wait_for_events();

    PRINT(OTEXT("=> Unregistering subscription...\n\n"));
    OCI_SubscriptionUnregister(sub);

    PRINT(OTEXT("=> Disconnecting from DB...\n\n"));
    OCI_ConnectionFree(con);

    PRINT(OTEXT("=> Cleaning up OCILIB resources...\n\n"));
    OCI_Cleanup();

    PRINT(OTEXT("=> Done...\n\n"));
    return EXIT_SUCCESS;
}
