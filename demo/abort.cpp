#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

#ifdef _WINDOWS
#define sleep(x) Sleep(x*1000)
#endif

#define wait_for_events() sleep(5)

ThreadHandle thread;
Connection con;

void OracleCallProc(ThreadHandle handle, void *data)
{
    try
    {
        Statement st(con);
        st.Execute("select table_name from all_tables");

        Resultset rs = st.GetResultset();
        while (rs++)
        {
            std::cout << rs.Get<ostring>(1) << std::endl;
        }
    }
    catch (Exception &ex)
    {
        std::cout << ex.GetMessage() << std::endl;
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
