#include <iostream>

#include "ocilib.hpp"

/* requires script demo/products.sql */

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");

        Statement st(con);
        st.Execute("select * from products");

        Resultset rs = st.GetResultset();
        while (rs++)
        {
            std::cout << "code:" << rs.Get<ostring>(1) << " name:" << rs.Get<ostring>(2) << std::endl;
        }

        std::cout << "=> Total fetched rows : " << rs.GetCount() << std::endl;

    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
