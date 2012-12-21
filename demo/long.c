#include "ocilib.h"

#define FILENAME "data.lst"
#define SIZE_BUF 512

int main(void)
{
    OCI_Connection *cn;
    OCI_Statement *st;
    OCI_Resultset *rs;
    OCI_Long *lg;
    FILE *f;
    char buffer[SIZE_BUF];
    int n;

    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
        return EXIT_FAILURE;

    cn = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    st = OCI_StatementCreate(cn);
 
     /* open the data file */

    f = fopen(FILENAME, "rb");

    if (f)
    {
        fseek (f , 0 , SEEK_END); 
        n = ftell(f); 
        rewind (f);  
  
        printf("\n%d bytes to write\n", n);

        lg = OCI_LongCreate(st, OCI_BLONG);

        OCI_Prepare(st,  "insert into test_long_raw(code, content) values (1, :data)");
        OCI_BindLong(st, ":data", lg, n);
        OCI_Execute(st);

        /* write data into table by chunks of SIZE_BUF bytes */       

        while ((n = fread(buffer, 1, sizeof(buffer), f)))
        {
            OCI_LongWrite(lg, buffer, n);
        }

        printf("\n%d bytes written\n", OCI_LongGetSize(lg));
        fclose(f);

        OCI_LongFree(lg);
        OCI_Commit(cn);
    }

    /* FETCHING LONGS  -------------------------------------------- */

 
    OCI_ExecuteStmt(st, "select content from test_long_raw where code = 1");
    OCI_SetLongMaxSize(st, 10000);

    rs = OCI_GetResultset(st);

    /* read data by chunks of 2048 bytes*/       
    while (OCI_FetchNext(rs))
    {
        lg = OCI_GetLong(rs, 1);

        while ((n = OCI_LongRead(lg, buffer, sizeof(buffer)))) {}

        printf("\n%d bytes read\n", OCI_LongGetSize(lg));
    }

    OCI_Cleanup();

    return EXIT_SUCCESS;
}
