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

typedef struct product_t
{
    int        code;
    char      *name;
} product_t;

typedef struct product_ind_t
{
    boolean    code;
    boolean    name;
} product_ind_t;

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Resultset  *rs;

    product_t     prd;
    product_ind_t ind;

    int i = 0;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_ExecuteStmt(st, "select * from products");

    rs = OCI_GetResultset(st);

    OCI_SetStructNumericType(rs, 1, OCI_NUM_INT);

    while (OCI_FetchNext(rs))
    {
        OCI_GetStruct(rs, &prd, &ind);

        printf("row #%d           \n"
            "                     \n"
            "...prd.code     : %d \n"
            "...prd.name     : %s \n"
            "                     \n",
            ++i, prd.code, prd.name);
    }

    printf("\n\n%d row(s) fetched\n", OCI_GetRowCount(rs));

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
