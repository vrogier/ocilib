#include <atomic>

#include "ocilib_tests.h"

const size_t MaxThread = 10;

static std::atomic<int> ThreadProcCallCount = 0;

static void WorkerProc(OCI_Thread* thread, void* data)
{
    const void* id = OCI_HandleGetThreadID(thread);
    ASSERT_NE(nullptr, id);

    ++ThreadProcCallCount;
}

TEST(TestThreadKey, RunAndJoin)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT | OCI_ENV_THREADED));

    std::array<OCI_Thread*, MaxThread> threads{};

    for (auto& thread : threads)
    {
        thread = OCI_ThreadCreate();
        ASSERT_NE(nullptr, thread);
        ASSERT_TRUE(OCI_ThreadRun(thread, WorkerProc, nullptr));
    }

    for (auto& thread : threads)
    {
        ASSERT_TRUE(OCI_ThreadJoin(thread));
        ASSERT_TRUE(OCI_ThreadFree(thread));
    }

    ASSERT_TRUE(OCI_Cleanup());

    ASSERT_EQ(MaxThread, ThreadProcCallCount);
}
