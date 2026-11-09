#include <atomic>

#include "ocilib_tests.h"
#include "NullArgs.h"
static std::atomic<OCI_Int32> MessageWithConsumersCount {0};

namespace TestCApi
{
    /* -------------------------------------------------------------------------
       Shared fixture: payload type + started queue + enqueue/dequeue handles
       ------------------------------------------------------------------------- */

    class TestQueueOps : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_TypeInfo* inf = nullptr;
        OCI_Enqueue* enq = nullptr;
        OCI_Dequeue* deq = nullptr;

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestQueueOpsType as object(title varchar2(50), content varchar2(100))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestQueueOpsType"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            OCI_QueueStop(conn, OTEXT("TestQueueOpsQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestQueueOpsQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestQueueOpsTable"), TRUE);

            ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestQueueOpsTable"), OTEXT("TestQueueOpsType"),
                                             NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
            ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestQueueOpsQueue"), OTEXT("TestQueueOpsTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
            ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestQueueOpsQueue"), TRUE, TRUE));

            inf = OCI_TypeInfoGet(conn, OTEXT("TestQueueOpsType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, inf);
            enq = OCI_EnqueueCreate(inf, OTEXT("TestQueueOpsQueue"));
            ASSERT_NE(nullptr, enq);
            deq = OCI_DequeueCreate(inf, OTEXT("TestQueueOpsQueue"));
            ASSERT_NE(nullptr, deq);
        }

        void TearDown() override
        {
            if (enq) OCI_EnqueueFree(enq);
            if (deq) OCI_DequeueFree(deq);
            if (conn)
            {
                OCI_QueueStop(conn, OTEXT("TestQueueOpsQueue"), TRUE, TRUE, FALSE);
                OCI_QueueDrop(conn, OTEXT("TestQueueOpsQueue"));
                OCI_QueueTableDrop(conn, OTEXT("TestQueueOpsTable"), TRUE);
                OCI_ConnectionFree(conn);
            }
        }
    };

    TEST_F(TestQueueOps, QueueAlterOnStartedQueue_Succeeds)
    {
        EXPECT_TRUE(OCI_QueueAlter(conn, OTEXT("TestQueueOpsQueue"), 0, 0, 0, OTEXT("altered comment")));
    }

    TEST_F(TestQueueOps, EnqueueDequeue_ReturnsMessagePayload)
    {
        const auto msg_in = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg_in);
        const auto obj_in = OCI_ObjectCreate(inf);
        ASSERT_NE(nullptr, obj_in);
        ASSERT_TRUE(OCI_ObjectSetString(obj_in, OTEXT("title"), OTEXT("NEXT MEETING")));
        ASSERT_TRUE(OCI_ObjectSetString(obj_in, OTEXT("content"), OTEXT("12:00 PM IN STARBUCKS")));
        ASSERT_TRUE(OCI_MsgSetObject(msg_in, obj_in));
        ASSERT_TRUE(OCI_EnqueuePut(enq, msg_in));
        ASSERT_TRUE(OCI_MsgFree(msg_in));
        ASSERT_TRUE(OCI_ObjectFree(obj_in));
        ASSERT_TRUE(OCI_Commit(conn));

        const auto msg_out = OCI_DequeueGet(deq);

