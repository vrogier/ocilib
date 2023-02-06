#include <atomic>

#include "ocilib_tests.h"


static std::atomic<int> MessageWithConsumersCount {0};

TEST(TestQueue, SingleMessage)
{
    ExecDML(OTEXT("create type TestQueueSingleMessageType as object(title varchar2(50), content varchar2(100))"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    // no check - for cleanup
    OCI_QueueStop(conn, OTEXT("TestQueueSingleMessage"), TRUE, TRUE, FALSE);
    OCI_QueueDrop(conn, OTEXT("TestQueueSingleMessage"));
    OCI_QueueTableDrop(conn, OTEXT("TestQueueSingleMessageTable"), TRUE);
    // end no check

    ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestQueueSingleMessageTable"), OTEXT("TestQueueSingleMessageType"), NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
    ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestQueueSingleMessage"), OTEXT("TestQueueSingleMessageTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
    ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestQueueSingleMessage"), TRUE, TRUE));

    const auto inf = OCI_TypeInfoGet(conn, OTEXT("TestQueueSingleMessageType"), OCI_TIF_TYPE);
    ASSERT_NE(nullptr, inf);

    const auto enq = OCI_EnqueueCreate(inf, OTEXT("TestQueueSingleMessage"));
    ASSERT_NE(nullptr, enq);

    const auto deq = OCI_DequeueCreate(inf, OTEXT("TestQueueSingleMessage"));
    ASSERT_NE(nullptr, deq);

    const auto msg_in = OCI_MsgCreate(inf);
    ASSERT_NE(nullptr, msg_in);

    const auto obj_in = OCI_ObjectCreate(conn, inf);
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

    ASSERT_EQ(ostring(OTEXT("NEXT MEETING")), ostring(OCI_ObjectGetString(obj_out, OTEXT("title"))));
    ASSERT_EQ(ostring(OTEXT("12:00 PM IN STARBUCKS")), ostring(OCI_ObjectGetString(obj_out, OTEXT("content"))));

    ASSERT_TRUE(OCI_EnqueueFree(enq));
    ASSERT_TRUE(OCI_DequeueFree(deq));

    ASSERT_TRUE(OCI_QueueStop(conn, OTEXT("TestQueueSingleMessage"), TRUE, TRUE, FALSE));
    ASSERT_TRUE(OCI_QueueDrop(conn, OTEXT("TestQueueSingleMessage")));
    ASSERT_TRUE(OCI_QueueTableDrop(conn, OTEXT("TestQueueSingleMessageTable"), TRUE));

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop type TestQueueSingleMessageType"));
}

static void OnMessage(OCI_Dequeue* deq)
{
    ++MessageWithConsumersCount;
    const auto msg_out = OCI_DequeueGet(deq);
    ASSERT_NE(nullptr, msg_out);

    const auto obj_out = OCI_MsgGetObject(msg_out);
    ASSERT_NE(nullptr, obj_out);

    ASSERT_EQ(ostring(OTEXT("NEXT MEETING")), ostring(OCI_ObjectGetString(obj_out, OTEXT("title"))));
    ASSERT_EQ(ostring(OTEXT("12:00 PM IN STARBUCKS")), ostring(OCI_ObjectGetString(obj_out, OTEXT("content"))));
}

TEST(TestQueue, MessageWithConsumers)
{
    ExecDML(OTEXT("create type TestQueueMessageWithConsumersType as object(title varchar2(50), content varchar2(100))"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT | OCI_ENV_EVENTS));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
    ASSERT_NE(nullptr, conn);

    // no check - for cleanup
    OCI_QueueStop(conn, OTEXT("TestQueueMessageWithConsumers"), TRUE, TRUE, FALSE);
    OCI_QueueDrop(conn, OTEXT("TestQueueMessageWithConsumers"));
    OCI_QueueTableDrop(conn, OTEXT("TestQueueMessageWithConsumersTable"), TRUE);
    // end no check

    ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestQueueMessageWithConsumersTable"), OTEXT("TestQueueMessageWithConsumersType"), NULL, NULL, TRUE, OCI_AGM_NONE, NULL, 0, 0, NULL));
    ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestQueueMessageWithConsumers"), OTEXT("TestQueueMessageWithConsumersTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
    ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestQueueMessageWithConsumers"), TRUE, TRUE));

    const auto inf = OCI_TypeInfoGet(conn, OTEXT("TestQueueMessageWithConsumersType"), OCI_TIF_TYPE);
    ASSERT_NE(nullptr, inf);

    const auto enq = OCI_EnqueueCreate(inf, OTEXT("TestQueueMessageWithConsumers"));
    ASSERT_NE(nullptr, enq);

    const auto deq1 = OCI_DequeueCreate(inf, OTEXT("TestQueueMessageWithConsumers"));
    ASSERT_NE(nullptr, deq1);

    const auto deq2 = OCI_DequeueCreate(inf, OTEXT("TestQueueMessageWithConsumers"));
    ASSERT_NE(nullptr, deq2);

    OCI_Agent* agents[2] {};

    agents[0] = OCI_AgentCreate(conn, OTEXT("C1"), nullptr);
    ASSERT_NE(nullptr, agents[0]);
    agents[1] = OCI_AgentCreate(conn, OTEXT("C2"), nullptr);
    ASSERT_NE(nullptr, agents[1]);

    ASSERT_TRUE(OCI_DequeueSetConsumer(deq1, OTEXT("C1")));
    ASSERT_TRUE(OCI_DequeueSetNavigation(deq1, OCI_ADN_FIRST_MSG));
    ASSERT_TRUE(OCI_DequeueSetConsumer(deq2, OTEXT("C2")));
    ASSERT_TRUE(OCI_DequeueSetNavigation(deq2, OCI_ADN_FIRST_MSG));

    ASSERT_TRUE(OCI_DequeueSubscribe(deq1, 9998, 0, OnMessage));
    ASSERT_TRUE(OCI_DequeueSubscribe(deq2, 9999, 0, OnMessage));

    const auto msg_in = OCI_MsgCreate(inf);
    ASSERT_NE(nullptr, msg_in);

    const auto obj_in = OCI_ObjectCreate(conn, inf);
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

    const int MaxRetries = 15;
    int retry = 1;

    while (MessageWithConsumersCount < 2 && retry <= MaxRetries)
    {
        Sleep(100);
        retry++;
    }

    ASSERT_EQ(MessageWithConsumersCount, 2);

    ASSERT_TRUE(OCI_DequeueUnsubscribe(deq1));
    ASSERT_TRUE(OCI_DequeueUnsubscribe(deq2));

    ASSERT_TRUE(OCI_AgentFree(agents[0]));
    ASSERT_TRUE(OCI_AgentFree(agents[1]));

    ASSERT_TRUE(OCI_EnqueueFree(enq));
    ASSERT_TRUE(OCI_DequeueFree(deq1));
    ASSERT_TRUE(OCI_DequeueFree(deq2));

    ASSERT_TRUE(OCI_QueueStop(conn, OTEXT("TestQueueMessageWithConsumers"), TRUE, TRUE, FALSE));
    ASSERT_TRUE(OCI_QueueDrop(conn, OTEXT("TestQueueMessageWithConsumers")));
    ASSERT_TRUE(OCI_QueueTableDrop(conn, OTEXT("TestQueueMessageWithConsumersTable"), TRUE));

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop type TestQueueMessageWithConsumersType"));
}

TEST(TestQueue, SingleRawMessage)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
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

    const auto byteBring = ostring(OTEXT("123456789"));
    ASSERT_TRUE(OCI_MsgSetRaw(msg_in, static_cast<const void *>(byteBring.data()), static_cast<unsigned int>(byteBring.size())));

    ASSERT_TRUE(OCI_EnqueuePut(enq, msg_in));

    ASSERT_TRUE(OCI_MsgFree(msg_in));

    ASSERT_TRUE(OCI_Commit(conn));

    const auto msg_out = OCI_DequeueGet(deq);
    ASSERT_NE(nullptr, msg_out);

    otext buffer[100] = OTEXT("");
    unsigned int size = 100;
    ASSERT_TRUE(OCI_MsgGetRaw(msg_out, static_cast<void*>(buffer), &size));

    ASSERT_EQ(byteBring, ostring(buffer));
    ASSERT_EQ(byteBring.size(), size);

    ASSERT_TRUE(OCI_EnqueueFree(enq));
    ASSERT_TRUE(OCI_DequeueFree(deq));

    ASSERT_TRUE(OCI_Commit(conn));

    ASSERT_TRUE(OCI_QueueStop(conn, OTEXT("TestQueueSingleRawMessage"), TRUE, TRUE, FALSE));
    ASSERT_TRUE(OCI_QueueDrop(conn, OTEXT("TestQueueSingleRawMessage")));
    ASSERT_TRUE(OCI_QueueTableDrop(conn, OTEXT("TestQueueSingleRawMessageTable"), TRUE));

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}

TEST(TestQueue, SingleRawMessageEmptyPayload)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
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

    const auto byteBring = ostring(OTEXT("123456789"));
    ASSERT_TRUE(OCI_MsgSetOriginalID(msg_in, static_cast<const void*>(byteBring.data()), static_cast<unsigned int>(byteBring.size())));

    ASSERT_TRUE(OCI_EnqueuePut(enq, msg_in));

    ASSERT_TRUE(OCI_MsgFree(msg_in));

    ASSERT_TRUE(OCI_Commit(conn));

    const auto msg_out = OCI_DequeueGet(deq);
    ASSERT_NE(nullptr, msg_out);

    otext buffer[100] = OTEXT("");
    unsigned int size = 100;
    ASSERT_TRUE(OCI_MsgGetOriginalID(msg_out, static_cast<void*>(buffer), &size));

    ASSERT_EQ(byteBring, ostring(buffer));
    ASSERT_EQ(byteBring.size(), size);

    otext payload[100] = OTEXT("");
    unsigned int payloadSize = 100;
    ASSERT_TRUE(OCI_MsgGetRaw(msg_out, static_cast<void*>(payload), &payloadSize));
    ASSERT_EQ(0, payloadSize);

    ASSERT_TRUE(OCI_EnqueueFree(enq));
    ASSERT_TRUE(OCI_DequeueFree(deq));

    ASSERT_TRUE(OCI_Commit(conn));

    ASSERT_TRUE(OCI_QueueStop(conn, OTEXT("TestQueueSingleRawMessageEmptyPayload"), TRUE, TRUE, FALSE));
    ASSERT_TRUE(OCI_QueueDrop(conn, OTEXT("TestQueueSingleRawMessageEmptyPayload")));
    ASSERT_TRUE(OCI_QueueTableDrop(conn, OTEXT("TestQueueSingleRawMessageEmptyPayloadTable"), TRUE));

    ASSERT_TRUE(OCI_ConnectionFree(conn));
    ASSERT_TRUE(OCI_Cleanup());
}


