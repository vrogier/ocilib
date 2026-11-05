#include "ocilib_tests.h"
#include "../include/ocilib.h"

class TestTimestamp : public ::testing::TestWithParam<OCI_UInt32>{};

static std::vector<OCI_UInt32> TimestampTypes{ OCI_TIMESTAMP, OCI_TIMESTAMP_TZ, OCI_TIMESTAMP_LTZ };

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestTimestampCpp, NullToString)
    {
        Timestamp tmsp;
        ASSERT_EQ(String(OTEXT("NULL")), tmsp.ToString());
    }
}