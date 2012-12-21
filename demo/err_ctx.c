#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT | OCI_ENV_CONTEXT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("wrong_db", "wrong_usr", "wrong_pwd", 
                              OCI_SESSION_DEFAULT);

    if (cn == NULL)
    {
        OCI_Error *err = OCI_GetLastError();

        printf("errcode %d, errmsg %s", OCI_ErrorGetOCICode(err), 
                                        OCI_ErrorGetString(err));
    }

    OCI_Cleanup();

 
    return EXIT_SUCCESS;
}
