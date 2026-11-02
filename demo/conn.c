#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate(env, "db", "usr", "pwd", OCI_SESSION_DEFAULT);

    printf("Server major    version : %i\n", OCI_GetServerMajorVersion(cn));
    printf("Server minor    version : %i\n", OCI_GetServerMinorVersion(cn));
    printf("Server revision version : %i\n\n", OCI_GetServerRevisionVersion(cn));
    printf("Connection      version : %i\n\n", OCI_GetVersionConnection(cn));

    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
