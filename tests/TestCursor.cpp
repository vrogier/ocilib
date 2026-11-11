#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestCursor, Fetch)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select rownum, cursor(select TO_DATE('19780430', 'yyyymmdd') from dual) from (select 1 from dual connect by level <= 10)")));

        const auto rset = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rset);

        OCI_Int32 index = 0;

        while (OCI_ResultsetFetchNext(rset))
        {
            index++;
            ASSERT_EQ(index, OCI_ResultsetGetInt32(rset, 0));

            const auto stmt2 = OCI_ResultsetGetCursor(rset, 1);
            ASSERT_NE(nullptr, stmt2);
            const auto rset2 = OCI_StatementGetResultset(stmt2);
            ASSERT_NE(nullptr, stmt2);

            while (OCI_ResultsetFetchNext(rset2))
            {
                const auto date = OCI_ResultsetGetDate(rset2, 0);
                ASSERT_NE(nullptr, date);

                otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
                ASSERT_TRUE(OCI_DateToText(date, OTEXT("yyyymmdd"), OCI_SIZE_BUFFER, buffer));

                ASSERT_EQ(String(OTEXT("19780430")), String(buffer));
            }
        }

        ASSERT_EQ(10, OCI_ResultsetGetRowCount(rset));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    /* -------------------------------------------------------------------------
       Shared fixture: executed resultset (3 rows, 5 cols incl. null/raw/text)
       ------------------------------------------------------------------------- */

    class TestCursorInfo : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_Statement* stmt = nullptr;
        OCI_Resultset* rs = nullptr;

        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
            stmt = OCI_StatementCreate(conn);
            ASSERT_NE(nullptr, stmt);
            ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select rownum as rn, cast(null as int) as nullable, HEXTORAW('ABCD') as raw_col, 'text' as text_col, 12345 as num_col from dual connect by level <= 3")));
            rs = OCI_StatementGetResultset(stmt);
            ASSERT_NE(nullptr, rs);
        }

        void TearDown() override
        {
            if (stmt) OCI_StatementFree(stmt);
            if (conn) OCI_ConnectionFree(conn);
        }
    };

    TEST_F(TestCursorInfo, GetColumnCountOnResultset_ReturnsFive)
    {
        EXPECT_EQ(5, OCI_ResultsetGetColumnCount(rs));
    }

    TEST_F(TestCursorInfo, GetColumnByIndexAndName_ReturnsColumns)
    {
        EXPECT_NE(nullptr, OCI_ResultsetGetColumn(rs, 0));
        EXPECT_NE(nullptr, OCI_ResultsetGetColumnByName(rs, OTEXT("RN")));
    }

    TEST_F(TestCursorInfo, GetColumnIndexOnKnownAndUnknownName_ReturnsIndexOrMinusOne)
    {
        EXPECT_EQ(0, OCI_ResultsetGetColumnIndex(rs, OTEXT("RN")));
        EXPECT_EQ(-1, OCI_ResultsetGetColumnIndex(rs, OTEXT("NOT_A_COLUMN")));
    }

    TEST_F(TestCursorInfo, FetchFirstRow_CoercesIntToTypedAndStringGetters)
    {
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        EXPECT_EQ(1, OCI_ResultsetGetInt32(rs, 0));
        EXPECT_EQ(String(OTEXT("1")), String(OCI_ResultsetGetString(rs, 0)));
    }

    TEST_F(TestCursorInfo, NullColumn_ReportsNullAndZeroSizeByIndexAndName)
    {
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        EXPECT_TRUE(OCI_ResultsetIsColumnNull(rs, 1));
        EXPECT_TRUE(OCI_ResultsetIsColumnNullByName(rs, OTEXT("NULLABLE")));
        EXPECT_EQ(0, OCI_ResultsetGetDataSize(rs, 1));
        EXPECT_EQ(0, OCI_ResultsetGetDataSizeByName(rs, OTEXT("NULLABLE")));
    }

    TEST_F(TestCursorInfo, RawColumn_ReturnsBytesAndSizeByIndexAndName)
    {
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));

        EXPECT_FALSE(OCI_ResultsetIsColumnNull(rs, 2));
        EXPECT_EQ(2, OCI_ResultsetGetDataSize(rs, 2));
        EXPECT_EQ(2, OCI_ResultsetGetDataLength(rs, 2));
        unsigned char buf[16] = { 0 };
        EXPECT_EQ(2, OCI_ResultsetGetRaw(rs, 2, buf, sizeof(buf)));
        memset(buf, 0, sizeof(buf));
        EXPECT_EQ(2, OCI_ResultsetGetRawByName(rs, OTEXT("RAW_COL"), buf, sizeof(buf)));
        EXPECT_EQ(0xAB, buf[0]);
        EXPECT_EQ(0xCD, buf[1]);
    }

    TEST_F(TestCursorInfo, FetchAllRows_FetchesThreeRows)
    {
        OCI_Int32 row = 0;

        while (OCI_ResultsetFetchNext(rs))
        {
            row++;
        }

        EXPECT_EQ(3, row);
    }
}
