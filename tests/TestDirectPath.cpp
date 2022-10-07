#include "ocilib_tests.h"
#include "ocilib.hpp"

using namespace ocilib;

enum DirectPathErrorTestMode
{
    Default = 0,
    InsufficientBufferError = 1,
    ConversionError = 2,
    LoadError = 4,
    ForceLoad = 8
};

class TestDirectPathErrorModes : public ::testing::TestWithParam<int> {};

std::vector<int> DirectPathErrorTestModes
{ 
    DirectPathErrorTestMode::Default,
  //  DirectPathErrorTestMode::InsufficientBufferError,
  //  DirectPathErrorTestMode::InsufficientBufferError | DirectPathErrorTestMode::ForceLoad,
    DirectPathErrorTestMode::ConversionError,
    DirectPathErrorTestMode::ConversionError | DirectPathErrorTestMode::ForceLoad,
    DirectPathErrorTestMode::LoadError,
    DirectPathErrorTestMode::LoadError | DirectPathErrorTestMode::ForceLoad
};
TEST(TestDirectPath, Simple)
{
    auto constexpr ArraySize = 10u;
    auto constexpr LoadSize = 10u;
    auto constexpr ColCount = 3u;
    auto constexpr ColSize1 = 20u;
    auto constexpr ColSize2 = 30u;
    auto constexpr ColSize3 = 8u;

    ExecDML(OTEXT("create table TestDirectPathTable1 (val_int int, val_str varchar2(50), val_date date)"));
    ExecDML(OTEXT("truncate table TestDirectPathTable1"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    otext val1[ColSize1 + 1];
    otext val2[ColSize2 + 1];
    otext val3[ColSize3 + 1];

    unsigned int i = 0, j = 0, nb_rows = ArraySize;
    boolean res = TRUE;
    
    const auto table = OCI_TypeInfoGet(conn, OTEXT("TestDirectPathTable1"), OCI_TIF_TABLE);
    ASSERT_NE(nullptr, table);

    const auto dp = OCI_DirPathCreate(table, NULL, ColCount, nb_rows);
    ASSERT_NE(nullptr, dp);

    /* optional attributes to set */

    ASSERT_TRUE(OCI_DirPathSetBufferSize(dp, 64000));
    ASSERT_TRUE(OCI_DirPathSetNoLog(dp, TRUE));
    ASSERT_TRUE(OCI_DirPathSetParallel(dp, TRUE));

    /* describe the target table */

    ASSERT_TRUE(OCI_DirPathSetColumn(dp, 1, OTEXT("val_int"), ColSize1, nullptr));
    ASSERT_TRUE(OCI_DirPathSetColumn(dp, 2, OTEXT("val_str"), ColSize2, nullptr));
    ASSERT_TRUE(OCI_DirPathSetColumn(dp, 3, OTEXT("val_date"), ColSize3, OTEXT("YYYYMMDD")));

    /* prepare the load */

    ASSERT_TRUE(OCI_DirPathPrepare(dp));

    nb_rows = OCI_DirPathGetMaxRows(dp);
    ASSERT_NE(0, nb_rows);

    for (i = 0; i < LoadSize; i++)
    {
        ASSERT_TRUE(OCI_DirPathReset(dp));

        for (j = 1; j <= nb_rows; j++)
        {
            /* fill test values */

            osprintf(val1, "%04d", i + (i * 100));
            osprintf(val2, "value %05d", j + (i * 100));
            osprintf(val3, "%04d%02d%02d", (j % 23) + 1 + 2000, (j % 11) + 1, (j % 23) + 1);

            ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 1, val1, (unsigned int)ostrlen(val1), TRUE));
            ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 2, val2, (unsigned int)ostrlen(val2), TRUE));
            ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 3, val3, (unsigned int)ostrlen(val3), TRUE));
        }

        /* load data to the server */

        while (res)
        {
            int state = OCI_DirPathConvert(dp);

            if ((state == OCI_DPR_FULL) || (state == OCI_DPR_COMPLETE))
                res = OCI_DirPathLoad(dp);

            if (state == OCI_DPR_COMPLETE)
                break;

            if (state == OCI_DPR_ERROR)
                break;
        }
    }

    /* commits changes */

    ASSERT_TRUE(OCI_DirPathFinish(dp));

    ASSERT_EQ(ArraySize * LoadSize, OCI_DirPathGetRowCount(dp));

    /* cleanup */

    ASSERT_TRUE(OCI_DirPathFree(dp));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestDirectPathTable1"));
}

