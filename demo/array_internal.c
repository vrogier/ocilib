#include "ocilib.h"

/* requires script demo/dates.sql */

#define ARRAY_SIZE 10

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Date ** tab;
    int i;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_SetBindAllocation(st, OCI_BAM_INTERNAL);
    OCI_Prepare(st, "insert into dates values (:tab)");
    OCI_BindArraySetSize(st, ARRAY_SIZE);
    OCI_BindArrayOfDates(st, ":tab", NULL, 0);

    tab = (OCI_Date **)OCI_BindGetData(OCI_GetBind(st, 1));

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        OCI_DateSysDate(tab[i]);
    }

    OCI_Execute(st);

    printf("row inserted : %d\n", OCI_GetAffectedRows(st));

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}

