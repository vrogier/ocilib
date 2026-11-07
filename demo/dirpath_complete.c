#include "ocilib.h"

/* Direct path complete test suite demonstrating the following workflows :

- Normal loads
- Loads with conversion errors
- Loads with loading errors
- Loads with insufficient buffers
- Loads using force mode
*/

#define SIZE_ARRAY 10
#define NB_LOAD    10
#define SIZE_COL1  20
#define SIZE_COL2  30
#define SIZE_COL3  8
#define NUM_COLS   3
#define TABLE_NAME "test_dp"
#define CONST_NAME "test_dp_unique"
#define NB_ERROR   2
#define NB_TOTAL    (SIZE_ARRAY * NB_LOAD)

static OCI_Boolean partionning_enabled = TRUE;
static OCI_Environment *env = NULL;

void create_table(OCI_Connection *con);
void drop_table(OCI_Connection *con);
void do_load(OCI_Connection *con, OCI_Boolean gen_conv_error, OCI_Boolean gen_load_error, OCI_Boolean gen_buffer_error, OCI_Boolean force, char *test_name);
void check_data(OCI_Connection *con, OCI_DirPath *dp, OCI_Boolean gen_conv_error, OCI_Boolean gen_load_error, OCI_Boolean force);
void check_results(OCI_Connection *con, OCI_DirPath *dp, OCI_Int32 expected, OCI_Int32 result, char *str);
void cleanup(OCI_Connection *con, OCI_DirPath *dp);

int main(void)
{
    OCI_Connection *con = NULL;

    env = OCI_EnvironmentCreate(NULL, NULL, OCI_ENV_DEFAULT);

    if (!env)
    {
        return EXIT_FAILURE;
    }

    const char *db  = getenv("OCILIB_DEMO_DB");
    const char *usr = getenv("OCILIB_DEMO_USR");
    const char *pwd = getenv("OCILIB_DEMO_PWD");

    con = OCI_ConnectionCreate(env, db, usr, pwd, OCI_SESSION_DEFAULT);

    /* Nominal test */
    do_load(con, FALSE, FALSE, FALSE, FALSE, "TEST BASE         - DEFAULT MODE");

    /* insufficient stream buffer size tests */
    do_load(con, FALSE, FALSE, TRUE, FALSE, "TEST SMALL BUFFER - DEFAULT MODE");
    do_load(con, FALSE, FALSE, TRUE, TRUE, "TEST SMALL BUFFER - FORCE   MODE");

    /* conversion error tests */
    do_load(con, TRUE, FALSE, FALSE, FALSE, "TEST CONV   ERROR - DEFAULT MODE");
    do_load(con, TRUE, FALSE, FALSE, TRUE, "TEST CONV   ERROR - FORCE   MODE");

    /* loading error tests */
    if (partionning_enabled)
    {
        do_load(con, FALSE, TRUE, FALSE, FALSE, "TEST LOAD   ERROR - DEFAULT MODE");
        do_load(con, FALSE, TRUE, FALSE, TRUE, "TEST LOAD   ERROR - FORCE   MODE");
    }

    OCI_ConnectionFree(con);
    OCI_EnvironmentFree(env);

    return EXIT_SUCCESS;
}

void create_table(OCI_Connection *con)
{
    /* create a partitioned table (for loading error) */
    OCI_Boolean res = OCI_ImmediateFmt(con, "create table %m(val_int int not null, val_str varchar2(30), val_date date) partition by range(val_int) ( partition test_dp_1 values less than (501),  partition test_dp_2 values less than (1001))", TABLE_NAME);

    /* in case partitioning is not available, create a regular table and disable loading errtests */
    if (!res)
    {
        partionning_enabled = FALSE;
        res = OCI_ImmediateFmt(con, "create table %m(val_int int not null, val_str varchar2(30), val_date date)", TABLE_NAME);
    }

    /* exit tests if table cannot be created */
    if (!res)
    {
        printf("FAILED : table cannot be created\n");
        drop_table(con);
        OCI_ConnectionFree(con);
        OCI_EnvironmentFree(env);
        exit(EXIT_FAILURE);
    }
}

void drop_table(OCI_Connection *con)
{
    OCI_ImmediateFmt(con, "drop table %m", TABLE_NAME);
}

void cleanup(OCI_Connection *con, OCI_DirPath *dp)
{
    OCI_DirPathFree(dp);
    OCI_ConnectionFree(con);
    OCI_EnvironmentFree(env);
}

void check_results(OCI_Connection *con, OCI_DirPath *dp, OCI_Int32 expected, OCI_Int32 result, char *str)
{
    if (result != expected)
    {
        printf("FAILED : %s - expected  %d - result %d\n", str, expected, result);
        OCI_DirPathFree(dp);
        drop_table(con);
        OCI_ConnectionFree(con);
        OCI_EnvironmentFree(env);
        exit(EXIT_FAILURE);
    }
}

