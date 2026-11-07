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

        String rowid;

        st.Execute("select rowid from products where code = 1");
        Resultset rs = st.GetResultset();
        rs.Next();
        rowid = rs.Get<String>(0);

        st.Prepare("select code, name, rowid from products where rowid = :id");
        st.Bind(":id", rowid, 20, BindInfo::In);
        st.ExecutePrepared();
        rs = st.GetResultset();
        auto row = *rs.Rows().begin();

        std::cout << "code  [" << row.Get<OCI_Int32>(0) << "] name [" << row.Get<String>(1) << "] rowid [" << row.Get<String>(2) << "]" << std::endl;
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
