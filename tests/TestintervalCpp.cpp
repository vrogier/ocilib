#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestIntervalCpp, NullToString)
    {
        Interval iv;
        ASSERT_EQ(String(OTEXT("NULL")), iv.ToString());
    }
}