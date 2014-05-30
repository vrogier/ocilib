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
    
    OCI_ExecuteStmt(st, 
                    "declare"
                    "  c1 sys_refcursor;"
                    "  c2 sys_refcursor;"
                    " begin"
                    "  open c1 for select * from tabs;"
                    "  dbms_sql.return_result (c1); "
                    "  open c2 for select * from cols;"
                    "  dbms_sql.return_result (c2); "
                    "end;");

    rs = OCI_GetResultset(st);

    while (rs)
    {
        while (OCI_FetchNext(rs))
        {
            printf("%s\n", OCI_GetString(rs,1));
        }

        rs = OCI_GetNextResultset(st);
    }

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
