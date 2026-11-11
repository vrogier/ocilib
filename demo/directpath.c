#include "ocilib.h"

/* requires script demo/directpath.sql */

#define SIZE_ARRAY 100
#define NB_LOAD    10
#define SIZE_COL1  20
#define SIZE_COL2  30
#define SIZE_COL3  8
#define NUM_COLS   3

void err_handler(OCI_Error *err)
{
    printf("%s\n", OCI_ErrorGetString(err));
}

int main(void)
{

    char val1[SIZE_COL1 + 1];
    char val2[SIZE_COL2 + 1];
    char val3[SIZE_COL3 + 1];

    OCI_Int32 i = 0, j = 0, nb_rows = SIZE_ARRAY;
    OCI_Boolean res = TRUE;

    OCI_Environment *env = OCI_EnvironmentCreate(err_handler, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    OCI_Connection * cn = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);
    OCI_TypeInfo   * tbl = OCI_TypeInfoGet(cn, "dirpath_data", OCI_TIF_TABLE);
    OCI_DirectPath    * dp = OCI_DirectPathCreate(tbl, NULL, NUM_COLS, nb_rows);

    /* optional attributes to set */

    OCI_DirectPathSetBufferSize(dp, 64000);
    OCI_DirectPathSetNoLog(dp, TRUE);
    OCI_DirectPathSetParallel(dp, TRUE);

    /* describe the target table */

    OCI_DirectPathSetColumn(dp, 0, "val_int", SIZE_COL1, NULL);
    OCI_DirectPathSetColumn(dp, 1, "val_str", SIZE_COL2, NULL);
    OCI_DirectPathSetColumn(dp, 2, "val_date", SIZE_COL3, "YYYYMMDD");

    /* prepare the load */

    OCI_DirectPathPrepare(dp);

    nb_rows = OCI_DirectPathGetMaxRows(dp);

    for (i = 0; i < NB_LOAD; i++)
    {
        OCI_DirectPathReset(dp);

        for (j = 0; j < nb_rows; j++)
        {
            /* fill test values */

            sprintf(val1, "%04d", i + (i * 100));
            sprintf(val2, "value %05d", j + (i * 100));
            sprintf(val3, "%04d%02d%02d", (j % 23) + 1 + 2000, (j % 11) + 1, (j % 23) + 1);

            OCI_DirectPathSetEntry(dp, j, 0, val1, (OCI_UInt32)strlen(val1), TRUE);
            OCI_DirectPathSetEntry(dp, j, 1, val2, (OCI_UInt32)strlen(val2), TRUE);
            OCI_DirectPathSetEntry(dp, j, 2, val3, (OCI_UInt32)strlen(val3), TRUE);
        }

        /* load data to the server */

        while (res)
        {
            OCI_Int32 state = OCI_DirectPathConvert(dp);

            if ((state == OCI_DPR_FULL) || (state == OCI_DPR_COMPLETE))
                res = OCI_DirectPathLoad(dp);

            if (state == OCI_DPR_COMPLETE)
                break;
        }
    }

    /* commits changes */

    OCI_DirectPathFinish(dp);

    printf("%04d row(s) loaded\n", OCI_DirectPathGetRowCount(dp));

    /* free direct path object */

    OCI_DirectPathFree(dp);

    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}
