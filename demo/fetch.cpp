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
        st.Execute("select * from all_users");

        Resultset rs = st.GetResultset();
        while (rs++)
        {
            std::cout << "UserName:" << rs.Get<ostring>("username") << " Created:" << rs.Get<ostring>("created") << std::endl;
        }

        std::cout << "=> Total fetched rows : " << rs.GetCount() << std::endl;

    }
    catch (Exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
