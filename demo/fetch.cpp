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
        st.Execute("select * from products");

        Resultset rs = st.GetResultset();
        for (auto&& row : rs.Rows())
        {
            std::cout << "code:" << row.Get<String>(0) << " name:" << row.Get<String>(1) << std::endl;
        }

        std::cout << "=> Total fetched rows : " << rs.GetCount() << std::endl;

    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
