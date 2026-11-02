#include <iostream>
#include <thread>
#include <vector>

#include "ocilib.hpp"

using namespace ocilib;

const OCI_Int32 MaxThreads = 50;

Environment env(Environment::Threaded);

thread_local OCI_Int32 key_id = -1;

void worker(OCI_Int32 id)
{
    key_id = id;

    /* ... do some more processing here... */

    std::cout << " Thread id = " << id
        << " Key (Thread Id) = " << key_id
        << std::endl;
}

int main()
{
    try
    {
        std::vector<std::thread> threads;

        for (OCI_Int32 i = 0; i < MaxThreads; ++i)
        {
            threads.emplace_back(worker, i);
        }

        for (auto& t : threads)
        {
            t.join();
        }
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
