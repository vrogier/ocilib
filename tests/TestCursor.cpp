#include "ocilib_tests.h"


TEST(TestCursor, Fetch)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select rownum, cursor(select TO_DATE('19780430', 'yyyymmdd') from dual) from (select 1 from dual connect by level <= 10)")));

    const auto rset = OCI_GetResultset(stmt);
    ASSERT_NE(nullptr, rset);

    int index = 0;

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

            ASSERT_EQ(ostring(OTEXT("19780430")), ostring(buffer));
        }
    }

    ASSERT_EQ(10, OCI_GetRowCount(rset));

    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}