#include <iostream>
#include <thread>
#include <vector>

#include "ocilib.hpp"

using namespace ocilib;

const OCI_Int32 MaxThreads = 50;
const OCI_Int32 MaxConnnections = 10;

Environment env(Environment::Threaded);
const char *db  = getenv("OCILIB_DEMO_DB");
const char *usr = getenv("OCILIB_DEMO_USR");
const char *pwd = getenv("OCILIB_DEMO_PWD");

Pool pool(env, db, usr, pwd, Pool::ConnectionPool, 0, MaxConnnections, 1, Environment::SessionDefault);

void worker(OCI_Int32 id)
{
    Connection con = pool.GetConnection(String());
    Statement st(con);
    st.Execute("select to_char(sysdate, 'YYYYMMDD HH24:MI:SS') from dual");
    Resultset rs = st.GetResultset();
    auto row = *rs.Rows().begin();
    std::cout << id << " - " << row.Get<String>(0) << std::endl;
    con.Close();
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
