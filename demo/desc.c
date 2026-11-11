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
    OCI_TypeInfo * tbl = OCI_TypeInfoGet(cn, "user_tables", OCI_TIF_VIEW);

    printf("Column Name                   Type      Length  Prec.   Scale   Null ?\n");
    printf("----------------------------------------------------------------------\n");

    n = OCI_TypeInfoGetColumnCount(tbl);

    for (i = 0; i < n; i++)
    {
        OCI_Column *col = OCI_TypeInfoGetColumn(tbl, i);

        printf("%-30s%-10s%-8i%-8i%-8i%-s\n",
            OCI_ColumnGetName(col),
            OCI_ColumnGetSqlType(col),
            OCI_ColumnGetSize(col),
            OCI_ColumnGetPrecision(col),
            OCI_ColumnGetScale(col),
            OCI_ColumnGetNullable(col) == TRUE ? "Y" : "N");
    }

    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}