#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Resultset  *rs;
 
    int code = 1;
    char name[50];
  
    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
 
    /* sql format with params ----------------------------------------------- */

    OCI_ExecuteStmtFmt(st, "select article from test_fetch where code = %i", code);

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
        printf("article : %s\n", OCI_GetString(rs, 1));

    /* sql immediate (parse, exec, one fetch) ------------------------------- */

    OCI_Immediate(cn, "select code, article from test_fetch where code = 1", 
                  OCI_ARG_INT, &code, OCI_ARG_TEXT, name);

    printf("article : %s - code %i\n", name, code);

    /* sql immediate (parse, exec, one fetch) with params ------------------- */

    
    OCI_ImmediateFmt(cn, "select article from test_fetch where code = %i", 
                          code, OCI_ARG_TEXT, name);

    printf("article : %s\n", name);

    OCI_Cleanup();
 
    return EXIT_SUCCESS;
}
