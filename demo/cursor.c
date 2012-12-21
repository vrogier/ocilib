#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Resultset  *rs;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
 
    st = OCI_StatementCreate(cn);

    OCI_ExecuteStmt(st, "select article, cursor(select sysdate from dual) from test_fetch");
    
    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        OCI_Statement *st2 = OCI_GetStatement(rs, 2);
        OCI_Resultset *rs2 = OCI_GetResultset(st2);

        while (OCI_FetchNext(rs2))
        {
            printf("article : %s, sysdate : %s\n",     
                   OCI_GetString(rs, 1),
                   OCI_GetString(rs2, 1));
        }
    }    

 
    return EXIT_SUCCESS;
}
