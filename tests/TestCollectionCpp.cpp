#include "ocilib_tests.h"

using namespace ocilib;

namespace TestCppApi
{
    TEST(TestCollectionCpp, ToStringCpp)
    {
        ExecDML(OTEXT("create type CollToStringCpp_Type as object(code int, name varchar2(50))"));
        ExecDML(OTEXT("create type CollToStringCpp_Coll as varray(2) of CollToStringCpp_Type"));

        Environment& env = GetEnvironmentCpp();

        Connection conn(env, DBS, USR, PWD);
        TypeInfo typinfColl(conn, OTEXT("CollToStringCpp_Coll"), TypeInfo::Type);
        TypeInfo typinfObj(conn, OTEXT("CollToStringCpp_Type"), TypeInfo::Type);
        Collection<Object> coll(typinfColl);

        Object obj1(typinfObj);
        obj1.Set<OCI_Int32>(OTEXT("code"), 1);
        obj1.Set<String>(OTEXT("name"), OTEXT("computer"));
        coll.Append(obj1);

        Object obj2(typinfObj);
        obj2.Set<OCI_Int32>(OTEXT("code"), 2);
        obj2.Set<String>(OTEXT("name"), OTEXT("book"));
        coll.Append(obj2);

        String expected = OTEXT("CollToStringCpp_Coll(CollToStringCpp_Type(1, 'computer'), CollToStringCpp_Type(2, 'book'))");

        ASSERT_EQ(ToUpper(expected), ToUpper(coll.ToString()));

        

        ExecDML(OTEXT("drop type CollToStringCpp_Coll"));
        ExecDML(OTEXT("drop type CollToStringCpp_Type"));
    }

    TEST(TestCollectionCpp, NullToString)
    {
        Collection<String> coll;
        ASSERT_EQ(String(OTEXT("NULL")), coll.ToString());
    }
}

