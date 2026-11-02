#include "ocilib_tests.h"
#include "test_internals.h"

namespace TestCApi
{
    TEST(TestEnqueue, CreateAndPut)
    {
        ExecDML(OTEXT("create type TestEnqueuePayloadType as object(title varchar2(50), content varchar2(100))"));

        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        // cleanup if previous run left objects around
        OCI_QueueStop(conn, OTEXT("TestEnqueueQueue"), TRUE, TRUE, FALSE);
        OCI_QueueDrop(conn, OTEXT("TestEnqueueQueue"));
        OCI_QueueTableDrop(conn, OTEXT("TestEnqueueQueueTable"), TRUE);

        ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestEnqueueQueueTable"), OTEXT("TestEnqueuePayloadType"), NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
        ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestEnqueueQueue"), OTEXT("TestEnqueueQueueTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
        ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestEnqueueQueue"), TRUE, TRUE));

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("TestEnqueuePayloadType"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto enq = OCI_EnqueueCreate(inf, OTEXT("TestEnqueueQueue"));
        ASSERT_NE(nullptr, enq);

        ASSERT_TRUE(OCI_EnqueueSetVisibility(enq, OCI_AMV_IMMEDIATE));
        ASSERT_EQ(OCI_AMV_IMMEDIATE, OCI_EnqueueGetVisibility(enq));

        ASSERT_TRUE(OCI_EnqueueSetSequenceDeviation(enq, OCI_ASD_TOP));
        ASSERT_EQ(OCI_ASD_TOP, OCI_EnqueueGetSequenceDeviation(enq));

        const unsigned char msgId[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
        ASSERT_TRUE(OCI_EnqueueSetRelativeMsgID(enq, msgId, sizeof(msgId)));

        unsigned char outMsgId[16] = {};
        OCI_UInt32 outMsgIdLen = sizeof(outMsgId);
        ASSERT_TRUE(OCI_EnqueueGetRelativeMsgID(enq, outMsgId, &outMsgIdLen));
        ASSERT_LE(outMsgIdLen, sizeof(outMsgId));

        const auto msg = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg);

        const auto obj = OCI_ObjectCreate(inf);
        ASSERT_NE(nullptr, obj);

        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("title"), OTEXT("Enqueue test")));
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("content"), OTEXT("hello")));
        ASSERT_TRUE(OCI_MsgSetObject(msg, obj));

        ASSERT_TRUE(OCI_EnqueuePut(enq, msg));

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_MsgFree(msg));

        ASSERT_TRUE(OCI_Commit(conn));

        ASSERT_TRUE(OCI_EnqueueFree(enq));

        ASSERT_TRUE(OCI_QueueStop(conn, OTEXT("TestEnqueueQueue"), TRUE, TRUE, FALSE));
        ASSERT_TRUE(OCI_QueueDrop(conn, OTEXT("TestEnqueueQueue")));
        ASSERT_TRUE(OCI_QueueTableDrop(conn, OTEXT("TestEnqueueQueueTable"), TRUE));

        ASSERT_TRUE(OCI_ConnectionFree(conn));

        ExecDML(OTEXT("drop type TestEnqueuePayloadType"));
    }

    /* shared fixture: payload type + started queue + live enqueue */

    class TestEnqQueue : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestEnqRelPayloadType as object(title varchar2(50))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestEnqRelPayloadType"));
        }

        void SetUp() override
        {
            const auto env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            // no check - for cleanup
            OCI_QueueStop(conn, OTEXT("TestEnqRelQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestEnqRelQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestEnqRelTable"), TRUE);
            // end no check

            ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestEnqRelTable"), OTEXT("TestEnqRelPayloadType"),
                                             NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
            ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestEnqRelQueue"), OTEXT("TestEnqRelTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
            ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestEnqRelQueue"), TRUE, TRUE));

            inf = OCI_TypeInfoGet(conn, OTEXT("TestEnqRelPayloadType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, inf);

            enq = OCI_EnqueueCreate(inf, OTEXT("TestEnqRelQueue"));
            ASSERT_NE(nullptr, enq);
        }

        void TearDown() override
        {
            OCI_EnqueueFree(enq);
            OCI_QueueStop(conn, OTEXT("TestEnqRelQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestEnqRelQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestEnqRelTable"), TRUE);
            OCI_ConnectionFree(conn);
        }

        OCI_Msg* CreateMsg()
        {
            const auto msg = OCI_MsgCreate(inf);
            EXPECT_NE(nullptr, msg);

            const auto obj = OCI_ObjectCreate(inf);
            EXPECT_NE(nullptr, obj);
            EXPECT_TRUE(OCI_ObjectSetString(obj, OTEXT("title"), OTEXT("rel id")));
            EXPECT_TRUE(OCI_MsgSetObject(msg, obj));
            EXPECT_TRUE(OCI_ObjectFree(obj));

            return msg;
        }

        OCI_Connection* conn{ nullptr };
        OCI_TypeInfo*   inf{ nullptr };
        OCI_Enqueue*    enq{ nullptr };
    };

    TEST_F(TestEnqQueue, GetRelativeMsgIdAfterPut)
    {
        /* after a successful enqueue the relative msg id attribute is set;
           a buffer larger than the raw id covers the length clamp arm */

        const auto msg = CreateMsg();
        ASSERT_NE(nullptr, msg);

        ASSERT_TRUE(OCI_EnqueuePut(enq, msg));
        ASSERT_TRUE(OCI_Commit(conn));

        unsigned char outMsgId[32] = {};
        OCI_UInt32 outMsgIdLen = sizeof(outMsgId);
        ASSERT_TRUE(OCI_EnqueueGetRelativeMsgID(enq, outMsgId, &outMsgIdLen));
        ASSERT_LE(outMsgIdLen, sizeof(outMsgId));

        ASSERT_TRUE(OCI_MsgFree(msg));
    }

    TEST_F(TestEnqQueue, PutToMissingQueue)
    {
        /* enqueue to a queue that does not exist -> OCI error thrown
           (also exercises the UTF16->ANSI queue name retry on wide builds) */

        const auto bad = OCI_EnqueueCreate(inf, OTEXT("TestEnqNoSuchQueue"));
        ASSERT_NE(nullptr, bad);

        const auto msg = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg);
        ASSERT_FALSE(OCI_EnqueuePut(bad, msg));
        ASSERT_NE(nullptr, OCI_GetLastError());

        ASSERT_TRUE(OCI_MsgFree(msg));
        ASSERT_TRUE(OCI_EnqueueFree(bad));
    }

    /* each armed fault makes the very next matching OCI call fail, which
       covers the error arm of the enclosing CHECK_ATTRIB_* / CHECK_OCI */

    class TestEnqErrors : public ::testing::Test
    {
    protected:

        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestEnqErrPayloadType as object(title varchar2(50))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestEnqErrPayloadType"));
        }

        void SetUp() override
        {
            env = GetEnvironment(nullptr);
            ASSERT_NE(nullptr, env);

            conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            // no check - for cleanup
            OCI_QueueStop(conn, OTEXT("TestEnqErrQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestEnqErrQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestEnqErrTable"), TRUE);
            // end no check

            ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestEnqErrTable"), OTEXT("TestEnqErrPayloadType"),
                                             NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
            ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestEnqErrQueue"), OTEXT("TestEnqErrTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
            ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestEnqErrQueue"), TRUE, TRUE));

            inf = OCI_TypeInfoGet(conn, OTEXT("TestEnqErrPayloadType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, inf);

            enq = OCI_EnqueueCreate(inf, OTEXT("TestEnqErrQueue"));
            ASSERT_NE(nullptr, enq);
        }

        void TearDown() override
        {
            OCI_TestApiRestore(env);
            OCI_EnqueueFree(enq);
            OCI_QueueStop(conn, OTEXT("TestEnqErrQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestEnqErrQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestEnqErrTable"), TRUE);
            OCI_ConnectionFree(conn);
        }

        OCI_Environment* env{ nullptr };
        OCI_Connection*  conn{ nullptr };
        OCI_TypeInfo*    inf{ nullptr };
        OCI_Enqueue*     enq{ nullptr };
    };

    TEST_F(TestEnqErrors, GetVisibilityFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 1);
        ASSERT_EQ(OCI_UNKNOWN, OCI_EnqueueGetVisibility(enq));
    }

    TEST_F(TestEnqErrors, GetSequenceDeviationFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 1);
        ASSERT_EQ(OCI_UNKNOWN, OCI_EnqueueGetSequenceDeviation(enq));
    }

    TEST_F(TestEnqErrors, GetRelativeMsgIDFailure)
    {
        unsigned char outId[16] = {};
        OCI_UInt32 outIdLen = sizeof(outId);

        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_GET, 1);
        ASSERT_FALSE(OCI_EnqueueGetRelativeMsgID(enq, outId, &outIdLen));
    }

    TEST_F(TestEnqErrors, SetVisibilityFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_EnqueueSetVisibility(enq, OCI_AMV_ON_COMMIT));
    }

    TEST_F(TestEnqErrors, SetSequenceDeviationFailure)
    {
        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_EnqueueSetSequenceDeviation(enq, OCI_ASD_TOP));
    }

    TEST_F(TestEnqErrors, SetRelativeMsgIDAssignFailure)
    {
        const unsigned char msgId[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

        OCI_TestFailApiCall(env, OCI_TEST_API_RAW_ASSIGN_BYTES, 1);
        ASSERT_FALSE(OCI_EnqueueSetRelativeMsgID(enq, msgId, sizeof(msgId)));
    }

    TEST_F(TestEnqErrors, SetRelativeMsgIDAttrFailure)
    {
        const unsigned char msgId[16] = { 1, 2, 3, 4, 5, 6, 7, 8, 9, 10, 11, 12, 13, 14, 15, 16 };

        OCI_TestFailApiCall(env, OCI_TEST_API_ATTR_SET, 1);
        ASSERT_FALSE(OCI_EnqueueSetRelativeMsgID(enq, msgId, sizeof(msgId)));
    }
}
