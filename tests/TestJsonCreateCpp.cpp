#include "ocilib_tests.h"

#include <cstring>

using namespace ocilib;

namespace TestCppApi
{


    TEST(TestJsonCreateCpp, CreateAndFree)
    {
        Environment& env = GetEnvironmentCpp();
        Connection con(env, DBS, USR, PWD);

        Json json;
        ASSERT_TRUE(json.IsNull());

        Json json2(con);
        ASSERT_FALSE(json2.IsNull());
    }
}
