#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");
        Statement st(con);
        int code;

        st.Prepare("delete from test_fetch where code = :code");
        st.Bind(":code", code, BindInfo::In);
        code = 5;
        st.Execute();

        code = 12;
        st.Execute();

        con.Commit();
    }
    catch(Exception &ex)
    {
         std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
