#include "ocilib.h"

typedef struct product_t
{
    int        code;
    char      *name;
    double     price;
    OCI_Date  *creation;
} product_t;

typedef struct product_ind_t
{
    boolean    code;
    boolean    name;
    boolean    price;
    boolean    creation;
} product_ind_t;


int main(void)
{
    OCI_Connection *cn;
    OCI_Statement  *st;
    OCI_Resultset  *rs;

    product_t     prd;
    product_ind_t ind;
 
    char buf[100];

    int i = 0;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn  = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st  = OCI_StatementCreate(cn);  

    OCI_ExecuteStmt(st, "select * from products");

    rs = OCI_GetResultset(st);

    OCI_SetStructNumericType(rs, 1,  OCI_NUM_INT);
    OCI_SetStructNumericType(rs, 3,  OCI_NUM_DOUBLE);
   
    while (OCI_FetchNext(rs))
    {
        i++;

        OCI_GetStruct(rs, &prd, &ind);

        OCI_DateToText(prd.creation, "DD-MM-YYYY", 100, buf); 
        
        printf("row #%d              \n"
               "...prd.code     : %d \n"
               "...prd.name     : %s \n"
               "...prd.price    : %g \n"
               "...prd.creation : %s \n"
               "                     \n",
               i, prd.code, prd.name, prd.price, buf
              );
    }

    printf("\n\n%d row(s) fetched\n", OCI_GetRowCount(rs));

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
