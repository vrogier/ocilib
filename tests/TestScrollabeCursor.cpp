#include "ocilib_tests.h"

void TestRow(OCI_Resultset *rslt, unsigned int index)
{
    ASSERT_EQ(index, OCI_GetCurrentRow(rslt));
    ASSERT_EQ(index, OCI_GetInt(rslt, 1));
    ASSERT_EQ(ostring("Row ") + std::to_string(index), ostring(OCI_GetString(rslt, 2)));
}

TEST(TestScrollableCursor, Fetch)
{
    const unsigned int RowCount = 65;

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
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

    unsigned int index = 1;
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
    ASSERT_TRUE(OCI_Cleanup());

}
