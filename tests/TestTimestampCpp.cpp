#include "ocilib_tests.h"
#include "../include/ocilib.h"

namespace TestCppApi
{
    using namespace ocilib;

    class TestTimestamp : public ::testing::TestWithParam<OCI_UInt32>{};

    static std::vector<OCI_UInt32> TimestampTypes{ OCI_TIMESTAMP, OCI_TIMESTAMP_TZ, OCI_TIMESTAMP_LTZ };

    TEST(TestTimestampCpp, ToStringOnNullTimestamp_ReturnsEmptyString)
    {
        Timestamp tmsp;
        ASSERT_EQ(String(OTEXT("NULL")), tmsp.ToString());
    }
}