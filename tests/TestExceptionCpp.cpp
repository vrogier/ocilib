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
        catch (const Exception& ex)
        {
            ASSERT_NE(nullptr, ex.what());
            ASSERT_EQ(Exception::OracleError, ex.GetCategory());
            ASSERT_EQ(942, ex.GetCode());
            ASSERT_FALSE(ex.GetStatement().IsNull());
            ASSERT_FALSE(ex.GetConnection().IsNull());
            ASSERT_EQ(0u, ex.GetRow());
            ASSERT_TRUE(ex.GetMessage().find(OTEXT("ORA-00942")) != String::npos);

            Exception ex2 = ex;
            Exception ex3 = ex;
            ex2 = ex3;
            ASSERT_EQ(ex.GetMessage(), ex2.GetMessage());
            ASSERT_EQ(ex.GetCategory(), ex2.GetCategory());
            ASSERT_EQ(ex.GetCode(), ex2.GetCode());
        }
    }
}
