#include "windows.h"
#include "process.h"
#include "ocilib.h"

/* Example on Microsoft platform */

static HANDLE evt;

void long_oracle_call(void *data)
{
    OCI_Statement *st  = OCI_StatementCreate((OCI_Connection *) data); 
    OCI_Resultset *rs;

    /* execute a query that takes a long time to process */

    OCI_ExecuteStmt(st, "select code, content from huge_table");

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        printf("%i - %s", OCI_GetInt(rs, 1), OCI_GetString(rs, 2));
    }

    SetEvent(evt);
}


int main(void)
{
   OCI_Connection *cn;

   if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    evt = CreateEvent(0, TRUE, FALSE, 0);

    _beginthread(long_oracle_call, 0, cn);

    if (WaitForSingleObject(evt, 10000) != WAIT_OBJECT_0)
    {
        OCI_Break(cn);
        Sleep(2000);
    }

    OCI_Cleanup();

 
    return EXIT_SUCCESS;
}
