#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    TEST(TestDirectPath, BasicLoad)
    {
        ExecDML(OTEXT("create table TestDirpath_Table1 (val_int int, val_str varchar2(50))"));
        ExecDML(OTEXT("truncate table TestDirpath_Table1"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestDirpath_Table1"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, typinf);

        const auto dp = OCI_DirectPathCreate(typinf, nullptr, 2, 10);
        ASSERT_NE(nullptr, dp);

        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 0, OTEXT("val_int"), 12, nullptr));
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 1, OTEXT("val_str"), 50, nullptr));

        ASSERT_TRUE(OCI_DirectPathPrepare(dp));

        const OCI_UInt32 nb_rows = OCI_DirectPathGetMaxRows(dp);
        ASSERT_NE(0u, nb_rows);

        for (OCI_Int32 r = 0; r < static_cast<OCI_Int32>(nb_rows); ++r)
        {
            otext v1[16];
            otext v2[64];

            osprintf(v1, OTEXT("%d"), r);
            osprintf(v2, OTEXT("name_%d"), r);

            ASSERT_TRUE(OCI_DirectPathSetEntry(dp, static_cast<OCI_UInt32>(r), 0, v1, (OCI_UInt32) ostrlen(v1), TRUE));
            ASSERT_TRUE(OCI_DirectPathSetEntry(dp, static_cast<OCI_UInt32>(r), 1, v2, (OCI_UInt32) ostrlen(v2), TRUE));
        }

        ASSERT_TRUE(OCI_DirectPathSetCurrentRows(dp, nb_rows));
        ASSERT_EQ((OCI_UInt32) OCI_DPR_COMPLETE, OCI_DirectPathConvert(dp));
        ASSERT_EQ((OCI_UInt32) OCI_DPR_COMPLETE, OCI_DirectPathLoad(dp));

        ASSERT_TRUE(OCI_DirectPathFinish(dp));
        ASSERT_EQ(nb_rows, OCI_DirectPathGetRowCount(dp));

        ASSERT_TRUE(OCI_DirectPathFree(dp));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestDirpath_Table1"));
    }
    TEST(TestDirectPath, ColumnTypeArms)
    {
        /* exercises the column datatype switch in OCI_DirectPathSetColumn():
           numeric with format, raw/binary types, date with format,
           unsupported type and unknown column name */

        ExecDML(OTEXT("create table TestDirpath_Table2 (val_num number(10,2), val_str varchar2(50), val_raw raw(16), val_blob blob, val_date date, val_lraw long raw, val_bool boolean)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestDirpath_Table2"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, typinf);

        const auto dp = OCI_DirectPathCreate(typinf, nullptr, 7, 4);
        ASSERT_NE(nullptr, dp);

        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 0, OTEXT("val_num"), 12, OTEXT("99999D99")));
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 1, OTEXT("val_str"), 50, nullptr));
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 2, OTEXT("val_raw"), 16, nullptr));
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 3, OTEXT("val_blob"), 64, nullptr));
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 4, OTEXT("val_date"), 20, OTEXT("YYYY-MM-DD")));
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 5, OTEXT("val_lraw"), 16, nullptr));

        /* unsupported datatype and unknown column name throw */

        ASSERT_FALSE(OCI_DirectPathSetColumn(dp, 6, OTEXT("val_bool"), 8, nullptr));
        ASSERT_FALSE(OCI_DirectPathSetColumn(dp, 6, OTEXT("no_such_col"), 8, nullptr));

        ASSERT_TRUE(OCI_DirectPathFree(dp));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestDirpath_Table2"));
    }

    TEST(TestDirectPath, SettersGettersAndStatusArms)
    {
        ExecDML(OTEXT("create table TestDirpath_Table3 (val_num number(10,2), val_str varchar2(50), val_date date) "
                      "partition by range (val_num) (partition P1 values less than (100), partition PM values less than (maxvalue))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        /* qualified name gives a schema -> OCI_ATTR_SCHEMA_NAME arm in Prepare() */

        auto typinf = OCI_TypeInfoGet(conn, OTEXT("usr.TestDirpath_Table3"), OCI_TIF_TABLE);
        if (nullptr == typinf)
        {
            typinf = OCI_TypeInfoGet(conn, OTEXT("TestDirpath_Table3"), OCI_TIF_TABLE);
        }
        ASSERT_NE(nullptr, typinf);

        const auto dp = OCI_DirectPathCreate(typinf, OTEXT("P1"), 3, 16);
        ASSERT_NE(nullptr, dp);

        /* NOT_PREPARED setters */

        ASSERT_TRUE(OCI_DirectPathSetDateFormat(dp, OTEXT("YYYY-MM-DD")));
        ASSERT_TRUE(OCI_DirectPathSetParallel(dp, TRUE));
        ASSERT_TRUE(OCI_DirectPathSetNoLog(dp, TRUE));
        ASSERT_TRUE(OCI_DirectPathSetConvertMode(dp, OCI_DCM_FORCE));
        OCI_DirectPathSetCacheSize(dp, 100);

        /* a small stream buffer makes OCI_DirectPathConvert() hit the FULL arm */

        OCI_DirectPathSetBufferSize(dp, 512);

        /* an unknown column name goes through the ColNotFound throw */

        ASSERT_FALSE(OCI_DirectPathSetColumn(dp, 0, OTEXT("no_such_col"), 10, nullptr));

        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 0, OTEXT("val_num"), 12, OTEXT("99999D99")));
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 1, OTEXT("val_str"), 50, nullptr));
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 2, OTEXT("val_date"), 20, OTEXT("YYYY-MM-DD")));

        if (OCI_DirectPathPrepare(dp))
        {
            ASSERT_EQ(16u, OCI_DirectPathGetMaxRows(dp));

            /* PREPARED-only operations */

            OCI_DirectPathSave(dp);
            OCI_DirectPathFlushRow(dp);

            for (OCI_UInt32 r = 0; r < 16; r++)
            {
                otext vnum[16];
                otext vstr[64];

                osprintf(vnum, OTEXT("%d.50"), r);
                osprintf(vstr, OTEXT("some_text_value_for_row_%d_padding"), r);

                OCI_DirectPathSetEntry(dp, r, 0, vnum, (OCI_UInt32) ostrlen(vnum), TRUE);
                OCI_DirectPathSetEntry(dp, r, 1, vstr, (OCI_UInt32) ostrlen(vstr), TRUE);
                OCI_DirectPathSetEntry(dp, r, 2, OTEXT("2024-01-15"), 10, TRUE);
            }

            /* NULL value, partial flag and oversized value arms of SetEntry() */

            OCI_DirectPathSetEntry(dp, 0, 1, nullptr, 0, TRUE);
            OCI_DirectPathSetEntry(dp, 2, 1, OTEXT("partial"), 7, FALSE);
            OCI_DirectPathSetEntry(dp, 3, 1, OTEXT("a_very_long_string_that_exceeds_the_column_maxsize_for_sure"), 64, TRUE);

            ASSERT_TRUE(OCI_DirectPathSetCurrentRows(dp, 16));
            ASSERT_EQ(16u, OCI_DirectPathGetCurrentRows(dp));

            /* conversion: may return COMPLETE, FULL or PARTIAL depending on
               the effective stream buffer size */

            const auto conv = OCI_DirectPathConvert(dp);
            ASSERT_TRUE(conv == OCI_DPR_COMPLETE || conv == OCI_DPR_FULL || conv == OCI_DPR_PARTIAL);

            /* a second Convert() after a FULL result resets the stream */

            OCI_DirectPathConvert(dp);

            OCI_DirectPathGetAffectedRows(dp);
            OCI_DirectPathGetErrorRow(dp);
            OCI_DirectPathGetErrorColumn(dp);

            const auto load = OCI_DirectPathLoad(dp);
            ASSERT_TRUE(load == OCI_DPR_COMPLETE || load == OCI_DPR_PARTIAL || load == OCI_DPR_FULL || load == OCI_DPR_EMPTY);

            OCI_DirectPathGetRowCount(dp);
            OCI_DirectPathFinish(dp);
        }

        ASSERT_TRUE(OCI_DirectPathFree(dp));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestDirpath_Table3"));
    }

    TEST(TestDirectPath, AbortAndEmptyLoad)
    {
        ExecDML(OTEXT("create table TestDirpath_Table4 (val_str varchar2(50))"));
        ExecDML(OTEXT("truncate table TestDirpath_Table4"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestDirpath_Table4"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, typinf);

        /* loading a stream that converted zero rows returns DPR_EMPTY */

        const auto dp = OCI_DirectPathCreate(typinf, nullptr, 1, 1);
        ASSERT_NE(nullptr, dp);
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 0, OTEXT("val_str"), 50, nullptr));
        ASSERT_TRUE(OCI_DirectPathPrepare(dp));

        const auto conv = OCI_DirectPathConvert(dp);
        ASSERT_TRUE(conv == OCI_DPR_COMPLETE || conv == OCI_DPR_EMPTY);

        const auto load = OCI_DirectPathLoad(dp);
        ASSERT_TRUE(load == OCI_DPR_COMPLETE || load == OCI_DPR_EMPTY);

        OCI_DirectPathReset(dp);

        ASSERT_TRUE(OCI_DirectPathFree(dp));

        /* abort on a prepared dirpath */

        const auto dp2 = OCI_DirectPathCreate(typinf, nullptr, 1, 1);
        ASSERT_NE(nullptr, dp2);
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp2, 0, OTEXT("val_str"), 50, nullptr));
        ASSERT_TRUE(OCI_DirectPathPrepare(dp2));
        ASSERT_TRUE(OCI_DirectPathAbort(dp2));
        ASSERT_TRUE(OCI_DirectPathFree(dp2));

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestDirpath_Table4"));
    }







    TEST(TestDirectPath, GetErrorSentinels_WithoutErrors_ReturnMinusOne)
    {
        ExecDML(OTEXT("create table TestDirpathErr_Table (a number)"));

        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto ti = OCI_TypeInfoGet(conn, OTEXT("TestDirpathErr_Table"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, ti);
        const auto dp = OCI_DirectPathCreate(ti, nullptr, 1, 1);
        ASSERT_NE(nullptr, dp);
        ASSERT_TRUE(OCI_DirectPathSetColumn(dp, 0, OTEXT("a"), 10, nullptr));
        ASSERT_TRUE(OCI_DirectPathPrepare(dp));

        ASSERT_EQ(-1, OCI_DirectPathGetErrorColumn(dp));
        ASSERT_EQ(-1, OCI_DirectPathGetErrorRow(dp));

        ASSERT_TRUE(OCI_DirectPathFree(dp));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ExecDML(OTEXT("drop table TestDirpathErr_Table"));
    }

    TEST(TestDirectPath, Prepare_WithoutColumns_ReturnsFalse)
    {
        ExecDML(OTEXT("create table TestDirpath_Table1 (val_int int, val_str varchar2(50))"));

        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        const auto ti = OCI_TypeInfoGet(conn, OTEXT("TestDirpath_Table1"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, ti);
        const auto dp = OCI_DirectPathCreate(ti, nullptr, 2, 10);
        ASSERT_NE(nullptr, dp);

        ASSERT_FALSE(OCI_DirectPathPrepare(dp));

        ASSERT_TRUE(OCI_DirectPathFree(dp));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    class TestDirpathNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestDirpathNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestDirpathNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "DirPathAbort", [] { return !OCI_DirectPathAbort(nullptr); } },
            { "DirPathConvert", [] { return OCI_DPR_ERROR == OCI_DirectPathConvert(nullptr); } },
            { "DirPathCreate", [] { return nullptr == OCI_DirectPathCreate(nullptr, OTEXT("x"), 1, 1); } },
            { "DirPathFinish", [] { return !OCI_DirectPathFinish(nullptr); } },
            { "DirPathFlushRow", [] { return !OCI_DirectPathFlushRow(nullptr); } },
            { "DirPathFree", [] { return !OCI_DirectPathFree(nullptr); } },
            { "DirPathGetAffectedRows", [] { return 0 == OCI_DirectPathGetAffectedRows(nullptr); } },
            { "DirPathGetCurrentRows", [] { return 0 == OCI_DirectPathGetCurrentRows(nullptr); } },
            { "DirPathGetErrorColumn", [] { return -1 == OCI_DirectPathGetErrorColumn(nullptr); } },
            { "DirPathGetErrorRow", [] { return -1 == OCI_DirectPathGetErrorRow(nullptr); } },
            { "DirPathGetMaxRows", [] { return 0 == OCI_DirectPathGetMaxRows(nullptr); } },
            { "DirPathGetRowCount", [] { return 0 == OCI_DirectPathGetRowCount(nullptr); } },
            { "DirPathLoad", [] { return OCI_DPR_ERROR == OCI_DirectPathLoad(nullptr); } },
            { "DirPathPrepare", [] { return !OCI_DirectPathPrepare(nullptr); } },
            { "DirPathReset", [] { return !OCI_DirectPathReset(nullptr); } },
            { "DirPathSave", [] { return !OCI_DirectPathSave(nullptr); } },
            { "DirPathSetBufferSize", [] { return !OCI_DirectPathSetBufferSize(nullptr, 1); } },
            { "DirPathSetCacheSize", [] { return !OCI_DirectPathSetCacheSize(nullptr, 1); } },
            { "DirPathSetColumn", [] { return !OCI_DirectPathSetColumn(nullptr, 1, OTEXT("x"), 1, OTEXT("x")); } },
            { "DirPathSetConvertMode", [] { return !OCI_DirectPathSetConvertMode(nullptr, 1); } },
            { "DirPathSetCurrentRows", [] { return !OCI_DirectPathSetCurrentRows(nullptr, 1); } },
            { "DirPathSetDateFormat", [] { return !OCI_DirectPathSetDateFormat(nullptr, OTEXT("x")); } },
            { "DirPathSetEntry", [] { return !OCI_DirectPathSetEntry(nullptr, 1, 1, NullArgBuffer(), 1, FALSE); } },
            { "DirPathSetNoLog", [] { return !OCI_DirectPathSetNoLog(nullptr, FALSE); } },
            { "DirPathSetParallel", [] { return !OCI_DirectPathSetParallel(nullptr, FALSE); } },
        }),
        &NullArgCaseName);

}
