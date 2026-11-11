#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Int32 res = 0;

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

    /* pl/sql call */

    OCI_StatementPrepare(st, "begin :res := trunc(sysdate+1)-trunc(sysdate-1); end;");
    OCI_StatementBindInt32(st, ":res", &res);
    OCI_StatementExecutePrepared(st);

    printf("result : %i\n", res);
 
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