TEST(TestQueue, SingleMessageEmptyPayload)
{
    ExecDML(OTEXT("create type TestQueueSingleMessageEmptyPayloadType as object(title varchar2(50), content varchar2(100))"));

    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT));

    const auto conn = OCI_ConnectionCreate(DBS, USR, PWD, OCI_SESSION_DEFAULT);
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

    const auto byteBring = ostring(OTEXT("123456789"));
    ASSERT_TRUE(OCI_MsgSetOriginalID(msg_in, static_cast<const void*>(byteBring.data()), static_cast<unsigned int>(byteBring.size())));

    ASSERT_TRUE(OCI_EnqueuePut(enq, msg_in));

    otext msgIDIn[100] = OTEXT("");
    unsigned int sizeMsgIDIn = 100;
    ASSERT_TRUE(OCI_MsgGetID(msg_in, static_cast<void*>(msgIDIn), &sizeMsgIDIn));

    ASSERT_TRUE(OCI_MsgFree(msg_in));

    ASSERT_TRUE(OCI_Commit(conn));

    const auto msg_out = OCI_DequeueGet(deq);
    ASSERT_NE(nullptr, msg_out);

    otext buffer[100] = OTEXT("");
    unsigned int sizeBuffer = 100;
    ASSERT_TRUE(OCI_MsgGetOriginalID(msg_out, static_cast<void*>(buffer), &sizeBuffer));

    ASSERT_EQ(byteBring, ostring(buffer));
    ASSERT_EQ(byteBring.size(), sizeBuffer);

    otext msgIDOut[100] = OTEXT("");
    unsigned int sizeMsgIDOut = 100;
    ASSERT_TRUE(OCI_MsgGetID(msg_out, static_cast<void*>(msgIDOut), &sizeMsgIDOut));

    ASSERT_EQ(ostring(msgIDIn), ostring(msgIDOut));
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
    ASSERT_TRUE(OCI_Cleanup());

    ExecDML(OTEXT("drop type TestQueueSingleMessageEmptyPayloadType"));
}