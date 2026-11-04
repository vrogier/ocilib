#include <iostream>

#include "ocilib.hpp"

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
        OCI_Int32 code;

        st.Prepare("delete from products where code = :code");
        st.Bind(":code", code, BindInfo::In);
        code = 5;
        st.ExecutePrepared();

        code = 12;
        st.ExecutePrepared();

        con.Commit();
    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
