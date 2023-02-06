#include "ocilib_tests.h"

constexpr size_t SingleDDL1ExpectedEventsCount{ 1 };
constexpr size_t SingleDDL2ExpectedEventsCount{ 2 };
constexpr size_t MultipleDDLExpectedEventsCount{ 2 };
constexpr size_t MultipleDMLExpectedEventsCount{ 5 };

static std::atomic<size_t> SingleDDL1EventCount{ 0 };
static std::atomic<size_t> SingleDDL2EventCount{ 0 };
static std::atomic<size_t> MultipleDDLEventCount{ 0 };
static std::atomic<size_t> MultipleDMLEventCount{ 0 };

struct Event
{
    ostring SubName;
    ostring DatabaseName;
    ostring ObjectName;
    unsigned int Type;
    unsigned int Operation;
};

static std::array< Event, SingleDDL1ExpectedEventsCount> SingleDDL1ExpectedEvents
{
    Event
    {
        OTEXT("sub-1"),
        DBSNAME,
        ostring(USR) + OTEXT(".TestTableSingleDDL1"),
        OCI_ENT_OBJECT_CHANGED,
        OCI_ONT_ALTER
    }
};

static std::array< Event, SingleDDL2ExpectedEventsCount> SingleDDL2ExpectedEvents
{
    Event
    {
        OTEXT("sub-2"),
        DBSNAME,
        ostring(USR) + OTEXT(".TestTableSingleDDL2"),
        OCI_ENT_OBJECT_CHANGED,
        OCI_ONT_ALTER
    },
    Event
    {
        OTEXT("sub-2"),
        DBSNAME,
        ostring(USR) + OTEXT(".TestTableSingleDDL2"),
        OCI_ENT_OBJECT_CHANGED,
        OCI_ONT_ALTER
    }
};

static std::array< Event, MultipleDDLExpectedEventsCount> MultipleDDLExpectedEvents
{
    Event
    {
        OTEXT("sub-3"),
        DBSNAME,
        ostring(USR) + OTEXT(".TestTableMultipleDDLTable1"),
        OCI_ENT_OBJECT_CHANGED,
        OCI_ONT_ALTER
    },
    Event
    {
        OTEXT("sub-3"),
        DBSNAME,
        ostring(USR) + OTEXT(".TestTableMultipleDDLTable2"),
        OCI_ENT_OBJECT_CHANGED,
        OCI_ONT_ALTER
    }
};

static std::array< Event, MultipleDMLExpectedEventsCount> MultipleDMLExpectedEvents
{
    Event
    {
        OTEXT("sub-4"),
        DBSNAME,
        ostring(USR) + OTEXT(".TestTableMultipleDML"),
        OCI_ENT_OBJECT_CHANGED,
        OCI_ONT_INSERT
    },
    Event
    {
        OTEXT("sub-4"),
        DBSNAME,
        ostring(USR) + OTEXT(".TestTableMultipleDML"),
        OCI_ENT_OBJECT_CHANGED,
        OCI_ONT_INSERT
    },
    Event
    {
        OTEXT("sub-4"),
        DBSNAME,
        ostring(USR) + OTEXT(".TestTableMultipleDML"),
        OCI_ENT_OBJECT_CHANGED,
        OCI_ONT_UPDATE
    },
    Event
    {
        OTEXT("sub-4"),
        DBSNAME,
        ostring(USR) + OTEXT(".TestTableMultipleDML"),
        OCI_ENT_OBJECT_CHANGED,
        OCI_ONT_DELETE
    },
        Event
    {
        OTEXT("sub-4"),
        DBSNAME,
        ostring(USR) + OTEXT(".TestTableMultipleDML"),
        OCI_ENT_OBJECT_CHANGED,
        OCI_ONT_DELETE
    }
};

static void CheckEvent(const Event& evt, OCI_Event* event)
{
    const auto sub = OCI_EventGetSubscription(event);
    ASSERT_NE(nullptr, sub);

    EXPECT_EQ(ToUpper(evt.SubName), ToUpper(OCI_SubscriptionGetName(sub)));
    EXPECT_EQ(ToUpper(evt.DatabaseName), ToUpper(OCI_EventGetDatabase(event)));
    EXPECT_EQ(evt.Type, OCI_EventGetType(event));
    EXPECT_EQ(evt.Operation, OCI_EventGetOperation(event));
    EXPECT_EQ(ToUpper(evt.ObjectName), ToUpper(OCI_EventGetObject(event)));
}

static void SingleDDL1EventEventHandler(OCI_Event* event)
{
    EXPECT_TRUE(SingleDDL1EventCount < SingleDDL1ExpectedEvents.size());

    CheckEvent(SingleDDL1ExpectedEvents[SingleDDL1EventCount], event);

    SingleDDL1EventCount++;
}

static void SingleDDL2EventEventHandler(OCI_Event* event)
{
    EXPECT_TRUE(SingleDDL2EventCount < SingleDDL2ExpectedEvents.size());

    CheckEvent(SingleDDL2ExpectedEvents[SingleDDL2EventCount], event);

    SingleDDL2EventCount++;
}


