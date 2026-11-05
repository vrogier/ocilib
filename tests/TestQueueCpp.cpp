#include <atomic>

#include "ocilib_tests.h"
static std::atomic<OCI_Int32> MessageWithConsumersCount {0};

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestQueueCpp, SingleRawMessageCpp)
    {
        const String queueTableName = OTEXT("TestQueueCppRawTable");
        const String queueName = OTEXT("TestQueueCppRaw");

        try
        {
            Environment& env = GetEnvironmentCpp();
            Connection conn(env, DBS, USR, PWD);

            // Best-effort cleanup of objects left by a previous run
            try
            {
                Queue::Stop(conn, queueName, true, true, false);
                Queue::Drop(conn, queueName);
                QueueTable::Drop(conn, queueTableName, true);
            }
            catch (...)
            {
            }

            QueueTable::Create(conn, queueTableName, OTEXT("RAW"), false);
            Queue::Create(conn, queueName, queueTableName);
            Queue::Start(conn, queueName);

            Queue::Alter(conn, queueName, 0, 0, 0, OTEXT("test comment"));

            TypeInfo rawType(conn, OTEXT("SYS.RAW"), TypeInfo::TypeInfoType(TypeInfo::Type));

            Message msg(rawType);
            Raw payload{ 'a', 'b', 'c' };
            msg.SetPayload<Raw>(payload);
            msg.SetPriority(5);
            msg.SetEnqueueDelay(0);
            msg.SetExpiration(60);
            msg.SetCorrelation(OTEXT("cpp_corr"));

            Enqueue enq(rawType, queueName);
            enq.SetVisibility(Enqueue::EnqueueVisibility(Enqueue::Immediate));
            enq.SetMode(Enqueue::EnqueueMode(Enqueue::OnTop));

            Raw relativeMsgID{ 0x01, 0x02, 0x03, 0x04, 0x05, 0x06, 0x07, 0x08, 0x09, 0x0A, 0x0B, 0x0C, 0x0D, 0x0E, 0x0F, 0x10 };
            enq.SetRelativeMsgID(relativeMsgID);
            (void)enq.GetRelativeMsgID();

            enq.Put(msg);
            conn.Commit();

            Dequeue deq(rawType, queueName);
            deq.SetMode(Dequeue::DequeueMode(Dequeue::Remove));
            deq.SetVisibility(Dequeue::DequeueVisibility(Dequeue::Immediate));
            deq.SetNavigation(Dequeue::NavigationMode(Dequeue::FirstMessage));
            deq.SetWaitTime(5);
            deq.SetConsumer(OTEXT(""));

            Message out = deq.Get();
            ASSERT_TRUE(out);
            ASSERT_EQ(payload, out.GetPayload<Raw>());
            ASSERT_EQ(5, out.GetPriority());
            ASSERT_EQ(String(OTEXT("cpp_corr")), out.GetCorrelation());
            ASSERT_TRUE(out.GetState() == Message::Ready);

            ASSERT_EQ(Enqueue::OnTop, enq.GetMode());
            ASSERT_EQ(Enqueue::Immediate, enq.GetVisibility());

            ASSERT_EQ(Dequeue::Remove, deq.GetMode());
            ASSERT_EQ(Dequeue::Immediate, deq.GetVisibility());
            ASSERT_EQ(Dequeue::FirstMessage, deq.GetNavigation());
            ASSERT_EQ(5, deq.GetWaitTime());

            QueueTable::Purge(conn, queueTableName, QueueTable::PurgeMode(QueueTable::All), OTEXT(""), false);

            Queue::Stop(conn, queueName, true, true, false);
            Queue::Drop(conn, queueName);
            QueueTable::Drop(conn, queueTableName, true);
        }
        catch (const ocilib::Exception& e)
        {
            const std::string msg = "OCILIB Exception: " + std::string(e.what() ? e.what() : "<null>") + " Oracle=" + std::to_string(e.GetCode()) + " Lib=" + std::to_string(e.GetCode());
            FAIL() << msg;
        }
        catch (const std::exception& e)
        {
            FAIL() << "std::exception: " << (e.what() ? e.what() : "<null>");
        }
        catch (...)
        {
            FAIL() << "Unknown exception";
        }
    }
}