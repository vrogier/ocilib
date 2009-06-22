#include "ocilib.h"

#define SIZE_ARRAY  1000
#define SIZE_COL1   20
#define SIZE_COL2   30
#define SIZE_COL3   8
#define NUM_COLS    3

int main(void)
{ 
    OCI_Connection *cn;
    OCI_DirPath    *dp;
    OCI_TypeInfo   *tbl;

    dtext val1[SIZE_COL1+1];
    dtext val2[SIZE_COL2+1];
    dtext val3[SIZE_COL3+1];

    int i = 0, nb_rows = SIZE_ARRAY;

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

    for (i = 1; i <= nb_rows; i++)
    {
        /* fill test values */

        ocisprintf(val1, SIZE_COL1+1, "%04d", i);
        ocisprintf(val2, SIZE_COL2+1, "value %05d", i);
        ocisprintf(val3, SIZE_COL3+1, "%04d%02d%02d", (i%23)+1 + 2000, 
                                                      (i%11)+1,
                                                      (i%23)+1);

        OCI_DirPathSetEntry(dp, i, 1, val1, (unsigned int) dtslen(val1), TRUE);
        OCI_DirPathSetEntry(dp, i, 2, val2, (unsigned int) dtslen(val2), TRUE);
        OCI_DirPathSetEntry(dp, i, 3, val3, (unsigned int) dtslen(val3), TRUE);
    }

    /* load data to the server */

    OCI_DirPathConvert(dp);
    OCI_DirPathLoad(dp);

    /* commits changes */

    OCI_DirPathFinish(dp);

    printf("%04d row(s) processed\n", OCI_DirPathGetAffectedRows(dp));
    printf("%04d row(s) loaded\n",    OCI_DirPathGetRowCount(dp));

    /* free direct path object */

    OCI_DirPathFree(dp);

    OCI_Cleanup();

   return EXIT_SUCCESS;
}