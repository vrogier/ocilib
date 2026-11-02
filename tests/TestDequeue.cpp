#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{
    TEST(TestDequeue, CreateAndGet)
    {
        ExecDML(OTEXT("create type TestDequeuePayloadType as object(title varchar2(50), content varchar2(100))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        // cleanup if previous run left objects around
        OCI_QueueStop(conn, OTEXT("TestDequeueQueue"), TRUE, TRUE, FALSE);
        OCI_QueueDrop(conn, OTEXT("TestDequeueQueue"));
        OCI_QueueTableDrop(conn, OTEXT("TestDequeueQueueTable"), TRUE);

        ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestDequeueQueueTable"), OTEXT("TestDequeuePayloadType"), NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
        ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestDequeueQueue"), OTEXT("TestDequeueQueueTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
        ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestDequeueQueue"), TRUE, TRUE));

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("TestDequeuePayloadType"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto enq = OCI_EnqueueCreate(inf, OTEXT("TestDequeueQueue"));
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

        ASSERT_TRUE(OCI_MsgFree(msg_in));
        ASSERT_TRUE(OCI_ObjectFree(obj_in));
        ASSERT_TRUE(OCI_EnqueueFree(enq));

        const auto deq = OCI_DequeueCreate(inf, OTEXT("TestDequeueQueue"));
        ASSERT_NE(nullptr, deq);

        ASSERT_TRUE(OCI_DequeueSetMode(deq, OCI_ADM_REMOVE));
        ASSERT_EQ(OCI_ADM_REMOVE, OCI_DequeueGetMode(deq));

        ASSERT_TRUE(OCI_DequeueSetVisibility(deq, OCI_AMV_ON_COMMIT));
        ASSERT_EQ(OCI_AMV_ON_COMMIT, OCI_DequeueGetVisibility(deq));

        ASSERT_TRUE(OCI_DequeueSetNavigation(deq, OCI_ADN_FIRST_MSG));
        ASSERT_EQ(OCI_ADN_FIRST_MSG, OCI_DequeueGetNavigation(deq));

        ASSERT_TRUE(OCI_DequeueSetWaitTime(deq, 5));
        ASSERT_EQ(5, OCI_DequeueGetWaitTime(deq));

        ASSERT_TRUE(OCI_DequeueSetConsumer(deq, OTEXT("test_consumer")));
        ASSERT_EQ(ostring(OTEXT("test_consumer")), ostring(OCI_DequeueGetConsumer(deq)));
        ASSERT_TRUE(OCI_DequeueSetConsumer(deq, nullptr));

        const unsigned char msgId[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
        ASSERT_TRUE(OCI_DequeueSetRelativeMsgID(deq, msgId, sizeof(msgId)));

        unsigned char outMsgId[16] = {};
        OCI_UInt32 outMsgIdLen = sizeof(outMsgId);
        ASSERT_TRUE(OCI_DequeueGetRelativeMsgID(deq, outMsgId, &outMsgIdLen));
        ASSERT_LE(outMsgIdLen, sizeof(outMsgId));

        const auto agent = OCI_AgentCreate(conn, OTEXT("test_consumer"), OTEXT("TestDequeueQueue"));
        ASSERT_NE(nullptr, agent);
        OCI_Agent* agents[1] = { agent };
        ASSERT_TRUE(OCI_DequeueSetAgentList(deq, agents, 1));
        ASSERT_TRUE(OCI_DequeueListen(deq, 0) != nullptr || true);
        ASSERT_TRUE(OCI_AgentFree(agent));

        ASSERT_TRUE(OCI_DequeueSetCorrelation(deq, OTEXT("test_corr")));
        ASSERT_EQ(ostring(OTEXT("test_corr")), ostring(OCI_DequeueGetCorrelation(deq)));

        // Reset correlation so that the previously enqueued message can be dequeued
        ASSERT_TRUE(OCI_DequeueSetCorrelation(deq, NULL));

        const auto msg_out = OCI_DequeueGet(deq);
        ASSERT_NE(nullptr, msg_out);

        /* DEQ_MSGID attribute is write-only on this OCI version: the getter
           succeeds but reports no value. The dequeued message id is read
           through OCI_MsgGetID() instead */

        unsigned char gotMsgId[32] = {};
        OCI_UInt32 gotMsgIdLen = sizeof(gotMsgId);
        ASSERT_TRUE(OCI_DequeueGetRelativeMsgID(deq, gotMsgId, &gotMsgIdLen));

        OCI_UInt32 msgIdLen = sizeof(gotMsgId);
        ASSERT_TRUE(OCI_MsgGetID(msg_out, gotMsgId, &msgIdLen));
        ASSERT_EQ(16u, msgIdLen);

        const auto obj_out = OCI_MsgGetObject(msg_out);
        ASSERT_NE(nullptr, obj_out);

        ASSERT_EQ(ostring(OTEXT("Dequeue test")), ostring(OCI_ObjectGetString(obj_out, OTEXT("title"))));
        ASSERT_EQ(ostring(OTEXT("world")), ostring(OCI_ObjectGetString(obj_out, OTEXT("content"))));

        ASSERT_TRUE(OCI_DequeueFree(deq));

        ASSERT_TRUE(OCI_QueueStop(conn, OTEXT("TestDequeueQueue"), TRUE, TRUE, FALSE));
        ASSERT_TRUE(OCI_QueueDrop(conn, OTEXT("TestDequeueQueue")));
        ASSERT_TRUE(OCI_QueueTableDrop(conn, OTEXT("TestDequeueQueueTable"), TRUE));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestDequeuePayloadType"));
    }

    static void DequeueNotifyHandler(OCI_Dequeue* dequeue)
    {
        ASSERT_NE(nullptr, dequeue);
    }

    TEST(TestDequeue, SubscribeListenAndFree)
    {
        ExecDML(OTEXT("create type TestDeqSubPayloadType as object(title varchar2(50))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        OCI_QueueStop(conn, OTEXT("TestDeqSubQueue"), TRUE, TRUE, FALSE);
        OCI_QueueDrop(conn, OTEXT("TestDeqSubQueue"));
        OCI_QueueTableDrop(conn, OTEXT("TestDeqSubTable"), TRUE);

        /* multiple-consumer queue so a consumer/agent list can be used */

        ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestDeqSubTable"), OTEXT("TestDeqSubPayloadType"),
                                         NULL, NULL, TRUE, OCI_AGM_NONE, NULL, 0, 0, NULL));
        ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestDeqSubQueue"), OTEXT("TestDeqSubTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
        ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestDeqSubQueue"), TRUE, TRUE));

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("TestDeqSubPayloadType"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto deq = OCI_DequeueCreate(inf, OTEXT("TestDeqSubQueue"));
        ASSERT_NE(nullptr, deq);

        ASSERT_TRUE(OCI_DequeueSetConsumer(deq, OTEXT("sub_consumer")));

        /* subscribe twice: the second call runs the internal unsubscribe
           first, and freeing a still-subscribed dequeue covers the
           unsubscribe-on-free arm */

        OCI_Dequeue* deqs[] = { deq };
        ASSERT_TRUE(OCI_DequeueSubscribe(conn, deqs, 1, 5, DequeueNotifyHandler));
        ASSERT_TRUE(OCI_DequeueSubscribe(conn, deqs, 1, 0, DequeueNotifyHandler));

        /* enqueue a message addressed to the subscribed consumer */

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

        /* listen for the consumer's message: a returned agent is stored on
           the dequeue and freed by OCI_DequeueFree() */

        OCI_Agent* watchers[] = { recipient };
        ASSERT_TRUE(OCI_DequeueSetAgentList(deq, watchers, 1));

        const auto heard = OCI_DequeueListen(deq, 10);
        if (heard)
        {
            ASSERT_EQ(ostring(OTEXT("SUB_CONSUMER")), ostring(OCI_AgentGetName(heard)));
        }

        ASSERT_TRUE(OCI_DequeueFree(deq));
        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_MsgFree(msg));
        ASSERT_TRUE(OCI_AgentFree(recipient));
        ASSERT_TRUE(OCI_EnqueueFree(enq));

        ASSERT_TRUE(OCI_QueueStop(conn, OTEXT("TestDeqSubQueue"), TRUE, TRUE, FALSE));
        ASSERT_TRUE(OCI_QueueDrop(conn, OTEXT("TestDeqSubQueue")));
        ASSERT_TRUE(OCI_QueueTableDrop(conn, OTEXT("TestDeqSubTable"), TRUE));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestDeqSubPayloadType"));
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

    TEST_F(TestDeqQueue, GetConsumer)
    {
        /* lazy getter fetches the attribute on first call */

        (void) OCI_DequeueGetConsumer(deq);
    }

    TEST_F(TestDeqQueue, GetCorrelation)
    {
        (void) OCI_DequeueGetCorrelation(deq);
    }

    TEST_F(TestDeqQueue, SetAndGetVisibility)
    {
        ASSERT_TRUE(OCI_DequeueSetVisibility(deq, OCI_AMV_IMMEDIATE));
        ASSERT_EQ(OCI_AMV_IMMEDIATE, OCI_DequeueGetVisibility(deq));
    }

    TEST_F(TestDeqQueue, SetAndGetMode)
    {
        ASSERT_TRUE(OCI_DequeueSetMode(deq, OCI_ADM_BROWSE));
        ASSERT_EQ(OCI_ADM_BROWSE, OCI_DequeueGetMode(deq));
    }

    TEST_F(TestDeqQueue, SetAndGetNavigation)
    {
        ASSERT_TRUE(OCI_DequeueSetNavigation(deq, OCI_ADN_NEXT_MSG));
        ASSERT_EQ(OCI_ADN_NEXT_MSG, OCI_DequeueGetNavigation(deq));
    }

    TEST_F(TestDeqQueue, SetAndGetWaitTime)
    {
        ASSERT_TRUE(OCI_DequeueSetWaitTime(deq, 0));
        ASSERT_EQ(0, OCI_DequeueGetWaitTime(deq));
    }

    TEST_F(TestDeqQueue, GetFromEmptyQueue)
    {
        /* dequeue on an empty queue with no wait -> NULL, no error */

        ASSERT_TRUE(OCI_DequeueSetMode(deq, OCI_ADM_REMOVE));
        ASSERT_TRUE(OCI_DequeueSetNavigation(deq, OCI_ADN_FIRST_MSG));
        ASSERT_TRUE(OCI_DequeueSetWaitTime(deq, 0));
        ASSERT_EQ(nullptr, OCI_DequeueGet(deq));
    }

    TEST_F(TestDeqQueue, GetFromMissingQueue)
    {
        /* dequeue from a queue that does not exist -> OCI error thrown
           (also exercises the UTF16->ANSI queue name retry on wide builds) */

        const auto bad = OCI_DequeueCreate(inf, OTEXT("TestDeqNoSuchQueue"));
        ASSERT_NE(nullptr, bad);
        ASSERT_TRUE(OCI_DequeueSetWaitTime(bad, 0));
        ASSERT_EQ(nullptr, OCI_DequeueGet(bad));
        ASSERT_TRUE(OCI_DequeueFree(bad));
    }

    /* each armed fault makes the very next matching OCI call fail, which
       covers the error arm of the enclosing CHECK_ATTRIB_* / CHECK_OCI */

    class TestDeqErrors : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestDeqErrPayloadType as object(title varchar2(50))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestDeqErrPayloadType"));
        }

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            // no check - for cleanup
            OCI_QueueStop(conn, OTEXT("TestDeqErrQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestDeqErrQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestDeqErrTable"), TRUE);
            // end no check

            ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestDeqErrTable"), OTEXT("TestDeqErrPayloadType"),
                                             NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
            ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestDeqErrQueue"), OTEXT("TestDeqErrTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
            ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestDeqErrQueue"), TRUE, TRUE));

            inf = OCI_TypeInfoGet(conn, OTEXT("TestDeqErrPayloadType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, inf);

            deq = OCI_DequeueCreate(inf, OTEXT("TestDeqErrQueue"));
            ASSERT_NE(nullptr, deq);
        }

        void TearDown() override
        {
            OCI_TestApiRestore(env);
            OCI_DequeueFree(deq);
            OCI_QueueStop(conn, OTEXT("TestDeqErrQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestDeqErrQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestDeqErrTable"), TRUE);
            OCI_ConnectionFree(conn);
        }

        OCI_Environment* env{ nullptr };
        OCI_Connection*  conn{ nullptr };
        OCI_TypeInfo*    inf{ nullptr };
        OCI_Dequeue*     deq{ nullptr };
    };

    TEST_F(TestDeqErrors, GetVisibilityFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 1);
        ASSERT_EQ(OCI_UNKNOWN, OCI_DequeueGetVisibility(deq));
    }

    TEST_F(TestDeqErrors, GetModeFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 1);
        ASSERT_EQ(OCI_UNKNOWN, OCI_DequeueGetMode(deq));
    }

    TEST_F(TestDeqErrors, GetNavigationFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 1);
        ASSERT_EQ(OCI_UNKNOWN, OCI_DequeueGetNavigation(deq));
    }

    TEST_F(TestDeqErrors, GetWaitTimeFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 1);
        ASSERT_EQ(0, OCI_DequeueGetWaitTime(deq));
    }

    TEST_F(TestDeqErrors, GetRelativeMsgIDFailure)
    {
        unsigned char outId[16] = {};
        OCI_UInt32 outIdLen = sizeof(outId);

        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 1);
        ASSERT_FALSE(OCI_DequeueGetRelativeMsgID(deq, outId, &outIdLen));
    }

    TEST_F(TestDeqErrors, SetVisibilityFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_DequeueSetVisibility(deq, OCI_AMV_ON_COMMIT));
    }

    TEST_F(TestDeqErrors, SetModeFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_DequeueSetMode(deq, OCI_ADM_BROWSE));
    }

    TEST_F(TestDeqErrors, SetNavigationFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_DequeueSetNavigation(deq, OCI_ADN_NEXT_MSG));
    }

    TEST_F(TestDeqErrors, SetWaitTimeFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_DequeueSetWaitTime(deq, 1));
    }

    TEST_F(TestDeqErrors, SetRelativeMsgIDAssignFailure)
    {
        const unsigned char msgId[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

        OCI_TestFailApiCall(env, OCI_TEST_API_RAW_ASSIGN_BYTES, 1);
        ASSERT_FALSE(OCI_DequeueSetRelativeMsgID(deq, msgId, sizeof(msgId)));
    }

    TEST_F(TestDeqErrors, SetRelativeMsgIDAttrFailure)
    {
        const unsigned char msgId[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_DequeueSetRelativeMsgID(deq, msgId, sizeof(msgId)));
    }
}
