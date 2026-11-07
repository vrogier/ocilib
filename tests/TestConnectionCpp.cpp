#include <thread>

#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestConnectionCpp, CreateOpenCloseAndBasicInfo)
    {
        Environment& env = GetEnvironmentCpp();

        Connection conn;
        ASSERT_TRUE(conn.IsNull());
        conn.Open(env, DBS, USR, PWD);
        ASSERT_FALSE(conn.IsNull());

        ASSERT_TRUE(conn.IsServerAlive());
        ASSERT_TRUE(conn.PingServer());

        ASSERT_EQ(DBS, conn.GetConnectionString());
        ASSERT_EQ(USR, conn.GetUserName());
        ASSERT_EQ(PWD, conn.GetPassword());

        ASSERT_FALSE(conn.GetServerVersion().empty());
        ASSERT_NE(OracleVersion(), conn.GetVersion());
        ASSERT_GT(conn.GetServerMajorVersion(), 0u);
        ASSERT_GE(conn.GetServerMinorVersion(), 0u);
        ASSERT_GE(conn.GetServerRevisionVersion(), 0u);

        ASSERT_FALSE(conn.GetDatabase().empty());
        ASSERT_FALSE(conn.GetInstance().empty());
        ASSERT_FALSE(conn.GetService().empty());
        ASSERT_FALSE(conn.GetServer().empty());
        (void)conn.GetDomain();
        ASSERT_FALSE(conn.GetInstanceStartTime().IsNull());

        conn.Close();
        ASSERT_TRUE(conn.IsNull());
    }

    TEST(TestConnectionCpp, AutoCommitAndRollback)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        conn.SetAutoCommit(false);
        ASSERT_FALSE(conn.GetAutoCommit());

        Statement stmt(conn);
        stmt.Execute(OTEXT("create table TestConnCpp (id int)"));
        stmt.Execute(OTEXT("insert into TestConnCpp values (1)"));
        conn.Rollback();

        stmt.Execute(OTEXT("select count(*) from TestConnCpp"));
        auto rs = stmt.GetResultset();
        ASSERT_TRUE(rs.Next());
        ASSERT_EQ(0, rs.Get<OCI_Int32>(0));

        conn.Commit();
        stmt.Execute(OTEXT("drop table TestConnCpp"));
        conn.Commit();

        conn.SetAutoCommit(true);
        ASSERT_TRUE(conn.GetAutoCommit());
    }

    TEST(TestConnectionCpp, ServerOutputAndTrace)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        conn.EnableServerOutput(32000, 5, 255);

        Statement stmt(conn);
        stmt.Execute(OTEXT("begin dbms_output.put_line('hello'); end;"));

        String line;
        ASSERT_TRUE(conn.GetServerOutput(line));
        ASSERT_EQ(String(OTEXT("hello")), line);

        stmt.Execute(OTEXT("begin dbms_output.put_line('world'); end;"));
        static std::vector<String> lines;
        conn.GetServerOutput(lines);
        ASSERT_EQ(1u, lines.size());
        ASSERT_EQ(String(OTEXT("world")), lines[0]);

        conn.DisableServerOutput();

        conn.SetTrace(Connection::TraceModule, OTEXT("TestModule"));
        ASSERT_EQ(String(OTEXT("TestModule")), conn.GetTrace(Connection::TraceModule));
    }

    TEST(TestConnectionCpp, FormatAndSessionSettings)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        ASSERT_TRUE(env.SetFormat(FormatTypeValues::FormatDate, OTEXT("YYYY-MM-DD")));
        ASSERT_EQ(String(OTEXT("YYYY-MM-DD")), env.GetFormat(FormatTypeValues::FormatDate));

        conn.SetStatementCacheSize(42);
        ASSERT_EQ(42u, conn.GetStatementCacheSize());
        conn.SetStatementCacheSize(20);
        ASSERT_EQ(20u, conn.GetStatementCacheSize());

        conn.SetDefaultLobPrefetchSize(4096);
        ASSERT_EQ(4096u, conn.GetDefaultLobPrefetchSize());
        conn.SetDefaultLobPrefetchSize(0);
        ASSERT_EQ(0u, conn.GetDefaultLobPrefetchSize());

        ASSERT_GE(conn.GetMaxCursors(), 0u);

        OCI_Int32 value = 42;
        conn.SetUserData(&value);
        ASSERT_EQ(static_cast<AnyPointer>(&value), conn.GetUserData());

        (void)conn.IsTAFCapable();
    }

    TEST(TestConnectionCpp, Timeouts)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        conn.SetTimeout(Connection::TimeoutSend, 60);
        ASSERT_EQ(60u, conn.GetTimeout(Connection::TimeoutSend));
        conn.SetTimeout(Connection::TimeoutSend, 0);
        ASSERT_EQ(0u, conn.GetTimeout(Connection::TimeoutSend));

        // TimeoutCall may not be supported by older clients; just exercise GetTimeout safely.
        (void)conn.GetTimeout(Connection::TimeoutCall);
    }
}