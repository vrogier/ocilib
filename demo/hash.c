#include "ocilib.h"

int main(void)
{
    int i, n;
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Resultset  *rs;
    OCI_HashTable *table;
    OCI_HashEntry *e;
    OCI_HashValue *v;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn    = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st    = OCI_StatementCreate(cn);
    table = OCI_HashCreate(256, OCI_HASH_INTEGER);

    /* fill the hash table with data from DB */

    OCI_ExecuteStmt(st, "select code, name from products");
                  
    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
        OCI_HashAddInt(table, OCI_GetString2(rs, "name"), OCI_GetInt2(rs, "code"));

    printf("%d row(s) fetched\n", OCI_GetRowCount(rs));

    /* lookup an entry */
   
    printf("code for %s is : %d\n", "Cars", OCI_HashGetInt(table, "Cars"));

    /* browse the hash table */

    n = OCI_HashGetSize(table);

    for (i = 0; i < n; i++)
    {
        e = OCI_HashGetEntry(table, i);

        while (e != NULL)
        {
            printf (">key: '%s'\n", e->key);

            v = e->values;

            while (v != NULL)
            {
                printf ("..... value : '%i'\n", v->value.num);
                v = v->next;
            }

            e = e->next;
        }
    }

    /* destroy table */

    OCI_HashFree(table);
    
    OCI_Cleanup();
 
    return EXIT_SUCCESS;
}