static void MultipleDDLEventEventHandler(OCI_Event* event)
{
    EXPECT_TRUE(MultipleDDLEventCount < MultipleDDLExpectedEvents.size());

    CheckEvent(MultipleDDLExpectedEvents[MultipleDDLEventCount], event);

    MultipleDDLEventCount++;
}

static void MultipleDMLEventEventHandler(OCI_Event* event)
{
    EXPECT_TRUE(MultipleDMLEventCount < MultipleDMLExpectedEvents.size());

    CheckEvent(MultipleDMLExpectedEvents[MultipleDMLEventCount], event);

    MultipleDMLEventCount++;
}

TEST(TestNotifications, SingleDDL1)
{
    ExecDML(OTEXT("drop table TestTableSingleDDL1"));
    ExecDML(OTEXT("create table TestTableSingleDDL1(code number)"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_EVENTS));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto sub = OCI_SubscriptionRegister(conn, OTEXT("sub-1"), OCI_CNT_ALL, SingleDDL1EventEventHandler, 0, 0);
    ASSERT_NE(nullptr, sub);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestTableSingleDDL1")));
    ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("alter table TestTableSingleDDL1 add price number")));

    Sleep(1000);

    ASSERT_EQ(SingleDDL1ExpectedEvents.size(), SingleDDL1EventCount);

    ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestTableSingleDDL1"));
}

TEST(TestNotifications, SingleDDL2)
{
    ExecDML(OTEXT("drop table TestTableSingleDDL2"));
    ExecDML(OTEXT("create table TestTableSingleDDL2(code number)"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_EVENTS));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto sub = OCI_SubscriptionRegister(conn, OTEXT("sub-2"), OCI_CNT_ALL, SingleDDL2EventEventHandler, 0, 0);
    ASSERT_NE(nullptr, sub);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestTableSingleDDL2")));
    ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("alter table TestTableSingleDDL2 add price1 number")));
    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("alter table TestTableSingleDDL2 add price2 number")));

    Sleep(1000);

    ASSERT_EQ(SingleDDL2ExpectedEvents.size(), SingleDDL2EventCount);

    ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestTableSingleDDL2"));
}

TEST(TestNotifications, MultipleDDL)
{
    ExecDML(OTEXT("drop table TestTableMultipleDDLTable1"));
    ExecDML(OTEXT("drop table TestTableMultipleDDLTable2"));
    ExecDML(OTEXT("create table TestTableMultipleDDLTable1(code number)"));
    ExecDML(OTEXT("create table TestTableMultipleDDLTable2(code number)"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_EVENTS));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto sub = OCI_SubscriptionRegister(conn, OTEXT("sub-3"), OCI_CNT_ALL, MultipleDDLEventEventHandler, 0, 0);
    ASSERT_NE(nullptr, sub);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);


    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestTableMultipleDDLTable1")));
    ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestTableMultipleDDLTable2")));
    ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("alter table TestTableMultipleDDLTable1 add price number")));
    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("alter table TestTableMultipleDDLTable2 add price number")));

    Sleep(1000);

    ASSERT_EQ(MultipleDDLExpectedEvents.size(), MultipleDDLEventCount);

    ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestTableMultipleDDLTable1"));
    ExecDML(OTEXT("drop table TestTableMultipleDDLTable2"));
}

TEST(TestNotifications, MultipleDML)
{
    ExecDML(OTEXT("drop table TestTableMultipleDML"));
    ExecDML(OTEXT("create table TestTableMultipleDML(code number)"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_EVENTS));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    const auto sub = OCI_SubscriptionRegister(conn, OTEXT("sub-4"), OCI_CNT_ALL, MultipleDMLEventEventHandler, 0, 0);
    ASSERT_NE(nullptr, sub);

    const auto stmt = OCI_StatementCreate(conn);
    ASSERT_NE(nullptr, stmt);

    ASSERT_TRUE(OCI_Prepare(stmt, OTEXT("select * from TestTableMultipleDML")));
    ASSERT_TRUE(OCI_SubscriptionAddStatement(sub, stmt));

    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("insert into TestTableMultipleDML(code) values (1)")));
    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("insert into TestTableMultipleDML(code) values (2)")));
    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("update TestTableMultipleDML set code = 3 where code = 2")));
    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("update TestTableMultipleDML set code = 4 where code = 999999"))); // shall not trigger event
    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("delete from TestTableMultipleDML where code = 1")));
    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("delete from TestTableMultipleDML where code = 3")));
    ASSERT_TRUE(OCI_ExecuteStmt(stmt, OTEXT("delete from TestTableMultipleDML where code = 77777"))); // shall not trigger event

    ASSERT_TRUE(OCI_Commit(conn));

    Sleep(2000);

    ASSERT_EQ(MultipleDMLExpectedEvents.size(), MultipleDMLEventCount);

    ASSERT_TRUE(OCI_SubscriptionUnregister(sub));
    ASSERT_TRUE(OCI_StatementFree(stmt));
    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop table TestTableMultipleDML"));
}