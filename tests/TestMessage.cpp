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

        const auto msg = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg);

        ASSERT_TRUE(OCI_MsgFree(msg));
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

        const auto msg = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg);

        const char payload[] = "Hello, message!";
        ASSERT_TRUE(OCI_MsgSetRaw(msg, static_cast<const void*>(payload), static_cast<OCI_UInt32>(sizeof(payload) - 1)));

        char buffer[64] = {};
        OCI_UInt32 size = sizeof(buffer);
        ASSERT_TRUE(OCI_MsgGetRaw(msg, static_cast<void*>(buffer), &size));
        ASSERT_EQ(static_cast<OCI_UInt32>(sizeof(payload) - 1), size);
        ASSERT_EQ(std::string(payload), std::string(buffer));

        ASSERT_TRUE(OCI_MsgFree(msg));
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

        const auto msg = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg);

        ASSERT_TRUE(OCI_MsgSetPriority(msg, 7));
        ASSERT_EQ(7, OCI_MsgGetPriority(msg));

        ASSERT_TRUE(OCI_MsgSetEnqueueDelay(msg, 10));
        ASSERT_EQ(10, OCI_MsgGetEnqueueDelay(msg));

        ASSERT_TRUE(OCI_MsgSetExpiration(msg, 60));
        ASSERT_EQ(60, OCI_MsgGetExpiration(msg));

        ASSERT_TRUE(OCI_MsgSetCorrelation(msg, OTEXT("TestCorrelation")));
        ASSERT_EQ(String(OTEXT("TestCorrelation")), String(OCI_MsgGetCorrelation(msg)));

        ASSERT_TRUE(OCI_MsgSetExceptionQueue(msg, OTEXT("TestExceptionQueue")));
        ASSERT_EQ(String(OTEXT("TestExceptionQueue")), String(OCI_MsgGetExceptionQueue(msg)));

        ASSERT_TRUE(OCI_MsgReset(msg));
        ASSERT_TRUE(OCI_MsgFree(msg));
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

        const auto msg = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg);

        const char payload[] = "payload";
        ASSERT_TRUE(OCI_MsgSetRaw(msg, static_cast<const void*>(payload), static_cast<OCI_UInt32>(sizeof(payload) - 1)));
        ASSERT_TRUE(OCI_MsgSetPriority(msg, 5));
        ASSERT_TRUE(OCI_MsgSetCorrelation(msg, OTEXT("correlation")));

        ASSERT_TRUE(OCI_MsgReset(msg));

        ASSERT_EQ(0, OCI_MsgGetPriority(msg));

        char buffer[64] = {};
        OCI_UInt32 size = sizeof(buffer);
        ASSERT_TRUE(OCI_MsgGetRaw(msg, static_cast<void*>(buffer), &size));
        ASSERT_EQ(0u, size);

        ASSERT_TRUE(OCI_MsgFree(msg));
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

        const auto msg = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg);

        const unsigned char originalID[] = { 0x01, 0x02, 0x03, 0x04 };
        ASSERT_TRUE(OCI_MsgSetOriginalID(msg, static_cast<const void*>(originalID), static_cast<OCI_UInt32>(sizeof(originalID))));

        unsigned char buffer[16] = {};
        OCI_UInt32 size = sizeof(buffer);
        ASSERT_TRUE(OCI_MsgGetOriginalID(msg, static_cast<void*>(buffer), &size));
        ASSERT_EQ(static_cast<OCI_UInt32>(sizeof(originalID)), size);
        ASSERT_EQ(0, std::memcmp(originalID, buffer, sizeof(originalID)));

        ASSERT_TRUE(OCI_MsgFree(msg));
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

        const auto msg = OCI_MsgCreate(inf);
        ASSERT_NE(nullptr, msg);

        const auto sender = OCI_AgentCreate(conn, OTEXT("TestSender"), nullptr);
        ASSERT_NE(nullptr, sender);

        ASSERT_TRUE(OCI_MsgSetSender(msg, sender));
        ASSERT_EQ(String(OTEXT("TestSender")), String(OCI_AgentGetName(OCI_MsgGetSender(msg))));

        ASSERT_TRUE(OCI_AgentFree(sender));
        ASSERT_TRUE(OCI_MsgFree(msg));
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
            { "MsgCreate", [] { return nullptr == OCI_MsgCreate(nullptr); } },
            { "MsgFree", [] { return !OCI_MsgFree(nullptr); } },
            { "MsgGetAttemptCount", [] { return 0 == OCI_MsgGetAttemptCount(nullptr); } },
            { "MsgGetCorrelation", [] { return nullptr == OCI_MsgGetCorrelation(nullptr); } },
            { "MsgGetEnqueueDelay", [] { return 0 == OCI_MsgGetEnqueueDelay(nullptr); } },
            { "MsgGetEnqueueTime", [] { return nullptr == OCI_MsgGetEnqueueTime(nullptr); } },
            { "MsgGetExceptionQueue", [] { return nullptr == OCI_MsgGetExceptionQueue(nullptr); } },
            { "MsgGetExpiration", [] { return 0 == OCI_MsgGetExpiration(nullptr); } },
            { "MsgGetID", [] { return !OCI_MsgGetID(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>()); } },
            { "MsgGetObject", [] { return nullptr == OCI_MsgGetObject(nullptr); } },
            { "MsgGetOriginalID", [] { return !OCI_MsgGetOriginalID(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>()); } },
            { "MsgGetPriority", [] { return 0 == OCI_MsgGetPriority(nullptr); } },
            { "MsgGetRaw", [] { return !OCI_MsgGetRaw(nullptr, NullArgBuffer(), NullArgOut<OCI_UInt32>()); } },
            { "MsgGetSender", [] { return nullptr == OCI_MsgGetSender(nullptr); } },
            { "MsgGetState", [] { return OCI_UNKNOWN == OCI_MsgGetState(nullptr); } },
            { "MsgReset", [] { return !OCI_MsgReset(nullptr); } },
            { "MsgSetConsumers", [] { return !OCI_MsgSetConsumers(nullptr, nullptr, 1); } },
            { "MsgSetCorrelation", [] { return !OCI_MsgSetCorrelation(nullptr, OTEXT("x")); } },
            { "MsgSetEnqueueDelay", [] { return !OCI_MsgSetEnqueueDelay(nullptr, 1); } },
            { "MsgSetExceptionQueue", [] { return !OCI_MsgSetExceptionQueue(nullptr, OTEXT("x")); } },
            { "MsgSetExpiration", [] { return !OCI_MsgSetExpiration(nullptr, 1); } },
            { "MsgSetObject", [] { return !OCI_MsgSetObject(nullptr, nullptr); } },
            { "MsgSetOriginalID", [] { return !OCI_MsgSetOriginalID(nullptr, NullArgBuffer(), 1); } },
            { "MsgSetPriority", [] { return !OCI_MsgSetPriority(nullptr, 1); } },
            { "MsgSetRaw", [] { return !OCI_MsgSetRaw(nullptr, NullArgBuffer(), 1); } },
            { "MsgSetSender", [] { return !OCI_MsgSetSender(nullptr, nullptr); } },
        }),
        &NullArgCaseName);

}