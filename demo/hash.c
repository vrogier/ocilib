#include "ocilib.h"

/* requires script demo/products.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    int i, n;
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Resultset  *rs;
    OCI_HashTable  *ht;
    OCI_HashEntry  *he;
    OCI_HashValue  *hv;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
    ht = OCI_HashCreate(256, OCI_HASH_INTEGER);

    /* fill the hash table with data from DB */

    OCI_ExecuteStmt(st, "select code, name from products");

    rs = OCI_GetResultset(st);
    while (OCI_FetchNext(rs))
    {
        OCI_HashAddInt(ht, OCI_GetString2(rs, "name"), OCI_GetInt2(rs, "code"));
    }

    printf("%d row(s) fetched\n", OCI_GetRowCount(rs));

    /* lookup an entry */

    printf("code for '%s' is : %d\n", "product 1", OCI_HashGetInt(ht, "product 1"));

    /* browse the hash table */

    n = OCI_HashGetSize(ht);

    for (i = 0; i < n; i++)
    {
        he = OCI_HashGetEntry(ht, i);

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

    OCI_HashFree(ht);
    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
