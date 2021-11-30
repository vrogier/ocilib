#include "ocilib_tests.h"
#include "ocilib.hpp"

using namespace ocilib;

enum DirectPathErrorTestMode
{
    Default = 0,
    InsufficientBufferError,
    ConversionError = 1,
    LoadError = 2,
    ForceLoad = 3
};

class TestDirectPathErrorModes : public ::testing::TestWithParam<int> {};

std::vector<int> DirectPathErrorTestModes
{ 
    DirectPathErrorTestMode::Default,
    DirectPathErrorTestMode::InsufficientBufferError,
    DirectPathErrorTestMode::InsufficientBufferError | DirectPathErrorTestMode::ForceLoad,
    DirectPathErrorTestMode::ConversionError,
    DirectPathErrorTestMode::ConversionError | DirectPathErrorTestMode::ForceLoad,
    DirectPathErrorTestMode::LoadError,
    DirectPathErrorTestMode::LoadError | DirectPathErrorTestMode::ForceLoad,
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

    char val1[ColSize1 + 1];
    char val2[ColSize2 + 1];
    char val3[ColSize3 + 1];

    int i = 0, j = 0, nb_rows = ArraySize;
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

TEST(TestDirectPathErrorModes, AllErrorWorkflows)
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

    char val1[ColSize1 + 1];
    char val2[ColSize2 + 1];
    char val3[ColSize3 + 1];

    int i = 0, j = 0, nb_rows = ArraySize;
    boolean res = TRUE;

    const auto table = OCI_TypeInfoGet(conn, OTEXT("TestDirectPathTable1"), OCI_TIF_TABLE);
    ASSERT_NE(nullptr, table);

    const auto dp = OCI_DirPathCreate(table, NULL, ColCount, nb_rows);
    ASSERT_NE(nullptr, dp);

    //TODO

    /* cleanup */

    ASSERT_TRUE(OCI_DirPathFree(dp));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestDirectPathTable1"));
}

INSTANTIATE_TEST_CASE_P(TestDirectPathErrorModes, TestDirectPathErrorModes, ::testing::ValuesIn(DirectPathErrorTestModes));

