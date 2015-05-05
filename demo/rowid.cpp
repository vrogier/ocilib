#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");

        Statement st1(con);
        Statement st2(con);

        ostring rowid;
        int code;

        st1.Prepare("update test_fetch set code = :i where rowid = : s");
        st1.Bind<ostring>(":s", rowid, 50, BindInfo::In);
        st1.Bind<int>(":i", code, BindInfo::In);

        st2.Execute("select code, rowid from test_fetch for update");
        Resultset rs = st2.GetResultset();

        while (rs++)
        {
            std::cout << "updated code is " << rs.Get<int>(1) << std::endl;

            code = rs.Get<int>(1);
            rowid = rs.Get<ostring>(2);

            /* updating value with some computation ... */
            code = (code + 4) % 2;

            st1.ExecutePrepared();
        }

        con.Commit();
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
