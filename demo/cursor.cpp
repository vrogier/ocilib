#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");
        
        Statement st(con);

        st.Execute("select code, cursor(select sysdate from dual) from products");
    
        Resultset rs = st.GetResultset();

        while (rs.Next())
        {
            Statement st2 = rs.Get<Statement>(2);
            Resultset rs2 = st2.GetResultset();

            while (rs2.Next())
            {
                std::cout << "Code : " << rs.Get<int>(1) << ", Date : " << rs2.Get<ostring>(1) << std::endl;
            }
        }    

    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}
