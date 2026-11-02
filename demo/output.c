#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    const char *p;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate(env, "db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    /* server output */

    OCI_ServerEnableOutput(cn, 32000, 5, 255);

    /* pl/sql call */
 
    OCI_ExecuteStmt(st, "begin "
        "   dbms_output.put_line('First  line'); "
        "   dbms_output.put_line('Second line'); "
        "   dbms_output.put_line('Third  line'); "
        "end;"
        );

    while (p = OCI_ServerGetOutput(cn))
    {
        printf(p);
        printf("\n");
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
