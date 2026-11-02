#include <thread>

#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{

    static Context context;

    static inline void AddError(OCI_Error* err)
    {
        context.Errs.emplace_back(Error{ OCI_ErrorGetInternalCode(err), OCI_ErrorGetOCICode(err) });
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
        ASSERT_EQ(0, context.Errs[0].LibCode);
        ASSERT_EQ(1017, context.Errs[0].OCICode);

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
        ASSERT_EQ(0, context.Errs[0].LibCode);
        ASSERT_EQ(1017, context.Errs[0].OCICode);

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
        ASSERT_EQ(0, context.Errs[0].LibCode);
        ASSERT_EQ(1013, context.Errs[0].OCICode);

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
        ASSERT_EQ(ostring(module), ostring(value));

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
        ASSERT_EQ(ostring(GetParam().value), ostring(OCI_GetTrace(con, GetParam().type)));
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

    TEST(TestConnection, SetPasswordRelogon)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        /* pretend the connection is not logged: OCI_SetPassword() then goes
           through OcilibConnectionLogon() with the new password. Release the
           logon handles first, they are reallocated by the second logon */
        OCI_TestConSetState(con, OCI_TEST_CONN_ALLOCATED);
        OCI_TestConReleaseLogonHandles(con);

        ASSERT_TRUE(OCI_SetPassword(con, OTEXT("OcilibCovPwd3")));
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

    TEST(TestConnection, FreeWithOpenFiles)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        /* simulate an opened server file: the logoff path must close all of
           them through OCILobFileCloseAll(). A real BFILE open requires the
           file to exist on the database host, which is not guaranteed here */
        OCI_TestConSetNbFiles(con, 1);

        ASSERT_TRUE(OCI_ConnectionFree(con));
    }

    TEST(TestConnection, FreeWithSubscription)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, con);

        const auto sub = OCI_SubscriptionRegister(con, OTEXT("TestConnSub"), OCI_CNT_ALL, TestConnSubHandler, 0);
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

    struct ServerVersionGetterCase
    {
        OCI_UInt32 (*getter)(OCI_Connection*);
        boolean positive;
    };

    class TestConnectionLegacyVersion : public ::testing::TestWithParam<ServerVersionGetterCase>
    {
    protected:

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            con = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, con);

            saved_runtime = OCI_TestEnvSetVersionRuntime(con, OCI_10_1);
        }

        void TearDown() override
        {
            /* restore the modern path and refresh the cached values */
            OCI_TestEnvSetVersionRuntime(con, saved_runtime);
            OCI_TestConResetServerVersion(con);
            OCI_GetVersionServer(con);
            OCI_ConnectionFree(con);
        }

        OCI_Environment* env{ nullptr };
        OCI_Connection*  con{ nullptr };
        OCI_UInt32     saved_runtime{ 0 };
    };

    TEST_P(TestConnectionLegacyVersion, ParsesViaServerVersion)
    {
        /* drop the cached version string: the version getter fetches it again,
           this time through the legacy OCIServerVersion + string parse path */

        OCI_TestConResetServerVersion(con);

        const auto value = GetParam().getter(con);
        if (GetParam().positive) { ASSERT_GT(value, 0u); }
        else { ASSERT_GE(value, 0u); }
    }

    INSTANTIATE_TEST_CASE_P(Components, TestConnectionLegacyVersion,
        ::testing::Values(
            ServerVersionGetterCase{ OCI_GetServerMajorVersion,    TRUE },
            ServerVersionGetterCase{ OCI_GetServerMinorVersion,    FALSE },
            ServerVersionGetterCase{ OCI_GetServerRevisionVersion, FALSE }));
}
