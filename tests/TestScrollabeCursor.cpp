#include "ocilib_tests.h"

namespace TestCApi
{

    void TestRow(OCI_Resultset* rslt, OCI_UInt32 index)
    {
        ASSERT_EQ(static_cast<OCI_Int32>(index) - 1, OCI_ResultsetGetCurrentRow(rslt));
        ASSERT_EQ(index, OCI_ResultsetGetInt32(rslt, 0));
        ASSERT_EQ(String(OTEXT("Row ")) + TO_STRING(index), String(OCI_ResultsetGetString(rslt, 1)));
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

        ASSERT_TRUE(OCI_StatementSetFetchMode(stmt, OCI_SFM_SCROLLABLE));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select rownum, 'Row ' || rownum from (select 1 from dual connect by level <= 65)")));

        auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_ResultsetFetchLast(rs));
        ASSERT_EQ(RowCount, OCI_ResultsetGetRowCount(rs));
        TestRow(rs, RowCount);

        ASSERT_TRUE(OCI_ResultsetFetchFirst(rs));
        TestRow(rs, 1);

        OCI_UInt32 index = 1;
        while (OCI_ResultsetFetchNext(rs))
        {
            TestRow(rs, ++index);
        }

        while (OCI_ResultsetFetchPrev(rs))
        {
            TestRow(rs, --index);
        }

        TestRow(rs, 1);

        ASSERT_TRUE(OCI_ResultsetFetchSeek(rs, OCI_SFD_ABSOLUTE, 29));
        TestRow(rs, 30);

        ASSERT_TRUE(OCI_ResultsetFetchSeek(rs, OCI_SFD_RELATIVE, -25));
        TestRow(rs, 5);

        ASSERT_TRUE(OCI_ResultsetFetchSeek(rs, OCI_SFD_RELATIVE, 42));
        TestRow(rs, 47);

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        TestRow(rs, 48);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }
}