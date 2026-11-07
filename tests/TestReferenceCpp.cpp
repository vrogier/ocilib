#include "ocilib_tests.h"

using namespace ocilib;

static void IniTestRef()
{
    ExecDML(OTEXT("create type vendor_t as object(code  number, name  varchar2(30))"));
    ExecDML(OTEXT("create table vendors of vendor_t"));
    ExecDML(OTEXT("insert into vendors values (vendor_t(1, 'John Doe'))"));
}

static void CleanupTestRef()
{
    ExecDML(OTEXT("drop table vendors"));
}
namespace TestCppApi
{
    TEST(TestRefCpp, ToStringCpp)
    {
        IniTestRef();

        Environment& env = GetEnvironmentCpp();

        Connection conn(env, DBS, USR, PWD);
        Statement stmt(conn);

        stmt.Execute(OTEXT("select ref(v) from vendors v"));
        auto rst = stmt.GetResultset();

        while (rst.Next())
        {
            auto ref = rst.Get<Reference>(0);
            auto s = ref.ToString();
            ASSERT_TRUE(s.size() > 0);
        }

        

        CleanupTestRef();
    }

    TEST(TestRefCpp, EmptyReferenceToStringAndDeleter)
    {
        IniTestRef();

        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);
        TypeInfo tif(conn, OTEXT("vendor_t"), TypeInfo::Type);

        Reference ref(tif);
        ASSERT_EQ(String(OTEXT("")), ref.ToString());

        CleanupTestRef();
    }

    TEST(TestRefCpp, NullReferenceToString)
    {
        Reference ref;
        ASSERT_EQ(String(OTEXT("NULL")), ref.ToString());
    }
}