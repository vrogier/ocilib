#include <iostream>

#include "ocilib.hpp"

/* requires script demo/super_types.sql */

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db", "usr", "pwd");

        Object obj(TypeInfo(con, "racing_car_type", TypeInfo::Type));

        Statement st(con);
        st.Prepare("begin :obj := racing_car_type(1,'Formula1', 123456789, 300); end; ");
        st.Bind(":obj", obj, BindInfo::InOut);
        st.ExecutePrepared();

        TypeInfo RacingCarTypeInfo = obj.GetTypeInfo();
        TypeInfo CarTypeTypeInfo = RacingCarTypeInfo.GetSuperType();
        TypeInfo VehiculeTypeInfo = CarTypeTypeInfo.GetSuperType();

        std::cout << "Object => " << obj << std::endl;
        std::cout << "Is type '" << RacingCarTypeInfo.GetName() << "' final => " << RacingCarTypeInfo.IsFinalType() << std::endl;
        std::cout << "Is type '" << CarTypeTypeInfo.GetName() << "' final => " << CarTypeTypeInfo.IsFinalType() << std::endl;
        std::cout << "Is type '" << VehiculeTypeInfo.GetName() << "' final => " << VehiculeTypeInfo.IsFinalType() << std::endl;
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
