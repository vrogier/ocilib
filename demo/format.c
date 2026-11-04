#include "ocilib.h"

/* requires script demo/products.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{

    char name[50];

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

    /* sql format with params ----------------------------------------------- */

    OCI_Int32 code = 1;
    OCI_ExecuteStmtFmt(st, "select name from products where code = %i", code);

    OCI_Resultset  * rs = OCI_GetResultset(st);
    while (OCI_FetchNext(rs))
    {
        printf("article : %s\n", OCI_GetString(rs, 1));
    }

    /* sql immediate (parse, exec, one fetch) ------------------------------- */

    OCI_Immediate(cn, "select name from products where code = 2", OCI_ARG_TEXT, name);
    printf("article : %s\n", name);

    /* sql immediate (parse, exec, one fetch) with params ------------------- */

    code = 1;
    OCI_ImmediateFmt(cn, "select name from products where code = %i", code, OCI_ARG_TEXT, name);
    printf("article : %s\n", name);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
