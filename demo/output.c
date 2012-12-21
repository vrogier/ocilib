#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    const dtext *p;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    /* pl/sql call */

    /* server output */

    OCI_ServerEnableOutput(cn, 32000, 5, 255);

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
 
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
