#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestColumnCpp, InfoCpp)
    {
        ExecDML(OTEXT("CREATE TABLE TestCppApiCovColInfo(id NUMBER, name VARCHAR2(50), dt DATE)"));

        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        Statement st(conn);
        st.Execute(OTEXT("SELECT * FROM TestCppApiCovColInfo"));
        Resultset rs = st.GetResultset();

        ASSERT_EQ(3u, rs.GetColumnCount());

        Column col = rs.GetColumn(1);
        ASSERT_EQ(String(OTEXT("ID")), col.GetName());
        ASSERT_TRUE(col.GetType() == TypeNumeric);
        ASSERT_FALSE(col.GetSQLType().empty());
        ASSERT_FALSE(col.GetFullSQLType().empty());
        ASSERT_GE(col.GetSize(), 0u);
        ASSERT_GE(col.GetPrecision(), 0);
        ASSERT_EQ(-127, col.GetScale());
        ASSERT_TRUE(col.IsNullable());
        ASSERT_FALSE(col.IsCharSemanticUsed());

        col = rs.GetColumn(2);
        ASSERT_EQ(String(OTEXT("NAME")), col.GetName());
        ASSERT_TRUE(col.GetType() == TypeString);
        ASSERT_GE(col.GetSize(), 0u);
        ASSERT_TRUE(col.IsNullable());

        col = rs.GetColumn(3);
        ASSERT_EQ(String(OTEXT("DT")), col.GetName());
        ASSERT_TRUE(col.GetType() == TypeDate);

        ASSERT_TRUE(col.GetPropertyFlags() == Column::NoFlags);

        ExecDML(OTEXT("DROP TABLE TestCppApiCovColInfo"));
    }
}
