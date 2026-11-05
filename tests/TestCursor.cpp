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

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select rownum, cursor(select TO_DATE('19780430', 'yyyymmdd') from dual) from (select 1 from dual connect by level <= 10)")));

        const auto rset = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rset);

        OCI_Int32 index = 0;

        while (OCI_FetchNext(rset))
        {
            index++;
            ASSERT_EQ(index, OCI_GetInt(rset, 1));

            const auto stmt2 = OCI_GetStatement(rset, 2);
            ASSERT_NE(nullptr, stmt2);
            const auto rset2 = OCI_GetResultset(stmt2);
            ASSERT_NE(nullptr, stmt2);

            while (OCI_FetchNext(rset2))
            {
                const auto date = OCI_GetDate(rset2, 1);
                ASSERT_NE(nullptr, date);

                otext buffer[OCI_SIZE_BUFFER] = OTEXT("");
                ASSERT_TRUE(OCI_DateToText(date, OTEXT("yyyymmdd"), OCI_SIZE_BUFFER, buffer));

                ASSERT_EQ(String(OTEXT("19780430")), String(buffer));
            }
        }

        ASSERT_EQ(10, OCI_GetRowCount(rset));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestCursor, FetchInfo)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select rownum as rn, cast(null as int) as nullable, HEXTORAW('ABCD') as raw_col, 'text' as text_col, 12345 as num_col from dual connect by level <= 3")));

        const auto rset = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rset);

        ASSERT_EQ(5, OCI_GetColumnCount(rset));

        ASSERT_NE(nullptr, OCI_GetColumn(rset, 1));
        ASSERT_NE(nullptr, OCI_GetColumnByName(rset, OTEXT("RN")));
        ASSERT_EQ(1, OCI_GetColumnIndex(rset, OTEXT("RN")));
        ASSERT_EQ(0, OCI_GetColumnIndex(rset, OTEXT("NOT_A_COLUMN")));

        OCI_Int32 row = 0;
        while (OCI_FetchNext(rset))
        {
            row++;

            ASSERT_EQ(row, OCI_GetInt(rset, 1));
            ASSERT_EQ(String(TO_STRING(row)), String(OCI_GetString(rset, 1)));

            ASSERT_TRUE(OCI_IsNull(rset, 2));
            ASSERT_TRUE(OCI_IsNullByName(rset, OTEXT("NULLABLE")));
            ASSERT_EQ(0, OCI_GetDataSize(rset, 2));
            ASSERT_EQ(0, OCI_GetDataSizeByName(rset, OTEXT("NULLABLE")));

            ASSERT_FALSE(OCI_IsNull(rset, 3));
            ASSERT_EQ(2, OCI_GetDataSize(rset, 3));
            ASSERT_EQ(2, OCI_GetDataLength(rset, 3));

            unsigned char rawBuf[16] = { 0 };
            ASSERT_EQ(2, OCI_GetRaw(rset, 3, rawBuf, sizeof(rawBuf)));
            ASSERT_EQ(2, OCI_GetRawByName(rset, OTEXT("RAW_COL"), rawBuf, sizeof(rawBuf)));
            ASSERT_EQ(0xAB, rawBuf[0]);
            ASSERT_EQ(0xCD, rawBuf[1]);

            ASSERT_EQ(String(OTEXT("text")), String(OCI_GetString(rset, 4)));
            ASSERT_EQ(String(OTEXT("12345")), String(OCI_GetString(rset, 5)));
        }

        ASSERT_EQ(3, row);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }
}