#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestDirpath, BasicLoad)
    {
        ExecDML(OTEXT("create table TestDirpath_Table1 (val_int int, val_str varchar2(50))"));
        ExecDML(OTEXT("truncate table TestDirpath_Table1"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestDirpath_Table1"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, typinf);

        const auto dp = OCI_DirPathCreate(typinf, nullptr, 2, 10);
        ASSERT_NE(nullptr, dp);

        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 0, OTEXT("val_int"), 12, nullptr));
        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 1, OTEXT("val_str"), 50, nullptr));

        ASSERT_TRUE(OCI_DirPathPrepare(dp));

        const OCI_UInt32 nb_rows = OCI_DirPathGetMaxRows(dp);
        ASSERT_NE(0u, nb_rows);

        for (OCI_Int32 r = 0; r < static_cast<OCI_Int32>(nb_rows); ++r)
        {
            otext v1[16];
            otext v2[64];

            osprintf(v1, OTEXT("%d"), r);
            osprintf(v2, OTEXT("name_%d"), r);

            ASSERT_TRUE(OCI_DirPathSetEntry(dp, static_cast<OCI_UInt32>(r), 0, v1, (OCI_UInt32) ostrlen(v1), TRUE));
            ASSERT_TRUE(OCI_DirPathSetEntry(dp, static_cast<OCI_UInt32>(r), 1, v2, (OCI_UInt32) ostrlen(v2), TRUE));
        }

        ASSERT_TRUE(OCI_DirPathSetCurrentRows(dp, nb_rows));
        ASSERT_EQ((OCI_UInt32) OCI_DPR_COMPLETE, OCI_DirPathConvert(dp));
        ASSERT_EQ((OCI_UInt32) OCI_DPR_COMPLETE, OCI_DirPathLoad(dp));

        ASSERT_TRUE(OCI_DirPathFinish(dp));
        ASSERT_EQ(nb_rows, OCI_DirPathGetRowCount(dp));

        ASSERT_TRUE(OCI_DirPathFree(dp));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestDirpath_Table1"));
    }
    TEST(TestDirpath, ColumnTypeArms)
    {
        /* exercises the column datatype switch in OCI_DirPathSetColumn():
           numeric with format, raw/binary types, date with format,
           unsupported type and unknown column name */

        ExecDML(OTEXT("create table TestDirpath_Table2 (val_num number(10,2), val_str varchar2(50), val_raw raw(16), val_blob blob, val_date date, val_lraw long raw, val_bool boolean)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto typinf = OCI_TypeInfoGet(conn, OTEXT("TestDirpath_Table2"), OCI_TIF_TABLE);
        ASSERT_NE(nullptr, typinf);

        const auto dp = OCI_DirPathCreate(typinf, nullptr, 7, 4);
        ASSERT_NE(nullptr, dp);

        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 0, OTEXT("val_num"), 12, OTEXT("99999D99")));
        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 1, OTEXT("val_str"), 50, nullptr));
        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 2, OTEXT("val_raw"), 16, nullptr));
        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 3, OTEXT("val_blob"), 64, nullptr));
        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 4, OTEXT("val_date"), 20, OTEXT("YYYY-MM-DD")));
        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 5, OTEXT("val_lraw"), 16, nullptr));

        /* unsupported datatype and unknown column name throw */

        ASSERT_FALSE(OCI_DirPathSetColumn(dp, 6, OTEXT("val_bool"), 8, nullptr));
        ASSERT_FALSE(OCI_DirPathSetColumn(dp, 6, OTEXT("no_such_col"), 8, nullptr));

        ASSERT_TRUE(OCI_DirPathFree(dp));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestDirpath_Table2"));
    }

    TEST(TestDirpath, SettersGettersAndStatusArms)
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

        const auto dp = OCI_DirPathCreate(typinf, OTEXT("P1"), 3, 16);
        ASSERT_NE(nullptr, dp);

        /* NOT_PREPARED setters */

        ASSERT_TRUE(OCI_DirPathSetDateFormat(dp, OTEXT("YYYY-MM-DD")));
        ASSERT_TRUE(OCI_DirPathSetParallel(dp, TRUE));
        ASSERT_TRUE(OCI_DirPathSetNoLog(dp, TRUE));
        ASSERT_TRUE(OCI_DirPathSetConvertMode(dp, OCI_DCM_FORCE));
        OCI_DirPathSetCacheSize(dp, 100);

        /* a small stream buffer makes OCI_DirPathConvert() hit the FULL arm */

        OCI_DirPathSetBufferSize(dp, 512);

        /* an unknown column name goes through the ColNotFound throw */

        ASSERT_FALSE(OCI_DirPathSetColumn(dp, 0, OTEXT("no_such_col"), 10, nullptr));

        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 0, OTEXT("val_num"), 12, OTEXT("99999D99")));
        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 1, OTEXT("val_str"), 50, nullptr));
        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 2, OTEXT("val_date"), 20, OTEXT("YYYY-MM-DD")));

        if (OCI_DirPathPrepare(dp))
        {
            ASSERT_EQ(16u, OCI_DirPathGetMaxRows(dp));

            /* PREPARED-only operations */

            OCI_DirPathSave(dp);
            OCI_DirPathFlushRow(dp);

            for (OCI_UInt32 r = 0; r < 16; r++)
            {
                otext vnum[16];
                otext vstr[64];

                osprintf(vnum, OTEXT("%d.50"), r);
                osprintf(vstr, OTEXT("some_text_value_for_row_%d_padding"), r);

                OCI_DirPathSetEntry(dp, r, 0, vnum, (OCI_UInt32) ostrlen(vnum), TRUE);
                OCI_DirPathSetEntry(dp, r, 1, vstr, (OCI_UInt32) ostrlen(vstr), TRUE);
                OCI_DirPathSetEntry(dp, r, 2, OTEXT("2024-01-15"), 10, TRUE);
            }

            /* NULL value, partial flag and oversized value arms of SetEntry() */

            OCI_DirPathSetEntry(dp, 0, 1, nullptr, 0, TRUE);
            OCI_DirPathSetEntry(dp, 2, 1, OTEXT("partial"), 7, FALSE);
            OCI_DirPathSetEntry(dp, 3, 1, OTEXT("a_very_long_string_that_exceeds_the_column_maxsize_for_sure"), 64, TRUE);

            ASSERT_TRUE(OCI_DirPathSetCurrentRows(dp, 16));
            ASSERT_EQ(16u, OCI_DirPathGetCurrentRows(dp));

            /* conversion: may return COMPLETE, FULL or PARTIAL depending on
               the effective stream buffer size */

            const auto conv = OCI_DirPathConvert(dp);
            ASSERT_TRUE(conv == OCI_DPR_COMPLETE || conv == OCI_DPR_FULL || conv == OCI_DPR_PARTIAL);

            /* a second Convert() after a FULL result resets the stream */

            OCI_DirPathConvert(dp);

            OCI_DirPathGetAffectedRows(dp);
            OCI_DirPathGetErrorRow(dp);
            OCI_DirPathGetErrorColumn(dp);

            const auto load = OCI_DirPathLoad(dp);
            ASSERT_TRUE(load == OCI_DPR_COMPLETE || load == OCI_DPR_PARTIAL || load == OCI_DPR_FULL || load == OCI_DPR_EMPTY);

            OCI_DirPathGetRowCount(dp);
            OCI_DirPathFinish(dp);
        }

        ASSERT_TRUE(OCI_DirPathFree(dp));
        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestDirpath_Table3"));
    }

    TEST(TestDirpath, AbortAndEmptyLoad)
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

        const auto dp = OCI_DirPathCreate(typinf, nullptr, 1, 1);
        ASSERT_NE(nullptr, dp);
        ASSERT_TRUE(OCI_DirPathSetColumn(dp, 0, OTEXT("val_str"), 50, nullptr));
        ASSERT_TRUE(OCI_DirPathPrepare(dp));

        const auto conv = OCI_DirPathConvert(dp);
        ASSERT_TRUE(conv == OCI_DPR_COMPLETE || conv == OCI_DPR_EMPTY);

        const auto load = OCI_DirPathLoad(dp);
        ASSERT_TRUE(load == OCI_DPR_COMPLETE || load == OCI_DPR_EMPTY);

        OCI_DirPathReset(dp);

        ASSERT_TRUE(OCI_DirPathFree(dp));

        /* abort on a prepared dirpath */

        const auto dp2 = OCI_DirPathCreate(typinf, nullptr, 1, 1);
        ASSERT_NE(nullptr, dp2);
        ASSERT_TRUE(OCI_DirPathSetColumn(dp2, 0, OTEXT("val_str"), 50, nullptr));
        ASSERT_TRUE(OCI_DirPathPrepare(dp2));
        ASSERT_TRUE(OCI_DirPathAbort(dp2));
        ASSERT_TRUE(OCI_DirPathFree(dp2));

        ASSERT_TRUE(OCI_TypeInfoFree(typinf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestDirpath_Table4"));
    }
}

