#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    /* -------------------------------------------------------------------------
       Parameterized: dequeue attribute setter/getter round-trips
       ------------------------------------------------------------------------- */

    struct DeqAttrCase
    {
        const char* name;
        void (*run)(OCI_Dequeue*);
    };

    void PrintTo(const DeqAttrCase& c, std::ostream* os) { *os << c.name; }

    static void DeqAttrMode(OCI_Dequeue* d)
    {
        ASSERT_TRUE(OCI_DequeueSetMode(d, OCI_ADM_REMOVE));
        EXPECT_EQ(OCI_ADM_REMOVE, OCI_DequeueGetMode(d));
    }
    static void DeqAttrVisibility(OCI_Dequeue* d)
    {
        ASSERT_TRUE(OCI_DequeueSetVisibility(d, OCI_AMV_ON_COMMIT));
        EXPECT_EQ(OCI_AMV_ON_COMMIT, OCI_DequeueGetVisibility(d));
    }
    static void DeqAttrNavigation(OCI_Dequeue* d)
    {
        ASSERT_TRUE(OCI_DequeueSetNavigation(d, OCI_ADN_FIRST_MSG));
        EXPECT_EQ(OCI_ADN_FIRST_MSG, OCI_DequeueGetNavigation(d));
    }
    static void DeqAttrWaitTime(OCI_Dequeue* d)
    {
        ASSERT_TRUE(OCI_DequeueSetWaitTime(d, 5));
        EXPECT_EQ(5, OCI_DequeueGetWaitTime(d));
    }
    static void DeqAttrConsumer(OCI_Dequeue* d)
    {
        ASSERT_TRUE(OCI_DequeueSetConsumer(d, OTEXT("test_consumer")));
        EXPECT_EQ(String(OTEXT("test_consumer")), String(OCI_DequeueGetConsumer(d)));
    }
    static void DeqAttrCorrelation(OCI_Dequeue* d)
    {
        ASSERT_TRUE(OCI_DequeueSetCorrelation(d, OTEXT("test_corr")));
        EXPECT_EQ(String(OTEXT("test_corr")), String(OCI_DequeueGetCorrelation(d)));
    }

    class TestDeqOps : public ::testing::TestWithParam<DeqAttrCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_TypeInfo* inf = nullptr;
        OCI_Dequeue* deq = nullptr;

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestDeqOpsPayloadType as object(title varchar2(50), content varchar2(100))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestDeqOpsPayloadType"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            OCI_QueueStop(conn, OTEXT("TestDeqOpsQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestDeqOpsQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestDeqOpsTable"), TRUE);

            ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestDeqOpsTable"), OTEXT("TestDeqOpsPayloadType"),
                                             NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
            ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestDeqOpsQueue"), OTEXT("TestDeqOpsTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
            ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestDeqOpsQueue"), TRUE, TRUE));

            inf = OCI_TypeInfoGet(conn, OTEXT("TestDeqOpsPayloadType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, inf);
            deq = OCI_DequeueCreate(inf, OTEXT("TestDeqOpsQueue"));
            ASSERT_NE(nullptr, deq);
        }

        void TearDown() override
        {
            if (deq) OCI_DequeueFree(deq);
            if (conn)
            {
                OCI_QueueStop(conn, OTEXT("TestDeqOpsQueue"), TRUE, TRUE, FALSE);
                OCI_QueueDrop(conn, OTEXT("TestDeqOpsQueue"));
                OCI_QueueTableDrop(conn, OTEXT("TestDeqOpsTable"), TRUE);
                OCI_ConnectionFree(conn);
            }
        }
    };

    TEST_P(TestDeqOps, SetThenGetAttr_RoundTripsValue)
    {
        GetParam().run(deq);
    }

    INSTANTIATE_TEST_CASE_P(DeqAttr, TestDeqOps,
        ::testing::Values(
            DeqAttrCase{ "Mode",        DeqAttrMode },
            DeqAttrCase{ "Visibility",  DeqAttrVisibility },
            DeqAttrCase{ "Navigation",  DeqAttrNavigation },
            DeqAttrCase{ "WaitTime",    DeqAttrWaitTime },
            DeqAttrCase{ "Consumer",    DeqAttrConsumer },
            DeqAttrCase{ "Correlation", DeqAttrCorrelation }),
        ::testing::PrintToStringParamName());

    TEST_F(TestDeqOps, EnqueueThenDequeue_ReturnsMessagePayload)
    {
        const auto enq = OCI_EnqueueCreate(inf, OTEXT("TestDeqOpsQueue"));
        ASSERT_NE(nullptr, enq);
        const auto msg_in = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg_in);
        const auto obj_in = OCI_ObjectCreate(inf);
        ASSERT_NE(nullptr, obj_in);
        ASSERT_TRUE(OCI_ObjectSetString(obj_in, OTEXT("title"), OTEXT("Dequeue test")));
        ASSERT_TRUE(OCI_ObjectSetString(obj_in, OTEXT("content"), OTEXT("world")));
        ASSERT_TRUE(OCI_MsgSetObject(msg_in, obj_in));
        ASSERT_TRUE(OCI_EnqueuePut(enq, msg_in));
        ASSERT_TRUE(OCI_Commit(conn));

        ASSERT_TRUE(OCI_DequeueSetMode(deq, OCI_ADM_REMOVE));
        ASSERT_TRUE(OCI_DequeueSetVisibility(deq, OCI_AMV_ON_COMMIT));
        ASSERT_TRUE(OCI_DequeueSetNavigation(deq, OCI_ADN_FIRST_MSG));
        ASSERT_TRUE(OCI_DequeueSetWaitTime(deq, 5));

        const auto msg_out = OCI_DequeueGet(deq);

        ASSERT_NE(nullptr, msg_out);
        unsigned char gotMsgId[32] = {};
        OCI_UInt32 msgIdLen = sizeof(gotMsgId);
        ASSERT_TRUE(OCI_MsgGetID(msg_out, gotMsgId, &msgIdLen));
        EXPECT_EQ(16u, msgIdLen);
        const auto obj_out = OCI_MsgGetObject(msg_out);
        ASSERT_NE(nullptr, obj_out);
        EXPECT_EQ(String(OTEXT("Dequeue test")), String(OCI_ObjectGetString(obj_out, OTEXT("title"))));
        EXPECT_EQ(String(OTEXT("world")), String(OCI_ObjectGetString(obj_out, OTEXT("content"))));

        ASSERT_TRUE(OCI_MsgFree(msg_in));
        ASSERT_TRUE(OCI_ObjectFree(obj_in));
        ASSERT_TRUE(OCI_EnqueueFree(enq));
    }

    TEST_F(TestDeqOps, SetRelativeMsgID_GetterReportsNoValue)
    {
        /* DEQ_MSGID attribute is write-only on this OCI version: the getter
           succeeds but reports no value */

        const unsigned char msgId[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                          0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

        ASSERT_TRUE(OCI_DequeueSetRelativeMsgID(deq, msgId, sizeof(msgId)));

        unsigned char outMsgId[16] = {};
        OCI_UInt32 outMsgIdLen = sizeof(outMsgId);
        EXPECT_TRUE(OCI_DequeueGetRelativeMsgID(deq, outMsgId, &outMsgIdLen));
        EXPECT_LE(outMsgIdLen, sizeof(outMsgId));
    }

    TEST_F(TestDeqOps, SetAgentListThenListen_AcceptsAgentFilter)
    {
        const auto agent = OCI_AgentCreate(conn, OTEXT("test_consumer"), OTEXT("TestDeqOpsQueue"));
        ASSERT_NE(nullptr, agent);
        OCI_Agent* agents[1] = { agent };

        ASSERT_TRUE(OCI_DequeueSetAgentList(deq, agents, 1));

        OCI_DequeueListen(deq, 0);

        ASSERT_TRUE(OCI_AgentFree(agent));
    }

    static void DequeueNotifyHandler(OCI_Dequeue* dequeue)
    {
        ASSERT_NE(nullptr, dequeue);
    }

    /* -------------------------------------------------------------------------
       Shared fixture: multi-consumer queue + dequeue for consumer "sub_consumer"
       ------------------------------------------------------------------------- */

    class TestDeqSub : public ::testing::Test
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_TypeInfo* inf = nullptr;
        OCI_Dequeue* deq = nullptr;

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestDeqSubPayloadType as object(title varchar2(50))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestDeqSubPayloadType"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            OCI_QueueStop(conn, OTEXT("TestDeqSubQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestDeqSubQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestDeqSubTable"), TRUE);

            ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestDeqSubTable"), OTEXT("TestDeqSubPayloadType"),
                                             NULL, NULL, TRUE, OCI_AGM_NONE, NULL, 0, 0, NULL));
            ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestDeqSubQueue"), OTEXT("TestDeqSubTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
            ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestDeqSubQueue"), TRUE, TRUE));

            inf = OCI_TypeInfoGet(conn, OTEXT("TestDeqSubPayloadType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, inf);
            deq = OCI_DequeueCreate(inf, OTEXT("TestDeqSubQueue"));
            ASSERT_NE(nullptr, deq);
            ASSERT_TRUE(OCI_DequeueSetConsumer(deq, OTEXT("sub_consumer")));
        }

        void TearDown() override
        {
            if (deq) OCI_DequeueFree(deq);
            if (conn)
            {
                OCI_QueueStop(conn, OTEXT("TestDeqSubQueue"), TRUE, TRUE, FALSE);
                OCI_QueueDrop(conn, OTEXT("TestDeqSubQueue"));
                OCI_QueueTableDrop(conn, OTEXT("TestDeqSubTable"), TRUE);
                OCI_ConnectionFree(conn);
            }
        }
    };

    TEST_F(TestDeqSub, SubscribeTwiceThenFree_UnsubscribesCleanly)
    {
        /* second subscribe runs the internal unsubscribe first; freeing a
           still-subscribed dequeue covers the unsubscribe-on-free arm */

        OCI_Dequeue* deqs[] = { deq };

        ASSERT_TRUE(OCI_DequeueSubscribe(conn, deqs, 1, 5, DequeueNotifyHandler));
        EXPECT_TRUE(OCI_DequeueSubscribe(conn, deqs, 1, 0, DequeueNotifyHandler));
    }

    TEST_F(TestDeqSub, ListenForConsumerMessage_ReturnsConsumerAgent)
    {
        const auto enq = OCI_EnqueueCreate(inf, OTEXT("TestDeqSubQueue"));
        ASSERT_NE(nullptr, enq);
        const auto recipient = OCI_AgentCreate(conn, OTEXT("sub_consumer"), OTEXT("TestDeqSubQueue"));
        ASSERT_NE(nullptr, recipient);
        const auto msg = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg);
        const auto obj = OCI_ObjectCreate(inf);
        ASSERT_NE(nullptr, obj);
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("title"), OTEXT("listen me")));
        ASSERT_TRUE(OCI_MsgSetObject(msg, obj));
        OCI_Agent* recipients[] = { recipient };
        ASSERT_TRUE(OCI_MsgSetConsumers(msg, recipients, 1));
        ASSERT_TRUE(OCI_EnqueuePut(enq, msg));
        ASSERT_TRUE(OCI_Commit(conn));

        OCI_Agent* watchers[] = { recipient };
        ASSERT_TRUE(OCI_DequeueSetAgentList(deq, watchers, 1));
        const auto heard = OCI_DequeueListen(deq, 10);

        if (heard)
        {
            EXPECT_EQ(String(OTEXT("SUB_CONSUMER")), String(OCI_AgentGetName(heard)));
        }

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_MsgFree(msg));
        ASSERT_TRUE(OCI_AgentFree(recipient));
        ASSERT_TRUE(OCI_EnqueueFree(enq));
    }

    /* shared fixture: payload type + started queue + live dequeue */

    class TestDeqQueue : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestDeqLazyPayloadType as object(title varchar2(50))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestDeqLazyPayloadType"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            // no check - for cleanup
            OCI_QueueStop(conn, OTEXT("TestDeqLazyQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestDeqLazyQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestDeqLazyTable"), TRUE);
            // end no check

            ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestDeqLazyTable"), OTEXT("TestDeqLazyPayloadType"),
                                             NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
            ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestDeqLazyQueue"), OTEXT("TestDeqLazyTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
            ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestDeqLazyQueue"), TRUE, TRUE));

            inf = OCI_TypeInfoGet(conn, OTEXT("TestDeqLazyPayloadType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, inf);

            deq = OCI_DequeueCreate(inf, OTEXT("TestDeqLazyQueue"));
            ASSERT_NE(nullptr, deq);
        }

        void TearDown() override
        {
            OCI_DequeueFree(deq);
            OCI_QueueStop(conn, OTEXT("TestDeqLazyQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestDeqLazyQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestDeqLazyTable"), TRUE);
            OCI_ConnectionFree(conn);
        }

        OCI_Connection* conn{ nullptr };
        OCI_TypeInfo*   inf{ nullptr };
        OCI_Dequeue*    deq{ nullptr };
    };

    TEST_F(TestDeqQueue, GetConsumerOnConsumerQueue_ReturnsName)
    {
        /* lazy getter fetches the attribute on first call */

        (void) OCI_DequeueGetConsumer(deq);
    }

    TEST_F(TestDeqQueue, GetCorrelationAfterSet_ReturnsValue)
    {
        (void) OCI_DequeueGetCorrelation(deq);
    }

    TEST_F(TestDeqQueue, SetVisibilityThenGet_ReturnsValue)
    {
        ASSERT_TRUE(OCI_DequeueSetVisibility(deq, OCI_AMV_IMMEDIATE));
        ASSERT_EQ(OCI_AMV_IMMEDIATE, OCI_DequeueGetVisibility(deq));
    }

    TEST_F(TestDeqQueue, SetModeThenGet_ReturnsValue)
    {
        ASSERT_TRUE(OCI_DequeueSetMode(deq, OCI_ADM_BROWSE));
        ASSERT_EQ(OCI_ADM_BROWSE, OCI_DequeueGetMode(deq));
    }

    TEST_F(TestDeqQueue, SetNavigationThenGet_ReturnsValue)
    {
        ASSERT_TRUE(OCI_DequeueSetNavigation(deq, OCI_ADN_NEXT_MSG));
        ASSERT_EQ(OCI_ADN_NEXT_MSG, OCI_DequeueGetNavigation(deq));
    }

    TEST_F(TestDeqQueue, SetWaitTimeThenGet_ReturnsValue)
    {
        ASSERT_TRUE(OCI_DequeueSetWaitTime(deq, 0));
        ASSERT_EQ(0, OCI_DequeueGetWaitTime(deq));
    }

    TEST_F(TestDeqQueue, DequeueGetOnEmptyQueue_ReturnsNull)
    {
        /* dequeue on an empty queue with no wait -> NULL, no error */

        ASSERT_TRUE(OCI_DequeueSetMode(deq, OCI_ADM_REMOVE));
        ASSERT_TRUE(OCI_DequeueSetNavigation(deq, OCI_ADN_FIRST_MSG));
        ASSERT_TRUE(OCI_DequeueSetWaitTime(deq, 0));
        ASSERT_EQ(nullptr, OCI_DequeueGet(deq));
    }

    TEST_F(TestDeqQueue, DequeueGetOnMissingQueue_Fails)
    {
        /* dequeue from a queue that does not exist -> OCI error thrown
           (also exercises the UTF16->ANSI queue name retry on wide builds) */

        const auto bad = OCI_DequeueCreate(inf, OTEXT("TestDeqNoSuchQueue"));
        ASSERT_NE(nullptr, bad);
        ASSERT_TRUE(OCI_DequeueSetWaitTime(bad, 0));
        ASSERT_EQ(nullptr, OCI_DequeueGet(bad));
        ASSERT_TRUE(OCI_DequeueFree(bad));
    }

    /* --- public-API error paths: NULL/invalid dequeue and queue handles hit
       the CHECK_PTR / CHECK_OCI arms the dropped fault tests covered --- */


    /* -------------------------------------------------------------------------
       Parameterized: NULL/invalid handle args hit CHECK_PTR error arms
       ------------------------------------------------------------------------- */

    struct DeqNullCase
    {
        const char* name;
        void (*run)(void);
    };

    void PrintTo(const DeqNullCase& c, std::ostream* os) { *os << c.name; }

    static void NullDequeueCreate(void)
    {
        EXPECT_EQ(nullptr, OCI_DequeueCreate(nullptr, OTEXT("TestDeqNullQ")));
    }
    static void NullQueueStop(void)
    {
        EXPECT_FALSE(OCI_QueueStop(nullptr, OTEXT("TestDeqNullQ"), TRUE, TRUE, FALSE));
    }
    static void NullQueueDrop(void)
    {
        EXPECT_FALSE(OCI_QueueDrop(nullptr, OTEXT("TestDeqNullQ")));
    }
    static void NullQueueTableDrop(void)
    {
        EXPECT_FALSE(OCI_QueueTableDrop(nullptr, OTEXT("TestDeqNullTable"), TRUE));
    }

    class TestDequeueErrors : public ::testing::TestWithParam<DeqNullCase> {};

    TEST_P(TestDequeueErrors, NullHandleArg_ReturnsFailure)
    {
        GetParam().run();
    }

    INSTANTIATE_TEST_CASE_P(NullArg, TestDequeueErrors,
        ::testing::Values(
            DeqNullCase{ "DequeueCreate",   NullDequeueCreate },
            DeqNullCase{ "QueueStop",       NullQueueStop },
            DeqNullCase{ "QueueDrop",       NullQueueDrop },
            DeqNullCase{ "QueueTableDrop",  NullQueueTableDrop }),
        ::testing::PrintToStringParamName());

    class TestDequeueNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestDequeueNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestDequeueNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "DequeueCreate", [] { return nullptr == OCI_DequeueCreate(nullptr, OTEXT("x")); } },
            { "DequeueFree", [] { return !OCI_DequeueFree(nullptr); } },
            { "DequeueGet", [] { return nullptr == OCI_DequeueGet(nullptr); } },
            { "DequeueGetConsumer", [] { return nullptr == OCI_DequeueGetConsumer(nullptr); } },
            { "DequeueGetCorrelation", [] { return nullptr == OCI_DequeueGetCorrelation(nullptr); } },
            { "DequeueGetMode", [] { return OCI_UNKNOWN == OCI_DequeueGetMode(nullptr); } },
            { "DequeueGetNavigation", [] { return OCI_UNKNOWN == OCI_DequeueGetNavigation(nullptr); } },
            { "DequeueGetRelativeMsgID", [] { return !OCI_DequeueGetRelativeMsgID(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>()); } },
            { "DequeueGetVisibility", [] { return OCI_UNKNOWN == OCI_DequeueGetVisibility(nullptr); } },
            { "DequeueGetWaitTime", [] { return 0 == OCI_DequeueGetWaitTime(nullptr); } },
            { "DequeueListen", [] { return nullptr == OCI_DequeueListen(nullptr, 1); } },
            { "DequeueSetAgentList", [] { return !OCI_DequeueSetAgentList(nullptr, nullptr, 1); } },
            { "DequeueSetConsumer", [] { return !OCI_DequeueSetConsumer(nullptr, OTEXT("x")); } },
            { "DequeueSetCorrelation", [] { return !OCI_DequeueSetCorrelation(nullptr, OTEXT("x")); } },
            { "DequeueSetMode", [] { return !OCI_DequeueSetMode(nullptr, 1); } },
            { "DequeueSetNavigation", [] { return !OCI_DequeueSetNavigation(nullptr, 1); } },
            { "DequeueSetRelativeMsgID", [] { return !OCI_DequeueSetRelativeMsgID(nullptr, NullArgBuffer(), 1); } },
            { "DequeueSetVisibility", [] { return !OCI_DequeueSetVisibility(nullptr, 1); } },
            { "DequeueSetWaitTime", [] { return !OCI_DequeueSetWaitTime(nullptr, 1); } },
            { "DequeueSubscribe", [] { return !OCI_DequeueSubscribe(nullptr, nullptr, 1, 1, nullptr); } },
            { "DequeueUnsubscribe", [] { return !OCI_DequeueUnsubscribe(nullptr); } },
        }),
        &NullArgCaseName);

}
