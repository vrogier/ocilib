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
    OCI_Resultset  *rs;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_Prepare(st, "update products set name = name || ' updated' returning code into :code");
    OCI_RegisterInt(st, ":code");
    OCI_Execute(st);

    rs = OCI_GetResultset(st);
    while (OCI_FetchNext(rs))
    {
        printf("element with code %d has been updated\n", OCI_GetInt(rs, 1));
    }

    printf("Number of affected rows : %d\n", OCI_GetAffectedRows(st));

    OCI_ExecuteStmt(st, "select count(*) from products where name like '%updated'");
    rs = OCI_GetResultset(st);
    OCI_FetchNext(rs);
    printf("%i elements updated in DB\n", OCI_GetInt(rs, 1));

    OCI_Commit(cn);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