void check_data(OCI_Connection *con, OCI_DirPath *dp, OCI_Boolean gen_conv_error, OCI_Boolean gen_load_error, OCI_Boolean force)
{
    OCI_Int32 nb_rows = 0;

    OCI_ImmediateFmt(con, "select count(*) from %m", TABLE_NAME, OCI_ARG_OF(&nb_rows));
    check_results(con, dp, ((gen_conv_error && force) || gen_load_error) ? (NB_TOTAL - NB_ERROR) : NB_TOTAL, nb_rows, "total rows in database");

    OCI_ImmediateFmt(con, "select count(distinct val_int) from %m", TABLE_NAME, OCI_ARG_OF(&nb_rows));
    check_results(con, dp, ((gen_conv_error && force) || gen_load_error) ? (NB_TOTAL - NB_ERROR) : NB_TOTAL, nb_rows, "distinct rows in database");
}

void do_load(OCI_Connection *con, OCI_Boolean gen_conv_error, OCI_Boolean gen_load_error, OCI_Boolean gen_buffer_error, OCI_Boolean force, char *test_name)
{

    char val1[SIZE_COL1 + 1];
    char val2[SIZE_COL2 + 1];
    char val3[SIZE_COL3 + 1];

    OCI_Int32 i = 0, j = 0, nb_rows = SIZE_ARRAY;

    printf("%s : ", test_name);

    /* create table */
    create_table(con);

    /* create direct object */
    OCI_DirPath    * dp = OCI_DirPathCreate(OCI_TypeInfoGet(con, TABLE_NAME, OCI_TIF_TABLE), NULL, NUM_COLS, nb_rows);

    /* optional attributes to set */
    OCI_DirPathSetBufferSize(dp, gen_buffer_error ? 100 : 64000);
    OCI_DirPathSetConvertMode(dp, force ? OCI_DCM_FORCE : OCI_DCM_DEFAULT);

    /* describe the target table */
    OCI_DirPathSetColumn(dp, 0, "VAL_INT", SIZE_COL1, NULL);
    OCI_DirPathSetColumn(dp, 1, "VAL_STR", SIZE_COL2, NULL);
    OCI_DirPathSetColumn(dp, 2, "VAL_DATE", SIZE_COL3, "YYYYMMDD");

    /* prepare the load */
    OCI_DirPathPrepare(dp);

    /* perform the load - NB_LOAD iterations of SIZE_ARRAY rows */
    nb_rows = OCI_DirPathGetMaxRows(dp);

    for (i = 0; i < NB_LOAD; i++)
    {
        /* reset the load at each iteration */
        OCI_DirPathReset(dp);

        /* set the values for the current load iteration */
        for (j = 0; j < nb_rows; j++)
        {
            /* conversion and loading errors are performed on the first load for rows 5 and 8 */
            if (i == 0 && (j == 5 || j == 8) && gen_conv_error)
            {
                /* generate a conversion error - not null clause violation on column val_int  */
                OCI_DirPathSetEntry(dp, j, 0, NULL, 0, TRUE);
                OCI_DirPathSetEntry(dp, j, 1, NULL, 0, TRUE);
                OCI_DirPathSetEntry(dp, j, 2, NULL, 0, TRUE);
            }
            else if (i == 0 && (j == 5 || j == 8) && gen_load_error)
            {
                /* generate a load error - if partitioning available, insert value out of declared partitions ranges */
                sprintf(val1, "%04d", 2500);
                sprintf(val2, "value %05d", j + (i*SIZE_ARRAY));
                sprintf(val3, "%04d%02d%02d", (j % 23) + 1 + 2000, (j % 11) + 1, (j % 23) + 1);

                OCI_DirPathSetEntry(dp, j, 0, val1, (OCI_UInt32)strlen(val1), TRUE);
                OCI_DirPathSetEntry(dp, j, 1, val2, (OCI_UInt32)strlen(val2), TRUE);
                OCI_DirPathSetEntry(dp, j, 2, val3, (OCI_UInt32)strlen(val3), TRUE);
            }
            else
            {
                /* default loading */
                sprintf(val1, "%04d", j + (i*SIZE_ARRAY));
                sprintf(val2, "value %05d", j + (i*SIZE_ARRAY));
                sprintf(val3, "%04d%02d%02d", (j % 23) + 1 + 2000, (j % 11) + 1, (j % 23) + 1);

                OCI_DirPathSetEntry(dp, j, 0, val1, (OCI_UInt32)strlen(val1), TRUE);
                OCI_DirPathSetEntry(dp, j, 1, val2, (OCI_UInt32)strlen(val2), TRUE);
                OCI_DirPathSetEntry(dp, j, 2, val3, (OCI_UInt32)strlen(val3), TRUE);
            }
        }

        /* load data to the server */
        while (TRUE)
        {
            OCI_Int32 nb_conv = 0;
            OCI_Int32 nb_error = 0;

            /* convert data*/
            OCI_Int32 state = OCI_DirPathConvert(dp);

            /* retrieve the number of converted rows */
            nb_conv += OCI_DirPathGetAffectedRows(dp);

            /* conversion or loading errors are performed on the first load */
            if (i == 0)
            {
                if (gen_conv_error && !force && state == OCI_DPR_ERROR)
                {
                    /* on conversion error in default mode, correct values for next conversion */
                    OCI_Int32 row = OCI_DirPathGetErrorRow(dp);

                    sprintf(val1, "%04d", row);
                    sprintf(val2, "value %05d", row);
                    sprintf(val3, "%04d%02d%02d", (row % 23) + 1 + 2000, (row % 11) + 1, (row % 23) + 1);

                    OCI_DirPathSetEntry(dp, row, 0, val1, (OCI_UInt32)strlen(val1), TRUE);
                    OCI_DirPathSetEntry(dp, row, 1, val2, (OCI_UInt32)strlen(val2), TRUE);
                    OCI_DirPathSetEntry(dp, row, 2, val3, (OCI_UInt32)strlen(val3), TRUE);

                    nb_conv = 0;
                }

                if (gen_conv_error && force && state == OCI_DPR_COMPLETE)
                {
                    /* on conversion error in force mode, check we got the expected errors*/
                    OCI_Int32 err_row = OCI_DirPathGetErrorRow(dp);
                    OCI_Int32 err_col = OCI_DirPathGetErrorColumn(dp);

                    while (err_row != 0)
                    {
                        check_results(con, dp, nb_error == 0 ? 5 : 8, err_row, "erred converted row index (force mode)");
                        check_results(con, dp, 0, err_col, "erred column index in force mode");

                        err_row = OCI_DirPathGetErrorRow(dp);
                        err_col = OCI_DirPathGetErrorColumn(dp);

                        nb_error++;
                    }

                    check_results(con, dp, NB_ERROR, nb_error, "conversion errors in force mode");
                }
            }

            if (state == OCI_DPR_FULL)
            {
                /* buffer is too small - load the stream */

                state = OCI_DirPathLoad(dp);

                /* as the stream cannot accept all rows in one go,
                we need to do conversion+load until all rows are processed */

                while (nb_conv < SIZE_ARRAY)
                {
                    for (j = 0; j < nb_rows - nb_conv; j++)
                    {
                        sprintf(val1, "%04d", (j + nb_conv) + (i*SIZE_ARRAY));
                        sprintf(val2, "value %05d", (j + nb_conv) + (i*SIZE_ARRAY));
                        sprintf(val3, "%04d%02d%02d", ((j + nb_conv) % 23) + 1 + 2000, ((j + nb_conv) % 11) + 1, ((j + nb_conv) % 23) + 1);

                        OCI_DirPathSetEntry(dp, j, 0, val1, (OCI_UInt32)strlen(val1), TRUE);
                        OCI_DirPathSetEntry(dp, j, 1, val2, (OCI_UInt32)strlen(val2), TRUE);
                        OCI_DirPathSetEntry(dp, j, 2, val3, (OCI_UInt32)strlen(val3), TRUE);
                    }

                    /* convert again */

                    state = OCI_DirPathConvert(dp);

                    nb_conv = OCI_DirPathGetAffectedRows(dp);

                    /* load again */

                    state = OCI_DirPathLoad(dp);
                }

                break;
            }
            else if (state == OCI_DPR_COMPLETE)
            {
                /* conversion is complete */

                check_results(con, dp, (gen_conv_error && i == 0 && force) ? (SIZE_ARRAY - NB_ERROR) : SIZE_ARRAY, nb_conv, "converted rows");

                state = OCI_DirPathLoad(dp);

                if (gen_load_error && i == 0)
                {
                    /* on loading error in force mode, check we got the expected errors*/
                    OCI_Int32 err_row = OCI_DirPathGetErrorRow(dp);

                    while (err_row != 0)
                    {
                        check_results(con, dp, nb_error == 0 ? 5 : 8, err_row, "erred loaded row index");

                        err_row = OCI_DirPathGetErrorRow(dp);

                        nb_error++;
                    }

                    check_results(con, dp, NB_ERROR, nb_error, "loading errors in force mode");
                }

                check_results(con, dp, (gen_load_error && i == 0) ? nb_conv - NB_ERROR : nb_conv, OCI_DirPathGetAffectedRows(dp), "loaded rows");

                break;
            }
        }
    }

    /* commit load */
    OCI_DirPathFinish(dp);

    /* verify loading results */
    check_results(con, dp, ((gen_conv_error && force) || gen_load_error) ? (NB_TOTAL - NB_ERROR) : NB_TOTAL, OCI_DirPathGetRowCount(dp), "total loaded rows");

    /* check data integrity in database */
    check_data(con, dp, gen_conv_error, gen_load_error, force);

    /* free direct path*/
    OCI_DirPathFree(dp);

    /* drop table */
    drop_table(con);

    printf("SUCCESS\n");
}



