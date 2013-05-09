#include "ocilib.hpp"

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");

        Statement st(con);

        st.Prepare("update products set code = code+10 returning code into :i");
        st.Register<int>(":i");
        st.Execute();

        Resultset rs = st.GetResultset();

        while (rs.Next())
        {
            std::cout << "updated code is " << rs.Get<int>(1) << std::endl;
        }
 
        std::cout << "=> Total updated rows : " << rs.GetCount() << std::endl;

    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}
