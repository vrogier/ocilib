#include "ocilib.h"

/* requires script demo/lob.sql */

#define SIZE_BUF 512

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Lob *lob1, *lob2;

    char temp[SIZE_BUF + 1];
    OCI_Size n;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    OCI_Statement * st = OCI_StatementCreate(cn);

    OCI_StatementExecute(st, "select code, obj from large_objects for update");

    OCI_Resultset * rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        char *str = "- ABC must have replaced 3 first chars";
        OCI_UInt32 char_count = 0, byte_count = 0;

        lob1 = OCI_ResultsetGetLob(rs, 1);
        lob2 = OCI_LobCreate(cn, OCI_CLOB);

        char_count = 3;
        OCI_LobWrite(lob1, "ABC", &char_count, &byte_count);
        n = char_count;
        OCI_LobSeek(lob1, n, OCI_SEEK_SET);

        char_count = (OCI_UInt32) strlen(str);
        byte_count = 0;
        OCI_LobWrite(lob2, str, &char_count, &byte_count);
        n = char_count;

        OCI_LobAppendLob(lob1, lob2);
        OCI_LobSeek(lob1, 0, OCI_SEEK_SET);

        char_count = SIZE_BUF;
        byte_count = 0;
        OCI_LobRead(lob1, temp, &char_count, &byte_count);
        n = char_count;
        temp[n] = 0;

        printf("code: %i, obj : %s\n", OCI_ResultsetGetInt32(rs, 0), temp);

        OCI_LobFree(lob2);
    }

    printf("\n%d row(s) fetched\n", OCI_ResultsetGetRowCount(rs));

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
