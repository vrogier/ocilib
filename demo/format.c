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

    int code;
    char name[50];

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    /* sql format with params ----------------------------------------------- */

    code = 1;
    OCI_ExecuteStmtFmt(st, "select name from products where code = %i", code);

    rs = OCI_GetResultset(st);
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
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
