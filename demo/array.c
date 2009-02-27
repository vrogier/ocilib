#include "ocilib.h"

int main()
{
    OCI_Connection *cn;
    OCI_Statement *st;

    int tab_int[1000];
    char tab_str[1000][21];

    int i;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    /* ... create connection and statement ... */

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
 
    /* binding */

    OCI_Prepare(st, "insert into products values(:i, :s)");
    OCI_BindArraySetSize(st, 1000);
    OCI_BindArrayOfInts(st, ":i", (int*) tab_int, 0);
    OCI_BindArrayOfStrings(st, ":s", (char*) tab_str, 20, 0);

    /* filling arrays */

    for(i=0;i<1000;i++)
    {
        tab_int[i] = i+1;
        sprintf(tab_str[i],"Name %d",i+1);
    }

    /* execute */

    OCI_Execute(st);
 
    printf("row processed : %d\n", OCI_GetAffectedRows(st));

    OCI_Commit(cn);

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
