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

        st.Execute("select rownum, cursor(select sysdate from dual) from (select 1 from dual connect by level <= 10)");

        Resultset rs = st.GetResultset();

        while (rs++)
        {
            Statement st2 = rs.Get<Statement>(2);
            Resultset rs2 = st2.GetResultset();

            while (rs2++)
            {
                std::cout << "Table : " << rs.Get<ostring>(1) << ", Date : " << rs2.Get<ostring>(1) << std::endl;
            }
        }

    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
