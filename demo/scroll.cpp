#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");

        Statement st(con);
        
        st.SetFetchMode(Statement::FetchScrollable);
        st.Execute("select table_name from tabs");
   
        Resultset rs = st.GetResultset();

        rs.Last();
        std::cout << "Total rows : " << rs.GetCount() << std::endl;

        rs.First();
        std::cout << "table " << rs.Get<ostring>(1) << std::endl;

        while (rs.Next())
        {
            std::cout << "table " << rs.Get<ostring>(1) << std::endl;
        }

        while (rs.Prev())
        {
            std::cout << "table " << rs.Get<ostring>(1) << std::endl;
        }

        rs.Seek(Resultset::SeeKRelative, 6);
        std::cout << "table " << rs.Get<ostring>(1) << std::endl;

        while (rs.Next())
        {
            std::cout << "table " << rs.Get<ostring>(1) << std::endl;
        }

        rs.Seek(Resultset::SeeKRelative, -3);
        std::cout << "table " << rs.Get<ostring>(1) << std::endl;
    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}
