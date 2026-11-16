#include "ocilib_tests.h"
#include "TestFeatures.h"

using namespace ocilib;

namespace TestCppApi
{
    /* SQL BOOLEAN columns (Oracle 23ai) */

    class TestBooleanCpp : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            if (FeatureAvailableCaps(OCI_FEATURE_SQL_BOOLEAN))
            {
                ExecDML(OTEXT("CREATE TABLE TestBooleanCpp_Tab (id NUMBER, flag BOOLEAN)"));
                ExecDML(OTEXT("INSERT INTO TestBooleanCpp_Tab VALUES (1, TRUE)"));
                ExecDML(OTEXT("INSERT INTO TestBooleanCpp_Tab VALUES (2, FALSE)"));
                ExecDML(OTEXT("INSERT INTO TestBooleanCpp_Tab VALUES (3, NULL)"));
            }
        }

        static void TearDownTestCase()
        {
            if (FeatureAvailableCaps(OCI_FEATURE_SQL_BOOLEAN))
            {
                ExecDML(OTEXT("DROP TABLE TestBooleanCpp_Tab PURGE"));
            }
        }
    };

    TEST_F(TestBooleanCpp, GetBoolByIndex_ReturnsTrueFalseAndNull)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SQL_BOOLEAN);

        Statement stmt(con);
        stmt.Execute(OTEXT("SELECT flag FROM TestBooleanCpp_Tab ORDER BY id"));

        Resultset rs = stmt.GetResultset();

        ASSERT_TRUE(rs.Next());
        EXPECT_TRUE(rs.Get<bool>(0));
        EXPECT_FALSE(rs.IsColumnNull(0));

        ASSERT_TRUE(rs.Next());
        EXPECT_FALSE(rs.Get<bool>(0));
        EXPECT_FALSE(rs.IsColumnNull(0));

        ASSERT_TRUE(rs.Next());
        EXPECT_FALSE(rs.Get<bool>(0));
        EXPECT_TRUE(rs.IsColumnNull(0));

        EXPECT_FALSE(rs.Next());
    }

    TEST_F(TestBooleanCpp, GetBoolByName_ReturnsTrueFalseAndNull)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SQL_BOOLEAN);

        Statement stmt(con);
        stmt.Execute(OTEXT("SELECT flag FROM TestBooleanCpp_Tab ORDER BY id"));

        Resultset rs = stmt.GetResultset();

        ASSERT_TRUE(rs.Next());
        EXPECT_TRUE(rs.Get<bool>(OTEXT("flag")));
        EXPECT_FALSE(rs.IsColumnNull(OTEXT("flag")));

        ASSERT_TRUE(rs.Next());
        EXPECT_FALSE(rs.Get<bool>(OTEXT("flag")));
        EXPECT_FALSE(rs.IsColumnNull(OTEXT("flag")));

        ASSERT_TRUE(rs.Next());
        EXPECT_FALSE(rs.Get<bool>(OTEXT("flag")));
        EXPECT_TRUE(rs.IsColumnNull(OTEXT("flag")));
    }

    TEST_F(TestBooleanCpp, GetBoolOptional_NullRowReturnsNullopt)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SQL_BOOLEAN);

        Statement stmt(con);
        stmt.Execute(OTEXT("SELECT flag FROM TestBooleanCpp_Tab ORDER BY id"));

        Resultset rs = stmt.GetResultset();

        ASSERT_TRUE(rs.Next());
        EXPECT_TRUE(rs.GetOptional<bool>(0).value_or(false));

        ASSERT_TRUE(rs.Next());
        ASSERT_TRUE(rs.GetOptional<bool>(0).has_value());
        EXPECT_FALSE(rs.GetOptional<bool>(0).value());

        ASSERT_TRUE(rs.Next());
        EXPECT_FALSE(rs.GetOptional<bool>(0).has_value());
    }

    TEST_F(TestBooleanCpp, BindBoolToSqlColumn_RoundTrips)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SQL_BOOLEAN);

        Statement stmt(con);
        bool flag = true;

        stmt.Prepare(OTEXT("INSERT INTO TestBooleanCpp_Tab VALUES (10, :flag)"));
        stmt.Bind<bool>(OTEXT(":flag"), flag, BindInfo::In);
        stmt.ExecutePrepared();

        Statement verify(con);
        verify.Execute(OTEXT("SELECT flag FROM TestBooleanCpp_Tab WHERE id = 10"));

        Resultset rs = verify.GetResultset();
        ASSERT_TRUE(rs.Next());
        EXPECT_TRUE(rs.Get<bool>(0));

        ExecDML(OTEXT("DELETE FROM TestBooleanCpp_Tab WHERE id = 10"));
    }

    TEST_F(TestBooleanCpp, RegisterBool_ReturningClause)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        SKIP_IF_FEATURE_UNAVAILABLE(env, &con, OCI_FEATURE_SQL_BOOLEAN);

        Statement stmt(con);

        stmt.Prepare(OTEXT("INSERT INTO TestBooleanCpp_Tab VALUES (20, TRUE) RETURNING flag INTO :flag"));
        stmt.Register<bool>(OTEXT(":flag"));
        stmt.ExecutePrepared();

        ExecDML(OTEXT("DELETE FROM TestBooleanCpp_Tab WHERE id = 20"));
    }

    /* PL/SQL BOOLEAN regression through the C++ surface */

    TEST(TestBooleanCppPlsql, PlsqlBooleanBind_StillWorks)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);
        Statement stmt(con);

        bool b = false;

        stmt.Prepare(OTEXT("BEGIN :b := TRUE; END;"));
        stmt.Bind<bool>(OTEXT(":b"), b, BindInfo::Out);
        stmt.ExecutePrepared();

        EXPECT_TRUE(b);
    }
}
