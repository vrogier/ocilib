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

    /* build a composite sharding key: NUMBER column + VARCHAR2 column */

    OCI_ShardingKey *skey = OCI_ShardingKeyCreate(env);
    OCI_Number *num = OCI_NumberCreate(env);
    OCI_NumberSetValue(num, OCI_NUM_INT, &(int){42});

    OCI_ShardingKeyAddNumber(skey, num);
    OCI_ShardingKeyAddString(skey, OTEXT("GOLD"));

    /* build a super sharding key (shard group) */

    OCI_ShardingKey *sskey = OCI_ShardingKeyCreate(env);
    OCI_ShardingKeyAddString(sskey, OTEXT("EUROPE"));

    printf("sharding key (base64)       : %s\n", OCI_ShardingKeyToText(skey));
    printf("super sharding key (base64) : %s\n", OCI_ShardingKeyToText(sskey));

    /* on a non-sharded database the keys are accepted and a regular
       connection is established */

    OCI_Connection *cn = OCI_ConnectionCreateSharded(env, db, usr, pwd,
                                                     OCI_SESSION_DEFAULT, skey, sskey);

    if (cn)
    {
        OCI_Statement *st = OCI_StatementCreate(cn);
        OCI_ExecuteStmt(st, OTEXT("select cust_id, name, region from customers where rownum <= 5"));

        OCI_Resultset *rs = OCI_GetResultset(st);
        while (OCI_FetchNext(rs))
        {
            printf("cust_id=%s name=%s region=%s\n",
                   OCI_GetString(rs, 0), OCI_GetString(rs, 1), OCI_GetString(rs, 2));
        }

        OCI_StatementFree(st);
        OCI_ConnectionFree(cn);
    }

    OCI_NumberFree(num);
    OCI_ShardingKeyFree(sskey);
    OCI_ShardingKeyFree(skey);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
