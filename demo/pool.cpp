#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

const int MaxThreads = 50;
const int MaxConnnections = 10;

void worker(ThreadHandle handle, void *data)
{
    Connection con = ((Pool *)data)->GetConnection();
    Statement st(con);
    st.Execute("select to_char(sysdate, 'YYYYMMDD HH24:MI:SS') from dual");
    Resultset rs = st.GetResultset();
    rs.Next();
    std::cout << handle << " - " << rs.Get<ostring>(1) << std::endl;
    con.Close();
}

int main(void)
{
    try
    {
        Environment::Initialize(Environment::Threaded);

        Pool pool("db", "usr", "pwd", Pool::ConnectionPool, 0, MaxConnnections);

        std::vector<ThreadHandle> threads;

        for (int i = 0; i < MaxThreads; i++)
        {
            ThreadHandle th = Thread::Create();
            threads.push_back(th);
            Thread::Run(th, worker, &pool);
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
