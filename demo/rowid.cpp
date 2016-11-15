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

        ostring rowid;

        st.Execute("select rowid from products where code = 1");
        Resultset rs = st.GetResultset();
        rs.Next();
        rowid = rs.Get<ostring>(1);

        st.Prepare("select code, name, rowid from products where rowid = :id");
        st.Bind(":id", rowid, 20, BindInfo::In);
        st.ExecutePrepared();
        rs = st.GetResultset();
        rs.Next();

        std::cout << "code  [" << rs.Get<int>(1) << "] name [" << rs.Get<ostring>(2) << "] rowid [" << rs.Get<ostring>(3) << "]" << std::endl;
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
