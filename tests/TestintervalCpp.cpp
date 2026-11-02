#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestIntervalCpp, NullToString)
    {
        Interval iv;
        ASSERT_EQ(ostring(OTEXT("NULL")), iv.ToString());
    }
}