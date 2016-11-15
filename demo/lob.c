#include "ocilib.h"

/* requires script demo/lob.sql */

#define SIZE_BUF 512

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Resultset *rs;
    OCI_Lob *lob1, *lob2;

    char temp[SIZE_BUF + 1];
    size_t n;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    OCI_ExecuteStmt(st, "select code, obj from large_objects for update");

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        char *str = "- ABC must have replaced 3 first chars";

        lob1 = OCI_GetLob(rs, 2);
        lob2 = OCI_LobCreate(cn, OCI_CLOB);

        n = OCI_LobWrite(lob1, "ABC", 3);
        OCI_LobSeek(lob1, n, OCI_SEEK_SET);

        n = OCI_LobWrite(lob2, str, (unsigned int) strlen(str));

        OCI_LobAppendLob(lob1, lob2);
        OCI_LobSeek(lob1, 0, OCI_SEEK_SET);

        n = OCI_LobRead(lob1, temp, SIZE_BUF);
        temp[n] = 0;

        printf("code: %i, obj : %s\n", OCI_GetInt(rs, 1), temp);

        OCI_LobFree(lob2);
    }

    printf("\n%d row(s) fetched\n", OCI_GetRowCount(rs));

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
