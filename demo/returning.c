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

    OCI_StatementPrepare(st, "update products set name = name || ' updated' returning code into :code");
    OCI_StatementRegisterInt32(st, ":code");
    OCI_StatementExecutePrepared(st);

    OCI_Resultset  * rs = OCI_StatementGetResultset(st);
    while (OCI_ResultsetFetchNext(rs))
    {
        printf("element with code %d has been updated\n", OCI_ResultsetGetInt32(rs, 0));
    }

    printf("Number of affected rows : %d\n", OCI_StatementGetAffectedRows(st));

    OCI_StatementExecute(st, "select count(*) from products where name like '%updated'");
    rs = OCI_StatementGetResultset(st);
    OCI_ResultsetFetchNext(rs);
    printf("%i elements updated in DB\n", OCI_ResultsetGetInt32(rs, 0));

    OCI_ConnectionCommit(cn);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
