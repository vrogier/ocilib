#include "ocilib.h"

/* requires script demo/file.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    char buffer[256];
    OCI_Int32 n;

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

    OCI_File * file = OCI_FileCreate(cn, OCI_BFILE);

    /* replace 'test.txt' with any existing file on the 'test_folder' folder */
    
    OCI_FileSetName(file, "TEMP_FOLDER", "test.txt");

    /* check if file exists */

    if (OCI_FileExists(file))
    {
        printf("file size : %llu\n", (unsigned long long) OCI_FileGetSize(file));
        printf("file dir  : %s\n", OCI_FileGetDirectory(file));
        printf("file name : %s\n", OCI_FileGetName(file));
    }

    /* bind for inserting into table */

    OCI_StatementPrepare(st, "insert into files(id, item) values (1, :f)");
    OCI_StatementBindFile(st, ":f", file);
    OCI_StatementExecutePrepared(st);

    /* free local file object */

    OCI_FileFree(file);

    /* fetch file data from table */

    OCI_StatementExecute(st, "select * from files");

    OCI_Resultset * rs = OCI_StatementGetResultset(st);

    while (OCI_ResultsetFetchNext(rs))
    {
        file = OCI_ResultsetGetFile(rs, 1);

        OCI_FileOpen(file);

        printf("file size  %llu\n", (unsigned long long) OCI_FileGetSize(file));
        printf("file dir   %s\n", OCI_FileGetDirectory(file));
        printf("file name  %s\n", OCI_FileGetName(file));

        while ((n = OCI_FileRead(file, buffer, sizeof(buffer) - 1)))
        {
            buffer[n] = 0;
            printf("%s", buffer);
        }

        OCI_FileClose(file);
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
