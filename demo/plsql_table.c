#include "ocilib.h"

/* requires script demo/plsql_table.sql */

#define SIZE_ARRAY 10
#define SIZE_NAME  20
#define SIZE_VALUE 100

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    int i;

    char tab_names[SIZE_ARRAY][SIZE_NAME + 1];
    char tab_values[SIZE_ARRAY][SIZE_VALUE + 1];
    char tab_results[SIZE_ARRAY][SIZE_VALUE + 1];

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    /* set values */

    for (i = 0; i < SIZE_ARRAY; i++)
    {
        sprintf(tab_names[i], "name  %03d", i + 1);
        sprintf(tab_values[i], "value %03d", i + 1);

        tab_results[i][0] = 0;
    }

    /* prepare call and bind local arrays */

    OCI_Prepare(st, "BEGIN test_plsqltables.combine_values(:tab_names, :tab_values, :tab_results); END;");
    OCI_BindArrayOfStrings(st, ":tab_names", (char*)tab_names, SIZE_NAME, SIZE_ARRAY);
    OCI_BindArrayOfStrings(st, ":tab_values", (char*)tab_values, SIZE_VALUE, SIZE_ARRAY);
    OCI_BindArrayOfStrings(st, ":tab_results", (char*)tab_results, SIZE_VALUE, SIZE_ARRAY);

    OCI_BindSetDirection(OCI_GetBind(st, 1), OCI_BDM_IN);
    OCI_BindSetDirection(OCI_GetBind(st, 2), OCI_BDM_IN);
    OCI_BindSetDirection(OCI_GetBind(st, 3), OCI_BDM_OUT);

    /* execute */

    OCI_Execute(st);

    for (i = 0; i < SIZE_ARRAY; i++)
    {
        printf("%d => %s\n", i+1, tab_results[i]);
    }

    /* cleanup */
    
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
