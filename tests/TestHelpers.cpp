#include "ocilib_tests.h"

namespace TestCApi
{
    TEST(TestHelpers, BindTimestampAndInterval)
    {
        ExecDML(OTEXT("create table TestHelpersTable (id int, ts timestamp, itv interval day to second)"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        const auto tmsp = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        const auto itv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);

        ASSERT_NE(nullptr, tmsp);
        ASSERT_NE(nullptr, itv);

        ASSERT_TRUE(OCI_TimestampSysTimestamp(tmsp));
        ASSERT_TRUE(OCI_IntervalSetDaySecond(itv, 1, 2, 3, 4, 0));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("insert into TestHelpersTable values (1, :1, :2)")));
        ASSERT_NE(nullptr, OCI_BindTimestamp(stmt, OTEXT(":1"), tmsp));
        ASSERT_NE(nullptr, OCI_BindInterval(stmt, OTEXT(":2"), itv));
        ASSERT_TRUE(OCI_Execute(stmt));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select count(*) from TestHelpersTable")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));
        ASSERT_EQ(1, OCI_GetInt(rs, 1));

        ASSERT_TRUE(OCI_IntervalFree(itv));
        ASSERT_TRUE(OCI_TimestampFree(tmsp));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestHelpersTable"));
    }

    TEST(TestHelpers, FetchLong)
    {
        ExecDML(OTEXT("create table TestHelpersLongTable (id int, content long)"));
        ExecDML(OTEXT("insert into TestHelpersLongTable values (1, 'hello world')"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select content from TestHelpersLongTable where id = 1")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_FetchNext(rs));

        const auto lg = OCI_GetLong(rs, 1);
        ASSERT_NE(nullptr, lg);
        ASSERT_GT(OCI_LongGetSize(lg), 0u);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestHelpersLongTable"));
    }
}
