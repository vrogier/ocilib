#include "ocilib.hpp"

using namespace ocilib;

static HANDLE evt;
ThreadHandle thread;
Connection con;

void OracleCallProc(ThreadHandle handle, void *data)
{ 
    try
    {
        Statement st(con);
        st.Execute("select table_name from all_tables");

        Resultset rs = st.GetResultset();
        while (rs.Next())
        {
            std::cout << rs.Get<ostring>(1) << std::endl;
        }
    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    SetEvent(evt);
}

int main(void)
{
    try
    {
        Environment::Initialize();

        con.Open("db", "usr", "pwd");      

        thread = Thread::Create();
        Thread::Run(thread, OracleCallProc, 0);

        Sleep(1000);
        if (WaitForSingleObject(evt, 1000) != WAIT_OBJECT_0)
        {
            con.Break();
        }

        Thread::Join(thread);
    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}
