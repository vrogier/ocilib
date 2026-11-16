#include "ocilib_tests.h"
#include "NullArgs.h"
#include "TestFeatures.h"

using namespace ocilib;

namespace TestCApi
{
    /* SQL BOOLEAN columns (Oracle 23ai) */

    class TestBoolean : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            if (FeatureAvailableCaps(OCI_FEATURE_SQL_BOOLEAN))
            {
                ExecDML(OTEXT("CREATE TABLE TestBoolean_Tab (id NUMBER, flag BOOLEAN)"));
                ExecDML(OTEXT("INSERT INTO TestBoolean_Tab VALUES (1, TRUE)"));
                ExecDML(OTEXT("INSERT INTO TestBoolean_Tab VALUES (2, FALSE)"));
                ExecDML(OTEXT("INSERT INTO TestBoolean_Tab VALUES (3, NULL)"));
            }
        }

        static void TearDownTestCase()
        {
            if (FeatureAvailableCaps(OCI_FEATURE_SQL_BOOLEAN))
            {
                ExecDML(OTEXT("DROP TABLE TestBoolean_Tab PURGE"));
            }
        }
    };

    TEST_F(TestBoolean, GetBooleanByIndex_ReturnsTrueFalseAndNull)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_SQL_BOOLEAN);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT flag FROM TestBoolean_Tab ORDER BY id")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        EXPECT_TRUE(OCI_ResultsetGetBoolean(rs, 0));
        EXPECT_FALSE(OCI_ResultsetIsColumnNull(rs, 0));

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        EXPECT_FALSE(OCI_ResultsetGetBoolean(rs, 0));
        EXPECT_FALSE(OCI_ResultsetIsColumnNull(rs, 0));

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        EXPECT_FALSE(OCI_ResultsetGetBoolean(rs, 0));
        EXPECT_TRUE(OCI_ResultsetIsColumnNull(rs, 0));

        EXPECT_FALSE(OCI_ResultsetFetchNext(rs));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestBoolean, GetBooleanByName_ReturnsTrueFalseAndNull)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_SQL_BOOLEAN);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT flag FROM TestBoolean_Tab ORDER BY id")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        EXPECT_TRUE(OCI_ResultsetGetBooleanByName(rs, OTEXT("flag")));
        EXPECT_FALSE(OCI_ResultsetIsColumnNullByName(rs, OTEXT("flag")));

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        EXPECT_FALSE(OCI_ResultsetGetBooleanByName(rs, OTEXT("flag")));
        EXPECT_FALSE(OCI_ResultsetIsColumnNullByName(rs, OTEXT("flag")));

        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        EXPECT_FALSE(OCI_ResultsetGetBooleanByName(rs, OTEXT("flag")));
        EXPECT_TRUE(OCI_ResultsetIsColumnNullByName(rs, OTEXT("flag")));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestBoolean, BooleanColumn_DescribedAsBoolean)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_SQL_BOOLEAN);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT flag FROM TestBoolean_Tab WHERE 1=0")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        const auto col = OCI_ResultsetGetColumn(rs, 0);
        ASSERT_NE(nullptr, col);
        EXPECT_EQ(OCI_CDT_BOOLEAN, OCI_ColumnGetType(col));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestBoolean, BindBooleanToSqlColumn_RoundTrips)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_SQL_BOOLEAN);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        OCI_Boolean flag = TRUE;

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("INSERT INTO TestBoolean_Tab VALUES (10, :flag)")));
        ASSERT_NE(nullptr, OCI_StatementBindBoolean(stmt, OTEXT(":flag"), &flag));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT flag FROM TestBoolean_Tab WHERE id = 10")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        EXPECT_TRUE(OCI_ResultsetGetBoolean(rs, 0));

        ASSERT_TRUE(OCI_StatementFree(stmt));

        ExecDML(OTEXT("DELETE FROM TestBoolean_Tab WHERE id = 10"));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestBoolean, RegisterBoolean_ReturningClause)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_SQL_BOOLEAN);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        /* registered out-binds are internal — register + execute must succeed
           and the row must land in the table */

        ASSERT_TRUE(OCI_StatementPrepare(stmt,
            OTEXT("INSERT INTO TestBoolean_Tab VALUES (20, TRUE) RETURNING flag INTO :flag")));
        ASSERT_TRUE(OCI_StatementRegisterBoolean(stmt, OTEXT(":flag")));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));
        ASSERT_TRUE(OCI_StatementFree(stmt));

        const auto stmt2 = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt2);
        ASSERT_TRUE(OCI_StatementPrepare(stmt2, OTEXT("SELECT flag FROM TestBoolean_Tab WHERE id = 20")));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt2));
        const auto rs = OCI_StatementGetResultset(stmt2);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        EXPECT_TRUE(OCI_ResultsetGetBoolean(rs, 0));
        ASSERT_TRUE(OCI_StatementFree(stmt2));

        ExecDML(OTEXT("DELETE FROM TestBoolean_Tab WHERE id = 20"));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    /* PL/SQL BOOLEAN regression — must keep working through the same APIs */

    TEST(TestBooleanPlsql, PlsqlBooleanBind_StillWorks)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        OCI_Boolean b = FALSE;

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("BEGIN :b := TRUE; END;")));
        ASSERT_NE(nullptr, OCI_StatementBindBoolean(stmt, OTEXT(":b"), &b));
        ASSERT_TRUE(OCI_StatementExecutePrepared(stmt));

        EXPECT_TRUE(b);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST_F(TestBoolean, GetBoolean_OnNumericColumn_ReturnsFalse)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_SQL_BOOLEAN);

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        /* datatype mismatch → getter returns FALSE without conversion */

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("SELECT id FROM TestBoolean_Tab WHERE ROWNUM = 1")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);
        ASSERT_TRUE(OCI_ResultsetFetchNext(rs));
        EXPECT_FALSE(OCI_ResultsetGetBoolean(rs, 0));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    class TestBooleanNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestBooleanNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestBooleanNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "RsGetBool",     [] { return !OCI_ResultsetGetBoolean(nullptr, 0); } },
            { "RsGetBoolName", [] { return !OCI_ResultsetGetBooleanByName(nullptr, OTEXT("x")); } },
            { "StmtRegBool",   [] { return !OCI_StatementRegisterBoolean(nullptr, OTEXT("x")); } },
        }));
}
