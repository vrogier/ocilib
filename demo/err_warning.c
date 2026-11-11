#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    OCI_Int32   err_type = OCI_ErrorGetType(err);
    const char *err_msg  = OCI_ErrorGetString(err);

    printf("%s - %s\n", err_type == OCI_ERR_TYPE_WARNING ? "warning" : "error", err_msg);
}

int main(void)
{

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    OCI_EnvironmentEnableWarnings(env, TRUE);

    OCI_Connection * cn = OCI_ConnectionCreate(env, "db", "usr", "expired_pwd_in_grace_period", OCI_SESSION_DEFAULT);

    (void) cn;

    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}

