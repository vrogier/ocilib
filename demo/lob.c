#include "ocilib.h"

#define SIZE_BUF 512

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Resultset *rs;
    OCI_Lob *lob1, *lob2;

    char temp[SIZE_BUF+1];
    int code, n;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
 
    OCI_ExecuteStmt(st, "select code, content from test_lob for update");

    rs = OCI_GetResultset(st);
    
    while (OCI_FetchNext(rs))
    {
        code = OCI_GetInt(rs, 1);
        lob1 = OCI_GetLob(rs, 2);
        lob2 = OCI_LobCreate(cn, OCI_CLOB);

        n = OCI_LobWrite(lob1, "Today, ", 7);
        OCI_LobSeek(lob1, n, OCI_SEEK_SET);
       
        n = OCI_LobWrite(lob2, "I'm going to the cinema !", 25);
       
        OCI_LobAppendLob(lob1, lob2);
        OCI_LobSeek(lob1, 0, OCI_SEEK_SET);
        
        n = OCI_LobRead(lob1, temp, SIZE_BUF);
        temp[n] = 0;

        printf("code: %i, action : %s\n", code, temp);
            
        OCI_LobFree(lob2);
    }

    printf("\n%d row(s) fetched\n", OCI_GetRowCount(rs));

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
