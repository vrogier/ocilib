#include <iostream>

#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");

        Statement st(con);
        st.Execute("declare"
            "  c1 sys_refcursor;"
            "  c2 sys_refcursor;"
            " begin"
            "  open c1 for select username from all_users;"
            "  dbms_sql.return_result (c1); "
            "  open c2 for select table_name from all_tables;"
            "  dbms_sql.return_result (c2); "
            "end;");

        Resultset rs = st.GetResultset();

        while (rs)
        {
            while (rs++)
            {
                std::cout << "Fetch column name:" << rs.GetColumn(1).GetName() << ", Value:" << rs.Get<ostring>(1) << std::endl;
            }

            std::cout << "=> Total fetched rows : " << rs.GetCount() << std::endl;

            rs = st.GetNextResultset();
        }
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
