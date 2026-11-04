#include "ocilib.h"

/* requires script demo/products.sql */

typedef struct product_t
{
    OCI_Int32        code;
    char      *name;
} product_t;

typedef struct product_ind_t
{
    OCI_Boolean    code;
    OCI_Boolean    name;
} product_ind_t;

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{

    product_t     prd;
    product_ind_t ind;

    OCI_Int32 i = 0;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    OCI_Statement  * st = OCI_StatementCreate(cn);

    OCI_ExecuteStmt(st, "select * from products");

    OCI_Resultset  * rs = OCI_GetResultset(st);

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
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
