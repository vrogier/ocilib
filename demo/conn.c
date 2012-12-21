#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);

    printf("Server major    version : %i\n",   OCI_GetServerMajorVersion(cn));
    printf("Server minor    version : %i\n",   OCI_GetServerMinorVersion(cn));
    printf("Server revision version : %i\n\n", OCI_GetServerRevisionVersion(cn));
    printf("Connection      version : %i\n\n", OCI_GetVersionConnection(cn));

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
