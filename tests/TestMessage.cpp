#include <cstring>

#include "ocilib_tests.h"
#include "NullArgs.h"

namespace TestCApi
{
    TEST(TestMessage, CreateAndFree)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("SYS.RAW"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto msg = OCI_MessageCreate(inf);
        ASSERT_NE(nullptr, msg);

        ASSERT_TRUE(OCI_MessageFree(msg));
        ASSERT_TRUE(OCI_TypeInfoFree(inf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestMessage, SetGetRaw)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("SYS.RAW"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto msg = OCI_MessageCreate(inf);
        ASSERT_NE(nullptr, msg);

        const char payload[] = "Hello, message!";
        ASSERT_TRUE(OCI_MessageSetRaw(msg, static_cast<const void*>(payload), static_cast<OCI_UInt32>(sizeof(payload) - 1)));

        char buffer[64] = {};
        OCI_UInt32 size = sizeof(buffer);
        ASSERT_TRUE(OCI_MessageGetRaw(msg, static_cast<void*>(buffer), &size));
        ASSERT_EQ(static_cast<OCI_UInt32>(sizeof(payload) - 1), size);
        ASSERT_EQ(std::string(payload), std::string(buffer));

        ASSERT_TRUE(OCI_MessageFree(msg));
        ASSERT_TRUE(OCI_TypeInfoFree(inf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestMessage, SetGetProperties)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("SYS.RAW"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto msg = OCI_MessageCreate(inf);
        ASSERT_NE(nullptr, msg);

        ASSERT_TRUE(OCI_MessageSetPriority(msg, 7));
        ASSERT_EQ(7, OCI_MessageGetPriority(msg));

        ASSERT_TRUE(OCI_MessageSetEnqueueDelay(msg, 10));
        ASSERT_EQ(10, OCI_MessageGetEnqueueDelay(msg));

        ASSERT_TRUE(OCI_MessageSetExpiration(msg, 60));
        ASSERT_EQ(60, OCI_MessageGetExpiration(msg));

        ASSERT_TRUE(OCI_MessageSetCorrelation(msg, OTEXT("TestCorrelation")));
        ASSERT_EQ(String(OTEXT("TestCorrelation")), String(OCI_MessageGetCorrelation(msg)));

        ASSERT_TRUE(OCI_MessageSetExceptionQueue(msg, OTEXT("TestExceptionQueue")));
        ASSERT_EQ(String(OTEXT("TestExceptionQueue")), String(OCI_MessageGetExceptionQueue(msg)));

        ASSERT_TRUE(OCI_MessageReset(msg));
        ASSERT_TRUE(OCI_MessageFree(msg));
        ASSERT_TRUE(OCI_TypeInfoFree(inf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestMessage, Reset)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("SYS.RAW"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto msg = OCI_MessageCreate(inf);
        ASSERT_NE(nullptr, msg);

        const char payload[] = "payload";
        ASSERT_TRUE(OCI_MessageSetRaw(msg, static_cast<const void*>(payload), static_cast<OCI_UInt32>(sizeof(payload) - 1)));
        ASSERT_TRUE(OCI_MessageSetPriority(msg, 5));
        ASSERT_TRUE(OCI_MessageSetCorrelation(msg, OTEXT("correlation")));

        ASSERT_TRUE(OCI_MessageReset(msg));

        ASSERT_EQ(0, OCI_MessageGetPriority(msg));

        char buffer[64] = {};
        OCI_UInt32 size = sizeof(buffer);
        ASSERT_TRUE(OCI_MessageGetRaw(msg, static_cast<void*>(buffer), &size));
        ASSERT_EQ(0u, size);

        ASSERT_TRUE(OCI_MessageFree(msg));
        ASSERT_TRUE(OCI_TypeInfoFree(inf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestMessage, OriginalID)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("SYS.RAW"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto msg = OCI_MessageCreate(inf);
        ASSERT_NE(nullptr, msg);

        const unsigned char originalID[] = { 0x01, 0x02, 0x03, 0x04 };
        ASSERT_TRUE(OCI_MessageSetOriginalID(msg, static_cast<const void*>(originalID), static_cast<OCI_UInt32>(sizeof(originalID))));

        unsigned char buffer[16] = {};
        OCI_UInt32 size = sizeof(buffer);
        ASSERT_TRUE(OCI_MessageGetOriginalID(msg, static_cast<void*>(buffer), &size));
        ASSERT_EQ(static_cast<OCI_UInt32>(sizeof(originalID)), size);
        ASSERT_EQ(0, std::memcmp(originalID, buffer, sizeof(originalID)));

        ASSERT_TRUE(OCI_MessageFree(msg));
        ASSERT_TRUE(OCI_TypeInfoFree(inf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    TEST(TestMessage, Sender)
    {
        auto env = GetEnvironment(nullptr);
        ASSERT_NE(nullptr, env);

        const auto conn = OCI_ConnectionCreate(env, DBS, USR, PWD, OCI_SESSION_DEFAULT);
        ASSERT_NE(nullptr, conn);

        const auto inf = OCI_TypeInfoGet(conn, OTEXT("SYS.RAW"), OCI_TIF_TYPE);
        ASSERT_NE(nullptr, inf);

        const auto msg = OCI_MessageCreate(inf);
        ASSERT_NE(nullptr, msg);

        const auto sender = OCI_AgentCreate(conn, OTEXT("TestSender"), nullptr);
        ASSERT_NE(nullptr, sender);

        ASSERT_TRUE(OCI_MessageSetSender(msg, sender));
        ASSERT_EQ(String(OTEXT("TestSender")), String(OCI_AgentGetName(OCI_MessageGetSender(msg))));

        ASSERT_TRUE(OCI_AgentFree(sender));
        ASSERT_TRUE(OCI_MessageFree(msg));
        ASSERT_TRUE(OCI_TypeInfoFree(inf));
        ASSERT_TRUE(OCI_ConnectionFree(conn));

    }

    class TestMessageNullArgs : public ::testing::TestWithParam<NullArgCase> {};

    TEST_P(TestMessageNullArgs, CalledWithNullHandle_ReturnsFailure)
    {
        RunNullArgCase(GetParam());
    }

    INSTANTIATE_TEST_CASE_P(NullArgs, TestMessageNullArgs,
        ::testing::ValuesIn(NullArgCases
        {
            { "MsgCreate", [] { return nullptr == OCI_MessageCreate(nullptr); } },
            { "MsgFree", [] { return !OCI_MessageFree(nullptr); } },
            { "MsgGetAttemptCount", [] { return 0 == OCI_MessageGetAttemptCount(nullptr); } },
            { "MsgGetCorrelation", [] { return nullptr == OCI_MessageGetCorrelation(nullptr); } },
            { "MsgGetEnqueueDelay", [] { return 0 == OCI_MessageGetEnqueueDelay(nullptr); } },
            { "MsgGetEnqueueTime", [] { return nullptr == OCI_MessageGetEnqueueTime(nullptr); } },
            { "MsgGetExceptionQueue", [] { return nullptr == OCI_MessageGetExceptionQueue(nullptr); } },
            { "MsgGetExpiration", [] { return 0 == OCI_MessageGetExpiration(nullptr); } },
            { "MsgGetID", [] { return !OCI_MessageGetID(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>()); } },
            { "MsgGetObject", [] { return nullptr == OCI_MessageGetObject(nullptr); } },
            { "MsgGetOriginalID", [] { return !OCI_MessageGetOriginalID(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>()); } },
            { "MsgGetPriority", [] { return 0 == OCI_MessageGetPriority(nullptr); } },
            { "MsgGetRaw", [] { return !OCI_MessageGetRaw(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>()); } },
            { "MsgGetSender", [] { return nullptr == OCI_MessageGetSender(nullptr); } },
            { "MsgGetState", [] { return OCI_UNKNOWN == OCI_MessageGetState(nullptr); } },
            { "MsgReset", [] { return !OCI_MessageReset(nullptr); } },
            { "MsgSetConsumers", [] { return !OCI_MessageSetConsumers(nullptr, nullptr, 1); } },
            { "MsgSetCorrelation", [] { return !OCI_MessageSetCorrelation(nullptr, OTEXT("x")); } },
            { "MsgSetEnqueueDelay", [] { return !OCI_MessageSetEnqueueDelay(nullptr, 1); } },
            { "MsgSetExceptionQueue", [] { return !OCI_MessageSetExceptionQueue(nullptr, OTEXT("x")); } },
            { "MsgSetExpiration", [] { return !OCI_MessageSetExpiration(nullptr, 1); } },
            { "MsgSetObject", [] { return !OCI_MessageSetObject(nullptr, nullptr); } },
            { "MsgSetOriginalID", [] { return !OCI_MessageSetOriginalID(nullptr, NullArgBuffer(), 1); } },
            { "MsgSetPriority", [] { return !OCI_MessageSetPriority(nullptr, 1); } },
            { "MsgSetRaw", [] { return !OCI_MessageSetRaw(nullptr, NullArgBuffer(), 1); } },
            { "MsgSetSender", [] { return !OCI_MessageSetSender(nullptr, nullptr); } },
        }),
        &NullArgCaseName);

}