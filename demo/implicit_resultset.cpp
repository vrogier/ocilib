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
                    "  open c1 for select * from tabs;"
                    "  dbms_sql.return_result (c1); "
                    "  open c2 for select * from cols;"
                    "  dbms_sql.return_result (c2); "
                    "end;");

        Resultset rs = st.GetResultset();

        while (rs)
        {
            while (rs.Next())
            {
                std::cout << "table name:" << rs.Get<ostring>(1) << std::endl;
            }
 
            std::cout << "=> Total fetched rows : " << rs.GetCount() << std::endl;

            rs = st.GetNextResultset();
        }
    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}
