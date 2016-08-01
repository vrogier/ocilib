#include "ocilib.h"

/* requires script demo/collections.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

void print_elem(OCI_Object *product)
{
    printf("...product: code %d, %s\n", OCI_ObjectGetInt(product, "code"), OCI_ObjectGetString(product, "name"));
}

void print_coll(OCI_Coll *coll)
{
    OCI_Iter *iter = OCI_IterCreate(coll);
    OCI_Elem *elem = OCI_IterGetNext(iter);

    while (elem)
    {
        print_elem(OCI_ElemGetObject(elem));
        elem = OCI_IterGetNext(iter);
    }

    OCI_IterFree(iter);
}

void bind_coll(OCI_Connection *cn)
{
    OCI_Coll *coll = OCI_CollCreate(OCI_TypeInfoGet(cn, "product_varray_t", OCI_TIF_TYPE));
    OCI_Statement *st = OCI_StatementCreate(cn);

    OCI_Prepare(st, "begin :array := product_varray_t(product_t(123, 'name 123'), product_t(456, 'name 456'), product_t(789, 'name 789')); end;");
    OCI_BindColl(st, ":array", coll);
    OCI_Execute(st);

    print_coll(coll);

    OCI_CollFree(coll);
    OCI_StatementFree(st);
}

void fetch_coll(OCI_Connection *cn, otext * table_name)
{
    OCI_Statement *st = OCI_StatementCreate(cn);
    OCI_Resultset *rs = NULL;

    OCI_ExecuteStmtFmt(st, "SELECT * from %m", table_name);

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        printf("#%s\n", OCI_GetString(rs, 1));

        print_coll(OCI_GetColl(rs, 2));
    }

    OCI_StatementFree(st);
}

int main(void)
{
    OCI_Connection *cn;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);

    bind_coll(cn);

    fetch_coll(cn, "products_varray");
    fetch_coll(cn, "products_nested_table");

    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
