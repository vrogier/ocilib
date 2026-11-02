    #include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestHandle, EnvironmentDestroyedFreesConnection)
    {
        Connection con;

        {
            Environment env(Environment::Default | Environment::Threaded);
            con = Connection(env, DBS, USR, PWD);
            ASSERT_FALSE(con.IsNull());
        }

        ASSERT_TRUE(con.IsNull());
    }

    TEST(TestHandle, ConnectionDestroyedFreesStatement)
    {
        Environment& env = GetEnvironmentCpp();
        Statement stmt;

        {
            Connection con(env, DBS, USR, PWD);
            stmt = Statement(con);
            ASSERT_FALSE(stmt.IsNull());
        }

        ASSERT_TRUE(stmt.IsNull());
    }

    TEST(TestHandle, StatementDestroyedFreesResultset)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Resultset rs;

        {
            Statement stmt(con);
            stmt.Execute(OTEXT("SELECT 1 FROM dual"));
            rs = stmt.GetResultset();
            ASSERT_FALSE(rs.IsNull());
        }

        ASSERT_TRUE(rs.IsNull());
    }

    TEST(TestHandle, ConnectionDestroyedFreesResultset)
    {
        Environment& env = GetEnvironmentCpp();
        Statement stmt;
        Resultset rs;

        {
            Connection con(env, DBS, USR, PWD);
            stmt = Statement(con);
            stmt.Execute(OTEXT("SELECT 1 FROM dual"));
            rs = stmt.GetResultset();
            ASSERT_FALSE(stmt.IsNull());
            ASSERT_FALSE(rs.IsNull());
        }

        ASSERT_TRUE(stmt.IsNull());
        ASSERT_TRUE(rs.IsNull());
    }

    TEST(TestHandle, EnvironmentDestroyedFreesResultset)
    {
        Statement stmt;
        Resultset rs;
        Connection con;

        {
            Environment env(Environment::Default | Environment::Threaded);
            con = Connection(env, DBS, USR, PWD);
            stmt = Statement(con);
            stmt.Execute(OTEXT("SELECT 1 FROM dual"));
            rs = stmt.GetResultset();
            ASSERT_FALSE(rs.IsNull());
        }

        ASSERT_TRUE(con.IsNull());
        ASSERT_TRUE(stmt.IsNull());
        ASSERT_TRUE(rs.IsNull());
    }

    TEST(TestHandle, EnvironmentDestroyedFreesNumber)
    {
        Number n;

        {
            Environment env(Environment::Default | Environment::Threaded);
            n = Number(env, 0);
            ASSERT_FALSE(n.IsNull());
        }

        ASSERT_TRUE(n.IsNull());
    }

    TEST(TestHandle, EnvironmentDestroyedFreesDate)
    {
        Date d;

        {
            Environment env(Environment::Default | Environment::Threaded);
            d = Date::SysDate(env);
            ASSERT_FALSE(d.IsNull());
        }

        ASSERT_TRUE(d.IsNull());
    }

    TEST(TestHandle, EnvironmentDestroyedFreesInterval)
    {
        Interval i;

        {
            Environment env(Environment::Default | Environment::Threaded);
            i = Interval(env, Interval::DaySecond);
            ASSERT_FALSE(i.IsNull());
        }

        ASSERT_TRUE(i.IsNull());
    }

    TEST(TestHandle, EnvironmentDestroyedFreesTimestamp)
    {
        Timestamp t;

        {
            Environment env(Environment::Default | Environment::Threaded);
            t = Timestamp::SysTimestamp(env);
            ASSERT_FALSE(t.IsNull());
        }

        ASSERT_TRUE(t.IsNull());
    }

    TEST(TestHandle, ConnectionDestroyedFreesLob)
    {
        Environment& env = GetEnvironmentCpp();
        Clob lob;

        {
            Connection con(env, DBS, USR, PWD);
            lob = Clob(con);
            ASSERT_FALSE(lob.IsNull());
        }

        ASSERT_TRUE(lob.IsNull());
    }

    TEST(TestHandle, ConnectionDestroyedFreesFile)
    {
        Environment& env = GetEnvironmentCpp();
        File file;

        {
            Connection con(env, DBS, USR, PWD);
            file = File(con);
            ASSERT_FALSE(file.IsNull());
        }

        ASSERT_TRUE(file.IsNull());
    }

    TEST(TestHandle, TypeInfoDestroyedKeepsObject)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Object obj;

        {
            ExecDML(OTEXT("create or replace type TestHandleTypeCpp as object(code number, name varchar2(30))"));
            TypeInfo ti(con, OTEXT("TestHandleTypeCpp"), TypeInfo::Type);
            obj = Object(ti);
            ASSERT_FALSE(obj.IsNull());
        }

        /* objects are owned by the connection, not by the type info */
        ASSERT_FALSE(obj.IsNull());

        ExecDML(OTEXT("drop type TestHandleTypeCpp force"));
    }

    TEST(TestHandle, ConnectionDestroyedFreesObject)
    {
        Environment& env = GetEnvironmentCpp();
        Object obj;

        {
            Connection con(env, DBS, USR, PWD);
            ExecDML(OTEXT("create or replace type TestHandleTypeCpp as object(code number, name varchar2(30))"));
            TypeInfo ti(con, OTEXT("TestHandleTypeCpp"), TypeInfo::Type);
            obj = Object(ti);
            ASSERT_FALSE(obj.IsNull());
        }

        ASSERT_TRUE(obj.IsNull());
        ExecDML(OTEXT("drop type TestHandleTypeCpp force"));
    }

    TEST(TestHandle, ConnectionDestroyedFreesCollection)
    {
        Environment& env = GetEnvironmentCpp();
        Collection<ostring> coll;

        {
            Connection con(env, DBS, USR, PWD);
            TypeInfo ti(con, OTEXT("SYS.ODCIVARCHAR2LIST"), TypeInfo::Type);
            coll = Collection<ostring>(ti);
            ASSERT_FALSE(coll.IsNull());
        }

        ASSERT_TRUE(coll.IsNull());
    }

    TEST(TestHandle, ConnectionDestroyedFreesTypeInfoAndObject)
    {
        Environment& env = GetEnvironmentCpp();
        TypeInfo ti;
        Object obj;

        {
            Connection con(env, DBS, USR, PWD);
            ExecDML(OTEXT("create or replace type TestHandleTypeCpp as object(code number, name varchar2(30))"));
            ti = TypeInfo(con, OTEXT("TestHandleTypeCpp"), TypeInfo::Type);
            obj = Object(ti);
            ASSERT_FALSE(ti.IsNull());
            ASSERT_FALSE(obj.IsNull());
        }

        ASSERT_TRUE(ti.IsNull());
        ASSERT_TRUE(obj.IsNull());
        ExecDML(OTEXT("drop type TestHandleTypeCpp force"));
    }
}
