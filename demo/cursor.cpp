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

        for (auto&& row : rs.Rows())
        {
            Statement st2 = row.Get<Statement>(1);
            Resultset rs2 = st2.GetResultset();

            for (auto&& row2 : rs2.Rows())
            {
                std::cout << "Table : " << row.Get<String>(0) << ", Date : " << row2.Get<String>(0) << std::endl;
            }
        }

    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
