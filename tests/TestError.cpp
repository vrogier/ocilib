#include "ocilib_tests.h"
#include "NullArgs.h"
#include <cstdlib>
#include <filesystem>
#include <string>

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

        ASSERT_FALSE(OCI_StatementExecute(stmt, OTEXT("select * from nonexistent_table_xyz")));

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

        ASSERT_FALSE(OCI_StatementExecute(stmt, OTEXT("insert into TestErrorData values ('too long')")));

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

        ASSERT_FALSE(OCI_StatementSetFetchMode(stmt, 9999));

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

        ASSERT_TRUE(OCI_StatementExecute(stmt, OTEXT("select 1 from dual")));

        const auto rs = OCI_StatementGetResultset(stmt);
        ASSERT_NE(nullptr, rs);

        ASSERT_FALSE(OCI_ResultsetFetchPrev(rs));

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
        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select :i from dual")));
        OCI_Int32 i = 1;
        ASSERT_NE(nullptr, OCI_StatementBindInt32(stmt, OTEXT(":i"), &i));
        const auto bnd = OCI_StatementGetBindByName(stmt, OTEXT(":i"));
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

    /* internal logger hooks (src/log.h) — the logger reads OCILIB_LOG* env
       vars only when its global ref_count reaches 0, so a test must drain the
       refcount first to re-trigger initialization for a single env */

    extern "C" OCI_SYM_LOCAL OCI_Boolean OcilibLogInitialize(void);
    extern "C" OCI_SYM_LOCAL OCI_Boolean OcilibLogCleanup(void);
    extern "C" OCI_SYM_LOCAL void OcilibLogWrite(OCI_LogCategory category, OCI_LogLevel level,
                                               const char* step, const char* function,
                                               const otext* format, ...);

    /* drains OcilibLogger.ref_count — clamped at 0 internally, so over-calling
       is safe; afterwards OcilibLogInitialize() fully re-reads the env vars */
    static void DrainLogRefCount()
    {
        for (int i = 0; i < 128; i++) OcilibLogCleanup();
    }

    static void SetLogEnvVar(const char* name, const char* value)
    {
    #ifdef _WINDOWS
        _putenv((std::string(name) + "=" + (value ? value : "")).c_str());
    #else
        if (value) setenv(name, value, 1);
        else       unsetenv(name);
    #endif
    }

    static void LogCycle(const char* level, const char* categories, const char* output)
    {
        SetLogEnvVar("OCILIB_LOG", nullptr);
        SetLogEnvVar("OCILIB_LOG_CATEGORIES", nullptr);
        SetLogEnvVar("OCILIB_LOG_OUTPUT", nullptr);

        DrainLogRefCount();

        SetLogEnvVar("OCILIB_LOG", level);
        SetLogEnvVar("OCILIB_LOG_CATEGORIES", categories);
        SetLogEnvVar("OCILIB_LOG_OUTPUT", output);

        ASSERT_TRUE(OcilibLogInitialize());

        OcilibLogWrite(OCI_LOG_CAT_OCI, OCI_LOG_DEBUG, nullptr, __func__, OTEXT("probe"));
        OcilibLogWrite(OCI_LOG_CAT_LIB, OCI_LOG_ERROR, "STP", __func__, OTEXT("probe %d"), 42);
        OcilibLogWrite(OCI_LOG_CAT_MEM, OCI_LOG_INFO, "", __func__, OTEXT("probe"));

        OcilibLogCleanup();   /* ref_count 0 + initialized → fclose + reset */

        SetLogEnvVar("OCILIB_LOG", nullptr);
        SetLogEnvVar("OCILIB_LOG_CATEGORIES", nullptr);
        SetLogEnvVar("OCILIB_LOG_OUTPUT", nullptr);
    }

    TEST(TestError, LogWrite_CoversWritePathAndCleanup)
    {
        /* covers OcilibLogWrite body (log.c 621+: timestamp, prefix, snprintf,
           lock/fputs) and the file-owned teardown arm of OcilibLogCleanup
           (fclose + reset), plus the STDOUT/STDERR/default sink arms and the
           step-prefix formatting that no library call site exercises */

        const std::string tmp = (std::filesystem::temp_directory_path() / "ocilib_test_log.txt").string();

        LogCycle("DEBUG", "LIB, MEM, OCI", tmp.c_str());
        ASSERT_TRUE(std::filesystem::file_size(tmp) > 0);

        LogCycle("DEBUG", "ALL", "STDOUT");
        LogCycle("WARN", "BOGUS_CAT,LIB", "");   /* empty output → stderr default + unknown token */
        LogCycle("NOTALEVEL", "ALL", tmp.c_str()); /* unknown level → early return */

        std::error_code ec;
        std::filesystem::remove(tmp, ec);
    }

    TEST(TestError, ElementError_CarriesConnection)
    {
        /* covers the OCI_IPC_ELEMENT arm of OcilibErrorGetConnection() */

        ExecDML(OTEXT("create or replace type TestErrDateColl as table of date"));

        const auto env = GetEnvironment(nullptr);
        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto typinf = OCI_TypeInfoGet(con, OTEXT("TestErrDateColl"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, typinf);
        const auto coll = OCI_CollectionCreate(typinf);
        const auto elem = OCI_ElementCreate(typinf);
        const auto date = OCI_DateCreate(env);
        ASSERT_NE(nullptr, coll);
        ASSERT_NE(nullptr, elem);
        ASSERT_NE(nullptr, date);
        ASSERT_TRUE(OCI_ElementSetDate(elem, date));
        ASSERT_TRUE(OCI_CollectionAppend(coll, elem));
        ASSERT_TRUE(OCI_ElementFree(elem));   /* append copies — caller keeps ownership */

        const auto got = OCI_CollectionGetElement(coll, 0);
        ASSERT_NE(nullptr, got);

        /* DATE element read as int -> OcilibExceptionTypeNotCompatible, source = element */

        ASSERT_EQ(0, OCI_ElementGetInt32(got));

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        EXPECT_EQ(con, OCI_ErrorGetConnection(err));

        ASSERT_TRUE(OCI_DateFree(date));
        ASSERT_TRUE(OCI_CollectionFree(coll));
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ExecDML(OTEXT("drop type TestErrDateColl"));
    }

    TEST(TestError, LongError_CarriesConnection)
    {
        /* covers the OCI_IPC_LONG arm of OcilibErrorGetConnection() :
           OCI_LongWrite() with a NULL buffer throws with the long as the
           error source */

        const auto env = GetEnvironment(nullptr);
        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);
        const auto stmt = OCI_StatementCreate(con);
        ASSERT_NE(nullptr, stmt);

        const auto lg = OCI_LongCreate(stmt, OCI_CLONG);
        ASSERT_NE(nullptr, lg);

        ASSERT_EQ(0u, OCI_LongWrite(lg, nullptr, 0));

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        EXPECT_EQ(con, OCI_ErrorGetConnection(err));

        ASSERT_TRUE(OCI_LongFree(lg));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(con));
    }

    static int handler_calls = 0;
    static void error_handler(OCI_Error*) { ++handler_calls; }

    TEST(TestError, Handler_InvokedOnError)
    {
        /* covers OcilibExceptionCallHandler - the app error-handler arm */

        const auto env = OCI_EnvironmentCreate(nullptr, nullptr, OCI_ENV_DEFAULT);
        ASSERT_NE(nullptr, env);
        ASSERT_TRUE(OCI_EnvironmentSetErrorHandler(env, error_handler));
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);
        const auto stmt = OCI_StatementCreate(conn);
        handler_calls = 0;

        OCI_StatementExecute(stmt, OTEXT("select * from table_that_does_not_exist_xyz"));

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
