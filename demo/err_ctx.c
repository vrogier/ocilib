#include "ocilib.h"

int main(void)
{

    OCI_Environment *env = OCI_EnvironmentCreate(NULL, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    OCI_Connection * cn = OCI_ConnectionCreate(env, "db", "wrong_usr", "wrong_pwd", OCI_SESSION_DEFAULT);

    if (cn == NULL)
    {
        OCI_Error *err = OCI_GetLastError();

        printf("errcode %d, errmsg %s", OCI_ErrorGetCode(err), OCI_ErrorGetString(err));
    }

    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
