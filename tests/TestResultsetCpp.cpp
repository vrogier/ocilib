#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestResultsetCpp, ReleaseResultsetsFreesLiveChild)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        stmt.Execute(OTEXT("SELECT 1 FROM dual"));
        Resultset rs = stmt.GetResultset();
        ASSERT_TRUE(rs.Next());
        ASSERT_EQ(1, rs.Get<OCI_Int32>(0));

        stmt.Execute(OTEXT("SELECT 2 FROM dual"));
        ASSERT_TRUE(rs.IsNull());
    }

    TEST(TestResultsetCpp, ReleaseResultsetsErasesExpiredChild)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        stmt.Execute(OTEXT("SELECT 1 FROM dual"));
        {
            Resultset rs = stmt.GetResultset();
            ASSERT_TRUE(rs.Next());
        }

        stmt.Execute(OTEXT("SELECT 2 FROM dual"));
    }
}
