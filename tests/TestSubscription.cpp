#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    static void OnSubscriptionEvent(OCI_Event* event)
    {
        (void) event;
    }

#ifdef OCI_CHARSET_ANSI

    TEST(TestSubscription, RegisterSecureMode)
    {
        ExecDML(OTEXT("drop table TestRegisterSecureMode"));
        ExecDML(OTEXT("create table TestRegisterSecureMode(id number)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        const auto sub = OCI_SubscriptionRegister(conn, OTEXT("TestRegisterSecureModeId"), OCI_CNT_ALL, OnSubscriptionEvent, OCI_SRM_SECURE_NOTIFICATION, 120);
        ASSERT_NE(nullptr, sub);
    
        ASSERT_EQ(String(OTEXT("TestRegisterSecureModeId")), String(OCI_SubscriptionGetName(sub)));
        ASSERT_EQ(conn, OCI_SubscriptionGetConnection(sub));
        ASSERT_EQ(120u, OCI_SubscriptionGetTimeout(sub));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestRegisterSecureMode")));
        ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

        ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

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
            { "SubscriptionRegister", [] { return nullptr == OCI_SubscriptionRegister(nullptr, OTEXT("x"), 1, nullptr, 1, 1); } },
            { "SubscriptionUnregister", [] { return !OCI_SubscriptionUnregister(nullptr); } },
        }),
        &NullArgCaseName);

}

namespace TestCppApi
{
    static void OnSubscriptionEventCpp(ocilib::Event& event)
    {
        (void) event;
    }

#ifdef OCI_CHARSET_ANSI

    TEST(TestSubscription, RegisterWithSecureNotification)
    {
        ExecDML(OTEXT("drop table TestSubscriptionData"));
        ExecDML(OTEXT("create table TestSubscriptionData (id number)"));

        ocilib::Environment& env = GetEnvironmentCpp();
        ocilib::Connection con(env, DBS, USR, PWD);
        ocilib::Statement stmt(con);

        ocilib::Subscription sub;

        sub.Register(con, OTEXT("TestSubscriptionSecureSubCpp"), ocilib::Subscription::AllChanges,
            OnSubscriptionEventCpp, ocilib::Subscription::ModeSecureNotification);

        ASSERT_FALSE(sub.IsNull());
        ASSERT_TRUE(sub.GetName() == String(OTEXT("TestSubscriptionSecureSubCpp")));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestSubscriptionData")));
        ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

        sub.Unregister();

        ExecDML(OTEXT("drop table TestSubscriptionData"));
    }

#endif

    /* --- public-API error paths: NULL subscription handles hit the CHECK_PTR
       arms the dropped fault tests covered --- */


}
