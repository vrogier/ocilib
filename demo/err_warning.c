#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    OCI_Int32   err_type = OCI_ErrorGetType(err);
    char *err_msg  = OCI_ErrorGetString(err);

    printf("%s - %s\n", err_type == OCI_ERR_WARNING ? "warning" : "error", err_msg);
}

int main(void)
{
    OCI_Connection *cn;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    OCI_EnableWarnings(env, TRUE);

    cn = OCI_ConnectionCreate(env, "db", "usr", "expired_pwd_in_grace_period", OCI_SESSION_DEFAULT);

    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}

