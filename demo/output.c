#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    const char *p;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    OCI_Statement  * st = OCI_StatementCreate(cn);

    /* server output */

    OCI_ConnectionEnableServerOutput(cn, 32000, 5, 255);

    /* pl/sql call */
 
    OCI_StatementExecute(st, "begin "
        "   dbms_output.put_line('First  line'); "
        "   dbms_output.put_line('Second line'); "
        "   dbms_output.put_line('Third  line'); "
        "end;"
        );

    while ((p = OCI_ConnectionGetServerOutput(cn)))
    {
        printf("%s", p);
        printf("\n");
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
