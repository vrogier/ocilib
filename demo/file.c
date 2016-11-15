#include "ocilib.h"

/* requires script demo/file.sql */

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Resultset *rs;
    OCI_File *file;
    char buffer[256];
    int n;

    if (!OCI_Initialize(err_handler, NULL, OCI_ENV_DEFAULT))
    {
        return EXIT_FAILURE;
    }

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    file = OCI_FileCreate(cn, OCI_BFILE);

    /* replace 'test.txt' with any existing file on the 'test_folder' folder */
    
    OCI_FileSetName(file, "TEMP_FOLDER", "test.txt");

    /* check if file exists */

    if (OCI_FileExists(file))
    {
        printf("file size : %d\n", OCI_FileGetSize(file));
        printf("file dir  : %s\n", OCI_FileGetDirectory(file));
        printf("file name : %s\n", OCI_FileGetName(file));
    }

    /* bind for inserting into table */

    OCI_Prepare(st, "insert into files(id, item) values (1, :f)");
    OCI_BindFile(st, ":f", file);
    OCI_Execute(st);

    /* free local file object */

    OCI_FileFree(file);

    /* fetch file data from table */

    OCI_ExecuteStmt(st, "select * from files");

    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        file = OCI_GetFile(rs, 2);

        OCI_FileOpen(file);

        printf("file size  %d\n", OCI_FileGetSize(file));
        printf("file dir   %s\n", OCI_FileGetDirectory(file));
        printf("file name  %s\n", OCI_FileGetName(file));

        while (n = OCI_FileRead(file, buffer, sizeof(buffer) - 1))
        {
            buffer[n] = 0;
            printf(buffer);
        }

        OCI_FileClose(file);
    }

    OCI_StatementFree(st);
    OCI_ConnectionFree(cn);
    OCI_Cleanup();

    return EXIT_SUCCESS;
}
