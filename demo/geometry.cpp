#include <iostream>

#include "ocilib.hpp"

/* requires script demo/geometry.sql */

using namespace ocilib;

const int ElemCount = 50;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");
        Statement st(con);

        Collection<int> elemInfos(TypeInfo(con, "mdsys.sdo_elem_info_array", TypeInfo::Type));
        Collection<double> ordinates(TypeInfo(con, "mdsys.sdo_ordinate_array", TypeInfo::Type));

        Object geometry(TypeInfo(con, "mdsys.sdo_geometry", TypeInfo::Type));

        /* build ordinates collection with test values */
        for (int i = 0; i < ElemCount; i++)
        {
            ordinates.Append(i);
            ordinates.Append(i * 2);
        }

        /* setup information collection attribute 'starting_offset' */
        elemInfos.Append(1);

        /* setup information collection attribute 'element_type' */
        elemInfos.Append(1);

        /* setup information collection attribute 'interpretation' */
        elemInfos.Append(1);

        /* set sdo object member attributes */
        geometry.SetAttributeNull("sdo_srid");
        geometry.SetAttributeNull("sdo_point");
        geometry.Set("sdo_gtype", 4);
        geometry.Set("sdo_elem_info", elemInfos);
        geometry.Set("sdo_ordinates", ordinates);

        /* prepare, bind and excute statement then commit*/
        st.Prepare("INSERT INTO geometry (code, obj) VALUES (1, :sdo)");
        st.Bind("sdo", geometry, BindInfo::In);
        st.ExecutePrepared();

        std::cout << st.GetAffectedRows() << " rows inserted" << std::endl;
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
