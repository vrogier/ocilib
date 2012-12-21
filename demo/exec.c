#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
  
    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
 
    /* prepare and execute in 2 steps */

    OCI_Prepare(st, "delete from test_fetch where code > 10");
    OCI_Execute(st);

    /* prepare/execute in 1 step */

    OCI_ExecuteStmt(st, "delete from test_fetch where code > 1");

    printf("%d row deleted", OCI_GetAffectedRows(st));

    OCI_Commit(cn);

    OCI_Cleanup();
 
    return EXIT_SUCCESS;
}
