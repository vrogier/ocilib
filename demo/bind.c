#include "ocilib.h"

/* requires script demo/products.sql */

OCI_Connection *cn;
OCI_Statement *st;
OCI_Int32 code;

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

void get_product_name(OCI_Int32 requested_code)
{
    code = requested_code;
    OCI_StatementExecutePrepared(st);
    OCI_Resultset * rs = OCI_StatementGetResultset(st);
    OCI_ResultsetFetchNext(rs);
    printf("product name for product code %d is %s\n", code, OCI_ResultsetGetString(rs, 0));
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

    cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_StatementPrepare(st, "select name from products where code  = :code");
    OCI_StatementBindInt32(st, ":code", &code);

    get_product_name(1);
    get_product_name(2);

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}

