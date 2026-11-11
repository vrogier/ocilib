#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Int32 i, n;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    OCI_Statement * st = OCI_StatementCreate(cn);

    OCI_StatementExecute(st, "select * from user_tables");

    OCI_Resultset * rs = OCI_StatementGetResultset(st);
    n = OCI_ResultsetGetColumnCount(rs);

    for (i = 0; i < n; i++)
    {
        OCI_Column *col = OCI_ResultsetGetColumn(rs, i);

        printf("Field #%i : name '%s' - size %i\n", i, OCI_ColumnGetName(col), OCI_ColumnGetSize(col));
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
