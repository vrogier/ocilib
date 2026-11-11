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

    OCI_StatementExecute(st, "create table dates (value date)");

    OCI_StatementSetBindAllocation(st, OCI_BAM_INTERNAL);
    OCI_StatementPrepare(st, "insert into dates values (:tab)");
    OCI_StatementBindArraySetSize(st, ARRAY_SIZE);
    OCI_StatementBindDateArray(st, ":tab", NULL, 0);

    tab = (OCI_Date **)OCI_BindGetData(OCI_StatementGetBind(st, 0));

    for (i = 0; i < ARRAY_SIZE; i++)
    {
        OCI_DateSysDate(tab[i]);
    }

    OCI_StatementExecutePrepared(st);

    printf("row inserted : %d\n", OCI_StatementGetAffectedRows(st));

    OCI_StatementExecute(st, "drop table dates");

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}

