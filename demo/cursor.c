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
    OCI_Statement  * st = OCI_StatementCreate(cn);

    OCI_StatementExecute(st, "select rownum, cursor(select sysdate from dual) from (select 1 from dual connect by level <= 10)");

    OCI_Resultset  * rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        OCI_Statement *st2 = OCI_ResultsetGetCursor(rs, 1);
        OCI_Resultset *rs2 = OCI_StatementGetResultset(st2);

        while (OCI_ResultsetFetchNext(rs2))
        {
            printf("index: %d, date: %s\n",  OCI_ResultsetGetInt32(rs, 0), OCI_ResultsetGetString(rs2, 0));
        }
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
