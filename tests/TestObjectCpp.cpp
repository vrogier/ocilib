#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCppApi
{
    TEST(TestObjectCpp, ToStringCpp)
    {
        ExecDML(OTEXT("create type TestObjectToStringCpp as object(code number, name varchar2(30))"));

        Environment& env = GetEnvironmentCpp();

        Connection conn(env, DBS, USR, PWD);
        TypeInfo typinf(conn, OTEXT("TestObjectToStringCpp"), TypeInfo::Type);
        Object obj(typinf);

        obj.Set<OCI_Int32>(OTEXT("code"), 1);
        obj.Set<String>(OTEXT("name"), OTEXT("computer"));

        String expected = OTEXT("TestObjectToStringCpp(1, 'computer')");

        ASSERT_EQ(ToUpper(expected), ToUpper(obj.ToString()));

        

        ExecDML(OTEXT("drop type TestObjectToStringCpp"));
    }

    TEST(TestObjectCpp, NullToString)
    {
        Object obj;
        ASSERT_EQ(String(OTEXT("NULL")), obj.ToString());
    }
}