        ASSERT_NE(nullptr, msg_out);
        const auto obj_out = OCI_MsgGetObject(msg_out);
        ASSERT_NE(nullptr, obj_out);
        EXPECT_EQ(String(OTEXT("NEXT MEETING")), String(OCI_ObjectGetString(obj_out, OTEXT("title"))));
        EXPECT_EQ(String(OTEXT("12:00 PM IN STARBUCKS")), String(OCI_ObjectGetString(obj_out, OTEXT("content"))));
    }

    TEST_F(TestQueueOps, QueueTablePurgeOnEmptyQueue_Succeeds)
    {
        EXPECT_TRUE(OCI_QueueTablePurge(conn, OTEXT("TestQueueOpsTable"), NULL, FALSE, OCI_APM_PERSISTENT));
    }

    static void OnMessage(OCI_Dequeue* deq)
    {
        ++MessageWithConsumersCount;
        const auto msg_out = OCI_DequeueGet(deq);
        ASSERT_NE(nullptr, msg_out);

        const auto obj_out = OCI_MsgGetObject(msg_out);
        ASSERT_NE(nullptr, obj_out);

        ASSERT_EQ(String(OTEXT("NEXT MEETING")), String(OCI_ObjectGetString(obj_out, OTEXT("title"))));
        ASSERT_EQ(String(OTEXT("12:00 PM IN STARBUCKS")), String(OCI_ObjectGetString(obj_out, OTEXT("content"))));
    }

    /* -------------------------------------------------------------------------
       Shared fixture: multi-consumer queue + two consumer dequeues + agents
       ------------------------------------------------------------------------- */

    class TestQueueConsumers : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_TypeInfo* inf = nullptr;
        OCI_Enqueue* enq = nullptr;
        OCI_Dequeue* deq1 = nullptr;
        OCI_Dequeue* deq2 = nullptr;
        OCI_Agent* agents[2]{};

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestQueueMessageWithConsumersType as object(title varchar2(50), content varchar2(100))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestQueueMessageWithConsumersType"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            OCI_QueueStop(conn, OTEXT("TestQueueMessageWithConsumers"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestQueueMessageWithConsumers"));
            OCI_QueueTableDrop(conn, OTEXT("TestQueueMessageWithConsumersTable"), TRUE);

            ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestQueueMessageWithConsumersTable"), OTEXT("TestQueueMessageWithConsumersType"),
                                             NULL, NULL, TRUE, OCI_AGM_NONE, NULL, 0, 0, NULL));
            ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestQueueMessageWithConsumers"), OTEXT("TestQueueMessageWithConsumersTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
            ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestQueueMessageWithConsumers"), TRUE, TRUE));

            inf = OCI_TypeInfoGet(conn, OTEXT("TestQueueMessageWithConsumersType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, inf);
            enq = OCI_EnqueueCreate(inf, OTEXT("TestQueueMessageWithConsumers"));
            ASSERT_NE(nullptr, enq);
            deq1 = OCI_DequeueCreate(inf, OTEXT("TestQueueMessageWithConsumers"));
            ASSERT_NE(nullptr, deq1);
            deq2 = OCI_DequeueCreate(inf, OTEXT("TestQueueMessageWithConsumers"));
            ASSERT_NE(nullptr, deq2);
            agents[0] = OCI_AgentCreate(conn, OTEXT("C1"), nullptr);
            ASSERT_NE(nullptr, agents[0]);
            agents[1] = OCI_AgentCreate(conn, OTEXT("C2"), nullptr);
            ASSERT_NE(nullptr, agents[1]);
            ASSERT_TRUE(OCI_DequeueSetConsumer(deq1, OTEXT("C1")));
            ASSERT_TRUE(OCI_DequeueSetNavigation(deq1, OCI_ADN_FIRST_MSG));
            ASSERT_TRUE(OCI_DequeueSetConsumer(deq2, OTEXT("C2")));
            ASSERT_TRUE(OCI_DequeueSetNavigation(deq2, OCI_ADN_FIRST_MSG));
        }

        void TearDown() override
        {
            if (agents[0]) OCI_AgentFree(agents[0]);
            if (agents[1]) OCI_AgentFree(agents[1]);
            if (enq) OCI_EnqueueFree(enq);
            if (deq1) OCI_DequeueFree(deq1);
            if (deq2) OCI_DequeueFree(deq2);
            if (conn)
            {
                OCI_QueueStop(conn, OTEXT("TestQueueMessageWithConsumers"), TRUE, TRUE, FALSE);
                OCI_QueueDrop(conn, OTEXT("TestQueueMessageWithConsumers"));
                OCI_QueueTableDrop(conn, OTEXT("TestQueueMessageWithConsumersTable"), TRUE);
                OCI_ConnectionFree(conn);
            }
        }
    };

    TEST_F(TestQueueConsumers, SubscribeTwoConsumers_Succeeds)
    {
        std::array<OCI_Dequeue*, 2> dequeues{ deq1, deq2 };

        ASSERT_TRUE(OCI_DequeueSubscribe(conn, dequeues.data(), static_cast<OCI_UInt32>(dequeues.size()), 0, OnMessage));

        EXPECT_TRUE(OCI_DequeueUnsubscribe(deq1));
        EXPECT_TRUE(OCI_DequeueUnsubscribe(deq2));
    }

    TEST_F(TestQueueConsumers, EnqueueForConsumers_DeliversToBothCallbacks)
    {
        std::array<OCI_Dequeue*, 2> dequeues{ deq1, deq2 };
        ASSERT_TRUE(OCI_DequeueSubscribe(conn, dequeues.data(), static_cast<OCI_UInt32>(dequeues.size()), 0, OnMessage));

        const auto msg_in = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg_in);
        const auto obj_in = OCI_ObjectCreate(inf);
        ASSERT_NE(nullptr, obj_in);
        ASSERT_TRUE(OCI_ObjectSetString(obj_in, OTEXT("title"), OTEXT("NEXT MEETING")));
        ASSERT_TRUE(OCI_ObjectSetString(obj_in, OTEXT("content"), OTEXT("12:00 PM IN STARBUCKS")));
        ASSERT_TRUE(OCI_MsgSetObject(msg_in, obj_in));
        ASSERT_TRUE(OCI_MsgSetConsumers(msg_in, agents, 2));

        MessageWithConsumersCount = 0;
        ASSERT_TRUE(OCI_EnqueuePut(enq, msg_in));
        ASSERT_TRUE(OCI_MsgFree(msg_in));
        ASSERT_TRUE(OCI_ObjectFree(obj_in));
        ASSERT_TRUE(OCI_Commit(conn));

        const OCI_Int32 MaxRetries = 15;
        OCI_Int32 retry = 1;
        while (MessageWithConsumersCount < 2 && retry <= MaxRetries)
        {
            Sleep(100);
            retry++;
        }

        EXPECT_EQ(2, MessageWithConsumersCount);

        ASSERT_TRUE(OCI_DequeueUnsubscribe(deq1));
        ASSERT_TRUE(OCI_DequeueUnsubscribe(deq2));
    }

    TEST(TestQueue, SingleRawMessage)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        // no check - for cleanup
        OCI_QueueStop(conn, OTEXT("TestQueueSingleRawMessage"), TRUE, TRUE, FALSE);
        OCI_QueueDrop(conn, OTEXT("TestQueueSingleRawMessage"));
        OCI_QueueTableDrop(conn, OTEXT("TestQueueSingleRawMessageTable"), TRUE);
        // end no check

        ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestQueueSingleRawMessageTable"), OTEXT("RAW"), NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
        ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestQueueSingleRawMessage"), OTEXT("TestQueueSingleRawMessageTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
        ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestQueueSingleRawMessage"), TRUE, TRUE));

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("SYS.RAW"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto enq = OCI_EnqueueCreate(inf, OTEXT("TestQueueSingleRawMessage"));
        ASSERT_NE(nullptr, enq);

        const auto deq = OCI_DequeueCreate(inf, OTEXT("TestQueueSingleRawMessage"));
        ASSERT_NE(nullptr, deq);

        const auto msg_in = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg_in);

        const auto byteBring = std::string("123456789");
        ASSERT_TRUE(OCI_MsgSetRaw(msg_in, static_cast<const void*>(byteBring.data()), static_cast<OCI_UInt32>(byteBring.size())));

        ASSERT_TRUE(OCI_EnqueuePut(enq, msg_in));

        ASSERT_TRUE(OCI_MsgFree(msg_in));

        ASSERT_TRUE(OCI_Commit(conn));

        const auto msg_out = OCI_DequeueGet(deq);
        ASSERT_NE(nullptr, msg_out);

        char buffer[100] = "";
        OCI_UInt32 size = 100;
        ASSERT_TRUE(OCI_MsgGetRaw(msg_out, static_cast<void*>(buffer), &size));

        ASSERT_EQ(byteBring, std::string(buffer));
        ASSERT_EQ(byteBring.size(), size);

        ASSERT_TRUE(OCI_EnqueueFree(enq));
        ASSERT_TRUE(OCI_DequeueFree(deq));

        ASSERT_TRUE(OCI_Commit(conn));

        ASSERT_TRUE(OCI_QueueStop(conn, OTEXT("TestQueueSingleRawMessage"), TRUE, TRUE, FALSE));
        ASSERT_TRUE(OCI_QueueDrop(conn, OTEXT("TestQueueSingleRawMessage")));
        ASSERT_TRUE(OCI_QueueTableDrop(conn, OTEXT("TestQueueSingleRawMessageTable"), TRUE));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestQueue, SingleRawMessageEmptyPayload)
    {
        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        // no check - for cleanup
        OCI_QueueStop(conn, OTEXT("TestQueueSingleRawMessageEmptyPayload"), TRUE, TRUE, FALSE);
        OCI_QueueDrop(conn, OTEXT("TestQueueSingleRawMessageEmptyPayload"));
        OCI_QueueTableDrop(conn, OTEXT("TestQueueSingleRawMessageEmptyPayloadTable"), TRUE);
        // end no check

        ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestQueueSingleRawMessageEmptyPayloadTable"), OTEXT("RAW"), NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
        ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestQueueSingleRawMessageEmptyPayload"), OTEXT("TestQueueSingleRawMessageEmptyPayloadTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
        ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestQueueSingleRawMessageEmptyPayload"), TRUE, TRUE));

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("SYS.RAW"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto enq = OCI_EnqueueCreate(inf, OTEXT("TestQueueSingleRawMessageEmptyPayload"));
        ASSERT_NE(nullptr, enq);

        const auto deq = OCI_DequeueCreate(inf, OTEXT("TestQueueSingleRawMessageEmptyPayload"));
        ASSERT_NE(nullptr, deq);

        const auto msg_in = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg_in);

        const auto byteBring = std::string("123456789");
        ASSERT_TRUE(OCI_MsgSetOriginalID(msg_in, static_cast<const void*>(byteBring.data()), static_cast<OCI_UInt32>(byteBring.size())));

        ASSERT_TRUE(OCI_EnqueuePut(enq, msg_in));

        ASSERT_TRUE(OCI_MsgFree(msg_in));

        ASSERT_TRUE(OCI_Commit(conn));

        const auto msg_out = OCI_DequeueGet(deq);
        ASSERT_NE(nullptr, msg_out);

        char buffer[100] = "";
        OCI_UInt32 size = 100;
        ASSERT_TRUE(OCI_MsgGetOriginalID(msg_out, static_cast<void*>(buffer), &size));

        ASSERT_EQ(byteBring, std::string(buffer));
        ASSERT_EQ(byteBring.size(), size);

        otext payload[100] = OTEXT("");
        OCI_UInt32 payloadSize = 100;
        ASSERT_TRUE(OCI_MsgGetRaw(msg_out, static_cast<void*>(payload), &payloadSize));
        ASSERT_EQ(0, payloadSize);

        ASSERT_TRUE(OCI_EnqueueFree(enq));
        ASSERT_TRUE(OCI_DequeueFree(deq));

        ASSERT_TRUE(OCI_Commit(conn));

        ASSERT_TRUE(OCI_QueueStop(conn, OTEXT("TestQueueSingleRawMessageEmptyPayload"), TRUE, TRUE, FALSE));
        ASSERT_TRUE(OCI_QueueDrop(conn, OTEXT("TestQueueSingleRawMessageEmptyPayload")));
        ASSERT_TRUE(OCI_QueueTableDrop(conn, OTEXT("TestQueueSingleRawMessageEmptyPayloadTable"), TRUE));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestQueue, SingleMessageEmptyPayload)
    {
        ExecDML(OTEXT("create type TestQueueSingleMessageEmptyPayloadType as object(title varchar2(50), content varchar2(100))"));

        auto env = GetEnvironment(nullptr);

        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        // no check - for cleanup
        OCI_QueueStop(conn, OTEXT("TestQueueSingleMessageEmptyPayload"), TRUE, TRUE, FALSE);
        OCI_QueueDrop(conn, OTEXT("TestQueueSingleMessageEmptyPayload"));
        OCI_QueueTableDrop(conn, OTEXT("TestQueueSingleMessageEmptyPayloadTable"), TRUE);
        // end no check

        ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestQueueSingleMessageEmptyPayloadTable"), OTEXT("TestQueueSingleMessageEmptyPayloadType"), NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
        ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestQueueSingleMessageEmptyPayload"), OTEXT("TestQueueSingleMessageEmptyPayloadTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
        ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestQueueSingleMessageEmptyPayload"), TRUE, TRUE));

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("TestQueueSingleMessageEmptyPayloadType"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto enq = OCI_EnqueueCreate(inf, OTEXT("TestQueueSingleMessageEmptyPayload"));
        ASSERT_NE(nullptr, enq);

        const auto deq = OCI_DequeueCreate(inf, OTEXT("TestQueueSingleMessageEmptyPayload"));
        ASSERT_NE(nullptr, deq);

        const auto msg_in = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg_in);

        const auto byteBring = std::string("123456789");
        ASSERT_TRUE(OCI_MsgSetOriginalID(msg_in, static_cast<const void*>(byteBring.data()), static_cast<OCI_UInt32>(byteBring.size())));

        ASSERT_TRUE(OCI_EnqueuePut(enq, msg_in));

        otext msgIDIn[100] = OTEXT("");
        OCI_UInt32 sizeMsgIDIn = 100;
        ASSERT_TRUE(OCI_MsgGetID(msg_in, static_cast<void*>(msgIDIn), &sizeMsgIDIn));

        ASSERT_TRUE(OCI_MsgFree(msg_in));

        ASSERT_TRUE(OCI_Commit(conn));

        const auto msg_out = OCI_DequeueGet(deq);
        ASSERT_NE(nullptr, msg_out);

        char buffer[100] = "";
        OCI_UInt32 sizeBuffer = 100;
        ASSERT_TRUE(OCI_MsgGetOriginalID(msg_out, static_cast<void*>(buffer), &sizeBuffer));

        ASSERT_EQ(byteBring, std::string(buffer));
        ASSERT_EQ(byteBring.size(), sizeBuffer);

        otext msgIDOut[100] = OTEXT("");
        OCI_UInt32 sizeMsgIDOut = 100;
        ASSERT_TRUE(OCI_MsgGetID(msg_out, static_cast<void*>(msgIDOut), &sizeMsgIDOut));

        ASSERT_EQ(String(msgIDIn), String(msgIDOut));
        ASSERT_EQ(sizeMsgIDIn, sizeMsgIDOut);

        const auto obj_out = OCI_MsgGetObject(msg_out);
        ASSERT_EQ(nullptr, obj_out);

        ASSERT_TRUE(OCI_EnqueueFree(enq));
        ASSERT_TRUE(OCI_DequeueFree(deq));

        ASSERT_TRUE(OCI_Commit(conn));

        ASSERT_TRUE(OCI_QueueStop(conn, OTEXT("TestQueueSingleMessageEmptyPayload"), TRUE, TRUE, FALSE));
        ASSERT_TRUE(OCI_QueueDrop(conn, OTEXT("TestQueueSingleMessageEmptyPayload")));
        ASSERT_TRUE(OCI_QueueTableDrop(conn, OTEXT("TestQueueSingleMessageEmptyPayloadTable"), TRUE));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestQueueSingleMessageEmptyPayloadType"));
    }

    class TestQueueTableAlter : public ::testing::Test
    {
    protected:

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            // no check - for cleanup
            OCI_QueueStop(conn, OTEXT("TestQueueAlterMigrate"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestQueueAlterMigrate"));
            OCI_QueueTableDrop(conn, OTEXT("TestQueueAlterMigrateTable"), TRUE);
            // end no check

            /* compatible '8.0' so the table can be migrated to '10.0' */
            ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestQueueAlterMigrateTable"), OTEXT("RAW"), NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, OTEXT("8.0")));
            ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestQueueAlterMigrate"), OTEXT("TestQueueAlterMigrateTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
        }

        void TearDown() override
        {
            OCI_QueueStop(conn, OTEXT("TestQueueAlterMigrate"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestQueueAlterMigrate"));
            OCI_QueueTableDrop(conn, OTEXT("TestQueueAlterMigrateTable"), TRUE);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
    };

    TEST_F(TestQueueTableAlter, QueueTableAlter_ChangesComment)
    {
        ASSERT_TRUE(OCI_QueueTableAlter(conn, OTEXT("TestQueueAlterMigrateTable"), OTEXT("altered comment"), 0, 0));
    }

    TEST_F(TestQueueTableAlter, QueueTableMigrate_Succeeds)
    {
        ASSERT_TRUE(OCI_QueueTableMigrate(conn, OTEXT("TestQueueAlterMigrateTable"), OTEXT("10.0")));
    }

    class TestQueueNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestQueueNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestQueueNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "QueueAlter", [] { return !OCI_QueueAlter(nullptr, OTEXT("x"), 1, 1, 1, OTEXT("x")); } },
            { "QueueCreate", [] { return !OCI_QueueCreate(nullptr, OTEXT("x"), OTEXT("x"), 1, 1, 1, 1, FALSE, OTEXT("x")); } },
            { "QueueDrop", [] { return !OCI_QueueDrop(nullptr, OTEXT("x")); } },
            { "QueueStart", [] { return !OCI_QueueStart(nullptr, OTEXT("x"), FALSE, FALSE); } },
            { "QueueStop", [] { return !OCI_QueueStop(nullptr, OTEXT("x"), FALSE, FALSE, FALSE); } },
            { "QueueTableAlter", [] { return !OCI_QueueTableAlter(nullptr, OTEXT("x"), OTEXT("x"), 1, 1); } },
            { "QueueTableCreate", [] { return !OCI_QueueTableCreate(nullptr, OTEXT("x"), OTEXT("x"), OTEXT("x"), OTEXT("x"), FALSE, 1, OTEXT("x"), 1, 1, OTEXT("x")); } },
            { "QueueTableDrop", [] { return !OCI_QueueTableDrop(nullptr, OTEXT("x"), FALSE); } },
            { "QueueTableMigrate", [] { return !OCI_QueueTableMigrate(nullptr, OTEXT("x"), OTEXT("x")); } },
            { "QueueTablePurge", [] { return !OCI_QueueTablePurge(nullptr, OTEXT("x"), OTEXT("x"), FALSE, 1); } },
        }),
        &NullArgCaseName);

}
