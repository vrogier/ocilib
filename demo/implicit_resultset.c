#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Int32 nb_rs = 0;

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

    OCI_StatementExecute(st,
        "declare"
        "  c1 sys_refcursor;"
        "  c2 sys_refcursor;"
        " begin"
        "  open c1 for select * from tabs;"
        "  dbms_sql.return_result (c1); "
        "  open c2 for select * from cols;"
        "  dbms_sql.return_result (c2); "
        "end;");

    OCI_Resultset  * rs = OCI_StatementGetResultset(st);
    while (rs)
    {
        printf("Iterating on resultset #%d\n", ++nb_rs);
        while (OCI_ResultsetFetchNext(rs))
        {
            printf("...%s\n", OCI_ResultsetGetString(rs, 0));
        }

        rs = OCI_StatementGetNextResultset(st);
    }

    printf("\nFound %d resultsets\n", nb_rs);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
