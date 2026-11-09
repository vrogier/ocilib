#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    /* -------------------------------------------------------------------------
       Parameterized: enqueue attribute setter/getter round-trips
       ------------------------------------------------------------------------- */

    struct EnqAttrCase
    {
        const char* name;
        void (*run)(OCI_Enqueue*);
    };

    void PrintTo(const EnqAttrCase& c, std::ostream* os) { *os << c.name; }

    static void EnqAttrVisibility(OCI_Enqueue* e)
    {
        ASSERT_TRUE(OCI_EnqueueSetVisibility(e, OCI_AMV_IMMEDIATE));
        EXPECT_EQ(OCI_AMV_IMMEDIATE, OCI_EnqueueGetVisibility(e));
    }
    static void EnqAttrSeqDev(OCI_Enqueue* e)
    {
        ASSERT_TRUE(OCI_EnqueueSetSequenceDeviation(e, OCI_ASD_TOP));
        EXPECT_EQ(OCI_ASD_TOP, OCI_EnqueueGetSequenceDeviation(e));
    }

    class TestEnqOps : public ::testing::TestWithParam<EnqAttrCase>
    {
    protected:
        OCI_Connection* conn = nullptr;
        OCI_TypeInfo* inf = nullptr;
        OCI_Enqueue* enq = nullptr;

    public:
        static void SetUpTestCase()
        {
            ExecDML(OTEXT("create type TestEnqOpsPayloadType as object(title varchar2(50), content varchar2(100))"));
        }

        static void TearDownTestCase()
        {
            ExecDML(OTEXT("drop type TestEnqOpsPayloadType"));
        }

    protected:
        void SetUp() override
        {
            conn = OCI_ConnectionCreate(GetEnvironment(nullptr), DBS, USR, PWD, OCI_SESSION_DEFAULT);
            ASSERT_NE(nullptr, conn);

            OCI_QueueStop(conn, OTEXT("TestEnqOpsQueue"), TRUE, TRUE, FALSE);
            OCI_QueueDrop(conn, OTEXT("TestEnqOpsQueue"));
            OCI_QueueTableDrop(conn, OTEXT("TestEnqOpsTable"), TRUE);

            ASSERT_TRUE(OCI_QueueTableCreate(conn, OTEXT("TestEnqOpsTable"), OTEXT("TestEnqOpsPayloadType"),
                                             NULL, NULL, FALSE, OCI_AGM_NONE, NULL, 0, 0, NULL));
            ASSERT_TRUE(OCI_QueueCreate(conn, OTEXT("TestEnqOpsQueue"), OTEXT("TestEnqOpsTable"), OCI_AQT_NORMAL, 0, 0, 0, FALSE, NULL));
            ASSERT_TRUE(OCI_QueueStart(conn, OTEXT("TestEnqOpsQueue"), TRUE, TRUE));

            inf = OCI_TypeInfoGet(conn, OTEXT("TestEnqOpsPayloadType"), OCI_TIF_TYPE);
            ASSERT_NE(nullptr, inf);
            enq = OCI_EnqueueCreate(inf, OTEXT("TestEnqOpsQueue"));
            ASSERT_NE(nullptr, enq);
        }

        void TearDown() override
        {
            if (enq) OCI_EnqueueFree(enq);
            if (conn)
            {
                OCI_QueueStop(conn, OTEXT("TestEnqOpsQueue"), TRUE, TRUE, FALSE);
                OCI_QueueDrop(conn, OTEXT("TestEnqOpsQueue"));
                OCI_QueueTableDrop(conn, OTEXT("TestEnqOpsTable"), TRUE);
                OCI_ConnectionFree(conn);
            }
        }
    };

    TEST_P(TestEnqOps, SetThenGetAttr_RoundTripsValue)
    {
        GetParam().run(enq);
    }

    INSTANTIATE_TEST_CASE_P(EnqAttr, TestEnqOps,
        ::testing::Values(
            EnqAttrCase{ "Visibility",        EnqAttrVisibility },
            EnqAttrCase{ "SequenceDeviation", EnqAttrSeqDev }),
        ::testing::PrintToStringParamName());

    TEST_F(TestEnqOps, SetRelativeMsgID_GetterReportsValue)
    {
        const unsigned char msgId[16] = { 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08,
                                          0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };

        ASSERT_TRUE(OCI_EnqueueSetRelativeMsgID(enq, msgId, sizeof(msgId)));

        unsigned char outMsgId[16] = {};
        OCI_UInt32 outMsgIdLen = sizeof(outMsgId);
        EXPECT_TRUE(OCI_EnqueueGetRelativeMsgID(enq, outMsgId, &outMsgIdLen));
        EXPECT_LE(outMsgIdLen, sizeof(outMsgId));
    }

    TEST_F(TestEnqOps, EnqueuePutThenCommit_EnqueuesMessage)
    {
        const auto msg = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg);
        const auto obj = OCI_ObjectCreate(inf);
        ASSERT_NE(nullptr, obj);
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("title"), OTEXT("Enqueue test")));
        ASSERT_TRUE(OCI_ObjectSetString(obj, OTEXT("content"), OTEXT("hello")));
        ASSERT_TRUE(OCI_MsgSetObject(msg, obj));

        ASSERT_TRUE(OCI_EnqueuePut(enq, msg));

        EXPECT_TRUE(OCI_Commit(conn));

        ASSERT_TRUE(OCI_ObjectFree(obj));
        ASSERT_TRUE(OCI_MsgFree(msg));
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

    TEST_F(TestEnqQueue, GetRelativeMsgIdAfterPut_ReturnsId)
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

    TEST_F(TestEnqQueue, EnqueuePutOnMissingQueue_Fails)
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

    /* --- public-API error paths: NULL/invalid enqueue and msg handles hit
       the CHECK_PTR / CHECK_OCI arms the dropped fault tests covered --- */


    TEST(TestEnqueue, NullTypeInfoAndMsgErrors)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        /* NULL typeinfo -> OCI_EnqueueCreate / OCI_MsgCreate fail */
        ASSERT_EQ(nullptr, OCI_EnqueueCreate(nullptr, OTEXT("TestEnqNullQ")));
        ASSERT_EQ(nullptr, OCI_MsgCreate(nullptr));
        ASSERT_FALSE(OCI_MsgFree(nullptr));

        ASSERT_TRUE(OCI_ConnectionFree(conn));
    }

    class TestEnqueueNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestEnqueueNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestEnqueueNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "EnqueueCreate", [] { return nullptr == OCI_EnqueueCreate(nullptr, OTEXT("x")); } },
            { "EnqueueFree", [] { return !OCI_EnqueueFree(nullptr); } },
            { "EnqueueGetRelativeMsgID", [] { return !OCI_EnqueueGetRelativeMsgID(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>()); } },
            { "EnqueueGetSequenceDeviation", [] { return OCI_UNKNOWN == OCI_EnqueueGetSequenceDeviation(nullptr); } },
            { "EnqueueGetVisibility", [] { return OCI_UNKNOWN == OCI_EnqueueGetVisibility(nullptr); } },
            { "EnqueuePut", [] { return !OCI_EnqueuePut(nullptr, nullptr); } },
            { "EnqueueSetRelativeMsgID", [] { return !OCI_EnqueueSetRelativeMsgID(nullptr, NullArgBuffer(), 1); } },
            { "EnqueueSetSequenceDeviation", [] { return !OCI_EnqueueSetSequenceDeviation(nullptr, 1); } },
            { "EnqueueSetVisibility", [] { return !OCI_EnqueueSetVisibility(nullptr, 1); } },
        }),
        &NullArgCaseName);

}
