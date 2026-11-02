#include <iostream>

#include "ocilib.hpp"

/* requires script demo/products.sql */

using namespace ocilib;

int main()
{
    try
    {
        Environment env;

        Connection con(env, "db", "usr", "pwd");

        Statement st(con);

        st.Prepare("update products set name = name || ' updated' returning code into :code");
        st.Register<OCI_Int32>(":code");
        st.ExecutePrepared();
        
        Resultset rs = st.GetResultset();
        while (rs++)
        {
            std::cout << "element with code " << rs.Get<OCI_Int32>(1) << " has been updated" << std::endl;
        }

        std::cout << "=> Total updated rows : " << rs.GetCount() << std::endl;

        st.Execute("select count(*) from products where name like '%updated'");
        rs = st.GetResultset();
        rs++;
        
        std::cout << rs.Get<OCI_Int32>(1) << " elements updated in DB" << std::endl;

    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
