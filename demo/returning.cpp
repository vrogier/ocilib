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

        st.Prepare("update products set name = name || ' updated' returning code into :code");
        st.Register<int>(":code");
        st.ExecutePrepared();
        
        Resultset rs = st.GetResultset();
        while (rs++)
        {
            std::cout << "element with code " << rs.Get<int>(1) << " has been updated" << std::endl;
        }

        std::cout << "=> Total updated rows : " << rs.GetCount() << std::endl;

        st.Execute("select count(*) from products where name like '%updated'");
        rs = st.GetResultset();
        rs++;
        
        std::cout << rs.Get<int>(1) << " elements updated in DB" << std::endl;

    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
