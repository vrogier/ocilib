#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    int   err_type = OCI_ErrorGetType(err);
    char *err_msg  = OCI_ErrorGetString(err);

    printf("%s - %s\n", err_type == OCI_ERR_WARNING ? "warning" : "error", err_msg);
}

int main(void)
{
    OCI_Connection *cn;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    OCI_EnableWarnings(TRUE);

    cn = OCI_ConnectionCreate("db", "usr", "expired_pwd_in_grace_period", 
                              OCI_SESSION_DEFAULT);

    /* ... application code here ... */

    OCI_Cleanup();

 
    return EXIT_SUCCESS;
}

