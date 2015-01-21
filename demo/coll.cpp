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

        /* bind local collection to a PL/SQL procedure */

        Collection<ostring> coll1(TypeInfo(con, "varray_type", TypeInfo::Type));

        st.Prepare("begin load_array(:coll); end;");
        st.Bind(":coll", coll1, BindInfo::In);
        st.Execute();

        Collection<ostring>::iterator it1 = coll1.begin();
        Collection<ostring>::iterator it2 = coll1.end();

        for (; it1 != it2; ++it1)
        {
            std::cout << "value " << static_cast<ostring>(*it1) << std::endl;
        }

        /* query on a table with varray column */

        st.Execute("SELECT products from items");

        Resultset rs = st.GetResultset();
        while (rs++)
        {
            Collection<Object> coll2 = rs.Get<Collection<Object>>("products");
            Collection<Object>::iterator it1 = coll2.begin();
            Collection<Object>::iterator it2 = coll2.end();

            for (; it1 != it2; ++it1)
            {
                Object obj = *it1;

                std::cout << "... product id : " << obj.Get<int>("Id") << ", name : " << obj.Get<ostring>("Name") << std::endl;
            }
        }
    }
    catch (Exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
