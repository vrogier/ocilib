#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

const int MaxThreads = 50;

void worker(ThreadHandle handle, void *data)
{
    ThreadKey::SetValue("ID", const_cast<AnyPointer>(Thread::GetThreadId(handle)));

    /* ... do some more processing here... */

    std::cout << " Thread handle = " << handle
        << " Key (Thread Id) = " << ThreadKey::GetValue("ID")
        << std::endl;
}

int main(void)
{
    try
    {
        Environment::Initialize(Environment::Threaded);

        ThreadKey::Create("ID");

        std::vector<ThreadHandle> threads;

        for (int i = 0; i < MaxThreads; i++)
        {
            ThreadHandle th = Thread::Create();
            threads.push_back(th);
            Thread::Run(th, worker, 0);
        }

        for (int i = 0; i < MaxThreads; i++)
        {
            Thread::Join(threads[i]);
            Thread::Destroy(threads[i]);
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
