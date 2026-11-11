#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);

    printf("Server major    version : %i\n", OCI_ConnectionGetServerMajorVersion(cn));
    printf("Server minor    version : %i\n", OCI_ConnectionGetServerMinorVersion(cn));
    printf("Server revision version : %i\n\n", OCI_ConnectionGetServerRevisionVersion(cn));
    printf("Connection      version : %i\n\n", OCI_ConnectionGetVersion(cn));

    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
