#include "ocilib.h"

/* requires script demo/products.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    /* prepare and execute in 2 steps */

    OCI_Prepare(st, "delete from products where code = 1");
    OCI_Execute(st);
    printf("%d row deleted\n", OCI_GetAffectedRows(st));

    /* prepare/execute in 1 step */

    OCI_ExecuteStmt(st, "delete from products where code = 2");
    printf("%d row deleted\n", OCI_GetAffectedRows(st));

    OCI_Commit(cn);
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
