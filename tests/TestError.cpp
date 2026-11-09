#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    TEST(TestError, Getters)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT);
        ASSERT_EQ(nullptr, con);

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(OCI_ERR_CAT_OCI, OCI_ErrorGetCategory(err));
        ASSERT_EQ(1017, OCI_ErrorGetCode(err));
        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_NE(nullptr, OCI_ErrorGetLocation(err));
        ASSERT_EQ(nullptr, OCI_ErrorGetConnection(err));
        ASSERT_EQ(nullptr, OCI_ErrorGetStatement(err));
        ASSERT_EQ(-1, OCI_ErrorGetRow(err));

    }

    TEST(TestError, SqlError)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_FALSE(OCI_ExecuteStmt(stmt, OTEXT("select * from nonexistent_table_xyz")));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(OCI_ERR_CAT_OCI, OCI_ErrorGetCategory(err));
        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_EQ(con, OCI_ErrorGetConnection(err));
        ASSERT_EQ(stmt, OCI_ErrorGetStatement(err));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestError, InsertValueTooLong)
    {
        ExecDML(OTEXT("drop table TestErrorData"));
        ExecDML(OTEXT("create table TestErrorData (value varchar2(5))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_FALSE(OCI_ExecuteStmt(stmt, OTEXT("insert into TestErrorData values ('too long')")));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(OCI_ERR_CAT_OCI, OCI_ErrorGetCategory(err));
        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_EQ(con, OCI_ErrorGetConnection(err));
        ASSERT_EQ(stmt, OCI_ErrorGetStatement(err));
        ASSERT_EQ(-1, OCI_ErrorGetRow(err)); 
        
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop table TestErrorData"));
    }

    TEST(TestError, PoolCreationError)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto pool = OCI_PoolCreate(env, DBS, USR, PWD_WRONG, OCI_POOL_SESSION, OCI_ENV_DEFAULT, 0, 1, 1);
        ASSERT_NE(nullptr, pool);

        const auto con = OCI_PoolGetConnection(env, pool, nullptr);
        ASSERT_EQ(nullptr, con);

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(OCI_ERR_CAT_OCI, OCI_ErrorGetCategory(err));
        ASSERT_EQ(1017, OCI_ErrorGetCode(err));
        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_EQ(nullptr, OCI_ErrorGetConnection(err));
        ASSERT_EQ(nullptr, OCI_ErrorGetStatement(err));
        (void) OCI_HandleGetEnvironment(err);

        ASSERT_TRUE(OCI_PoolFree(env, pool));

    }

    TEST(TestError, InternalError)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_FALSE(OCI_SetFetchMode(stmt, 9999));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(OCI_ERR_CAT_LIB, OCI_ErrorGetCategory(err));
        ASSERT_EQ(OCI_ERR_ARG_INVALID_VALUE, OCI_ErrorGetCode(err));
        ASSERT_NE(nullptr, OCI_ErrorGetString(err));
        ASSERT_EQ(stmt, OCI_ErrorGetStatement(err));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestError, DateTimestampIntervalSource)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto date = OCI_DateCreate(env);
        const auto ts = OCI_TimestampCreate(env, OCI_TIMESTAMP);
        const auto iv = OCI_IntervalCreate(env, OCI_INTERVAL_DS);
        ASSERT_NE(nullptr, date);
        ASSERT_NE(nullptr, ts);
        ASSERT_NE(nullptr, iv);

        otext buf[64];

        ASSERT_FALSE(OCI_DateToText(date, OTEXT("BAD_FMT"), 64, buf));
        auto err = OCI_GetLastError();
        if (err != nullptr)
        {
            ASSERT_EQ(nullptr, OCI_ErrorGetConnection(err));
        }

        ASSERT_FALSE(OCI_TimestampToText(ts, OTEXT("BAD_FMT"), 64, buf, 3));
        err = OCI_GetLastError();
        if (err != nullptr)
        {
            ASSERT_EQ(nullptr, OCI_ErrorGetConnection(err));
        }

        ASSERT_FALSE(OCI_IntervalToText(iv, 2, 3, 1, buf));
        err = OCI_GetLastError();
        if (err != nullptr)
        {
            ASSERT_EQ(nullptr, OCI_ErrorGetConnection(err));
        }

        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_TimestampFree(ts));
        ASSERT_TRUE(OCI_IntervalFree(iv));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestError, ResultsetError)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 1 from dual")));

        const auto rs = OCI_GetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_FALSE(OCI_FetchPrev(rs));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);

        ASSERT_EQ(stmt, OCI_ErrorGetStatement(err));
        ASSERT_EQ(con, OCI_ErrorGetConnection(err));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestError, BindError_CarriesConnectionAndStatement)
    {
        const auto con = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);
        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select :i from dual")));
        OCI_Int32 i = 1;
        ASSERT_NE(nullptr, OCI_BindInt(stmt, OTEXT(":i"), &i));
        const auto bnd = OCI_GetBindByName(stmt, OTEXT(":i"));
        ASSERT_NE(nullptr, bnd);

        ASSERT_EQ(0u, OCI_BindGetDataSizeAtPos(bnd, 2));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        EXPECT_EQ(con, OCI_ErrorGetConnection(err));
        EXPECT_EQ(stmt, OCI_ErrorGetStatement(err));

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));
    }

    TEST(TestError, NumberConversionError_CarriesNoConnection)
    {
        const auto con = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);
        const auto num = OCI_NumberCreate(GetEnvironment(nullptr));
        ASSERT_NE(nullptr, num);

        ASSERT_FALSE(OCI_NumberFromText(num, OTEXT("abc"), OTEXT("FM999")));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        EXPECT_EQ(nullptr, OCI_ErrorGetConnection(err));

        ASSERT_TRUE(OCI_NumberFree(num));
        ASSERT_TRUE(OCI_ConnectionFree(con));
    }

    TEST(TestError, LobError_CarriesConnection)
    {
        const auto con = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);
        const auto lob = OCI_LobCreate(con, OCI_CLOB);
        ASSERT_NE(nullptr, lob);

        ASSERT_EQ(0u, OCI_LobErase(lob, 1, 0));

        auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        EXPECT_EQ(con, OCI_ErrorGetConnection(err));

        ASSERT_TRUE(OCI_LobFree(lob));
        ASSERT_TRUE(OCI_ConnectionFree(con));
    }

    static int handler_calls = 0;
    static void error_handler(OCI_Error*) { ++handler_calls; }

    TEST(TestError, Handler_InvokedOnError)
    {
        /* covers OcilibExceptionCallHandler - the app error-handler arm */

        const auto env = OCI_EnvironmentCreate(nullptr, nullptr, OCI_ENV_DEFAULT);
        ASSERT_NE(nullptr, env);
        ASSERT_TRUE(OCI_SetErrorHandler(env, error_handler));
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto stmt = OCI_StatementCreate(conn);
        handler_calls = 0;

        OCI_ExecuteStmt(stmt, OTEXT("select * from table_that_does_not_exist_xyz"));

        ASSERT_GT(handler_calls, 0);

        OCI_StatementFree(stmt);
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }


    class TestErrorNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestErrorNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestErrorNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "ErrorGetCategory", [] { return OCI_UNKNOWN == OCI_ErrorGetCategory(nullptr); }, NoNullPointerError }, // error.c
            { "ErrorGetCode", [] { return 0 == OCI_ErrorGetCode(nullptr); }, NoNullPointerError }, // error.c
            { "ErrorGetConnection", [] { return nullptr == OCI_ErrorGetConnection(nullptr); }, NoNullPointerError }, // error.c
            { "ErrorGetLocation", [] { return nullptr == OCI_ErrorGetLocation(nullptr); }, NoNullPointerError }, // error.c
            { "ErrorGetRow", [] { return -1 == OCI_ErrorGetRow(nullptr); }, NoNullPointerError }, // error.c
            { "ErrorGetStatement", [] { return nullptr == OCI_ErrorGetStatement(nullptr); }, NoNullPointerError }, // error.c
            { "ErrorGetString", [] { return nullptr == OCI_ErrorGetString(nullptr); }, NoNullPointerError }, // error.c
            { "ErrorGetType", [] { return OCI_UNKNOWN == OCI_ErrorGetType(nullptr); }, NoNullPointerError }, // error.c
        }),
        &NullArgCaseName);

}
