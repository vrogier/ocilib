#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestTypeInfoCpp, MetadataCpp)
    {
        ExecDML(OTEXT("CREATE OR REPLACE TYPE TestTypeInfoCovType AS OBJECT(code NUMBER, name VARCHAR2(50))"));

        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        TypeInfo typeInfo(conn, OTEXT("TestTypeInfoCovType"), TypeInfo::TypeInfoType(TypeInfo::Type));

        ASSERT_TRUE(typeInfo.GetType() == TypeInfo::Type);
        ASSERT_EQ(ostring(OTEXT("TESTTYPEINFOCOVTYPE")), ToUpper(typeInfo.GetName()));
        ASSERT_TRUE(typeInfo.GetConnection());
        ASSERT_EQ(2u, typeInfo.GetColumnCount());
        ASSERT_TRUE(typeInfo.IsFinalType());

        Column col = typeInfo.GetColumn(1);
        ASSERT_FALSE(col.GetName().empty());
        ASSERT_TRUE(col.GetType() == TypeNumeric);

        col = typeInfo.GetColumn(2);
        ASSERT_FALSE(col.GetName().empty());
        ASSERT_TRUE(col.GetType() == TypeString);

        ASSERT_FALSE(typeInfo.GetSuperType());

        ExecDML(OTEXT("DROP TYPE TestTypeInfoCovType"));
    }
}
