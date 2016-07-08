#include "ocilib.h"

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

    OCI_ExecuteStmt(st, "select rownum, cursor(select sysdate from dual) from (select 1 from dual connect by level <= 10)");

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        OCI_Statement *st2 = OCI_GetStatement(rs, 2);
        OCI_Resultset *rs2 = OCI_GetResultset(st2);

        while (OCI_FetchNext(rs2))
        {
            printf("index: %d, date: %s\n",  OCI_GetInt(rs, 1), OCI_GetString(rs2, 1));
        }
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