void CheckAllErrorWorkflows(OCI_Connection* con, OCI_DirPath* dp, int expected, int result, const char* str)
{
    ASSERT_EQ(expected, result) << str;
}

void CheckAllErrorWorkflowsData(OCI_Connection* con, OCI_DirPath* dp, boolean gen_conv_error, boolean gen_load_error, boolean force)
{
    auto constexpr ArraySize = 10u;
    auto constexpr LoadSize = 10u;
    auto constexpr TotalCount = ArraySize * LoadSize;
    auto constexpr ErrCount = 2u;
    
    int nb_rows = 0;

    OCI_Immediate(con, "select count(*) from TestDirectPathTable2", OCI_ARG_INT, &nb_rows);
    CheckAllErrorWorkflows(con, dp, ((gen_conv_error && force) || gen_load_error) ? (TotalCount - ErrCount) : TotalCount, nb_rows, "total rows in database");

    OCI_ImmediateFmt(con, "select count(distinct val_int) from TestDirectPathTable2", OCI_ARG_INT, &nb_rows);
    CheckAllErrorWorkflows(con, dp, ((gen_conv_error && force) || gen_load_error) ? (TotalCount - ErrCount) : TotalCount, nb_rows, "distinct rows in database");
}

TEST_P(TestDirectPathErrorModes, AllErrorWorkflows)
{
    auto constexpr ArraySize = 10u;
    auto constexpr LoadSize = 10u;
    auto constexpr ColCount = 3u;
    auto constexpr ColSize1 = 20u;
    auto constexpr ColSize2 = 30u;
    auto constexpr ColSize3 = 8u;
    auto constexpr ErrCount = 2u;
    auto constexpr TotalCount = ArraySize * LoadSize;

    auto mode = GetParam();

    ExecDML(OTEXT("create table TestDirectPathTable2(val_int int not null, val_str varchar2(30), val_date date) partition by range(val_int) ( partition test_dp_1 values less than (501),  partition test_dp_2 values less than (1001))"));
    ExecDML(OTEXT("truncate table TestDirectPathTable2"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    otext val1[ColSize1 + 1];
    otext val2[ColSize2 + 1];
    otext val3[ColSize3 + 1];

    unsigned int i = 0, j = 0, nb_rows = ArraySize;

    const auto table = OCI_TypeInfoGet(conn, OTEXT("TestDirectPathTable2"), OCI_TIF_TABLE);
    ASSERT_NE(nullptr, table);

    const auto dp = OCI_DirPathCreate(table, NULL, ColCount, nb_rows);
    ASSERT_NE(nullptr, dp);

    bool force = mode & DirectPathErrorTestMode::ForceLoad;
    bool gen_buffer_error = mode & DirectPathErrorTestMode::InsufficientBufferError;
    bool gen_conv_error = mode & DirectPathErrorTestMode::ConversionError;
    bool gen_load_error = mode & DirectPathErrorTestMode::LoadError;

    /* optional attributes to set */

    ASSERT_TRUE(OCI_DirPathSetBufferSize(dp, gen_buffer_error ? 100 : 64000));
    ASSERT_TRUE(OCI_DirPathSetConvertMode(dp, force ? OCI_DCM_FORCE : OCI_DCM_DEFAULT));

    /* describe the target table */

    ASSERT_TRUE(OCI_DirPathSetColumn(dp, 1, OTEXT("val_int"), ColSize1, nullptr));
    ASSERT_TRUE(OCI_DirPathSetColumn(dp, 2, OTEXT("val_str"), ColSize2, nullptr));
    ASSERT_TRUE(OCI_DirPathSetColumn(dp, 3, OTEXT("val_date"), ColSize3, OTEXT("YYYYMMDD")));

    /* prepare the load */

    ASSERT_TRUE(OCI_DirPathPrepare(dp));

    nb_rows = OCI_DirPathGetMaxRows(dp);
    ASSERT_NE(0, nb_rows);

    for (i = 0; i < LoadSize; i++)
    {
        ASSERT_TRUE(OCI_DirPathReset(dp));

        /* set the values for the current load iteration */
        for (j = 1; j <= nb_rows; j++)
        {
            /* conversion and loading errors are performed on the first load for rows 5 and 8 */
            if (i == 0 && (j == 5 || j == 8) && gen_conv_error)
            {
                /* generate a conversion error - not null clause violation on column val_int  */
                ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 1, NULL, 0, TRUE));
                ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 2, NULL, 0, TRUE));
                ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 3, NULL, 0, TRUE));
            }
            else if (i == 0 && (j == 5 || j == 8) && gen_load_error)
            {
                /* generate a load error - if partitioning available, insert value out of declared partitions ranges */
                osprintf(val1, OTEXT("%04d"), 2500);
                osprintf(val2, OTEXT("value %05d"), j + (i * ArraySize));
                osprintf(val3, OTEXT("%04d%02d%02d"), (j % 23) + 1 + 2000, (j % 11) + 1, (j % 23) + 1);

                ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 1, val1, (unsigned int)ostrlen(val1), TRUE));
                ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 2, val2, (unsigned int)ostrlen(val2), TRUE));
                ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 3, val3, (unsigned int)ostrlen(val3), TRUE));
            }
            else
            {
                /* default loading */
                osprintf(val1, OTEXT("%04d"), j + (i * ArraySize));
                osprintf(val2, OTEXT("value %05d"), j + (i * ArraySize));
                osprintf(val3, OTEXT("%04d%02d%02d"), (j % 23) + 1 + 2000, (j % 11) + 1, (j % 23) + 1);

                ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 1, val1, (unsigned int)ostrlen(val1), TRUE));
                ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 2, val2, (unsigned int)ostrlen(val2), TRUE));
                ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 3, val3, (unsigned int)ostrlen(val3), TRUE));
            }
        }

        /* load data to the server */
        while (TRUE)
        {
            unsigned int nb_conv = 0;
            int nb_error = 0;

            /* convert data*/
            int state = OCI_DirPathConvert(dp);

            /* retrieve the number of converted rows */
            nb_conv += OCI_DirPathGetAffectedRows(dp);

            /* conversion or loading errors are performed on the first load */
            if (i == 0)
            {
                if (gen_conv_error && !force && state == OCI_DPR_ERROR)
                {
                    /* on conversion error in default mode, correct values for next conversion */
                    int row = OCI_DirPathGetErrorRow(dp);

                    osprintf(val1, OTEXT("%04d"), row);
                    osprintf(val2, OTEXT("value %05d"), row);
                    osprintf(val3, OTEXT("%04d%02d%02d"), (row % 23) + 1 + 2000, (row % 11) + 1, (row % 23) + 1);

                    ASSERT_TRUE(OCI_DirPathSetEntry(dp, row, 1, val1, (unsigned int)ostrlen(val1), TRUE));
                    ASSERT_TRUE(OCI_DirPathSetEntry(dp, row, 2, val2, (unsigned int)ostrlen(val2), TRUE));
                    ASSERT_TRUE(OCI_DirPathSetEntry(dp, row, 3, val3, (unsigned int)ostrlen(val3), TRUE));

                    nb_conv = 0;
                }

                if (gen_conv_error && force && state == OCI_DPR_COMPLETE)
                {
                    /* on conversion error in force mode, check we got the expected errors*/
                    int err_row = OCI_DirPathGetErrorRow(dp);
                    int err_col = OCI_DirPathGetErrorColumn(dp);

                    while (err_row != 0)
                    {
                        CheckAllErrorWorkflows(conn, dp, nb_error == 0 ? 5 : 8, err_row, "erred converted row index (force mode)");
                        CheckAllErrorWorkflows(conn, dp, err_col, 1, "erred column index in force mode");

                        err_row = OCI_DirPathGetErrorRow(dp);
                        err_col = OCI_DirPathGetErrorColumn(dp);

                        nb_error++;
                    }

                    CheckAllErrorWorkflows(conn, dp, ErrCount, nb_error, "conversion errors in force mode");
                }
            }

            if (state == OCI_DPR_FULL)
            {
                /* buffer is too small - load the stream */

                state = OCI_DirPathLoad(dp);

                /* as the stream cannot accept all rows in one go,
                we need to do conversion+load until all rows are processed */

                while (nb_conv < ArraySize)
                {
                    for (j = 1; j <= nb_rows - nb_conv; j++)
                    {
                        osprintf(val1, OTEXT("%04d"), (j + nb_conv) + (i * ArraySize));
                        osprintf(val2, OTEXT("value %05d"), (j + nb_conv) + (i * ArraySize));
                        osprintf(val3, OTEXT("%04d%02d%02d"), ((j + nb_conv) % 23) + 1 + 2000, ((j + nb_conv) % 11) + 1, ((j + nb_conv) % 23) + 1);

                        ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 1, val1, (unsigned int)ostrlen(val1), TRUE));
                        ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 2, val2, (unsigned int)ostrlen(val2), TRUE));
                        ASSERT_TRUE(OCI_DirPathSetEntry(dp, j, 3, val3, (unsigned int)ostrlen(val3), TRUE));
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

                CheckAllErrorWorkflows(conn, dp, (gen_conv_error && i == 0 && force) ? (ArraySize - ErrCount) : ArraySize, nb_conv, "converted rows");

                state = OCI_DirPathLoad(dp);

                if (gen_load_error && i == 0)
                {
                    /* on loading error in force mode, check we got the expected errors*/
                    int err_row = OCI_DirPathGetErrorRow(dp);

                    while (err_row != 0)
                    {
                        CheckAllErrorWorkflows(conn, dp, nb_error == 0 ? 5 : 8, err_row, "erred loaded row index");

                        err_row = OCI_DirPathGetErrorRow(dp);

                        nb_error++;
                    }

                    CheckAllErrorWorkflows(conn, dp, ErrCount, nb_error, "loading errors in force mode");
                }

                CheckAllErrorWorkflows(conn, dp, (gen_load_error && i == 0) ? nb_conv - ErrCount : nb_conv, OCI_DirPathGetAffectedRows(dp), "loaded rows");

                break;
            }
        }
    }

    /* commits changes */

    ASSERT_TRUE(OCI_DirPathFinish(dp));

    /* verify loading results */
    CheckAllErrorWorkflows(conn, dp, ((gen_conv_error&& force) || gen_load_error) ? (TotalCount - ErrCount) : TotalCount, OCI_DirPathGetRowCount(dp), "total loaded rows");

    /* check data integrity in database */
    CheckAllErrorWorkflowsData(conn, dp, gen_conv_error, gen_load_error, force);

    /* cleanup */

    ASSERT_TRUE(OCI_DirPathFree(dp));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestDirectPathTable2"));
}

INSTANTIATE_TEST_CASE_P(TestDirectPathErrorModes, TestDirectPathErrorModes, ::testing::ValuesIn(DirectPathErrorTestModes));

