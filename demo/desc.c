    
#include "ocilib.h"

int main()
{
    OCI_Connection *cn;
    OCI_Schema *tbl;
    int i,n;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn  = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    tbl = OCI_SchemaGet(cn, "products", OCI_SCHEMA_TABLE);

    if (tbl != NULL)
    {
        printf ("Column Name         Type      Length  Prec.   Scale   Null ?\n");
        printf ("----------------------------  ------------------------------\n");

        n = OCI_SchemaGetColumnCount(tbl);
   
        for(i = 1; i <= n; i++)
        {
            OCI_Column *col = OCI_SchemaGetColumn(tbl, i);

            printf("%-20s%-10s%-8i%-8i%-8i%-s\n",
                    OCI_GetColumnName(col),
                    OCI_GetColumnSQLType(col),
                    OCI_GetColumnSize(col),
                    OCI_GetColumnPrecision(col),
                    OCI_GetColumnScale(col),
                    OCI_GetColumnNullable(col) == TRUE ? "Y" : "N");
        }
    }

    OCI_Cleanup();

    return EXIT_SUCCESS;
}