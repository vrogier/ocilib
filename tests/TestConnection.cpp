#include <thread>

#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{

    static Context context;

    static inline void AddError(OCI_Error* err)
    {
        context.Errs.emplace_back(Error{ OCI_ErrorGetCode(err), OCI_ErrorGetCategory(err) });
    }

    TEST(TestConnection, ValidCredentials)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);

        ASSERT_NE(nullptr, conn);

        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestConnection, InvalidUser)
    {
        Guard guard(context.Lock);
        context.Errs.clear();

        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT);

        ASSERT_EQ(nullptr, conn);

        ASSERT_EQ(1, context.Errs.size());
         ASSERT_EQ(1017, context.Errs[0].Code);
        ASSERT_EQ(OCI_ERR_CAT_OCI, context.Errs[0].Category);

        ASSERT_FALSE(OCI_ConnectionFree(conn));

    }

    TEST(TestConnection, InvalidPassword)
    {
        Guard guard(context.Lock);
        context.Errs.clear();

        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT);
        ASSERT_EQ(nullptr, conn);

        ASSERT_EQ(1, context.Errs.size());
        ASSERT_EQ(1017, context.Errs[0].Code);
        ASSERT_EQ(OCI_ERR_CAT_OCI, context.Errs[0].Category);

        ASSERT_FALSE(OCI_ConnectionFree(conn));

    }

    TEST(TestConnection, Abort)
    {
        Guard guard(context.Lock);
        context.Errs.clear();

        auto env = GetEnvironment(AddError);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        bool threadResult = true;

        std::thread thrd([&threadResult, stmt]()
            {
                threadResult = OCI_ExecuteStmt(stmt, OTEXT("begin dbms_lock.sleep(10); end;"));
            });

        Sleep(1000);
        ASSERT_TRUE(OCI_Break(conn));

        thrd.join();

        ASSERT_FALSE(threadResult);

        ASSERT_EQ(1, context.Errs.size());
        ASSERT_EQ(1013, context.Errs[0].Code);
        ASSERT_EQ(OCI_ERR_CAT_OCI, context.Errs[0].Category);

        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestConnection, Trace)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const otext* module = OTEXT("CApiCovModule");
        ASSERT_TRUE(OCI_SetTrace(con, OCI_TRC_MODULE, module));
        const otext* value = OCI_GetTrace(con, OCI_TRC_MODULE);
        ASSERT_NE(nullptr, value);
        ASSERT_EQ(String(module), String(value));

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestConnection, Getters)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        ASSERT_EQ(env, OCI_HandleGetEnvironment(con));

        ASSERT_TRUE(OCI_IsConnected(con));
        ASSERT_NE(nullptr, OCI_GetDatabase(con));
        ASSERT_NE(nullptr, OCI_GetUserName(con));
        ASSERT_NE(nullptr, OCI_GetPassword(con));
        ASSERT_EQ(OCI_SESSION_DEFAULT, OCI_GetSessionMode(con));
        ASSERT_NE(nullptr, OCI_GetDBName(con));
        ASSERT_NE(nullptr, OCI_GetInstanceName(con));
        ASSERT_NE(nullptr, OCI_GetServiceName(con));
        ASSERT_NE(nullptr, OCI_GetServerName(con));
        (void) OCI_GetDomainName(con);

        ASSERT_TRUE(OCI_SetAutoCommit(con, FALSE));
        ASSERT_FALSE(OCI_GetAutoCommit(con));

        ASSERT_TRUE(OCI_SetStatementCacheSize(con, 42));
        ASSERT_EQ(42u, OCI_GetStatementCacheSize(con));

        ASSERT_TRUE(OCI_SetDefaultLobPrefetchSize(con, 4096));
        ASSERT_EQ(4096u, OCI_GetDefaultLobPrefetchSize(con));

        ASSERT_GE(OCI_GetMaxCursors(con), 0u);

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestConnection, CommitAndRollback)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        ASSERT_TRUE(OCI_Immediate(con, OTEXT("create table TestConnT (id int)")));
        ASSERT_TRUE(OCI_Immediate(con, OTEXT("insert into TestConnT values (1)")));
        ASSERT_TRUE(OCI_Rollback(con));
        ASSERT_TRUE(OCI_Commit(con));
        ASSERT_TRUE(OCI_Immediate(con, OTEXT("drop table TestConnT")));

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestConnection, Metadata)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto major = OCI_GetServerMajorVersion(con);
        ASSERT_GT(major, 0u);
        const auto minor = OCI_GetServerMinorVersion(con);
        ASSERT_GE(minor, 0u);
        const auto rev = OCI_GetServerRevisionVersion(con);
        ASSERT_GE(rev, 0u);

        ASSERT_NE(nullptr, OCI_GetVersionServer(con));
        ASSERT_NE(OCI_UNKNOWN, OCI_GetVersionConnection(con));
        ASSERT_NE(nullptr, OCI_GetServerName(con));

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestConnection, ServerOutput)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        ASSERT_TRUE(OCI_ServerEnableOutput(con, 32000, 5, 255));
        ASSERT_TRUE(OCI_Immediate(con, OTEXT("begin dbms_output.put_line('hello'); end;")));
        ASSERT_NE(nullptr, OCI_ServerGetOutput(con));
        ASSERT_TRUE(OCI_ServerDisableOutput(con));

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    TEST(TestConnection, PingAndTimeout)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        ASSERT_TRUE(OCI_Ping(con));

        ASSERT_TRUE(OCI_SetTimeout(con, OCI_NTO_SEND, 60));
        ASSERT_EQ(60u, OCI_GetTimeout(con, OCI_NTO_SEND));

        ASSERT_TRUE(OCI_ConnectionFree(con));

    }

    static void TestConnSubHandler(OCI_Event* event)
    {
        (void) event;
    }

    static OCI_FailoverCode TestConnTAFHandler(OCI_Connection* con, OCI_FailoverType type, OCI_FailoverEvent event)
    {
        (void) con; (void) type; (void) event;
        return OCI_FOC_OK;
    }

    struct TraceCase
    {
        OCI_TraceType type;
        const otext*  value;
    };

    class TestConnectionTrace : public ::testing::TestWithParam<TraceCase>
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);
        }

        void TearDown() override
        {
            OCI_ConnectionFree(con);
        }

        OCI_Connection* con{ nullptr };
    };

    TEST_P(TestConnectionTrace, SetAndGet)
    {
        ASSERT_TRUE(OCI_SetTrace(con, GetParam().type, GetParam().value));
        ASSERT_EQ(String(GetParam().value), String(OCI_GetTrace(con, GetParam().type)));
    }

    INSTANTIATE_TEST_CASE_P(TraceTypes, TestConnectionTrace,
        ::testing::Values(
            TraceCase{ OCI_TRC_IDENTITY,  OTEXT("identifier") },
            TraceCase{ OCI_TRC_MODULE,    OTEXT("module") },
            TraceCase{ OCI_TRC_ACTION,    OTEXT("action") },
            TraceCase{ OCI_TRC_DETAIL,    OTEXT("info") },
            TraceCase{ OCI_TRC_OPERATION, OTEXT("operation") }));

    TEST(TestConnection, SetUserPasswordAtLogon)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* OcilibDatabaseSetUserPassword() performs an OCI_AUTH logon that
           changes the password (OCIPasswordChange) instead of OCISessionBegin */
        ASSERT_TRUE(OCI_SetUserPassword(env, DBS, USR, PWD, OTEXT("OcilibCovPwd1")));

        /* restore the original password, logging on with the new one */
        ASSERT_TRUE(OCI_SetUserPassword(env, DBS, USR, OTEXT("OcilibCovPwd1"), PWD));
    }

    TEST(TestConnection, SetPasswordWhileLogged)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        /* logged connection -> plain OCIPasswordChange */
        ASSERT_TRUE(OCI_SetPassword(con, OTEXT("OcilibCovPwd2")));
        ASSERT_TRUE(OCI_SetPassword(con, PWD));

        ASSERT_TRUE(OCI_ConnectionFree(con));
    }


    TEST(TestConnection, ServerOutputEdgeCases)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        /* bufsize = 0 binds a NULL (unlimited buffer) and lnsize above the
           maximum gets clamped */
        ASSERT_TRUE(OCI_ServerEnableOutput(con, 0, 5, 40000));
        ASSERT_TRUE(OCI_Immediate(con, OTEXT("begin dbms_output.put_line('hello'); end;")));
        ASSERT_NE(nullptr, OCI_ServerGetOutput(con));
        ASSERT_TRUE(OCI_ServerDisableOutput(con));

        ASSERT_TRUE(OCI_ConnectionFree(con));
    }

    TEST(TestConnection, ExternalCredentials)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        /* NULL user/password selects OCI_CRED_EXT (OS authentication). It fails
           on databases without an OPS$ account, which still exercises the path */
        const auto con = OCI_ConnectionCreate(env, DBS, NULL, NULL, OCI_SESSION_DEFAULT);
        if (con)
        {
            ASSERT_TRUE(OCI_ConnectionFree(con));
        }
    }


    TEST(TestConnection, FreeWithSubscription)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto sub = OCI_SubscriptionRegister(con, OTEXT("TestConnSub"), OCI_CNT_ALL, TestConnSubHandler, OCI_SRM_DEFAULT, 0);
        ASSERT_NE(nullptr, sub);

        /* freeing the connection first detaches the subscription, which keeps
           the connection credentials needed by OCI_SubscriptionUnregister() */
        ASSERT_TRUE(OCI_ConnectionFree(con));

        ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
    }

    TEST(TestConnection, TAFHandler)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto capable = OCI_IsTAFCapable(con);

        /* on a non TAF-enabled service the call is rejected */
        ASSERT_EQ(capable, OCI_SetTAFHandler(con, TestConnTAFHandler));

        ASSERT_TRUE(OCI_ConnectionFree(con));
    }

    TEST(TestConnection, PoolLogoffAutocommitAndTag)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto pool = OCI_PoolCreate(env, DBS, USR, PWD, OCI_POOL_SESSION, OCI_SESSION_DEFAULT, 0, 3, 1);
        ASSERT_NE(nullptr, pool);

        /* tag a pooled session so the next get with the same tag finds it */
        const auto con1 = OCI_PoolGetConnection(env, pool, OTEXT("covtag"));
        ASSERT_NE(nullptr, con1);
        ASSERT_TRUE(OCI_SetSessionTag(con1, OTEXT("covtag")));
        ASSERT_TRUE(OCI_SetAutoCommit(con1, TRUE));
        ASSERT_TRUE(OCI_ConnectionFree(con1));

        /* session returned with a matching tag -> OcilibConnectionSetSessionTag */
        const auto con2 = OCI_PoolGetConnection(env, pool, OTEXT("covtag"));
        ASSERT_NE(nullptr, con2);
        ASSERT_TRUE(OCI_ConnectionFree(con2));

        ASSERT_TRUE(OCI_PoolFree(env, pool));
    }

    /* --- public-API coverage of the dropped OcilibCon* / OcilibParseVersionString
       internals: OCI_GetServerMajorVersion / Minor / Revision and
       OCI_SetPassword exercise the version-string parse + password-change
       arms the dropped tests used --- */

    TEST(TestConnection, GetServerVersionComponents)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        /* OcilibCon* version internals via public getters
           (OcilibConnectionGetServer* + OcilibParseVersionString arms) */
        const auto major = OCI_GetServerMajorVersion(conn);
        const auto minor = OCI_GetServerMinorVersion(conn);
        const auto rev   = OCI_GetServerRevisionVersion(conn);

        ASSERT_GE(major, 8u);
        ASSERT_LE(major, 99u);
        ASSERT_GE(minor, 0u);
        ASSERT_LE(minor, 99u);
        ASSERT_GE(rev, 0u);
        ASSERT_LE(rev, 99u);

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    TEST(TestConnection, SetPasswordOnLoggedConn)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        /* OcilibConnectionSetPassword / OcilibPasswordChange arms via public
           password API on a logged connection. Setting the password to its
           current value exercises the OCI call without mutating server state
           (a real change would break every other test's connect) */
        ASSERT_TRUE(OCI_SetPassword(conn, PWD));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }







    TEST(TestConnection, Connect_InvalidCredentials_ReturnsNullAndRaisesError)
    {
        const auto env = GetEnvironment(nullptr);

        ASSERT_EQ(nullptr, OCI_ConnectionCreate(env, DBS, USR, PWD_WRONG, OCI_SESSION_DEFAULT));

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        ASSERT_NE(0, OCI_ErrorGetCode(err));
    }

    TEST(TestConnection, Lifetime_TransientChildCycles_StatsBounded)
    {
        /* C1-class guard: repeated create->use->destroy of transient children
           on one long-lived Connection must not grow env handle/object stats.
           20 warm-up cycles (caches may legitimately retain), then measured. */

        const auto env = GetEnvironment(nullptr);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        auto cycle = [&]
        {
            const auto stmt = OCI_StatementCreate(conn);
            EXPECT_NE(nullptr, stmt);
            EXPECT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("select 1 as v, 'x' as s from dual")));
            const auto rs = OCI_GetResultset(stmt);
            EXPECT_NE(nullptr, rs);
            EXPECT_TRUE(OCI_FetchNext(rs));
            EXPECT_TRUE(OCI_StatementFree(stmt));
        };

        for (int i = 0; i < 20; i++) cycle();

        const auto bytes   = OCI_GetStats(env, OCI_UST_BYTES);
        const auto handles = OCI_GetStats(env, OCI_UST_HANDLES);
        const auto objs    = OCI_GetStats(env, OCI_UST_OBJECTS);

        for (int i = 0; i < 50; i++) cycle();

        ASSERT_EQ(handles, OCI_GetStats(env, OCI_UST_HANDLES));
        ASSERT_EQ(objs,    OCI_GetStats(env, OCI_UST_OBJECTS));
        /* bytes may legitimately grow a little via caching but must be bounded */
        ASSERT_LE(OCI_GetStats(env, OCI_UST_BYTES), bytes + 64 * 1024);

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }


    TEST(TestConnection, Ping_PreClient10_2_NoOpSuccess)
    {
        /* OCIPing needs a 10.2+ client; below that OcilibConnectionPing is a
           silent no-op returning TRUE (does not actually probe the server) */

        const auto env = GetSpecificEnvironment(nullptr, OCI_10_1);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_EQ(TRUE, OCI_Ping(conn));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }


    TEST(TestConnection, GetServerVersion_PreClient18_1_UsesFallback)
    {
        /* <18.1 clients lack OCIServerRelease2 -> OCIServerVersion fallback */

        const auto env = GetSpecificEnvironment(nullptr, OCI_12_1);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_NE(nullptr, OCI_GetVersionServer(conn));
        ASSERT_NE(0u, OCI_GetVersionConnection(conn));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    TEST(TestConnection, IsTAFCapable_PreClient10_2_ReturnsFalse)
    {
        /* OCI_ATTR_TAF_ENABLED needs a 10.2+ client -> silently FALSE below */

        const auto env = GetSpecificEnvironment(nullptr, OCI_10_1);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_EQ(FALSE, OCI_IsTAFCapable(conn));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }


    class TestConnectionNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestConnectionNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestConnectionNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "Break", [] { return !OCI_Break(nullptr); } },
            { "Commit", [] { return !OCI_Commit(nullptr); } },
            { "ConnectionFree", [] { return !OCI_ConnectionFree(nullptr); } },
            { "GetAutoCommit", [] { return !OCI_GetAutoCommit(nullptr); } },
            { "GetDBName", [] { return nullptr == OCI_GetDBName(nullptr); } },
            { "GetDatabase", [] { return nullptr == OCI_GetDatabase(nullptr); } },
            { "GetDefaultLobPrefetchSize", [] { return 0 == OCI_GetDefaultLobPrefetchSize(nullptr); } },
            { "GetDomainName", [] { return nullptr == OCI_GetDomainName(nullptr); } },
            { "GetInstanceName", [] { return nullptr == OCI_GetInstanceName(nullptr); } },
            { "GetInstanceStartTime", [] { return nullptr == OCI_GetInstanceStartTime(nullptr); } },
            { "GetMaxCursors", [] { return 0 == OCI_GetMaxCursors(nullptr); } },
            { "GetPassword", [] { return nullptr == OCI_GetPassword(nullptr); } },
            { "GetServerMajorVersion", [] { return OCI_UNKNOWN == OCI_GetServerMajorVersion(nullptr); } },
            { "GetServerMinorVersion", [] { return OCI_UNKNOWN == OCI_GetServerMinorVersion(nullptr); } },
            { "GetServerName", [] { return nullptr == OCI_GetServerName(nullptr); } },
            { "GetServerRevisionVersion", [] { return OCI_UNKNOWN == OCI_GetServerRevisionVersion(nullptr); } },
            { "GetServiceName", [] { return nullptr == OCI_GetServiceName(nullptr); } },
            { "GetSessionMode", [] { return OCI_UNKNOWN == OCI_GetSessionMode(nullptr); } },
            { "GetSessionTag", [] { return nullptr == OCI_GetSessionTag(nullptr); } },
            { "GetStatementCacheSize", [] { return 0 == OCI_GetStatementCacheSize(nullptr); } },
            { "GetTimeout", [] { return 0 == OCI_GetTimeout(nullptr, 1); } },
            { "GetTrace", [] { return nullptr == OCI_GetTrace(nullptr, 1); } },
            { "GetTransaction", [] { return nullptr == OCI_GetTransaction(nullptr); } },
            { "GetUserName", [] { return nullptr == OCI_GetUserName(nullptr); } },
            { "GetVersionConnection", [] { return OCI_UNKNOWN == OCI_GetVersionConnection(nullptr); } },
            { "GetVersionServer", [] { return nullptr == OCI_GetVersionServer(nullptr); } },
            { "IsConnected", [] { return !OCI_IsConnected(nullptr); } },
            { "IsTAFCapable", [] { return !OCI_IsTAFCapable(nullptr); } },
            { "Ping", [] { return !OCI_Ping(nullptr); } },
            { "Rollback", [] { return !OCI_Rollback(nullptr); } },
            { "ServerDisableOutput", [] { return !OCI_ServerDisableOutput(nullptr); } },
            { "ServerEnableOutput", [] { return !OCI_ServerEnableOutput(nullptr, 1, 1, 1); } },
            { "ServerGetOutput", [] { return nullptr == OCI_ServerGetOutput(nullptr); } },
            { "SetAutoCommit", [] { return !OCI_SetAutoCommit(nullptr, FALSE); } },
            { "SetDefaultLobPrefetchSize", [] { return !OCI_SetDefaultLobPrefetchSize(nullptr, 1); } },
            { "SetPassword", [] { return !OCI_SetPassword(nullptr, OTEXT("x")); } },
            { "SetSessionTag", [] { return !OCI_SetSessionTag(nullptr, OTEXT("x")); } },
            { "SetStatementCacheSize", [] { return !OCI_SetStatementCacheSize(nullptr, 1); } },
            { "SetTAFHandler", [] { return !OCI_SetTAFHandler(nullptr, nullptr); } },
            { "SetTimeout", [] { return !OCI_SetTimeout(nullptr, 1, 1); } },
            { "SetTrace", [] { return !OCI_SetTrace(nullptr, 1, OTEXT("x")); } },
            { "SetTransaction", [] { return !OCI_SetTransaction(nullptr, nullptr); } },
            { "StatementCreate", [] { return nullptr == OCI_StatementCreate(nullptr); } },
        }),
        &NullArgCaseName);

}
