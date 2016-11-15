#include <iostream>

#include "ocilib.hpp"

#if defined(_WINDOWS)
    #define sleep(x) Sleep(x*1000)
#else
    #include <unistd.h>
#endif

using namespace ocilib;

ThreadHandle thread;
Connection con;

void OracleCallProc(ThreadHandle handle, void *data)
{
    try
    {
        Statement st(con);
        st.Execute("begin dbms_lock.sleep(10); end;");
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

int main(void)
{
    try
    {
        Environment::Initialize(Environment::Threaded);

        con.Open("db", "usr", "pwd");

        thread = Thread::Create();
        Thread::Run(thread, OracleCallProc, 0);

        sleep(1);
        con.Break();

        Thread::Join(thread);
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
