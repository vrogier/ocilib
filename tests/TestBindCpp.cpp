#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestBindCpp, ArrayBindIntegers)
    {
        ExecDML(OTEXT("create table TestBindCpp_Array (id int)"));

        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        static std::vector<OCI_Int32> ids = { 10, 20, 30 };

        stmt.Prepare(OTEXT("insert into TestBindCpp_Array values (:id)"));
        stmt.SetBindArraySize(static_cast<OCI_UInt32>(ids.size()));
        stmt.Bind(OTEXT(":id"), ids, BindInfo::In);
        stmt.AllowRebinding(true);
        stmt.Bind(OTEXT(":id"), ids, BindInfo::In);
        stmt.ExecutePrepared();

        Statement verify(con);
        verify.Execute(OTEXT("select count(*) from TestBindCpp_Array where id in (10, 20, 30)"));
        Resultset rs = verify.GetResultset();
        ASSERT_TRUE(rs.Next());
        ASSERT_EQ(3, rs.Get<OCI_Int32>(0));

        ExecDML(OTEXT("drop table TestBindCpp_Array"));
    }

    TEST(TestBindCpp, ArrayBindStrings)
    {
        ExecDML(OTEXT("create table TestBindCpp_ArrayString (id int, name varchar2(50))"));

        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        static std::vector<OCI_Int32> ids = { 1, 2, 3 };
        static std::vector<String> names = { String(OTEXT("alice")), String(OTEXT("bob")), String(OTEXT("charlie")) };

        stmt.Prepare(OTEXT("insert into TestBindCpp_ArrayString values (:id, :name)"));
        stmt.SetBindArraySize(static_cast<OCI_UInt32>(ids.size()));
        stmt.Bind(OTEXT(":id"), ids, BindInfo::In);
        stmt.Bind(OTEXT(":name"), names, 50u, BindInfo::In);
        stmt.AllowRebinding(true);
        stmt.Bind(OTEXT(":name"), names, 50u, BindInfo::In);
        stmt.ExecutePrepared();

        Statement verify(con);
        verify.Execute(OTEXT("select count(*) from TestBindCpp_ArrayString where name in ('alice','bob','charlie')"));
        Resultset rs = verify.GetResultset();
        ASSERT_TRUE(rs.Next());
        ASSERT_EQ(3, rs.Get<OCI_Int32>(0));

        ExecDML(OTEXT("drop table TestBindCpp_ArrayString"));
    }

    TEST(TestBindCpp, ScalarOutBind)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        OCI_Int32 value1 = 0;
        OCI_Int32 value2 = 0;

        stmt.Prepare(OTEXT("BEGIN :value := 123; END;"));
        stmt.Bind(OTEXT(":value"), value1, BindInfo::Out);
        stmt.AllowRebinding(true);
        stmt.Bind(OTEXT(":value"), value2, BindInfo::Out);
        stmt.ExecutePrepared();

        ASSERT_EQ(123, value2);
    }

    TEST(TestBindCpp, PlSqlTableIntOut)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        static std::vector<OCI_Int32> out(3);

        stmt.Prepare(OTEXT("DECLARE TYPE num_tab IS TABLE OF INTEGER INDEX BY BINARY_INTEGER; v num_tab; BEGIN v(1) := 10; v(2) := 20; v(3) := 30; :ids := v; END;"));
        stmt.Bind(OTEXT(":ids"), out, BindInfo::Out, BindInfo::AsPlSqlTable);
        stmt.ExecutePrepared();

        ASSERT_EQ(10, out[0]);
        ASSERT_EQ(20, out[1]);
        ASSERT_EQ(30, out[2]);
    }

    TEST(TestBindCpp, PlSqlTableStringOut)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        static std::vector<String> out(2);

        stmt.Prepare(OTEXT("DECLARE TYPE str_tab IS TABLE OF VARCHAR2(50) INDEX BY BINARY_INTEGER; v str_tab; BEGIN v(1) := 'alice'; v(2) := 'bob'; :names := v; END;"));
        stmt.Bind(OTEXT(":names"), out, 50u, BindInfo::Out, BindInfo::AsPlSqlTable);
        stmt.ExecutePrepared();

        ASSERT_EQ(String(OTEXT("alice")), out[0]);
        ASSERT_EQ(String(OTEXT("bob")), out[1]);
    }

    TEST(TestBindCpp, BindInfoUsage)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        OCI_Int32 value = 0;
        stmt.Prepare(OTEXT("BEGIN :value := 456; END;"));
        stmt.Bind(OTEXT(":value"), value, BindInfo::Out);

        BindInfo bind = stmt.GetBind(OTEXT(":value"));
        ASSERT_EQ(String(OTEXT(":value")), bind.GetName());

        stmt.ExecutePrepared();
        ASSERT_EQ(456, value);
    }

    TEST(TestBindCpp, DuplicateVectorBindThrows)
    {
        ExecDML(OTEXT("create table TestBind_Dup (id int)"));

        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        static std::vector<OCI_Int32> ids = { 1, 2, 3 };

        stmt.Prepare(OTEXT("insert into TestBind_Dup values (:id)"));
        stmt.SetBindArraySize(static_cast<OCI_UInt32>(ids.size()));
        stmt.Bind(OTEXT(":id"), ids, BindInfo::In);

        ASSERT_THROW((void)(stmt.Bind(OTEXT(":id"), ids, BindInfo::In)), Exception);

        ExecDML(OTEXT("drop table TestBind_Dup"));
    }

    TEST(TestBindCpp, DuplicateStringVectorBindThrows)
    {
        ExecDML(OTEXT("create table TestBind_DupStr (name varchar2(50))"));

        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        static std::vector<String> names(1, OTEXT("a"));

        stmt.Prepare(OTEXT("insert into TestBind_DupStr values (:name)"));
        stmt.SetBindArraySize(static_cast<OCI_UInt32>(names.size()));
        stmt.Bind(OTEXT(":name"), names, 50u, BindInfo::In);

        ASSERT_THROW((void)(stmt.Bind(OTEXT(":name"), names, 50u, BindInfo::In)), Exception);

        ExecDML(OTEXT("drop table TestBind_DupStr"));
    }
}
