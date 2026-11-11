#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestMemory, PrefetchMemory)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ExecDML(OTEXT("drop table TestMemoryPrefetch"));
        ExecDML(OTEXT("create table TestMemoryPrefetch (id number)"));
        ExecDML(OTEXT("insert into TestMemoryPrefetch values (1)"));

        ASSERT_TRUE(OCI_StatementSetPrefetchMemory(stmt, 8192));
        ASSERT_EQ(8192u, OCI_StatementGetPrefetchMemory(stmt));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select * from TestMemoryPrefetch")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        ASSERT_EQ(1, OCI_ResultsetGetInt32(rs, 0));

        ExecDML(OTEXT("drop table TestMemoryPrefetch"));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }
}
