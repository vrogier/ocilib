#include <cstring>

#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestMessageCpp, RawPayloadAndPropertiesCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        TypeInfo rawType(conn, OTEXT("SYS.RAW"), TypeInfo::TypeInfoType(TypeInfo::Type));

        Message msg(rawType);

        Raw payload{ 0x01, 0x02, 0x03, 0x04 };
        msg.SetPayload<Raw>(payload);
        ASSERT_EQ(payload, msg.GetPayload<Raw>());

        msg.SetPriority(7);
        ASSERT_EQ(7, msg.GetPriority());

        msg.SetEnqueueDelay(2);
        ASSERT_EQ(2, msg.GetEnqueueDelay());

        msg.SetExpiration(60);
        ASSERT_EQ(60, msg.GetExpiration());

        msg.SetCorrelation(OTEXT("corr"));
        ASSERT_EQ(String(OTEXT("corr")), msg.GetCorrelation());

        msg.SetExceptionQueue(OTEXT("exception_queue"));
        ASSERT_EQ(String(OTEXT("exception_queue")), msg.GetExceptionQueue());

        Raw originalID{ 0xAA, 0xBB };
        msg.SetOriginalID(originalID);
        ASSERT_EQ(originalID, msg.GetOriginalID());

        ASSERT_TRUE(msg.GetState() == Message::Ready);
        ASSERT_EQ(0, msg.GetAttemptCount());

        Date enqueueTime = msg.GetEnqueueTime();
        ASSERT_TRUE(enqueueTime);

        (void)msg.GetID();
    }

    TEST(TestMessageCpp, ResetCpp)
    {
        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        TypeInfo rawType(conn, OTEXT("SYS.RAW"), TypeInfo::TypeInfoType(TypeInfo::Type));

        Message msg(rawType);

        msg.SetPayload<Raw>(Raw{ 0x01, 0x02 });
        msg.SetPriority(5);
        msg.SetCorrelation(OTEXT("to reset"));
        msg.SetOriginalID(Raw{ 0xCC });

        msg.Reset();

        ASSERT_EQ(0, msg.GetPriority());
        ASSERT_TRUE(msg.GetCorrelation().empty());
        ASSERT_TRUE(msg.GetPayload<Raw>().empty());
        ASSERT_TRUE(msg.GetOriginalID().empty());
    }
}
