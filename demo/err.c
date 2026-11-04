#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf
    (
        "code  : ORA-%05i\n"
        "msg   : %s\n"
        "sql   : %s\n",
        OCI_ErrorGetCode(err),
        OCI_ErrorGetString(err),
        OCI_GetSql(OCI_ErrorGetStatement(err))
    );
}

int main(void)
{
    OCI_Connection *cn;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate(env, "db", "wrong_usr", "wrong_pwd", OCI_SESSION_DEFAULT);

    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
