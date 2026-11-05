#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestDateCpp, NullToString)
    {
        Date d;
        ASSERT_EQ(String(OTEXT("NULL")), d.ToString());
    }
}