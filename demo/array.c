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

    OCI_Int32 tab_int[ARRAY_SIZE];
    char tab_str[ARRAY_SIZE][STRING_SIZE+1];

    OCI_Int32 i;

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

    OCI_Prepare(st, "insert into products values(:i, :s)");
    OCI_BindArraySetSize(st, ARRAY_SIZE);
    OCI_BindArrayOfInts(st, ":i", (OCI_Int32*)tab_int, 0);
    OCI_BindArrayOfStrings(st, ":s", (char*)tab_str, STRING_SIZE, 0);

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        tab_int[i] = i + 1;
        sprintf(tab_str[i], "Name %d", i + 1);
    }

    if (!OCI_Execute(st))
    {
        printf("Number of DML array errors : %d\n", OCI_GetBatchErrorCount(st));

        OCI_Error * err = OCI_GetBatchError(st);

        while (err)
        {
            printf("Error at row %d : %s\n", OCI_ErrorGetRow(err), OCI_ErrorGetString(err));

            err = OCI_GetBatchError(st);
        }
    }

    printf("row inserted : %d\n", OCI_GetAffectedRows(st));

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
