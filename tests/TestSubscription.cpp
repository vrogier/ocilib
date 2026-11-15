#include "ocilib_tests.h"
#include "TestFeatures.h"
#include "NullArgs.h"

namespace TestCApi
{
    static void OnSubscriptionEvent(OCI_Event* event)
    {
        (void) event;
    }

#ifdef OCI_CHARSET_MBCS

    TEST(TestSubscription, RegisterSubscriptionInSecureMode_Subscribes)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_SECURE_NOTIFICATION);
        ExecDML(OTEXT("drop table TestRegisterSecureMode"));
        ExecDML(OTEXT("create table TestRegisterSecureMode(id number)"));

        /* dedicated env: OCI_SECURE_NOTIFICATION registrations reject the
           callback address/port attrs that the shared env sets from
           OCILIB_TEST_SUBSCR_ADDR (ORA-24912) */
        const auto env = GetSpecificEnvironment(nullptr, OCI_UNKNOWN, OCI_ENV_DEFAULT | OCI_ENV_THREADED | OCI_ENV_EVENTS);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        const auto sub = OCI_SubscriptionCreate(conn, OTEXT("TestRegisterSecureModeId"), OCI_CNT_ALL, OnSubscriptionEvent, OCI_SRM_SECURE_NOTIFICATION, 120);
        ASSERT_NE(nullptr, sub);
    
        ASSERT_EQ(String(OTEXT("TestRegisterSecureModeId")), String(OCI_SubscriptionGetName(sub)));
        ASSERT_EQ(conn, OCI_SubscriptionGetConnection(sub));
        ASSERT_EQ(120u, OCI_SubscriptionGetTimeout(sub));

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select * from TestRegisterSecureMode")));
        ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

        ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));

        ExecDML(OTEXT("drop table TestRegisterSecureMode"));
    }

#endif

    class TestSubscriptionNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestSubscriptionNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestSubscriptionNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "SubscriptionAddStatement", [] { return !OCI_SubscriptionAddStatement(nullptr, nullptr); } },
            { "SubscriptionGetConnection", [] { return nullptr == OCI_SubscriptionGetConnection(nullptr); } },
            { "SubscriptionGetName", [] { return nullptr == OCI_SubscriptionGetName(nullptr); } },
            { "SubscriptionGetTimeout", [] { return 0 == OCI_SubscriptionGetTimeout(nullptr); } },
            { "SubscriptionRegister", [] { return nullptr == OCI_SubscriptionCreate(nullptr, OTEXT("x"), 1, nullptr, 1, 1); } },
            { "SubscriptionUnregister", [] { return !OCI_SubscriptionUnregister(nullptr); } },
        }));

    /* OCI_SRM_SECURE_NOTIFICATION requires client and server >= 19.4 —
       verify the OCI_ERR_NOT_AVAILABLE contract on unsupported versions
       (runs on both charsets: the gate fires before any OCI call) */

    TEST(TestSubscription, RegisterSecureMode_EmulatedClient_NotAvailable)
    {
        const auto env = GetSpecificEnvironment(nullptr, OCI_12_1,
            OCI_ENV_DEFAULT | OCI_ENV_THREADED | OCI_ENV_EVENTS);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_EQ(nullptr, OCI_SubscriptionCreate(conn, OTEXT("TestSecureSubNotAvail"),
            OCI_CNT_ALL, OnSubscriptionEvent, OCI_SRM_SECURE_NOTIFICATION, 0));

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        EXPECT_EQ(OCI_ERR_NOT_AVAILABLE, OCI_ErrorGetCode(err));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

    TEST(TestSubscription, RegisterSecureMode_RealServer_NotAvailable)
    {
        SKIP_IF_FEATURE_SUPPORTED(OCI_FEATURE_SECURE_NOTIFICATION);

        const auto env = GetSpecificEnvironment(nullptr, OCI_UNKNOWN,
            OCI_ENV_DEFAULT | OCI_ENV_THREADED | OCI_ENV_EVENTS);
        ASSERT_NE(nullptr, env);
        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        ASSERT_EQ(nullptr, OCI_SubscriptionCreate(conn, OTEXT("TestSecureSubNotAvail"),
            OCI_CNT_ALL, OnSubscriptionEvent, OCI_SRM_SECURE_NOTIFICATION, 0));

        const auto err = OCI_GetLastError();
        ASSERT_NE(nullptr, err);
        EXPECT_EQ(OCI_ERR_NOT_AVAILABLE, OCI_ErrorGetCode(err));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
        ASSERT_TRUE(OCI_EnvironmentFree(env));
    }

}

namespace TestCppApi
{
    static void OnSubscriptionEventCpp(ocilib::Event& event)
    {
        (void) event;
    }

#ifdef OCI_CHARSET_MBCS

    TEST(TestSubscription, RegisterWithSecureNotification_Subscribes)
    {
        SKIP_IF_FEATURE_UNSUPPORTED(OCI_FEATURE_SECURE_NOTIFICATION);
        ExecDML(OTEXT("drop table TestSubscriptionData"));
        ExecDML(OTEXT("create table TestSubscriptionData (id number)"));

        /* dedicated env: OCI_SECURE_NOTIFICATION registrations reject the
           callback address/port attrs that the shared env sets from
           OCILIB_TEST_SUBSCR_ADDR (ORA-24912) */
        ocilib::Environment env(ocilib::Environment::Default | ocilib::Environment::Threaded | ocilib::Environment::Events, HOME ? HOME : OTEXT(""));
        ocilib::Connection con(env, DBS, USR, PWD);
        ocilib::Statement stmt(con);

        ocilib::Subscription sub;

        sub.Register(con, OTEXT("TestSubscriptionSecureSubCpp"), ocilib::Subscription::AllChanges,
            OnSubscriptionEventCpp, ocilib::Subscription::ModeSecureNotification);

        ASSERT_FALSE(sub.IsNull());
        ASSERT_TRUE(sub.GetName() == String(OTEXT("TestSubscriptionSecureSubCpp")));

        ASSERT_TRUE(OCI_StatementPrepare(stmt, OTEXT("select * from TestSubscriptionData")));
        ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

        sub.Unregister();

        ExecDML(OTEXT("drop table TestSubscriptionData"));
    }

#endif

    /* same contract through the real client+server versions — runs only
       when secure notification is unsupported (needs a dedicated env:
       OCI_SECURE_NOTIFICATION rejects SUBSCR_ADDR attrs, ORA-24912) */

    TEST(TestSubscription, RegisterSecureMode_NotAvailable)
    {
        ocilib::Environment env(ocilib::Environment::Default | ocilib::Environment::Threaded | ocilib::Environment::Events,
                                HOME ? HOME : OTEXT(""));
        ocilib::Connection con(env, DBS, USR, PWD);

        SKIP_IF_FEATURE_AVAILABLE(env, &con, OCI_FEATURE_SECURE_NOTIFICATION);

        ocilib::Subscription sub;

        try
        {
            sub.Register(con, OTEXT("TestSecureSubNotAvail"), ocilib::Subscription::AllChanges,
                         OnSubscriptionEventCpp, ocilib::Subscription::ModeSecureNotification);
            FAIL() << "expected ocilib::Exception";
        }
        catch (const ocilib::Exception& ex)
        {
            EXPECT_EQ(ocilib::Exception::OcilibError, ex.GetCategory());
            EXPECT_EQ(static_cast<ocilib::UInt32>(OCI_ERR_NOT_AVAILABLE), ex.GetCode());
        }
    }

    /* --- public-API error paths: NULL subscription handles hit the CHECK_PTR
       arms the dropped fault tests covered --- */


}
