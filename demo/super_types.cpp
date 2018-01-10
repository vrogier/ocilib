#include <iostream>

#include "ocilib.hpp"

/* requires script demo/super_types.sql */

using namespace ocilib;

int main(void)
{
    try
    {
        Environment::Initialize();

        Connection con("db12c", "usr", "pwd");

        Object obj(TypeInfo(con, "derived_type", TypeInfo::Type));

        Statement st(con);
        st.Prepare("begin :obj := derived_type(1, 'Car', 31245.99); end; ");
        st.Bind(":obj", obj, BindInfo::InOut);
        st.ExecutePrepared();

        TypeInfo derivedTypeInfo = obj.GetTypeInfo();
        TypeInfo baseTypeInfo = derivedTypeInfo.GetSuperType();

        std::cout << "Object => " << obj << std::endl;
        std::cout << "Is derived type '" << derivedTypeInfo.GetName() << "' final => " << derivedTypeInfo.IsFinalType() << std::endl;
        std::cout << "Is base type '" << baseTypeInfo.GetName() << "' final => " << baseTypeInfo.IsFinalType() << std::endl;
    }
    catch (std::exception &ex)
    {
        std::cout << ex.what() << std::endl;
    }

    Environment::Cleanup();

    return EXIT_SUCCESS;
}
