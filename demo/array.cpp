#include "ocilib.hpp"

using namespace ocilib;

const int ArraySize = 1000;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");

        std::vector<int> ints;
        std::vector<dstring> strs;
        
        Statement st(con);
        st.Prepare("insert into products values(:i, :s)");
        st.SetBindArraySize(ArraySize);
        st.Bind(":i", ints, OCI_BDM_IN);
        st.Bind(":s", strs, 20, OCI_BDM_IN);

        for(int i=0;i<ArraySize;i++)
        {
            dstring str;
            str += "Name";
            str += ( (i+1) +'0');

            ints.push_back(i+1);
            strs.push_back(str);
        }

        st.Execute();

        std::cout << "row processed : " << st.GetAffectedRows() << std::endl; 

    }
    catch(Exception &ex)
    {
         std::cout << ex.GetMessage() << std::endl;
    }

    Environment::Cleanup();
 
    return EXIT_SUCCESS;
}
