#include <atomic>

#include "ocilib_tests.h"

const size_t MaxThread = 10;

static std::atomic<int> ThreadKeyFreeCount = 0;

static void KeyCleanup(void* str)
{
    ++ThreadKeyFreeCount;
    ASSERT_NE(nullptr, str);
    delete [] static_cast<char*>(str);
}

static void WorkerProc(OCI_Thread* thread, void* data)
{
    const void* id = OCI_HandleGetThreadID(thread);
    ASSERT_NE(nullptr, id);

    auto str1= new char[100];
    sprintf(str1, "thread %p !\n", id);

    ASSERT_TRUE(OCI_ThreadKeySetValue(OTEXT("ID"), str1));

    Sleep(10);
    const auto str2 = static_cast<char *>(OCI_ThreadKeyGetValue(OTEXT("ID")));
    ASSERT_NE(nullptr, str2);

    ASSERT_EQ(std::string(str1), std::string(str2));
}

TEST(TestThreadKey, GetSetValue)
{
    ASSERT_TRUE(OCI_Initialize(nullptr, HOME, OCI_ENV_DEFAULT | OCI_ENV_THREADED));

    OCI_ThreadKeyCreate("ID", KeyCleanup);

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

    ASSERT_EQ(MaxThread, ThreadKeyFreeCount);
}