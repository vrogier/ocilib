#include "ocilib.h"

/* requires script demo/collections.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

void print_elem(OCI_Object *product)
{
    printf("...product: code %d, %s\n", OCI_ObjectGetInt32(product, "code"), OCI_ObjectGetString(product, "name"));
}

void print_coll(OCI_Collection *coll)
{
    OCI_Iterator *iter = OCI_IteratorCreate(coll);
    OCI_Element *elem = OCI_IteratorGetNext(iter);

    while (elem)
    {
        print_elem(OCI_ElementGetObject(elem));
        elem = OCI_IteratorGetNext(iter);
    }

    OCI_IteratorFree(iter);
}

void bind_coll(OCI_Connection *cn)
{
    OCI_Collection *coll = OCI_CollectionCreate(OCI_TypeInfoGet(cn, "product_varray_t", OCI_TIF_TYPE));
    OCI_Statement *st = OCI_StatementCreate(cn);

    OCI_StatementPrepare(st, "begin :array := product_varray_t(product_t(123, 'name 123'), product_t(456, 'name 456'), product_t(789, 'name 789')); end;");
    OCI_StatementBindCollection(st, ":array", coll);
    OCI_StatementExecutePrepared(st);

    print_coll(coll);

    OCI_CollectionFree(coll);
    OCI_StatementFree(st);
}

void fetch_coll(OCI_Connection *cn, otext * table_name)
{
    OCI_Statement *st = OCI_StatementCreate(cn);
    OCI_Resultset *rs = NULL;

    otext sql[64];
    snprintf(sql, sizeof(sql), "SELECT * from %s", table_name);
    OCI_StatementExecute(st, sql);

    rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        printf("#%s\n", OCI_ResultsetGetString(rs, 0));

        print_coll(OCI_ResultsetGetColl(rs, 1));
    }

    OCI_StatementFree(st);
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

    bind_coll(cn);

    fetch_coll(cn, "products_varray");
    fetch_coll(cn, "products_nested_table");

    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
