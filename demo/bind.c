#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;

    int code;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
 
    st = OCI_StatementCreate(cn);
  
    OCI_Prepare(st, "delete from test_fetch where code = :code");
    OCI_BindInt(st, ":code", &code);
    
    code = 5;
    OCI_Execute(st);

    code = 12;
    OCI_Execute(st);
   
    OCI_Commit(cn);

    OCI_Cleanup();
 
    return EXIT_SUCCESS;
}
