#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Resultset  *rs;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_Prepare(st, "update products set code = code+10 returning code into :i");
    OCI_RegisterInt(st, ":i");
    OCI_Execute(st);
  
    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
        printf("%i\n", OCI_GetInt(rs, 1));
 
    printf("count : %i\n", OCI_GetRowCount(rs));
  
    OCI_Commit(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
