#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Resultset *rs;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
       return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);

    st = OCI_StatementCreate(cn);

    OCI_SetFetchMode(st, OCI_SFM_SCROLLABLE);

    OCI_ExecuteStmt(st, "select int_col1, str_col from my_table");
   
    rs = OCI_GetResultset(st);
  
    /* get resultset row count */
    OCI_FetchLast(rs);
    printf("resultset contains %i rows\n", OCI_GetRowCount(rs));

    /* go to row 1 */
    OCI_FetchFirst(rs);
    printf("%i - %s\n", OCI_GetInt(rs, 1), OCI_GetString(rs, 2));

    /* enumerate from row 2 to row X */
    while (OCI_FetchNext(rs))
        printf("%i - %s\n", OCI_GetInt(rs, 1), OCI_GetString(rs, 2));

    /* enumerate from row X back to row 1 */
    while (OCI_FetchPrev(rs))
        printf("%i - %s\n", OCI_GetInt(rs, 1), OCI_GetString(rs, 2));

    /* print the 30th row */
    OCI_FetchSeek(rs, OCI_SFD_ABSOLUTE,  30);
    printf("%i - %s\n", OCI_GetCurrentRow(rs), OCI_GetInt(rs, 1), OCI_GetString(rs, 2));

    /* fetch next 30 rows */
    while ((OCI_GetCurrentRow(rs) < 60) && OCI_FetchNext(rs))
        printf("%0i - %s\n", OCI_GetInt(rs, 1), OCI_GetString(rs, 2));

    /* move back to the 45th row */
    OCI_FetchSeek(rs, OCI_SFD_RELATIVE,  -15);
    printf("%i - %s\n", OCI_GetInt(rs, 1), OCI_GetString(rs, 2));

   OCI_Cleanup();

   return EXIT_SUCCESS;
}