#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");
        
        Statement st(con);
        st.Execute("select * from products");

        Resultset rs = st.GetResultset();
        while (rs.Next())
        {
            std::cout << "code:" << rs.Get<int>(1) << " name: " <<  rs.Get<ostring>(2) << std::endl;
        }
 
        std::cout << "=> Total fetched rows : " << rs.GetCount() << std::endl;

    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}