#include "ocilib.h"

#define NB_ELEMS 1000

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    int i;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
 
    st = OCI_StatementCreate(cn);
   
    OCI_SetBindAllocation(st, OCI_BAM_INTERNAL);

    OCI_Prepare(st, "insert into test_array values (:tab)");

    OCI_BindArraySetSize(st, NB_ELEMS);

    OCI_BindArrayOfDates(st, ":tab", NULL, 0);
    {
        OCI_Date ** tab = (OCI_Date **) OCI_BindGetData(OCI_GetBind(st, 1));

        for (i=0; i < NB_ELEMS; i++)
        {
            OCI_DateSysDate(tab[i]);
        }

        OCI_Execute(st);
    }

    OCI_Commit(cn); 

    OCI_Cleanup();
 
    return EXIT_SUCCESS;
}

