#include <iostream>
#include <thread>

#if defined(_WIN32)
    #include <windows.h>
    #define sleep(x) Sleep(x*1000)
#else
    #include <unistd.h>
#endif

#include "ocilib.hpp"

using namespace ocilib;

Environment env(Environment::Threaded);
Connection con;

void OracleCallProc()
{
    try
    {
        Statement st(con);
        st.Execute("begin dbms_lock.sleep(10); end;");
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
}

int main()
{
    try
    {
        const char *db  = getenv("OCILIB_DEMO_DB");
        const char *usr = getenv("OCILIB_DEMO_USR");
        const char *pwd = getenv("OCILIB_DEMO_PWD");

        con.Open(env, db, usr, pwd, Environment::SessionDefault);

        std::thread t(OracleCallProc);

        sleep(1);
        con.Break();

        t.join();
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    return EXIT_SUCCESS;
}
