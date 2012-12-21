#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf(
                "code  : ORA-%05i\n"
                "msg   : %s\n"
                "sql   : %s\n",
                OCI_ErrorGetOCICode(err), 
                OCI_ErrorGetString(err),
                OCI_GetSql(OCI_ErrorGetStatement(err))
           );
}

int main(void)
{
    OCI_Connection *cn;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("wrong_db", "wrong_usr", "wrong_pwd", 
                              OCI_SESSION_DEFAULT);

    /* ... application code here ... */

    OCI_Cleanup();

 
    return EXIT_SUCCESS;
}
