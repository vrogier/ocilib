#include "ocilib_tests.h"

namespace TestCApi
{

    void TestRow(OCI_Resultset* rslt, OCI_UInt32 index)
    {
        ASSERT_EQ(index, OCI_GetCurrentRow(rslt));
        ASSERT_EQ(index, OCI_GetInt(rslt, 1));
        ASSERT_EQ(ostring(OTEXT("Row ")) + TO_STRING(index), ostring(OCI_GetString(rslt, 2)));
    }

    TEST(TestScrollableCursor, Fetch)
    {
        const OCI_UInt32 RowCount = 65;

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_SetFetchMode(stmt, OCI_SFM_SCROLLABLE));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select rownum, 'Row ' || rownum from (select 1 from dual connect by level <= 65)")));

        auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_FetchLast(rs));
        ASSERT_EQ(RowCount, OCI_GetRowCount(rs));
        TestRow(rs, RowCount);

        ASSERT_TRUE(OCI_FetchFirst(rs));
        TestRow(rs, 1);

        OCI_UInt32 index = 1;
        while (OCI_FetchNext(rs))
        {
            TestRow(rs, ++index);
        }

        while (OCI_FetchPrev(rs))
        {
            TestRow(rs, --index);
        }

        TestRow(rs, 1);

        ASSERT_TRUE(OCI_FetchSeek(rs, OCI_SFD_ABSOLUTE, 30));
        TestRow(rs, 30);

        ASSERT_TRUE(OCI_FetchSeek(rs, OCI_SFD_RELATIVE, -25));
        TestRow(rs, 5);

        ASSERT_TRUE(OCI_FetchSeek(rs, OCI_SFD_RELATIVE, 42));
        TestRow(rs, 47);

        ASSERT_TRUE(OCI_FetchNext(rs));
        TestRow(rs, 48);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }
}