#include "ocilib.h"

/* requires script demo/products.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    OCI_Statement  * st = OCI_StatementCreate(cn);

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
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
