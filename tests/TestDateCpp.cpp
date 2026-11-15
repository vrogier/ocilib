#include "ocilib_tests.h"

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestDateCpp, ToStringOnNullDate_ReturnsEmptyString)
    {
        Date d;
        ASSERT_EQ(String(OTEXT("NULL")), d.ToString());
    }
}