#include "ocilib.h"

#define SIZE_ARRAY 100
#define NB_LOAD    10
#define SIZE_COL1  20
#define SIZE_COL2  30
#define SIZE_COL3  8
#define NUM_COLS   3

int main(void)
{ 
    OCI_Connection *cn;
    OCI_DirPath    *dp;
    OCI_TypeInfo   *tbl;

    dtext val1[SIZE_COL1+1];
    dtext val2[SIZE_COL2+1];
    dtext val3[SIZE_COL3+1];

    int i = 0, j = 0, nb_rows = SIZE_ARRAY;
    boolean res = TRUE;
    
    if (!OCI_Initialize(NULL, NULL, OCI_ENV_DEFAULT))
       return EXIT_FAILURE;

    cn  = OCI_ConnectionCreate("db", "usr", "pwd", OCI_SESSION_DEFAULT);
    tbl = OCI_TypeInfoGet(cn, "test_directpath", OCI_TIF_TABLE);
    dp  = OCI_DirPathCreate(tbl, NULL, NUM_COLS, nb_rows);

    /* optional attributes to set */

    OCI_DirPathSetBufferSize(dp, 64000);
    OCI_DirPathSetNoLog(dp, TRUE);
    OCI_DirPathSetParallel(dp, TRUE);

    /* describe the target table */

    OCI_DirPathSetColumn(dp, 1, "VAL_INT",  SIZE_COL1, NULL);
    OCI_DirPathSetColumn(dp, 2, "VAL_STR",  SIZE_COL2, NULL);
    OCI_DirPathSetColumn(dp, 3, "VAL_DATE", SIZE_COL3, "YYYYMMDD");

    /* prepare the load */

    OCI_DirPathPrepare(dp);

    nb_rows = OCI_DirPathGetMaxRows(dp);          
    
    for (i = 0; i < NB_LOAD ; i++)
    {
        OCI_DirPathReset(dp);

        for (j = 1; j <= nb_rows; j++)
        {
            /* fill test values */

            sprintf(val1, "%04d", i + (i*100));
            sprintf(val2, "value %05d", j + (i*100));
            sprintf(val3, "%04d%02d%02d", (j%23)+1 + 2000, (j%11)+1, (j%23)+1);

            OCI_DirPathSetEntry(dp, j, 1, val1, (unsigned int) strlen(val1), TRUE);
            OCI_DirPathSetEntry(dp, j, 2, val2, (unsigned int) strlen(val2), TRUE);
            OCI_DirPathSetEntry(dp, j, 3, val3, (unsigned int) strlen(val3), TRUE);
        }
        
       /* load data to the server */

        while (res)
        {
            int state = OCI_DirPathConvert(dp);

            if ((state == OCI_DPR_FULL) || (state == OCI_DPR_COMPLETE))
                res = OCI_DirPathLoad(dp);

            if (state == OCI_DPR_COMPLETE)
                break;
        }
    }

    /* commits changes */

    OCI_DirPathFinish(dp);

    printf("%04d row(s) loaded\n", OCI_DirPathGetRowCount(dp));

    /* free direct path object */

    OCI_DirPathFree(dp);

    OCI_Cleanup();

   return EXIT_SUCCESS;
}
