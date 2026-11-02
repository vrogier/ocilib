#include "ocilib_tests.h"
#include "ocilib.hpp"

using namespace ocilib;

enum DirectPathErrorTestMode
{
    Default = 0,
    InsufficientBufferError = 1,
    ConversionError = 2,
    LoadError = 4,
    ForceLoad = 8
};

class TestDirectPathErrorModes : public ::testing::TestWithParam<OCI_Int32> {};

static std::vector<OCI_Int32> DirectPathErrorTestModes
{ 
    DirectPathErrorTestMode::Default,
  //  DirectPathErrorTestMode::InsufficientBufferError,
  //  DirectPathErrorTestMode::InsufficientBufferError | DirectPathErrorTestMode::ForceLoad,
    DirectPathErrorTestMode::ConversionError,
    DirectPathErrorTestMode::ConversionError | DirectPathErrorTestMode::ForceLoad,
    DirectPathErrorTestMode::LoadError,
    DirectPathErrorTestMode::LoadError | DirectPathErrorTestMode::ForceLoad
};

namespace TestCppApi
{
    using namespace ocilib;

    TEST(TestDirectPathCpp, SimpleCpp)
    {
        ExecDML(OTEXT("create table TestDirectPathTableCpp (val_int int, val_str varchar2(50), val_date date)"));
        ExecDML(OTEXT("truncate table TestDirectPathTableCpp"));

        Environment& env = GetEnvironmentCpp();
        Connection conn(env, DBS, USR, PWD);

        TypeInfo typeInfo(conn, OTEXT("TestDirectPathTableCpp"), TypeInfo::Table);

        constexpr auto ArraySize = 10u;
        constexpr auto ColCount = 3u;
        constexpr auto ColSize1 = 20u;
        constexpr auto ColSize2 = 30u;
        constexpr auto ColSize3 = 8u;

        DirectPath dp(typeInfo, ColCount, ArraySize);

        dp.SetBufferSize(64000);
        dp.SetNoLog(true);
        dp.SetParallel(true);

        dp.SetColumn(1, OTEXT("val_int"), ColSize1);
        dp.SetColumn(2, OTEXT("val_str"), ColSize2);
        dp.SetColumn(3, OTEXT("val_date"), ColSize3, OTEXT("YYYYMMDD"));

        dp.Prepare();

        ASSERT_NE(0u, dp.GetMaxRows());

        otext val1[ColSize1 + 1];
        otext val2[ColSize2 + 1];
        otext val3[ColSize3 + 1];

        for (OCI_UInt32 i = 0; i < ArraySize; i++)
        {
            dp.Reset();

            for (OCI_UInt32 j = 1; j <= dp.GetMaxRows(); j++)
            {
                osprintf(val1, OTEXT("%04d"), i + (i * 100));
                osprintf(val2, OTEXT("value %05d"), j + (i * 100));
                osprintf(val3, OTEXT("%04d%02d%02d"), (j % 23) + 1 + 2000, (j % 11) + 1, (j % 23) + 1);

                dp.SetEntry(j, 1, ostring(val1));
                dp.SetEntry(j, 2, ostring(val2));
                dp.SetEntry(j, 3, ostring(val3));
            }

            DirectPath::Result res = dp.Convert();
            while (res == DirectPath::ResultFull)
            {
                dp.Load();
                res = dp.Convert();
            }
            ASSERT_EQ(DirectPath::ResultComplete, res);
            dp.Load();
        }

        dp.Finish();

        ASSERT_EQ(ArraySize * ArraySize, dp.GetRowCount());

        ExecDML(OTEXT("drop table TestDirectPathTableCpp"));
    }
}