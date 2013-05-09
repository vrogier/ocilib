
#include "ocilib.hpp"

using namespace ocilib;

const int MaxThreads        = 50;
const int MaxConnnections   = 10;

void worker(ThreadHandle handle, void *data)
{
    Connection con = ((Pool *) data)->GetConnection();
    Statement st(con);
    st.Execute("select to_char(sysdate, 'YYYYMMDD HH24:MI:SS') from dual");
    Resultset rs = st.GetResultset();
    rs.Next();
    std::cout << handle << " - " << rs.Get<dstring>(1) << std::endl;
    con.Close();
}

int main(void)
{
    try
    {
        Environment::Initialize(OCI_ENV_THREADED);

        Pool pool("db", "usr", "pwd", OCI_POOL_CONNECTION, 0, MaxConnnections);

        std::vector<ThreadHandle> threads;

        for (int i = 0; i < MaxThreads; i++)
        {
            ThreadHandle th = Environment::CreateThread();            
            threads.push_back(th);
            Environment::RunThread(th, worker, &pool);
        }

        for (int i = 0; i < MaxThreads; i++)
        {
            Environment::JoinThread(threads[i]);
            Environment::DestroyThread(threads[i]);
        }
    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}
