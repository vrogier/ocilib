#include "ocilib.h"

/* requires script demo/products.sql */

#define ARRAY_SIZE 1000
#define STRING_SIZE 20

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Error      *err;

    int tab_int[ARRAY_SIZE];
    char tab_str[ARRAY_SIZE][STRING_SIZE+1];

    int i;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT | OCI_ENV_CONTEXT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_Prepare(st, "insert into products values(:i, :s)");
    OCI_BindArraySetSize(st, ARRAY_SIZE);
    OCI_BindArrayOfInts(st, ":i", (int*)tab_int, 0);
    OCI_BindArrayOfStrings(st, ":s", (char*)tab_str, STRING_SIZE, 0);

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        tab_int[i] = i + 1;
        sprintf(tab_str[i], "Name %d", i + 1);
    }

    if (!OCI_Execute(st))
    {
        printf("Number of DML array errors : %d\n", OCI_GetBatchErrorCount(st));

        err = OCI_GetBatchError(st);

        while (err)
        {
            printf("Error at row %d : %s\n", OCI_ErrorGetRow(err), OCI_ErrorGetString(err));

            err = OCI_GetBatchError(st);
        }
    }

    printf("row inserted : %d\n", OCI_GetAffectedRows(st));

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
