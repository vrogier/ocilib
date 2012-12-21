#include "ocilib.h"

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Resultset *rs;
    OCI_File *file;
    char buffer[256];
    int n;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);

    file = OCI_FileCreate(cn, OCI_CFILE);

    OCI_FileSetName(file, "MYDIR", "MyfileName");

    /* check if faile exists */

    if (OCI_FileExists(file))
    {
         printf("file size : %d\n", OCI_FileGetSize(file));
         printf("file dir  : %s\n", OCI_FileGetDirectory(file));
         printf("file name : %s\n", OCI_FileGetName(file));
    }

    /* bind for inserting into table */

    OCI_Prepare(st, "insert into my_bfile_table(code, value) values (1, :bfile)");
    OCI_BindFile(st, ":bfile", file);
    OCI_Execute(st);
    OCI_Commit(cn);

    /* free local file object */

    OCI_FileFree(file),

    /* fetch bfile data from table */
 
    OCI_ExecuteStmt(st, "select code, value from my_bfile_table");
 
    rs = OCI_GetResultset(st);

    while (OCI_FetchNext(rs))
    {
        file = OCI_GetFile(rs, 2);

        OCI_FileOpen(file);

        printf("file size  %d\n", OCI_FileGetSize(file));
        printf("file dir   %s\n", OCI_FileGetDirectory(file));
        printf("file name  %s\n", OCI_FileGetName(file));
 
        while (n = OCI_FileRead(file, buffer, sizeof(buffer)-1))
        {
            buffer[n] = 0;
            printf(buffer);
        }

        OCI_FileClose(file);
    }

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
