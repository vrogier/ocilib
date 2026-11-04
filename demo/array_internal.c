#include "ocilib.h"

/* requires script demo/dates.sql */

#define ARRAY_SIZE 10

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Date ** tab;
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
    OCI_Statement * st = OCI_StatementCreate(cn);

    OCI_ExecuteStmt(st, "create table dates (value date)");

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

    OCI_ExecuteStmt(st, "drop table dates");

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}

