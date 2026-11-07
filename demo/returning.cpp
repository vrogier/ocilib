#include <iostream>

#include "ocilib.hpp"

/* requires script demo/products.sql */

using namespace ocilib;

int main()
{
    try
    {
        Environment env;

        const char *db  = getenv("OCILIB_DEMO_DB");
        const char *usr = getenv("OCILIB_DEMO_USR");
        const char *pwd = getenv("OCILIB_DEMO_PWD");

        Connection con(env, db, usr, pwd);

        Statement st(con);

        st.Prepare("update products set name = name || ' updated' returning code into :code");
        st.Register<OCI_Int32>(":code");
        st.ExecutePrepared();
        
        Resultset rs = st.GetResultset();
        for (auto&& row : rs.Rows())
        {
            std::cout << "element with code " << row.Get<OCI_Int32>(0) << " has been updated" << std::endl;
        }

        std::cout << "=> Total updated rows : " << rs.GetCount() << std::endl;

        st.Execute("select count(*) from products where name like '%updated'");
        rs = st.GetResultset();
                rs.Next();
        std::cout << rs.Get<OCI_Int32>(0) << " elements updated in DB" << std::endl;

    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
