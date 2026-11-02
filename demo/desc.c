#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_TypeInfo *tbl;
    OCI_Int32 i, n;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate(env, "db", "usr", "pwd", OCI_SESSION_DEFAULT);
    tbl = OCI_TypeInfoGet(cn, "user_tables", OCI_TIF_VIEW);

    printf("Column Name                   Type      Length  Prec.   Scale   Null ?\n");
    printf("----------------------------------------------------------------------\n");

    n = OCI_TypeInfoGetColumnCount(tbl);

    for (i = 1; i <= n; i++)
    {
        OCI_Column *col = OCI_TypeInfoGetColumn(tbl, i);

        printf("%-30s%-10s%-8i%-8i%-8i%-s\n",
            OCI_GetColumnName(col),
            OCI_GetColumnSQLType(col),
            OCI_GetColumnSize(col),
            OCI_GetColumnPrecision(col),
            OCI_GetColumnScale(col),
            OCI_GetColumnNullable(col) == TRUE ? "Y" : "N");
    }

    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}