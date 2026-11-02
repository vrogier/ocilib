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

        ASSERT_TRUE(OCI_SetPrefetchMemory(stmt, 8192));
        ASSERT_EQ(8192u, OCI_GetPrefetchMemory(stmt));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select * from TestMemoryPrefetch")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(1, OCI_GetInt(rs, 1));

        ExecDML(OTEXT("drop table TestMemoryPrefetch"));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }
}
