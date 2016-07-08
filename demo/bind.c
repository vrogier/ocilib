#include "ocilib.h"

/* Create DDL for sample code: 

create table products (code int, name varchar2(50));

*/

/* Cleanup DDL for sample code: 

drop table products;

*/

/* DML for sample code: 

insert into products values (1, 'product 1');
insert into products values (2, 'product 2');
commit;

*/

OCI_Connection *cn;
OCI_Statement *st;
int code;

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

void get_product_name(int requested_code)
{
    OCI_Resultset *rs;
    code = requested_code;
    OCI_Execute(st);
    rs = OCI_GetResultset(st);
    OCI_FetchNext(rs);
    printf("product name for product code %d is %s\n", code, OCI_GetString(rs, 1));
}

int main(void)
{
    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_Prepare(st, "select name from products where code  = :code");
    OCI_BindInt(st, ":code", &code);

    get_product_name(1);
    get_product_name(2);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}

