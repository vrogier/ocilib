#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestExceptionCpp, InspectException)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Statement stmt(con);
        stmt.Prepare(OTEXT("select * from non_existent_table_for_cpp_exception"));

        try
        {
            stmt.ExecutePrepared();
            FAIL() << "Expected an exception";
        }
        catch (Exception ex)
        {
            ASSERT_NE(nullptr, ex.what());
            ASSERT_EQ(Exception::OracleError, ex.GetType());
            ASSERT_EQ(942, ex.GetOracleErrorCode());
            ASSERT_EQ(0, ex.GetInternalErrorCode());
            ASSERT_FALSE(ex.GetStatement().IsNull());
            ASSERT_FALSE(ex.GetConnection().IsNull());
            ASSERT_EQ(0u, ex.GetRow());
            ASSERT_TRUE(ex.GetMessage().find(OTEXT("ORA-00942")) != ostring::npos);

            Exception ex2 = ex;
            Exception ex3 = ex;
            ex2 = ex3;
            ASSERT_EQ(ex.GetMessage(), ex2.GetMessage());
            ASSERT_EQ(ex.GetType(), ex2.GetType());
            ASSERT_EQ(ex.GetOracleErrorCode(), ex2.GetOracleErrorCode());
        }
    }
}
