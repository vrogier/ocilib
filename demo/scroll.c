#include "ocilib.h"

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{

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

    OCI_StatementSetFetchMode(st, OCI_SFM_SCROLLABLE);

    OCI_StatementExecute(st, "select rownum, 'Row ' || rownum from (select 1 from dual connect by level <= 65)");

    OCI_Resultset * rs = OCI_StatementGetResultset(st);

    /* get resultset row count */
    OCI_ResultsetFetchLast(rs);
    printf("resultset contains %i rows\n", OCI_ResultsetGetRowCount(rs));

    /* go to row 1 */
    OCI_ResultsetFetchFirst(rs);
    printf("%i - %s\n", OCI_ResultsetGetInt32(rs, 0), OCI_ResultsetGetString(rs, 1));

    /* enumerate from row 2 to row X */
    while (OCI_ResultsetFetchNext(rs))
    {
        printf("%i - %s\n", OCI_ResultsetGetInt32(rs, 0), OCI_ResultsetGetString(rs, 1));
    }

    /* enumerate from row X back to row 1 */
    while (OCI_ResultsetFetchPrev(rs))
    {
        printf("%i - %s\n", OCI_ResultsetGetInt32(rs, 0), OCI_ResultsetGetString(rs, 1));
    }

    /* print the 30th row */
    OCI_ResultsetFetchSeek(rs, OCI_SFD_ABSOLUTE, 30);
    printf("%i - %s\n", OCI_ResultsetGetInt32(rs, 0), OCI_ResultsetGetString(rs, 1));

    /* fetch next 30 rows */
    while ((OCI_ResultsetGetCurrentRow(rs) < 60) && OCI_ResultsetFetchNext(rs))
    {
        printf("%0i - %s\n", OCI_ResultsetGetInt32(rs, 0), OCI_ResultsetGetString(rs, 1));
    }

    /* move back to the 45th row */
    OCI_ResultsetFetchSeek(rs, OCI_SFD_RELATIVE, -15);
    printf("%i - %s\n", OCI_ResultsetGetInt32(rs, 0), OCI_ResultsetGetString(rs, 1));

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
