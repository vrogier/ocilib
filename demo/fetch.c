#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Resultset  *rs;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn  = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st  = OCI_StatementCreate(cn);

    OCI_ExecuteStmt(st, "select * from products");

    rs = OCI_GetResultset(st);
   
    while (OCI_FetchNext(rs))
        printf("code: %i, name %s\n", OCI_GetInt(rs, 1)  , OCI_GetString(rs, 2));

    printf("\n%d row(s) fetched\n", OCI_GetRowCount(rs));

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
