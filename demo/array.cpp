#include <iostream>

#include "ocilib.hpp"

/* requires script demo/products.sql */

using namespace ocilib;

const OCI_Int32 ArraySize = 1000;

int main()
{
    try
    {
        Environment env;

        Connection con(env, "db", "usr", "pwd");

        std::vector<OCI_Int32> ints;
        std::vector<ostring> strs;

        Statement st(con);
        st.Prepare("insert into products values(:i, :s)");
        st.SetBindArraySize(ArraySize);
        st.Bind(":i", ints, BindInfo::In);
        st.Bind(":s", strs, 20, BindInfo::In);

        for (OCI_Int32 i = 0; i < ArraySize; ++i)
        {
            ostring str;
            str += "Name";
            str += ((i + 1) + '0');

            ints.push_back(i + 1);
            strs.push_back(str);
        }

        st.ExecutePrepared();

        std::cout << "row processed : " << st.GetAffectedRows() << std::endl;

    }
    catch (const std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }
    return EXIT_SUCCESS;
}
