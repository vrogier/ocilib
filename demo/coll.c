#include "ocilib.h"

/* Create DDL for sample code: 

create type product_t as object (code int, name varchar2(50));
/
create type product_varray_t as varray(3) of product_t;
/
create type product_table_t as table of product_t;
/
create table products_varray (category varchar2(50), products product_varray_t);
create table products_nested_table(category varchar2(50), products product_table_t) nested table products store as products_nested_table_products;

*/

/* Cleanup DDL for sample code:

drop table products_nested_table;
drop table products_varray;
drop type product_table_t;
drop type product_varray_t;
drop type product_t;

*/

/* DML for sample code: 

insert into products_varray values ('category A', product_varray_t(product_t(1, 'name 1'), product_t(2, 'name 2'), product_t(3, 'name 3')));
insert into products_varray values ('category B', product_varray_t(product_t(4, 'name 4'), product_t(5, 'name 5'), product_t(6, 'name 6')));
insert into products_nested_table values ('category C', product_table_t(product_t(5, 'name 5'), product_t(7, 'name 7')));
insert into products_nested_table values ('category D', product_table_t(product_t(6, 'name 6'), product_t(8, 'name 8')));
commit;

*/

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
