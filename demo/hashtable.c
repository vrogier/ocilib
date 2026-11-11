#include "ocilib.h"

/* requires script demo/products.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Int32 i, n;
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Resultset  *rs;
    OCI_HashTable  *ht;
    OCI_HashEntry  *he;
    OCI_HashValue  *hv;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
    ht = OCI_HashTableCreate(env, 256, OCI_HASH_INTEGER);

    /* fill the hash table with data from DB */

    OCI_StatementExecute(st, "select code, name from products");

    rs = OCI_StatementGetResultset(st);
    while (OCI_ResultsetFetchNext(rs))
    {
        OCI_HashTableAddInt32(ht, OCI_ResultsetGetStringByName(rs, "name"), OCI_ResultsetGetInt32ByName(rs, "code"));
    }

    printf("%d row(s) fetched\n", OCI_ResultsetGetRowCount(rs));

    /* lookup an entry */

    printf("code for '%s' is : %d\n", "product 1", OCI_HashTableGetInt32(ht, "product 1"));

    /* browse the hash table */

    n = OCI_HashTableGetSize(ht);

    for (i = 0; i < n; i++)
    {
        he = OCI_HashTableGetEntry(ht, i);

        while (he)
        {
            printf(">key: '%s'\n", he->key);

            hv = he->values;

            while (hv)
            {
                printf("..... value : '%i'\n", hv->value.num);
                hv = hv->next;
            }

            he = he->next;
        }
    }

    /* destroy table */

    OCI_HashTableFree(ht);
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
