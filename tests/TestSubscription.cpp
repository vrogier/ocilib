#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{
    static void OnSubscriptionEvent(OCI_Event* event)
    {
        (void) event;
    }

    TEST(TestSubscription, RegisterAndGet)
    {
        ExecDML(OTEXT("drop table TestSubscriptionData"));
        ExecDML(OTEXT("create table TestSubscriptionData (id number)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        const auto sub = OCI_SubscriptionRegister(conn, OTEXT("TestSubscriptionSub"), OCI_CNT_ALL, OnSubscriptionEvent, OCI_SRM_DEFAULT, 120);
        ASSERT_NE(nullptr, sub);

        ASSERT_EQ(ostring(OTEXT("TestSubscriptionSub")), ostring(OCI_SubscriptionGetName(sub)));
        ASSERT_EQ(conn, OCI_SubscriptionGetConnection(sub));
        ASSERT_EQ(120u, OCI_SubscriptionGetTimeout(sub));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestSubscriptionData")));
        ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

        ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestSubscriptionData"));
    }

    TEST(TestSubscription, RegisterSecureMode)
    {
        ExecDML(OTEXT("drop table TestSubscriptionData"));
        ExecDML(OTEXT("create table TestSubscriptionData (id number)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        const auto sub = OCI_SubscriptionRegister(conn, OTEXT("TestSubscriptionSecureSub"), OCI_CNT_ALL, OnSubscriptionEvent, OCI_SRM_SECURE_NOTIFICATION, 0);
        ASSERT_NE(nullptr, sub);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestSubscriptionData")));
        ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

        ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestSubscriptionData"));
    }
}

namespace TestCppApi
{
    static void OnSubscriptionEventCpp(ocilib::Event& event)
    {
        (void) event;
    }

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
        ASSERT_TRUE(sub.GetName() == ostring(OTEXT("TestSubscriptionSecureSubCpp")));

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestSubscriptionData")));
        ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

        sub.Unregister();

        ExecDML(OTEXT("drop table TestSubscriptionData"));
    }
}

namespace TestCApi
{
    /* each armed fault makes the Nth subsequent OCIAttrSet fail, covering
       the error arm of the attribute set at that position in the register
       sequence (timeout, name, namespace, protocol, callback, rowids,
       context) */

    class TestSubRegisterAttrFail : public ::testing::TestWithParam<OCI_Int32>
    {
    protected:

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);
        }

        void TearDown() override
        {
            OCI_TestApiRestore(env);
            OCI_ConnectionFree(conn);
        }

        OCI_Environment* env{ nullptr };
        OCI_Connection*  conn{ nullptr };
    };

    TEST_P(TestSubRegisterAttrFail, Fails)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, GetParam());
        ASSERT_EQ(nullptr, OCI_SubscriptionRegister(conn, OTEXT("TestSubErr"), OCI_CNT_ROWS, OnSubscriptionEvent, OCI_SRM_DEFAULT, 60));
    }

    INSTANTIATE_TEST_CASE_P(AttrSetPositions, TestSubRegisterAttrFail,
        ::testing::Range(1, 8));

    TEST_F(TestSubRegisterAttrFail, RegisterOciFailure)
    {
        /* OCISubscriptionRegister failure arm */

        OCI_TestFailApiCall(env, OCI_TEST_API_SUBSCR_REGISTER, 1);
        ASSERT_EQ(nullptr, OCI_SubscriptionRegister(conn, OTEXT("TestSubErr"), OCI_CNT_ROWS, OnSubscriptionEvent, OCI_SRM_DEFAULT, 60));
    }

    /* the remaining error arms need a working registration */

    class TestSubErrRegistered : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("drop table TestSubscriptionErrData"));
            ExecDML(OTEXT("create table TestSubscriptionErrData (id number)"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop table TestSubscriptionErrData"));
        }

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            sub = OCI_SubscriptionRegister(conn, OTEXT("TestSubErr"), OCI_CNT_ALL, OnSubscriptionEvent, OCI_SRM_DEFAULT, 60);
            ASSERT_NE(nullptr, sub);
        }

        void TearDown() override
        {
            OCI_TestApiRestore(env);
            if (sub)
            {
                OCI_SubscriptionUnregister(sub);
            }
            OCI_ConnectionFree(conn);
        }

        OCI_Environment*   env{ nullptr };
        OCI_Connection*    conn{ nullptr };
        OCI_Subscription*  sub{ nullptr };
    };

    TEST_F(TestSubErrRegistered, AddStatementAttrSetFailure)
    {
        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);
        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestSubscriptionErrData")));

        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_SubscriptionAddStatement(sub, stmt));

        ASSERT_TRUE(OCI_StatementFree(stmt));
    }

    TEST_F(TestSubErrRegistered, UnregisterOciFailure)
    {
        /* OCISubscriptionUnRegister failure: the UTF16 code path reads the
           error code and throws when it is not the tolerated ORA-24915 */

        OCI_TestFailApiCall(env, OCI_TEST_API_SUBSCR_UNREGISTER, 1);
        ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
        sub = nullptr;
    }
}
