#include "ocilib.h"

/* requires script demo/products.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Resultset *rs;

    char rowid[OCI_SIZE_ROWID + 1] = "";

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_Immediate(cn, "select rowid from products where code = 1", OCI_ARG_TEXT, rowid);

    OCI_Prepare(st, "select code, name, rowid from products where rowid = :id");
    OCI_BindString(st, ":id", rowid, (unsigned int) strlen(rowid));
    OCI_Execute(st);
    rs = OCI_GetResultset(st);
    OCI_FetchNext(rs);
    printf("code [%d], name [%s], rowid [%s]", OCI_GetInt(rs, 1), OCI_GetString(rs, 2), OCI_GetString(rs, 3));

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
