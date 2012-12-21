#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st1, *st2;
    OCI_Resultset *rs;

    char rowid[OCI_SIZE_ROWID+1];
    int value;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn  = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
 
    st1 = OCI_StatementCreate(cn);
    st2 = OCI_StatementCreate(cn);

    OCI_Prepare(st1, "update test_fetch set code = :i where rowid = :s");
    OCI_BindInt(st1, ":i", &value);
    OCI_BindString(st1, ":s", rowid, sizeof(rowid)-1);

    OCI_ExecuteStmt(st2, "select code, rowid from test_fetch for update");

    rs = OCI_GetResultset(st2);

    while (OCI_FetchNext(rs))
    {
        value = OCI_GetInt(rs, 1);
        strcpy(rowid, OCI_GetString(rs, 2));
   
        /* updating value with some computation ... */
        value = (value + 4 ) % 2;

        OCI_Execute(st1);
    }

    OCI_Commit(cn);

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
