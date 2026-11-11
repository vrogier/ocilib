#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestImplicitResultset, Fetch)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecute(stmt,
            OTEXT("declare")
            OTEXT("  c1 sys_refcursor;")
            OTEXT("  c2 sys_refcursor;")
            OTEXT(" begin")
            OTEXT("  open c1 for select rownum from dual connect by level <= 3;")
            OTEXT("  dbms_sql.return_result (c1); ")
            OTEXT("  open c2 for select rownum from dual connect by level <= 5;")
            OTEXT("  dbms_sql.return_result (c2); ")
            OTEXT("end;")));

        auto rset = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rset);

        static std::vector<OCI_Int32> counts;

        while (rset)
        {
            OCI_Int32 count = 0;

            while (OCI_ResultsetFetchNext(rset))
            {
                count++;
            }

            counts.push_back(count);

            rset = OCI_StatementGetNextResultset(stmt);
        }

        ASSERT_EQ(2, counts.size());
        ASSERT_EQ(3, counts[0]);
        ASSERT_EQ(5, counts[1]);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }
}