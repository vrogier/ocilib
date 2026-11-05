#include "ocilib_tests.h"

namespace TestCApi
{
    static std::atomic<OCI_Int32> TestEventCount{0};
    static OCI_Subscription* TestEventSub = nullptr;

    static void TestEventHandler(OCI_Event* event)
    {
        EXPECT_NE(nullptr, event);
        EXPECT_EQ(OCI_ENT_OBJECT_CHANGED, OCI_EventGetType(event));
        EXPECT_EQ(OCI_ONT_INSERT, OCI_EventGetOperation(event));

        const auto sub = OCI_EventGetSubscription(event);
        EXPECT_NE(nullptr, sub);
        EXPECT_EQ(TestEventSub, sub);

        EXPECT_NE(nullptr, OCI_EventGetDatabase(event));
        EXPECT_EQ(ToUpper(String(DBSNAME)), ToUpper(OCI_EventGetDatabase(event)));

        const String expectedObject = String(USR) + OTEXT(".TestEventTable");
        EXPECT_NE(nullptr, OCI_EventGetObject(event));
        EXPECT_EQ(ToUpper(expectedObject), ToUpper(OCI_EventGetObject(event)));

        EXPECT_NE(nullptr, OCI_EventGetRowid(event));

        TestEventCount++;
    }

    TEST(TestEvent, ObjectChanged)
    {
        TestEventCount = 0;

        ExecDML(OTEXT("drop table TestEventTable"));
        ExecDML(OTEXT("create table TestEventTable(code number)"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto sub = OCI_SubscriptionRegister(conn, OTEXT("TestEventSub"), OCI_CNT_ROWS, TestEventHandler, OCI_SRM_DEFAULT, 0);
        ASSERT_NE(nullptr, sub);

        TestEventSub = sub;

        const auto stmt = OCI_StatementCreate(conn);
        ASSERT_NE(nullptr, stmt);

        ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestEventTable")));
        ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

        ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("insert into TestEventTable(code) values (42)")));
        ASSERT_TRUE(OCI_Commit(conn));

        Sleep(1000);

        ASSERT_EQ(1, TestEventCount);

        ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
        ASSERT_TRUE(OCI_StatementFree(stmt));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop table TestEventTable"));
    }
}
