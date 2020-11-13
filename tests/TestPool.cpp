#include <atomic>

#include "ocilib_tests.h"

const size_t MaxThread = 10;

static std::atomic<int> ConnCreatedCount{ 0 };

static void WorkerProc(OCI_Thread* thread, void* data)
{
    const auto conn = OCI_PoolGetConnection(static_cast<OCI_Pool*>(data), nullptr);
    ASSERT_NE(nullptr, conn);

    int anwser = 0;
    OCI_Immediate(conn, OTEXT("select 42 from dual"), OCI_ARG_INT, &anwser);
    ASSERT_EQ(42, anwser);

    ASSERT_TRUE(OCI_ConnectionFree(conn));

    ++ConnCreatedCount;
}

TEST(TestPool, SessionPoolGetMultipleConnections)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT | OCI_ENV_THREADED));

    const auto pool = OCI_PoolCreate(DBS, USR, PWD, OCI_POOL_SESSION, OCI_SESSION_DEFAULT, 0, 10, 1);
    ASSERT_NE(nullptr, pool);

    std::array<OCI_Thread*, MaxThread> threads{};

    for (auto& thread : threads)
    {
        thread = OCI_ThreadCreate();
        ASSERT_NE(nullptr, thread);
        ASSERT_TRUE(OCI_ThreadRun(thread, WorkerProc, pool));
    }

    for (auto& thread : threads)
    {
        ASSERT_TRUE(OCI_ThreadJoin(thread));
        ASSERT_TRUE(OCI_ThreadFree(thread));
    }

    ASSERT_TRUE(OCI_PoolFree(pool));
    ASSERT_TRUE(OCI_Cleanup());

    ASSERT_EQ(MaxThread, ConnCreatedCount);
}
