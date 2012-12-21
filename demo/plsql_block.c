#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    int res = 0;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    /* pl/sql call */

    OCI_Prepare(st, MT("begin :res := trunc(sysdate+1)-trunc(sysdate-1); end;"));
    OCI_BindInt(st, MT(":res"), &res);
    OCI_Execute(st);

    printf("result : %i\n", res);
 
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
